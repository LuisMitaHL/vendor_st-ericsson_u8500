package com.stericsson.sdk.signing.l9540;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericPayloadFactory;
import com.stericsson.sdk.signing.generic.GenericSignedPayload;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;

/**
 * @author - Goran Gjorgoski(xxvs0005)  <Goran.Gjorgoski@seavus.com>
 *
 */
public final class L9540PayloadFactory {

    /**
     * @param signerSettings
     *            L9540 signer settings
     * @return Payload interface instance
     * @throws SignerException
     *             If a signer related error occurred
     */
    public static IGenericSignablePayload getPayload(GenericSignerSettings signerSettings) throws SignerException {

        IGenericSignablePayload payload = null;
        GenericSoftwareType swType = (GenericSoftwareType) signerSettings.getSignerSetting(GenericSignerSettings.KEY_SW_TYPE);

        if (swType == GenericSoftwareType.MODEM) {
            payload = new GenericSignedPayload(signerSettings);
        } else {
            payload = GenericPayloadFactory.getPayload(signerSettings);
        }

        return payload;
    }

    private L9540PayloadFactory() {
    }
}
