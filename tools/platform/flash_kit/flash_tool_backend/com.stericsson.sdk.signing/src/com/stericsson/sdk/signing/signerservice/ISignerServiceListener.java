package com.stericsson.sdk.signing.signerservice;

/**
 * The ISignerServiceListener interface can be implemented by another class to receive notifications
 * from the signer service.
 * 
 * @author xtomlju
 */
public interface ISignerServiceListener {

    /**
     * Signer service calls this method to notify listener about a message
     * 
     * @param signerService
     *            Signer service object that called this method
     * @param message
     *            Message from signer service
     */
    void signerServiceMessage(ISignerService signerService, String message);
}
