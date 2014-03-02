package com.stericsson.sdk.signing.u7x00;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSigner;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;

/**
 * @author xtomzap
 * 
 */
public class M7X00Signer extends GenericSigner {

    /**
     * Constructor
     */
    public M7X00Signer() {
        super(Logger.getLogger("M7X00Signer"));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IGenericSignablePayload getPayload(GenericSignerSettings signerSettings) throws SignerException {
        return M7X00PayloadFactory.getPayload(signerSettings);
    }
}
