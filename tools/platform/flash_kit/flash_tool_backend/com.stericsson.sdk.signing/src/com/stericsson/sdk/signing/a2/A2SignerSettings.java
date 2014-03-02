package com.stericsson.sdk.signing.a2;

import com.stericsson.sdk.signing.AbstractSignerSettings;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.cops.ICOPSSignerSettings;
import com.stericsson.sdk.signing.util.BasicFileValidator;

/**
 * @author xtomlju
 */
public class A2SignerSettings extends AbstractSignerSettings implements ICOPSSignerSettings {

    /**
     * @param infile
     *            Input filename
     * @param outfile
     *            path to the file to save in.
     * @throws SignerSettingsException
     *             If a signer settings related error occurred
     */
    public A2SignerSettings(String infile, String outfile) throws SignerSettingsException {
        try {
            BasicFileValidator.validateInputFile(infile);
        } catch (SignerException e) {
            throw new SignerSettingsException(e.getMessage());
        }

        setSignerSetting(KEY_INPUT_FILE, infile);
        setSignerSetting(KEY_OUTPUT_FILE, outfile);

        addSignerSetting(KEY_ACL_SW_TYPE);
        addSignerSetting(KEY_ACL_DEBUG);
        addSignerSetting(KEY_ACL_TARGET_CPU);
        addSignerSetting(KEY_ACL_EXT_LEVEL);
        addSignerSetting(KEY_ACL_ETX_LEVEL_IN_HEADER);
        addSignerSetting(KEY_ACL_INTERACTIVE_LOAD_BIT);
        addSignerSetting(KEY_ACL_APP_SEC);
        addSignerSetting(KEY_ACL_FORMAT);
        addSignerSetting(KEY_ACL_HDR_SECURITY);
        addSignerSetting(KEY_HEADER_DESTINATION_ADDRESS);
        addSignerSetting(KEY_ACL_PTYPE);
        addSignerSetting(KEY_ACL_SHORT_MAC);
        addSignerSetting(KEY_SW_TYPE);
        addSignerSetting(KEY_MAC_MODE);
        addSignerSetting(KEY_SW_VERSION_MAC_MODE);
        addSignerSetting(KEY_SW_VERSION_ANTI_ROLL_REQUIRED);
        addSignerSetting(KEY_SW_VERSION);
        addSignerSetting(KEY_LOADER_COMPRESSION);
        addSignerSetting(KEY_LOADER_ENCRYPTION);
        addSignerSetting(KEY_KEYS_CATALOGUE);
        addSignerSetting(KEY_ENCRYPT_REQUIRED);
        addSignerSetting(KEY_CHIPIDS);
        addSignerSetting(KEY_RANDOM_KEY_VALUE);
        addSignerSetting(COMMON_FOLDER_KEY_AND_PACKAGES);
    }

    /**
     * Set all possible settings from another ISignerSettings instance
     * 
     * @param settings
     *            ISignerSettings instance to read values from
     * @throws SignerSettingsException
     *             If a signer settings related error occurred
     */
    public void setFrom(ISignerSettings settings) throws SignerSettingsException {
        checkAndSet(settings, KEY_ACL_SW_TYPE);
        checkAndSet(settings, KEY_ACL_DEBUG);
        checkAndSet(settings, KEY_ACL_TARGET_CPU);
        checkAndSet(settings, KEY_ACL_EXT_LEVEL);
        checkAndSet(settings, KEY_ACL_ETX_LEVEL_IN_HEADER);
        checkAndSet(settings, KEY_ACL_INTERACTIVE_LOAD_BIT);
        checkAndSet(settings, KEY_ACL_APP_SEC);
        checkAndSet(settings, KEY_ACL_FORMAT);
        checkAndSet(settings, KEY_ACL_HDR_SECURITY);
        checkAndSet(settings, KEY_HEADER_DESTINATION_ADDRESS);
        checkAndSet(settings, KEY_ACL_PTYPE);
        checkAndSet(settings, KEY_ACL_SHORT_MAC);
        checkAndSet(settings, KEY_SW_TYPE);
        checkAndSet(settings, KEY_MAC_MODE);
        checkAndSet(settings, KEY_SW_VERSION_MAC_MODE);
        checkAndSet(settings, KEY_SW_VERSION_ANTI_ROLL_REQUIRED);
        checkAndSet(settings, KEY_SW_VERSION);
        checkAndSet(settings, KEY_LOADER_COMPRESSION);
        checkAndSet(settings, KEY_LOADER_ENCRYPTION);
    }

    private void checkAndSet(ISignerSettings settings, String key) throws SignerSettingsException {
        if (settings.getSignerSetting(key) != null) {
            setSignerSetting(key, settings.getSignerSetting(key));
        }
    }
}
