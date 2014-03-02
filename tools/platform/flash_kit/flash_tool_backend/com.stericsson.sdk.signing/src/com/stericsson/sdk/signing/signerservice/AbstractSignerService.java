package com.stericsson.sdk.signing.signerservice;

import java.util.LinkedList;
import java.util.List;

/**
 * Abstract signer service class that implements the listener handling parts of the ISignerService
 * interface.
 * 
 * @author xtomlju
 */
public abstract class AbstractSignerService implements ISignerService {

    private List<ISignerServiceListener> signerServiceListeners;

    /**
     * Constructor
     */
    public AbstractSignerService() {
        signerServiceListeners = new LinkedList<ISignerServiceListener>();
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerService#addListener(com.stericsson.sdk.signing.ISignerServiceListener)
     */
    /**
     * Add a listener that receives notifications from the signer service
     * 
     * @param listener
     *            Listener to add
     */
    public void addListener(ISignerServiceListener listener) {
        if (!signerServiceListeners.contains(listener)) {
            signerServiceListeners.add(listener);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerService#removeListener(com.stericsson.sdk.signing.ISignerServiceListener)
     */
    /**
     * Remove specified listener from signer service object
     * 
     * @param listener
     *            Listener to remove
     */
    public void removeListener(ISignerServiceListener listener) {
        signerServiceListeners.remove(listener);
    }

    /**
     * Sends a message to all registered listeners.
     * 
     * @param message
     *            Message to send
     */
    protected void notifyMessage(String message) {
        for (ISignerServiceListener listener : signerServiceListeners) {
            listener.signerServiceMessage(this, message);
        }
    }
}
