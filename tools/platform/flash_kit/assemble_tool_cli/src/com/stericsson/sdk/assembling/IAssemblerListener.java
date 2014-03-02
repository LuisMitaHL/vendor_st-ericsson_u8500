package com.stericsson.sdk.assembling;

/**
 * The IAssemblerListener interface can be implemented by another class to receive notifications
 * from the assembler.
 * 
 * @author xolabju
 * 
 */
public interface IAssemblerListener {

    /**
     * Assemblers call this method to notify the listener about a message
     * 
     * @param assembler
     *            Assembler object that called this method
     * @param message
     *            Message from signer
     */
    void assemblerMessage(IAssembler assembler, String message);
}
