package com.stericsson.sdk.assembling;

import java.util.LinkedList;
import java.util.List;

/**
 * Abstract assembler class that implements the listener handling part from the IAssembler
 * interface.
 * 
 * @author xolabju
 */
public abstract class AbstractAssembler implements IAssembler {

    private List<IAssemblerListener> assemblerListeners;

    /**
     * Constructor
     */
    public AbstractAssembler() {
        assemblerListeners = new LinkedList<IAssemblerListener>();
    }

    /**
     * {@inheritDoc}
     */
    public void addListener(IAssemblerListener listener) {
        if (!assemblerListeners.contains(listener)) {
            assemblerListeners.add(listener);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void removeListener(IAssemblerListener listener) {
        assemblerListeners.remove(listener);
    }

    /**
     * {@inheritDoc}
     */
    protected void notifyMessage(String message) {
        for (IAssemblerListener listener : assemblerListeners) {
            listener.assemblerMessage(this, message);
        }
    }

}
