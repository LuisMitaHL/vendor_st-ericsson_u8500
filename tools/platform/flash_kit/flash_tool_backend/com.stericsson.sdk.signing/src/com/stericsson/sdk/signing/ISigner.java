package com.stericsson.sdk.signing;


/**
 * The ISigner interface is the super interface for all signer implementations.
 * 
 * @author xtomlju
 */
public interface ISigner {

    /**
     * Use this method to sign what is defined in the specified signer settings.
     * 
     * @param settings
     *            Signer settings to use
     * @throws SignerException
     *             If signer related error occurred
     */
    void sign(ISignerSettings settings) throws SignerException;

    /**
     * Add a listener that will receive notifications from the signer object
     * 
     * @param listener
     *            Listener to add
     */
    void addListener(ISignerListener listener);

    /**
     * Remove specified listener from the signer object
     * 
     * @param listener
     *            Listener to remove
     */
    void removeListener(ISignerListener listener);
}
