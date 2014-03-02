package com.stericsson.sdk.signing.l9540;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSigner;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;

/**
 * @author - Goran Gjorgoski(xxvs0005)  <Goran.Gjorgoski@seavus.com>
 *
 */
public class L9540Signer extends GenericSigner {

    /**
     * Constructor
     */
    public L9540Signer() {
        super(Logger.getLogger("L9540Signer"));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IGenericSignablePayload getPayload(GenericSignerSettings signerSettings) throws SignerException {
        return L9540PayloadFactory.getPayload(signerSettings);
    }

}
