package com.stericsson.sdk.signing.signerservice;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.LinkedList;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketFactory;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketInput;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketOutput;

/**
 * Default implementation of the ISignerService interface to interact with the sign server running
 * on (most likely) different host(s).
 * 
 * @author xtomlju
 */
public class SignerService extends AbstractSignerService {

    private static final long TIMEOUT = 150000L;

    private static final int EXP_LENGTH = 160;

    private static final BigInteger ALPHA_VALUE =
        new BigInteger("E7734EBBCF50893C760181B2AA2DB0ACF2D5B6E775EE88BAFC7AA5A6BB2"
            + "0A64EB9F54301141F90291B7B37513539450481C9F9CB2BA3E67B4580E2153FD22B80", 16).abs();

    private static final BigInteger MOD_VALUE =
        new BigInteger("FAF30C63D171E54A8131CD331D7C8D6C8AED41B0354E1A29D8DAD03E2E6"
            + "7FF8E00053A07FD28A1EE6AF199FD70330EA8C4C602B86EDFBF47FD1D7BFB6456BD57", 16).abs();

    /** */
    public static final String ENV_SIGNER_SERVICE_SERVERS = "SIGNSERVER";

    private LinkedList<InetSocketAddress> hostAddresses;

    private boolean connected;

    private Logger logger;

    private Socket serviceSocket;

    private SignerProtocolPacketInput packetInput;

    private SignerProtocolPacketOutput packetOutput;

    private BigInteger clientRandom;

    private BigInteger clientNumber;

    private boolean useCommonFolder;

    private ISignerSettings settings;

    /**
     * Constructor
     * @param useCommonFolderParam false(use mirror structure to locate keys and packages) / 
     *                             true(use common folder to locate keys and packages)
     * @param genericSignerSettings
     *            passing info for overriding env variables.
     */
    public SignerService(boolean useCommonFolderParam, ISignerSettings genericSignerSettings) {
        hostAddresses = new LinkedList<InetSocketAddress>();
        logger = Logger.getLogger(getClass().getName());
        connected = false;
        useCommonFolder = useCommonFolderParam;
        if (genericSignerSettings!=null) {
            settings = genericSignerSettings;
        }
    }

