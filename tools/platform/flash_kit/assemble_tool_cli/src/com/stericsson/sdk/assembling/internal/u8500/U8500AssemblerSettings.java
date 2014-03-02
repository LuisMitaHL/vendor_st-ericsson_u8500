package com.stericsson.sdk.assembling.internal.u8500;

import com.stericsson.sdk.assembling.AbstractAssemblerSettings;
import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.AssemblerSettingsException;
import com.stericsson.sdk.assembling.internal.BasicFileValidator;

/**
 * 
 * @author xolabju
 * 
 */
public class U8500AssemblerSettings extends AbstractAssemblerSettings implements IU8500AssemblerSettings {

    /**
     * @param configuration
     *            The xml configuration file to use
     * @param fileList
     *            the file list to use
     * @param outfile
     *            path to the output file
     * @throws AssemblerSettingsException
     *             If an assembler settings related error occurred
     * @throws AssemblerException
     *             If an assembler related error occurred
     */
    public U8500AssemblerSettings(String configuration, String fileList, String outfile) throws AssemblerException,
        AssemblerSettingsException {
        BasicFileValidator.validateInputFile(configuration);
        BasicFileValidator.validateInputFile(fileList);
        put(KEY_CONFIGURATION_FILE, configuration);
        put(KEY_FILE_LIST_FILE, fileList);
        put(KEY_OUTPUT_FILE, outfile);
    }
}
