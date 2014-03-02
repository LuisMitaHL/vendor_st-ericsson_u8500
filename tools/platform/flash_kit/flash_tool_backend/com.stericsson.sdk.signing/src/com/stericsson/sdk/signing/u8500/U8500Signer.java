package com.stericsson.sdk.signing.u8500;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSigner;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;

/**
 * @author xtomzap
 *
 */
public class U8500Signer extends GenericSigner {

    /**
     * Constructor
     */
    public U8500Signer() {
        super(Logger.getLogger("U8500Signer"));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IGenericSignablePayload getPayload(GenericSignerSettings signerSettings) throws SignerException {
        return U8500PayloadFactory.getPayload(signerSettings);
    }

}