    /**
     * Connects to first available signer service
     * 
     * @throws SignerServiceException
     *             If an signer service related error occurred
     */
    public void connect() throws SignerServiceException {

        // Only continue if not already connected

        if (connected) {
            throw new SignerServiceException("Already connected");
        }

        for (InetSocketAddress address : hostAddresses) {
            logger.info("Trying to connect signer service " + address.getHostName() + " at port " + address.getPort());
            serviceSocket = SignerServiceSocketFactory.createSocket(address, logger);
            if (serviceSocket != null) {
                connected = true;
                break;
            }
        }

        if (serviceSocket != null) {
            try {
                connect(serviceSocket.getInputStream(), serviceSocket.getOutputStream());
            } catch (Exception e) {
                throw new SignerServiceException("Failed to get I/O streams from socket");
            }
        } else {
            throw new SignerServiceException("No signer service could be connected");
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerService#connect(java.io.InputStream,
     * java.io.OutputStream)
     */
    /**
     * Connects to signer service on specified socket
     * 
     * @param input
     *            Input stream for reading protocol packets
     * @param output
     *            Output stream for writing protocol packets
     */
    public void connect(InputStream input, OutputStream output) {
        packetInput = new SignerProtocolPacketInput(input);
        packetOutput = new SignerProtocolPacketOutput(output);
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerService#disconnect()
     */
    /**
     * Disconnects from signer service
     */
    public void disconnect() {

        if (packetInput != null) {
            packetInput.terminate();
        }

        if (packetOutput != null) {
            packetOutput.terminate();
        }

        if (serviceSocket != null) {
            try {
                serviceSocket.close();
            } catch (Exception e) {
                logger.warn(e.getMessage());
            } finally {
                serviceSocket = null;
            }
        }

        connected = false;
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackage getSignPackage(String alias, boolean enableHelloHandshake) throws SignerServiceException {

        ISignPackage signPackage = null;

        notifyMessage("Getting sign package with alias '" + alias + "'");

        if (!connected) {
            throw new SignerServiceException("Not connected");
        }

        logon(alias, enableHelloHandshake);

        useCommonFolderRequest(useCommonFolder);

        SignerProtocolPacket packet =
            SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_HEADERPACKAGE_REQUEST, new byte[0]);

        packetOutput.putPacket(packet);

        packet = packetInput.getPacket(TIMEOUT);

        if (SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_HEADERPACKAGE_RESPONSE)) {

            signPackage = SignPackageFactory.createSignPackage(packet.getData());

        } else {
            throw new SignerServiceException("Sign package with alias " + alias
                + " is not available or you don't have access to it");
        }

        return signPackage;
    }

    /**
     * {@inheritDoc}
     */
    public void setSignKeyPath(String pSignKeyPath, boolean enableHelloHandshake) throws SignerServiceException {

       throw new SignerServiceException("Only local signing allowed with sign-key parameter.");
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerService#sign(byte[])
     */
    /**
     * Signs specified data and returns the signature
     * 
     * @param data
     *            Data to be signed
     * @return Signature of signed data
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    public byte[] signA2(byte[] data) throws SignerServiceException {
        return SignerServiceHelper.signA2(packetOutput, packetInput, data, TIMEOUT);
    }

    /**
     * Register a host that can be used as a signer service.
     * 
     * @param host
     *            Host name or IP address for remote signer service server
     * @param port
     *            Port number for remote signer service server
     */
    public void registerHost(String host, int port) {

        boolean add = true;

        for (InetSocketAddress address : hostAddresses) {
            if (address.getHostName().equalsIgnoreCase(host) && (address.getPort() == port)) {
                add = false;
                break;
            }
        }

        if (add) {
            hostAddresses.add(new InetSocketAddress(host, port));
            logger.info("Host " + host + ":" + port + " registered as a signer service server");
        }
    }

    /**
     * Method to configure the signer service object
     * 
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    public void configure() throws SignerServiceException {
        String signerServiceServers = null;
        if (settings != null){
            signerServiceServers = (settings.getSignerSetting(ISignerSettings.KEY_SIGN_SERVICE_SERVERS)!=null)
            ? settings.getSignerSetting(ISignerSettings.KEY_SIGN_SERVICE_SERVERS).toString() : null;
        }
        if (signerServiceServers == null){
            signerServiceServers = System.getProperty(ENV_SIGNER_SERVICE_SERVERS);
        }
        if (signerServiceServers == null) {
            signerServiceServers = System.getenv(ENV_SIGNER_SERVICE_SERVERS);
        }

        if (signerServiceServers != null) {

            String[] servers = signerServiceServers.split(";");

            for (String server : servers) {

                // Check correct format

                int pos = server.lastIndexOf(':');

                if (pos == -1) {
                    throw new SignerServiceException(
                        "Sign server configuration must be on the format <host>:<port>(;<host>:<port>)* ");
                }

                // Get the hostname and port number

                String[] host = server.split(":");
                registerHost(host[0], Integer.parseInt(host[1]));
            }
        } else {
            throw new SignerServiceException("No sign server specified!");
        }

        connect();
    }

    /**
     * @see com.stericsson.sdk.signing.ISignerService#getSignPackagesInformation(boolean, boolean)
     *      {@inheritDoc}
     */
    public ISignPackageInformation[] getSignPackagesInformation(boolean enableHelloHandshake, boolean includeSubPackages)
        throws SignerServiceException {
        SignerProtocolPacket packet;

        if (enableHelloHandshake) {
            packet = packetInput.getPacket(TIMEOUT);
            if (!SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_HELLO)) {
                throw new SignerServiceException("Invalid initial handshake from signer service");
            }

            packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_HELLO,
                new byte[0]));
        }

