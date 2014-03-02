package com.stericsson.sdk.signing.a2;

import java.io.ByteArrayOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.HashMap;
import java.util.Locale;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.AbstractSigner;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cops.COPSCertificateList;
import com.stericsson.sdk.signing.cops.COPSControlFields;
import com.stericsson.sdk.signing.cops.COPSHashList;
import com.stericsson.sdk.signing.cops.COPSPadding;
import com.stericsson.sdk.signing.cops.COPSSignature;
import com.stericsson.sdk.signing.cops.COPSSignedData;
import com.stericsson.sdk.signing.cops.ICOPSPayload;
import com.stericsson.sdk.signing.cops.ICOPSPayloadWithHashList;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * A2 implementation of the abstract signer class and signer interface
 * 
 * @author xtomlju
 */
public class A2Signer extends AbstractSigner {

    /** */
    public static final String INPUT_FILE_TYPE_TAR = "tar";

    /** */
    public static final String INPUT_FILE_TYPE_ELF = "elf";

    /** */
    public static final String INPUT_FILE_TYPE_BIN = "bin";

    /** */
    public static final String INPUT_FILE_TYPE_S_RECORDS = "s-records";

    private static final String EXTENSION_SIGNED_SW = ".ssw";

    private static final String EXTENSION_SIGNED_TAR = ".ssw";

    private static final String EXTENSION_SIGNED_LOADER = ".ldr";

    private static final String EXTENSION_SIGNED_UNKNOWN = ".signed";

    private static final String EXTENSION_A01 = ".a01";

    private static final String EXTENSION_BIN = ".bin";

    private static final String EXTENSION_ELF = ".elf";

    private static final String EXTENSION_TAR = ".tar";

    private static final int DEFAULT_HASH_LIST_TYPE = 0;

    private static final int DEFAULT_HASH_BLOCK_SIZE = 0x10000;

    private Logger logger;

    private static HashMap<String, String> inputFileExtensionMap;

    private static HashMap<Integer, String> outputFileExtensionMap;

    static {
        // unsigned file extensions
        inputFileExtensionMap = new HashMap<String, String>();
        inputFileExtensionMap.put(EXTENSION_A01, INPUT_FILE_TYPE_S_RECORDS);
        inputFileExtensionMap.put(EXTENSION_BIN, INPUT_FILE_TYPE_BIN);
        inputFileExtensionMap.put(EXTENSION_ELF, INPUT_FILE_TYPE_ELF);
        inputFileExtensionMap.put(EXTENSION_TAR, INPUT_FILE_TYPE_TAR);

        // signed file extensions
        outputFileExtensionMap = new HashMap<Integer, String>();
        outputFileExtensionMap.put(IA2PayloadTypeConstants.TYPE_LOADER, EXTENSION_SIGNED_LOADER);
        outputFileExtensionMap.put(IA2PayloadTypeConstants.TYPE_SW_IMAGE, EXTENSION_SIGNED_SW);
        outputFileExtensionMap.put(IA2PayloadTypeConstants.TYPE_TAR, EXTENSION_SIGNED_TAR);
    }

    /**
     * Constructor
     */
    public A2Signer() {
        logger = Logger.getLogger(getClass().getName());
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISigner#sign(com.stericsson.sdk.signing.ISignPackage,
     * com.stericsson.sdk.signing.ISignerSettings, com.stericsson.sdk.signing.ISignerService)
     */
    /**
     * @param settings
     *            Signer settings to use
     * @throws SignerException
     *             If a signer related error occurred
     */
    public void sign(ISignerSettings settings) throws SignerException {

        A2SignerSettings signerSettings = (A2SignerSettings) settings;

        validateInput(signerSettings);

        createOutput(signerSettings);

        StringBuffer buffer = new StringBuffer();
        buffer.append("completed successfully");
//        buffer.append((String) settings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
//        buffer.append("' signed successfully as '");
//        buffer.append((String) settings.getSignerSetting(ISignerSettings.KEY_OUTPUT_FILE));
//        buffer.append("'");

        notifyMessage(buffer.toString());
    }

    private void validateInput(A2SignerSettings signerSettings) throws SignerException {

        String inputFilename = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE);

        // Look at the file extension and if it is not recognized log warning
        // but throw exception if
        // no overriding payload type found in signer settings

        boolean recognizedExtension = false;

        // Make input filename lower case for further processing

        String lowerCaseInputFilename = inputFilename.toLowerCase(Locale.getDefault());

