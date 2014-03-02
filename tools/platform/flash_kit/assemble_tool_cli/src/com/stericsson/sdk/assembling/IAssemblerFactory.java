package com.stericsson.sdk.assembling;

import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.assembling.cli.ICommandLineProxy;

/**
 * 
 * @author xolabju
 */
public interface IAssemblerFactory {

    /**
     * Create an assembler object
     * 
     * @param settings
     *            Assembler settings
     * @return An IAssembler interface
     */
    IAssembler createAssembler(IAssemblerSettings settings);

    /**
     * Create an assembler settings object
     * 
     * @param outFile
     *            The output file path/name
     * @param cmdLineProxy
     *            settings that was set by the commandline.
     * 
     * @return An IAssemblerSettings interface
     * 
     * @throws AssemblerSettingsException
     *             If an assembler settings related error occurred
     * @throws AssemblerException
     *             If an assembler related error occurred
     * @throws ParseException
     *             parseException
     */
    IAssemblerSettings createAssemblerSettings(String outFile, ICommandLineProxy cmdLineProxy)
        throws AssemblerSettingsException, AssemblerException, ParseException;

}
