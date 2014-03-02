package com.stericsson.sdk.signing.signerservice;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.LinkedList;
import java.util.Random;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.signerservice.local.SignPackageManager;
import com.stericsson.sdk.signing.signerservice.local.encryption.EncryptionAlgorithm;
import com.stericsson.sdk.signing.signerservice.local.encryption.RSAPKCS1Encrypter;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketFactory;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketInput;
import com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketOutput;

/**
 * Helper class to reduce coupling
 * 
 * @author xtomlju
 */
public final class SignerServiceHelper {

    private static final String PATH_REGEX_PATTERN = "^\\\\{2}(.)+\\\\{1}(.)*";

    private static String translatePath(String path, String signPackagesRoot) throws SignerServiceException {

        if (path == null) {
            throw new SignerServiceException("Path is null. ");
        }

        if (signPackagesRoot == null) {
            signPackagesRoot = System.getenv(ISignerService.ENV_SIGN_PACKAGE_ROOT);
        }
        if (signPackagesRoot == null) {
            signPackagesRoot = System.getProperty(ISignerService.ENV_SIGN_PACKAGE_ROOT);
        }

        if ((signPackagesRoot != null) && path.matches(PATH_REGEX_PATTERN)) {
            if (!signPackagesRoot.trim().equals("")) {
                int p = path.indexOf('\\', 2);
                p = path.indexOf('\\', p + 1);
                path = (signPackagesRoot + path.substring(p)).replace('\\', File.separatorChar);
            }
        } else if (signPackagesRoot != null && !signPackagesRoot.trim().equals("")) {
            throw new SignerServiceException("Given sign package root path '" + path
                + "' doesn't match expected pattern: " + PATH_REGEX_PATTERN);
        }

        return path;
    }

