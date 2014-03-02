package com.stericsson.sdk.signing;

import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * 
 * @author xolabju
 * 
 */
public interface ISignerFacade {

    /**
     * 
     * @param settings
     *            the signer settings instance
     * @param listener
     *            the listener object
     * @param local
     *            true to use local sign packages and keys, false to use a sign server
     * @throws SignerException
     *             if anything goes wrong
     */
    void sign(ISignerSettings settings, ISignerListener listener, boolean local) throws SignerException;

    /**
     *@param listener
     *            the listener object
     * @param local
     *            true to use local sign packages and keys, false to use a sign server
     * @param common
     *            common folder where keys and packages are stored
     * @param signerSettings
     *         passing info for overriding env variables.
     * @return the found sign packages
     * @throws SignerException
     *             if anything goes wrong
     */
    ISignPackageInformation[] getSignPackages(ISignerServiceListener listener, boolean local, 
            boolean common, ISignerSettings signerSettings) throws SignerException;

    /**
     * 
     * @param packageName
     *            name
     * @param listener
     *            the listener
     * @param local
     *            true/false
     * @param common
     *            common folder where keys and packages are stored
     * @param signerSettings
     *         passing info for overriding env variables.
     * @return the sign package with the corresponding alias
     * @throws SignerServiceException
     *             on errors
     */
    ISignPackage getSignPackage(String packageName, ISignerServiceListener listener, 
            boolean local, boolean common, ISignerSettings signerSettings)
        throws SignerServiceException;
}
