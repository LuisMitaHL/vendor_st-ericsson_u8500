package com.stericsson.sdk.assembling;

/**
 * The IAssembler interface is the super interface for all assembler implementations.
 * 
 * @author xolabju
 * 
 */
public interface IAssembler {

    /**
     * Use this method to assemble what is defined in the specified signer settings.
     * 
     * @param settings
     *            Assembler settings to use
     * @throws AssemblerException
     *             If assembler related error occurred
     */
    void assemble(IAssemblerSettings settings) throws AssemblerException;

    /**
     * Add a listener that will receive notifications from the assembler object
     * 
     * @param listener
     *            Listener to add
     */
    void addListener(IAssemblerListener listener);

    /**
     * Remove specified listener from the assembler object
     * 
     * @param listener
     *            Listener to remove
     */
    void removeListener(IAssemblerListener listener);
}
