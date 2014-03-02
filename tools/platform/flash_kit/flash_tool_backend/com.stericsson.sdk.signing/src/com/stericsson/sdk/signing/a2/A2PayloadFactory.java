package com.stericsson.sdk.signing.a2;

import java.io.IOException;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cops.ArchivePayload;
import com.stericsson.sdk.signing.cops.COPSControlFields;
import com.stericsson.sdk.signing.cops.ELFSoftwareImagePayload;
import com.stericsson.sdk.signing.cops.ICOPSPayload;
import com.stericsson.sdk.signing.cops.LoaderPayload;
import com.stericsson.sdk.signing.cops.SoftwareImagePayload;

/**
 * Factory to create payload instances from signer settings.
 * 
 * @author xtomlju
 */
public final class A2PayloadFactory {

    /**
     * @param signerSettings
     *            A2 signer settings
     * @param controlFields
     *            Cops control fields
     * @return Payload interface instance
     * @throws SignerException
     *             If a signer related error occurred
     */
    public static ICOPSPayload getPayload(A2SignerSettings signerSettings, COPSControlFields controlFields)
        throws SignerException {

        ICOPSPayload payload = null;

        try {
            int payloadType = (Integer) signerSettings.getSignerSetting(A2SignerSettings.KEY_SW_TYPE);

            switch (payloadType) {
                case IA2PayloadTypeConstants.TYPE_LOADER:
                    payload = new LoaderPayload(signerSettings, controlFields);
                    break;
                case IA2PayloadTypeConstants.TYPE_TAR:
                case IA2PayloadTypeConstants.TYPE_ARCHIVE:
                    payload = new ArchivePayload(signerSettings);
                    break;
                case IA2PayloadTypeConstants.TYPE_SW_IMAGE:

                    String inputFileType =
                        (String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE_TYPE);

                    if (A2Signer.INPUT_FILE_TYPE_ELF.equalsIgnoreCase(inputFileType)) {
                        payload = new ELFSoftwareImagePayload(signerSettings);
                    } else if (inputFileType.equalsIgnoreCase(A2Signer.INPUT_FILE_TYPE_S_RECORDS)) {
                        payload = new SoftwareImagePayload(signerSettings);
                    } else {
                        throw new SignerException(
                            "The sofware image payload type can not be generated from the input file type "
                                + inputFileType);
                    }
                    break;
                case IA2PayloadTypeConstants.TYPE_ELF:
                    payload = new ELFSoftwareImagePayload(signerSettings);
                    break;
                case IA2PayloadTypeConstants.TYPE_GENERIC:
                    payload = new ArchivePayload(signerSettings);
                    break;
                default:
                    break;
            }

        } catch (IOException e) {
            throw new SignerException(e);
        }

        return payload;
    }

    private A2PayloadFactory() {

    }
}
