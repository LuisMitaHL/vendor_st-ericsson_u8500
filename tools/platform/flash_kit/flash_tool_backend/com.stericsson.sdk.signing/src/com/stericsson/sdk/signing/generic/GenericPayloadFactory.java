package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;

/**
 * Factory to create payload instances from signer settings.
 * 
 * @author xolabju
 */
public final class GenericPayloadFactory {

    /**
     * @param signerSettings
     *            U8500 signer settings
     * @return Payload interface instance
     * @throws SignerException
     *             If a signer related error occurred
     */
    public static IGenericSignablePayload getPayload(GenericSignerSettings signerSettings) throws SignerException {

        IGenericSignablePayload payload = null;
        GenericSoftwareType swType =
            (GenericSoftwareType) signerSettings.getSignerSetting(GenericSignerSettings.KEY_SW_TYPE);

        if (swType == GenericSoftwareType.ISSW) {
            payload = new GenericSignedISSW(signerSettings);
        } else if (swType == GenericSoftwareType.AUTH_CHALLENGE) {
            String certificateFilename =
                (String) signerSettings
                    .getSignerSetting(IGenericSignerSettings.KEY_TEMP_AUTHENTICATION_CERTIFICATE_FILENAME);
            String challengeFilename = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE);
            String payloadFilename = null;

            try {
                File tempPayload = File.createTempFile("payload" + System.currentTimeMillis(), ".bin");
                tempPayload.deleteOnExit();
                payloadFilename = tempPayload.getAbsolutePath();
                concatenateFiles(certificateFilename, challengeFilename, payloadFilename);
            } catch (Exception e) {
                throw new SignerException(e.getMessage());
            }

            // Delete temporary files and replace original input file
            new File(certificateFilename).delete();
            signerSettings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE, payloadFilename);

            payload = new GenericSignedPayload(signerSettings);

        } else if (swType == GenericSoftwareType.TRUSTED || swType == GenericSoftwareType.USTA) {
            payload = new GenericSignedTrustedApplication(signerSettings);
        } else if (swType == GenericSoftwareType.FLASH_ARCHIVE) {
            payload = new GenericSignedFlashArchive(signerSettings);
        } else if (swType == GenericSoftwareType.AUTH_CERT) {
            payload = new GenericSignedAuthenticationCertificate(signerSettings);
        } else if (swType == GenericSoftwareType.DNT || swType == GenericSoftwareType.FRAC) {
            payload = new GenericSignedDebugAndTestCertificate(signerSettings);
        } else if (swType == GenericSoftwareType.CRKC) {
            payload = new GenericSignedCustomerRootKeyCertificate(signerSettings);
        } else {
            payload = getPayload2(signerSettings, swType);
        }

        return payload;
    }

    private static IGenericSignablePayload getPayload2(GenericSignerSettings signerSettings, GenericSoftwareType swType)
        throws SignerException {
        IGenericSignablePayload payload = null;

        if (swType == GenericSoftwareType.ELF || swType == GenericSoftwareType.MODEM) {
            payload = new GenericSignedLoadModule(signerSettings);
        } else if (swType == GenericSoftwareType.FOTA) {
            payload = new GenericSignedFotaArchive(signerSettings);
        } else {
            payload = new GenericSignedPayload(signerSettings);
        }
        return payload;
    }

    private GenericPayloadFactory() {

    }

    private static void concatenateFiles(String firstFilename, String secondFilename, String resultFilename)
        throws Exception {

        FileInputStream fileInput = null;
        FileOutputStream fileOutput = null;
        File file;
        byte[] fileBuffer;

        try {
            // Open up the result file
            fileOutput = new FileOutputStream(resultFilename);

            // Open, read and output first file
            file = new File(firstFilename);
            fileInput = new FileInputStream(file);
            fileBuffer = new byte[(int) file.length()];
            int read = fileInput.read(fileBuffer);
            if (read == -1) {
                throw new Exception("Unexpectedly reached EOF");
            }
            fileOutput.write(fileBuffer);
            fileInput.close();

            // Open, read and output second file
            file = new File(secondFilename);
            fileInput = new FileInputStream(file);
            fileBuffer = new byte[(int) file.length()];
            read = fileInput.read(fileBuffer);
            if (read == -1) {
                throw new Exception("Unexpectedly reached EOF");
            }
            fileOutput.write(fileBuffer);
            fileInput.close();

        } finally {
            // Close any open files

            if (fileInput != null) {
                fileInput.close();
            }

            if (fileOutput != null) {
                fileOutput.close();
            }
        }
    }

}
