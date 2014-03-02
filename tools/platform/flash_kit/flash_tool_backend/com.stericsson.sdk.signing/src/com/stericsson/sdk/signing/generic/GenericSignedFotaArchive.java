package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.SignerException;

/**
 * @author xtomzap
 *
 */
public class GenericSignedFotaArchive extends GenericSignedAbstractArchive {

    /** */
    public static final String ARCHIVE_NAME = "update_package.zip";

    /** */
    public static final String SIGNATURE_NAME = "signature.sin";

    /**
     * Constructor
     * 
     * @param pSignerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public GenericSignedFotaArchive(GenericSignerSettings pSignerSettings) throws SignerException {
        super(pSignerSettings, ARCHIVE_NAME, SIGNATURE_NAME);
    }

}
