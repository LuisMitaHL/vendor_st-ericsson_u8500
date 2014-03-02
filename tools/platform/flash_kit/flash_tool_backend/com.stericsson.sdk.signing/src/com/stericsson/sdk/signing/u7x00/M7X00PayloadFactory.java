package com.stericsson.sdk.signing.u7x00;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericPayloadFactory;
import com.stericsson.sdk.signing.generic.GenericSignedPayload;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;

/**
 * @author tsikor01
 * 
 */
public final class M7X00PayloadFactory {

    /**
     * @param signerSettings
     *            M7X00 signer settings
     * @return Payload interface instance
     * @throws SignerException
     *             If a signer related error occurred
     */
    public static IGenericSignablePayload getPayload(GenericSignerSettings signerSettings) throws SignerException {

        IGenericSignablePayload payload = null;
        GenericSoftwareType swType =
            (GenericSoftwareType) signerSettings.getSignerSetting(GenericSignerSettings.KEY_SW_TYPE);

        if (swType == GenericSoftwareType.OSLOADER || swType == GenericSoftwareType.XLOADER
            || swType == GenericSoftwareType.LDR) {
            payload = new M7X00ElfPayload(signerSettings);
        } else if (swType == GenericSoftwareType.FOTA) {
            payload = new GenericSignedPayload(signerSettings);
        } else {
            payload = GenericPayloadFactory.getPayload(signerSettings);
        }

        return payload;
    }

    private M7X00PayloadFactory() {
    }
}
