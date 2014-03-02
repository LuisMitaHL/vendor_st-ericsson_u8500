package com.stericsson.sdk.signing;

import java.util.LinkedList;
import java.util.List;

/**
 * Abstract signer class that implements the listener handling part from the ISigner interface.
 * 
 * @author xtomlju
 */
public abstract class AbstractSigner implements ISigner {

    private List<ISignerListener> signerListeners;

    /**
     * Constructor
     */
    public AbstractSigner() {
        signerListeners = new LinkedList<ISignerListener>();
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISigner#addListener(com.stericsson.sdk.signing.ISignerListener)
     */
    /**
     * Add a listener that will receive notifications from the signer object
     * 
     * @param listener
     *            Listener to add
     */
    public void addListener(ISignerListener listener) {
        if (!signerListeners.contains(listener)) {
            signerListeners.add(listener);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISigner#removeListener(com.stericsson.sdk.signing.ISignerListener)
     */
    /**
     * Remove specified listener from the signer object
     * 
     * @param listener
     *            Listener to remove
     */
    public void removeListener(ISignerListener listener) {
        signerListeners.remove(listener);
    }

    /**
     * Sends a message to all registered listeners.
     * 
     * @param message
     *            Message to send
     */
    protected void notifyMessage(String message) {
        for (ISignerListener listener : signerListeners) {
            listener.signerMessage(this, message);
        }
    }
}