    /**
     * Returns the authentication secret on the given path.
     * 
     * @param logger
     *            Logger instance
     * @param path
     *            The path to read the secret from.
     * @param signPackagesRoot
     *            sign packages root.
     * @return The secret.
     * @throws SignerServiceException
     *             If secret could not be found or read.
     */
    public static byte[] getSecret(Logger logger, String path, String signPackagesRoot) throws SignerServiceException {

        // Network latency might cause client to ask for secret file before it is available,
        // make several attempts to get secret file if this happens

        path = translatePath(path, signPackagesRoot);
        File f = new File(path);

        int attempts = 0;
        while ((!f.exists() || !f.isFile()) && attempts++ < 50) {
            try {
                logger.warn("Secret file " + path + " not found, retry (attempt " + attempts + ")");
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        // End of network latency code

        if (!f.exists() || !f.isFile()) {
            throw new SignerServiceException("Could not read secret file " + path);
        }

        logger.info("Found secret file " + path);

        int fileLength = (int) f.length();
        byte[] secret = new byte[fileLength];

        FileInputStream fis = null;
        try {
            fis = new FileInputStream(f);

            int i = fis.read(secret);
            if (i != fileLength) {
                throw new SignerServiceException("Failed reading the whole secret");
            }
        } catch (IOException e) {
            throw new SignerServiceException("Could not read secret");
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }

        return secret;
    }

    /**
     * @param chipid
     *            Data to sign.
     * @param key
     *            The session key to encrypt.
     * @throws Exception
     *             if something goes wrong.
     * @return data to encrypt
     */
    static byte[] prepareSessionKeyEncryption(int chipid, int[] key) throws Exception {
        byte[] temp1 = new byte[16];

        for (int i = 0; i < 4; ++i) {
            byte[] temp2 = new byte[4];
            EncryptionAlgorithm.fromIntToBytes(key[i], temp2);
            for (int j = 0; j < 4; ++j) {
                temp1[i * 4 + j] = temp2[j];
            }
        }

        byte[] toSend = new byte[4 + key.length * 4];
        byte[] chipIDAsArray = new byte[4];

        chipIDAsArray = EncryptionAlgorithm.fromIntToBytes(chipid, chipIDAsArray);

        for (int i = 0; i < toSend.length; ++i) {
            if (i < 4) {
                toSend[i] = chipIDAsArray[i];
            } else {
                toSend[i] = temp1[i - 4];
            }
        }

        return toSend;
    }

    /**
     * Creates the necessary authentication values for setting up the session.
     * @return 
     */
    static Random makeRandomNumber() {
        return new Random(System.currentTimeMillis());
    }

    /**
     * @param data raw data
     * @param currentSignPackage current sign package
     * @param useCommonFolder useCommonFolder
     * @return signature
     */
    public static byte[] calculateRSAPKCS1Signature(byte[] data, String currentSignPackage, boolean useCommonFolder) {
        return new RSAPKCS1Encrypter().calculateRSAPKCS1Signature(data, SignPackageManager.getInstance(useCommonFolder).getKey(
            currentSignPackage));
    }


    private SignerServiceHelper() {

    }

    /**
     * @param signatureHashTypeAlgoritm signatureHashTypeAlgoritm
     * @return bytes
     */
    protected static byte getRSACommand(int signatureHashTypeAlgoritm) {
        byte toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withRSA;

        if (signatureHashTypeAlgoritm == HashType.SHA1_HASH.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA1withRSA;
        } else if (signatureHashTypeAlgoritm == HashType.SHA256_HASH.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withRSA;
        } else if (signatureHashTypeAlgoritm == HashType.SHA384_HASH.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA384withRSA;
        } else if (signatureHashTypeAlgoritm == HashType.SHA512_HASH.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA512withRSA;
        }
        return toReturn;
    }

    /**
     * @param signatureHashTypeAlgoritm signatureHashTypeAlgoritm
     * @return bytes
     */
    protected static byte getPSSCommand(int signatureHashTypeAlgoritm) {
        byte toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withRSA;

        if (signatureHashTypeAlgoritm == HashType.SHA1_HASH.getHashType()
            || signatureHashTypeAlgoritm == HashType.SHA1_HMAC.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA1withPSS;
        } else if (signatureHashTypeAlgoritm == HashType.SHA256_HASH.getHashType()
            || signatureHashTypeAlgoritm == HashType.SHA256_HMAC.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withPSS;
        } else if (signatureHashTypeAlgoritm == HashType.SHA384_HASH.getHashType()
            || signatureHashTypeAlgoritm == HashType.SHA384_HMAC.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA384withPSS;
        } else if (signatureHashTypeAlgoritm == HashType.SHA512_HASH.getHashType()
            || signatureHashTypeAlgoritm == HashType.SHA512_HMAC.getHashType()) {
            toReturn = SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA512withPSS;
        }
        return toReturn;
    }

    /**
     * Signs specified data and returns the signature
     * 
     * @param data Data to be signed
     * @param packetOutput packetOutput
     * @param packetInput packetInput
     * @param timeout timeout
     * @return Signature of signed data
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    public static byte[] signA2(SignerProtocolPacketOutput packetOutput, SignerProtocolPacketInput packetInput, 
            byte[] data, long timeout) throws SignerServiceException {

        packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
            SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_REQUEST, data));
        SignerProtocolPacket packet = packetInput.getPacket(timeout);
        if (!SignerProtocolPacketFactory.isPackageCommand(packet,
            SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_RESPONSE)) {
            throw new SignerServiceException("Expected command data a2 encryption response packet");
        }

        return packet.getData();
    }

    /**
     * @param data data
     * @param includeSubPackages includeSubPackages
     * @throws SignerServiceException SignerServiceException
     * @return list of ISignPackageInformation
     */
    protected static LinkedList<ISignPackageInformation> getSignPackageInformation(byte[] data, 
            boolean includeSubPackages) throws SignerServiceException{
        MESPConfiguration configuration = null;
        LinkedList<ISignPackageInformation> signPackageInformation = new LinkedList<ISignPackageInformation>();

        try {
            configuration = MESPConfiguration.read(data);
            MESPConfigurationRecord[] signPackageRecords = configuration.getRecords(new String[] {
                ISignPackage.MESP_NAME});
            for (MESPConfigurationRecord record : signPackageRecords) {
                SignPackageInformation pkgInfo = new SignPackageInformation(record);
                if (pkgInfo.getParent() == null || (pkgInfo.getParent() != null && includeSubPackages)) {
                    signPackageInformation.add(pkgInfo);
                }
            }
        } catch (Exception e) {
            throw new SignerServiceException(e.getMessage());
        }

        return signPackageInformation;
    }
}
