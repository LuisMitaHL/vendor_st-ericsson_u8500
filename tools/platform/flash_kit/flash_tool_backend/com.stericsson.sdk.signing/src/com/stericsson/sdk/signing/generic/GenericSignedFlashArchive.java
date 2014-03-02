package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.SignerException;

/**
 * U5500 flash archive
 * 
 * @author xolabju
 * 
 */
public class GenericSignedFlashArchive extends GenericSignedAbstractArchive {

    /** */
    public static final String ARCHIVE_NAME = "contents.zip";

    /** */
    public static final String SIGNATURE_NAME = "signature.sf";

    /**
     * Constructor
     * 
     * @param pSignerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public GenericSignedFlashArchive(GenericSignerSettings pSignerSettings) throws SignerException {
        super(pSignerSettings, ARCHIVE_NAME, SIGNATURE_NAME);
    }

}
