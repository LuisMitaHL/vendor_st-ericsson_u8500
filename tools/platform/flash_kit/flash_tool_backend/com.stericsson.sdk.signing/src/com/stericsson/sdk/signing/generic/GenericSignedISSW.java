package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.SignerException;

/**
 * @author xtomlju
 */
public class GenericSignedISSW extends GenericSignedPayload {

    /**
     * @param signerSettings
     *            TBD
     * @throws SignerException
     *             TBD
     */
    public GenericSignedISSW(GenericSignerSettings signerSettings) throws SignerException {
        super(signerSettings);
    }

}