        if (isRecognizedExtension(lowerCaseInputFilename)) {

            if (signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE_TYPE) == null) {
                // Only set input payload type if not already specified in the
                // signer settings
                signerSettings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE_TYPE,
                    getInputTypeFromExtension(lowerCaseInputFilename));
            }

            recognizedExtension = true;
        }

        if (!recognizedExtension) {
            if (signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE_TYPE) == null) {
                throw new SignerException(
                    "File extension for input file not recognized and no input file format specified");
            } else {
                logger
                    .warn("Unrecognized file extension for input file. Will use input file format from signer settings");
            }
        }

        notifyMessage("Input file '" + inputFilename + "' validated");
    }

    private boolean isRecognizedExtension(String lowerCaseInputFilename) {
        return inputFileExtensionMap.keySet().contains(getFileExtension(lowerCaseInputFilename));
    }

    private String getOutputFilename(A2SignerSettings signerSettings) throws SignerException {
        String outputFilename = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_OUTPUT_FILE);

        if (outputFilename == null) {

            String inputFilename = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE);
            int p = inputFilename.lastIndexOf('.');
            if (p != -1) {
                outputFilename = inputFilename.substring(0, p);
                outputFilename =
                    outputFilename
                        + getSuffixFromSoftwareType((Integer) signerSettings
                            .getSignerSetting(A2SignerSettings.KEY_SW_TYPE));
            } else {
                outputFilename =
                    inputFilename
                        + getSuffixFromSoftwareType((Integer) signerSettings
                            .getSignerSetting(A2SignerSettings.KEY_SW_TYPE));
            }

            logger.info("No output filename in settings. Will use " + outputFilename + " as output filename");

            signerSettings.setSignerSetting(ISignerSettings.KEY_OUTPUT_FILE, outputFilename);
        }

        return outputFilename;
    }

    private void createOutput(A2SignerSettings signerSettings) throws SignerException {

        String outputFilename = getOutputFilename(signerSettings);
        FileChannel channel = null;
        FileOutputStream fis = null;

        try {
            fis = new FileOutputStream(outputFilename);
            channel = fis.getChannel();

            COPSControlFields controlFields = createControlFields(signerSettings);
            COPSCertificateList certificateList = getCertificateList(signerSettings);

            COPSHashList hashList = new COPSHashList();

            notifyMessage("Fetching payload...");
            ICOPSPayload payload = A2PayloadFactory.getPayload(signerSettings, controlFields);
            if (payload == null) {
                throw new SignerException("Failed to fetch payload.");
            }
            notifyMessage("Payload fetched");

            if (payload instanceof ICOPSPayloadWithHashList) {
                notifyMessage("Fetching payload hash list");
                ((ICOPSPayloadWithHashList) payload).populateHashList(hashList, getHashAlgorithm(certificateList
                    .getProtectionType(controlFields.getHeaderID())));
                controlFields.setHashListType(DEFAULT_HASH_LIST_TYPE);
                controlFields.setHashListBlockSize(DEFAULT_HASH_BLOCK_SIZE);
                controlFields.setHashListNumberOfBlocks(hashList.getNumberOfHashValues());
            }

            // Calculate header length

            int headerLength =
                controlFields.getLength() + certificateList.getLength() + controlFields.getSignatureLength()
                    + hashList.getLength();

            controlFields.setHeaderLength(headerLength);
            controlFields.setPayloadLength(payload.getLength());
            controlFields.setPayloadDestinationAddress(payload.getDestinationAddress());

            payload.updateCOPSControlFields(controlFields);

            COPSPadding padding = new COPSPadding(controlFields);

            // Do the signing

            notifyMessage("Signing...");

            COPSSignature signature =
                createSignature(signerSettings, controlFields, certificateList, hashList, padding, payload);

            COPSSignedData signedData =
                new COPSSignedData(controlFields, certificateList, signature, hashList, padding);

            notifyMessage("Signing done");

            // Write the COPS header
            signedData.write(channel);

            // Write payload
            notifyMessage("Writing payload to ouput file...");
            payload.write(channel);
        } catch (IOException e) {
            throw new SignerException(e);
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }
    }

    private COPSCertificateList getCertificateList(A2SignerSettings signerSettings) {

        A2SignPackage signPackage = (A2SignPackage) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE);

        return signPackage.getCertificateList();
    }

    private COPSControlFields createControlFields(A2SignerSettings signerSettings) throws SignerException {

        A2SignPackage signPackage = (A2SignPackage) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE);
        COPSCertificateList certificateList = signPackage.getCertificateList();

        // TODO: TEST
        int testSignPackageVersion = signPackage.getVersion();

        int version = COPSControlFields.HID_BABE0200;
        if (testSignPackageVersion == 0xBABE0300) {
            version = COPSControlFields.HID_BABE0300;
        }

        // For now make the assumption that HID_BABE0200 is 'A2'. In the future
        // the HID perhaps
        // should be based on some other property like 'software type'.

        COPSControlFields controlFields = new COPSControlFields(version);

        // Values from signer settings
        controlFields.setA2Acl(signerSettings);
        controlFields.setA2SoftwareVersion(signerSettings);
        controlFields.setA2Mode(signerSettings);
        controlFields.setA2PLType(signerSettings);
        controlFields.setA2HeaderDestinationAddress(signerSettings);

        // Values from the sign package
        controlFields.setChipClass(signPackage.getChipClass());

        // Values from the certificate
        controlFields.setProtectionType(certificateList.getProtectionType(version));
        byte[] keyUsage = certificateList.getKeyUsage();
        if (keyUsage == null) {
            throw new SignerException("Failed to get key usage.");
        }
        controlFields.setKeyUsage(keyUsage);
        controlFields.setNumberOfCertificates(certificateList.getNumberOfCertificates());
        controlFields.setCertificateListLength(certificateList.getLength());
        controlFields.setCustomerID(certificateList.getSubjectCID());

        return controlFields;
    }

    private int getHashAlgorithm(int protectionType) throws SignerException {

        int hashAlgorithm = 0;

        switch (protectionType) {
            case COPSControlFields.PROT_TYPE_RSA_1024_SIG_WITH_SHA_1:
            case COPSControlFields.PROT_TYPE_RSA_2048_SIG_WITH_SHA_1:
                hashAlgorithm = DigestDecorator.SHA_1;
                break;
            case COPSControlFields.PROT_TYPE_RSA_1024_SIG_WITH_SHA_256:
            case COPSControlFields.PROT_TYPE_RSA_2048_SIG_WITH_SHA_256:
                hashAlgorithm = DigestDecorator.SHA_256;
                break;
            default:
                throw new SignerException("Unknown hash algorithm");
        }

        return hashAlgorithm;
    }

    private COPSSignature createSignature(A2SignerSettings signerSettings, COPSControlFields controlFields,
        COPSCertificateList certificateList, COPSHashList hashList, COPSPadding padding, ICOPSPayload payload)
        throws SignerException {

        COPSSignature copsSignature = new COPSSignature(controlFields);
        ISignerService signerService =
            (ISignerService) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGNER_SERVICE);

        // Find out what hash digest algorithm to use

        int hashAlgorithm = getHashAlgorithm(certificateList.getProtectionType(controlFields.getHeaderID()));
        ByteArrayOutputStream hashOutput = new ByteArrayOutputStream();

        DigestDecorator digester;

        try {
            digester = new DigestDecorator(hashAlgorithm);
            digester.update(controlFields.getBytes());
            digester.update(certificateList.getBytes());

            hashOutput.write(digester.digest());
            hashOutput.write(payload.getHashValue(hashAlgorithm));

            // Only add hash list hash if payload comes with a hash list

            if (payload instanceof ICOPSPayloadWithHashList) {
                digester = new DigestDecorator(hashAlgorithm);
                digester.update(hashList.getBytes());
                hashOutput.write(digester.digest());
            }

            byte[] signatureFromSignerService = signerService.signA2(hashOutput.toByteArray());
            if (signatureFromSignerService == null) {
                throw new SignerException("Failed to sign A2 software. Signature is null.");
            }
            byte[] signature = new byte[controlFields.getSignatureLength()];

            int paddingCount = controlFields.getSignatureLength() - signatureFromSignerService.length;
            if (paddingCount < 0) {
                throw new SignerException("Signature recieved from signer service too long");
            } else if (paddingCount > 0) {
                logger.info("Padding signature from signer service with " + paddingCount + " bytes");
            }

            System.arraycopy(signatureFromSignerService, 0, signature, paddingCount, signatureFromSignerService.length);

            copsSignature.setBytes(signature);
        } catch (IOException e) {
            throw new SignerException(e);
        }

        return copsSignature;
    }

    private String getInputTypeFromExtension(String inputFilename) {
        return inputFileExtensionMap.get(getFileExtension(inputFilename));
    }

    private String getSuffixFromSoftwareType(int softwareType) {

        String result = outputFileExtensionMap.get(softwareType);

        if (result == null || result.equals("")) {
            result = EXTENSION_SIGNED_UNKNOWN;
        }

        return result;
    }

    private String getFileExtension(String fileName) {

        String extension = "";

        if (fileName != null && !fileName.trim().equals("")) {
            int dotPos = fileName.lastIndexOf(".");
            if (dotPos != -1) {
                extension = fileName.substring(dotPos, fileName.length());
            }
        }

        return extension.toLowerCase(Locale.getDefault());
    }
}
