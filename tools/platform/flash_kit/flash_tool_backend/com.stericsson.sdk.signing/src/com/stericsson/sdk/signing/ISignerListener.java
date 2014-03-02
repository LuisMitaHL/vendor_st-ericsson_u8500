package com.stericsson.sdk.signing;

import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;

/**
 * The ISignerListener interface can be implemented by another class to receive notifications from
 * the signer.
 * 
 * @author xtomlju
 */
public interface ISignerListener extends ISignerServiceListener{

    /**
     * Signer call this method to notify the listener about a message
     * 
     * @param signer
     *            Signer object that called this method
     * @param message
     *            Message from signer
     */
    void signerMessage(ISigner signer, String message);
}