        useCommonFolderRequest(useCommonFolder);

        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
            SignerProtocolPacket.COMMAND_KEYCATALOGUE_REQUEST, new byte[0]));

        notifyMessage("Requesting list of available sign packages");

        packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_KEYCATALOGUE_RESPONSE)) {
            throw new SignerServiceException("Could not receive list of available sign packages");
        }

        LinkedList<ISignPackageInformation> signPackageInformation = SignerServiceHelper.
                getSignPackageInformation(packet.getData(), includeSubPackages);

        return signPackageInformation.toArray(new ISignPackageInformation[0]);
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackageInformation[] getSignPackagesInformation() throws SignerServiceException {
        return getSignPackagesInformation(true, false);
    }

    /**
     * encrypt data with session key and returns result
     * 
     * @param data
     *            input not encrypted data
     * @return encrypted data
     * @throws SignerServiceException
     *             sse
     */
    public byte[] a2EncryptSessionKey(byte[] data) throws SignerServiceException {

        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
            SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_SESSIONKEY_REQUEST, data));
        SignerProtocolPacket packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet,
            SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_SESSIONKEY_RESPONSE)) {
            throw new SignerServiceException("Expected command data a2 data encryption session key response");
        }

        return packet.getData();
    }

    /**
     * @param chipid
     *            Data to sign.
     * @param key
     *            The session key to encrypt.
     * @throws Exception
     *             if something goes wrong.
     * @return encrypted session key.
     */
    public byte[] encryptSessionKey(int chipid, int[] key) throws Exception {
        byte[] toSend = SignerServiceHelper.prepareSessionKeyEncryption(chipid, key);
        return a2EncryptSessionKey(toSend);
    }

    private void logon(String alias, boolean enableHelloHandshake) throws SignerServiceException {

        SignerProtocolPacket packet;

        if (enableHelloHandshake) {
            packet = packetInput.getPacket(TIMEOUT);

            if (!SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_HELLO)) {
                throw new SignerServiceException("Invalid initial handshake from signer service");
            }

            packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_HELLO,
                new byte[0]));

            logger.debug("HELLO handshake with signer service completed");
        }
        try {
            packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_LOGIN_REQUEST,
                alias.getBytes("UTF-8")));

            useCommonFolderRequest(useCommonFolder);
        } catch (Exception e) {
            throw new SignerServiceException(e.getMessage());
        }

        packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_RANDOMNUMBER_REQUEST)) {
            throw new SignerServiceException("Expected random number request packet");
        }

        logger.debug("Random number request received");

        makeClientNumber();
        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
            SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE, clientNumber.toByteArray()));

        packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_SECRET_REQUEST)) {
            throw new SignerServiceException("Expected secret request packet for sign package with alias '" + alias
                + "'. Maybe it is missing or you don't have access to it.");
        }

        logger.debug("Secret request received");

        String secretFilename = "";
        try {
            secretFilename = new String(packet.getData(), "UTF-8");
        } catch (UnsupportedEncodingException e) {
            throw new SignerServiceException(e.getMessage());
        }

        notifyMessage("Trying to read secret from " + secretFilename);

        String signPackagesRoot = null;
        if(settings != null){
            signPackagesRoot = (settings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ROOT)!= null)
            ? settings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ROOT).toString() : null;
        }

        BigInteger clientSecret =
            new BigInteger(SignerServiceHelper.getSecret(logger, secretFilename, signPackagesRoot))
                .modPow(clientRandom, MOD_VALUE);
        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_SECRET_RESPONSE,
            clientSecret.toByteArray()));

        packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_LOGIN_ACCEPT)) {
            throw new SignerServiceException("Logon sequence filed");
        }

        logger.info("Logon successful for sign package with alias '" + alias + "'");
    }

    /**
     * Creates the necessary authentication values for setting up the session.
     */
    private void makeClientNumber() {
        clientRandom = new BigInteger(EXP_LENGTH, SignerServiceHelper.makeRandomNumber()).abs();
        clientNumber = ALPHA_VALUE.modPow(clientRandom, MOD_VALUE);
    }

    /**
     * {@inheritDoc}
     */
    public HashMap<String, String> getSubPackages(String parent) throws SignerServiceException {
        HashMap<String, String> map = new HashMap<String, String>();

        ISignPackageInformation[] signPackagesInformation = getSignPackagesInformation(false, true);
        if (signPackagesInformation == null) {
            throw new SignerServiceException("Failed to get sign packages information.");
        }
        for (ISignPackageInformation info : signPackagesInformation) {
            String infoParent = null;
            if (info != null) {
                infoParent = info.getParent();
            }
            if (info != null && infoParent != null && parent.equals(infoParent)) {
                map.put(info.getType(), info.getName());
            }
        }
        return map;
    }

    /**
     * {@inheritDoc}
     */
    public byte[] signU8500(byte[] data) throws SignerServiceException {
        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
            SignerProtocolPacket.COMMAND_DATA_U8500_ENCRYPTION_REQUEST, data));
        SignerProtocolPacket packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet,
            SignerProtocolPacket.COMMAND_DATA_U8500_ENCRYPTION_RESPONSE)) {
            throw new SignerServiceException("Expected command data u8500 encryption response packet");
        }

        return packet.getData();
    }

    /**
     * {@inheritDoc}
     */
    public Object getSubPackageAlias(String pkgName, int exponent, byte[] modulus, String encrRoot) 
            throws SignerServiceException {
        byte[] parentBytes = null;
        try {
            parentBytes = pkgName.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e1) {
            parentBytes = new byte[0];
        }
        ByteBuffer buffer = ByteBuffer.allocate(4 * 3 + parentBytes.length + modulus.length);
        buffer.rewind();
        buffer.putInt(parentBytes.length);
        buffer.put(parentBytes);
        buffer.putInt(exponent);
        buffer.putInt(modulus.length);
        buffer.put(modulus);
        buffer.rewind();
        String pkgAlias = null;
        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
            SignerProtocolPacket.COMMAND_SUB_PACKAGE_BY_KEY_REQUEST, buffer.array()));
        SignerProtocolPacket packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet,
            SignerProtocolPacket.COMMAND_SUB_PACKAGE_BY_KEY_RESPONSE)) {
            throw new SignerServiceException("Expected command sub package by key response packet");
        }
        try {
            pkgAlias = new String(packet.getData(), "UTF-8");
            if(useCommonFolder){
                try{
                    return (Boolean.valueOf(pkgAlias)) ? new GenericSignPackage(
                            null, null, ISignPackage.PACKAGE_TYPE_EMPTY_GENERIC, null) : null; 
                }catch(Exception e){
                    logger.error(e.getMessage());
                }
            }else{
                return pkgAlias;
            }
        } catch (UnsupportedEncodingException e) {
            logger.warn("Failed to read alias from sub package by key response");
        }
        return pkgAlias;
    }

    /**
     * {@inheritDoc}
     */
    public byte[] signU5500(byte[] data, int signatureHashTypeAlgoritm, SignatureType pSignatureType)
        throws SignerServiceException {
        SignerProtocolPacketFactory.createPacket(getCommand(signatureHashTypeAlgoritm,
                pSignatureType), data).toArray();
        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(getCommand(signatureHashTypeAlgoritm,
            pSignatureType), data));
        SignerProtocolPacket packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet,
            SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_RESPONSE)) {
            throw new SignerServiceException("Expected command data u5500 encryption response packet");
        }

        return packet.getData();
    }

    private byte getCommand(int signatureHashTypeAlgoritm, SignatureType pSignatureType) {
        byte toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withRSA;

        if (pSignatureType == SignatureType.RSASSA_PKCS_V1_5) {
            toReturn = getRSACommand(signatureHashTypeAlgoritm);
        } else if (pSignatureType == SignatureType.RSASSA_PPS) {
            toReturn = getPSSCommand(signatureHashTypeAlgoritm);
        }
        return toReturn;
    }

    private byte getPSSCommand(int signatureHashTypeAlgoritm) {
        return SignerServiceHelper.getPSSCommand(signatureHashTypeAlgoritm);
    }

    private byte getRSACommand(int signatureHashTypeAlgoritm) {
        return SignerServiceHelper.getRSACommand(signatureHashTypeAlgoritm);
    }

    /**
     * {@inheritDoc}
     */
    public byte[] signX509(byte[] data) throws SignerServiceException {
        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
            SignerProtocolPacket.COMMAND_DATA_PKCS1_ENCRYPTION_REQUEST, data));
        SignerProtocolPacket packet = packetInput.getPacket(TIMEOUT);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet,
            SignerProtocolPacket.COMMAND_DATA_PKCS1_ENCRYPTION_RESPONSE)) {
            throw new SignerServiceException("Expected command data PKCS1 encryption response packet");
        }

        return packet.getData();
    }

    /**
     * Request to inform the server if we will use the mirrored structure for keys and packages (false) or we 
     * will use one common folder where keys and packages are stored (true)
     * @param useCommonFolder true/false
     */
    private void useCommonFolderRequest(boolean useCommonFolderParam){
        try {
            SignerProtocolPacket packet = SignerProtocolPacketFactory.createPacket(
                    SignerProtocolPacket.COMMAND_USE_COMMON_FOLDER_REQUEST, String.valueOf(useCommonFolderParam).getBytes());

            packetOutput.putPacket(packet);
        } catch (SignerServiceException e) {
            e.printStackTrace();
        }
    }
}
