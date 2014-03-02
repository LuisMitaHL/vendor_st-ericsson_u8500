package com.stericsson.sdk.signing.cli.loadmodules;

import com.stericsson.sdk.signing.AbstractSignerSettings;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.util.BasicFileValidator;

/**
 * @author xtomzap
 *
 */
public class LoadModulesSignerSettings extends AbstractSignerSettings {

    /***/
    public static final String KEY_CONFIGURATION = "CONFIGURATION";

    /***/
    public static final String KEY_OUTPUT_FOLDER = "OUTPUT_FOLDER";

    /***/
    public static final String KEY_SIGN = "SIGN";

    /**
     * @param fileList path to file list containing paths to load modules
     * @param outputFolder path to output folder, signed load modules is stored there
     * @throws SignerException exception when validation fails
     */
    public LoadModulesSignerSettings(String fileList, String outputFolder) throws SignerException {
        BasicFileValidator.validateInputFile(fileList);

        addSignerSetting(KEY_CONFIGURATION);
        addSignerSetting(KEY_OUTPUT_FOLDER);
        addSignerSetting(KEY_SIGN);

        setSignerSetting(KEY_INPUT_FILE, fileList);
        setSignerSetting(KEY_OUTPUT_FOLDER, outputFolder);
    }

    /**
     * {@inheritDoc}
     */
    public void setFrom(ISignerSettings pSettings) throws SignerSettingsException {
        // not used
    }
}
