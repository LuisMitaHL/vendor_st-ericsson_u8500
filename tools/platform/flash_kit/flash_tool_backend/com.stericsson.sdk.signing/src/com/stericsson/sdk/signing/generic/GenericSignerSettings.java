package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.AbstractSignerSettings;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.util.BasicFileValidator;

/**
 * 
 * @author xolabju
 * 
 */
public abstract class GenericSignerSettings extends AbstractSignerSettings implements IGenericSignerSettings {

    /**
     * @param infile
     *            Input filename
     * @param outfile
     *            Path to the file to save in.
     * @throws SignerException
     *            If a signer settings related error occurred
     */
    public GenericSignerSettings(String infile, String outfile) throws SignerException {
        BasicFileValidator.validateInputFile(infile);

        setSignerSetting(KEY_INPUT_FILE, infile);
        setSignerSetting(KEY_OUTPUT_FILE, outfile);
        addSignerSetting(KEY_SIGN_PACKAGE_TYPE);
        addSignerSetting(KEY_CUT_ID);
        addSignerSetting(KEY_HASH_BLOCK_SIZE);
        addSignerSetting(KEY_SW_TYPE);
        addSignerSetting(KEY_LOAD_ADDRESS);
        addSignerSetting(KEY_START_ADDRESS);
        addSignerSetting(KEY_UNCHECKED_BLOB);
        addSignerSetting(KEY_PKA_FLAG);
        addSignerSetting(KEY_DMA_FLAG);
        addSignerSetting(KEY_SIGNED_HEADER_SIGNATURE_SIZE);
        addSignerSetting(KEY_TEMP_AUTHENTICATION_CERTIFICATE_FILENAME);
        addSignerSetting(KEY_ROOT_KEY_HASH_TYPE);
        addSignerSetting(KEY_PAYLOAD_HASH_TYPE);
        addSignerSetting(KEY_SIGNATURE_HASH_TYPE);
        addSignerSetting(KEY_BUFFER_SIZE);
        addSignerSetting(KEY_LOAD_ADDRESS_XML);
        addSignerSetting(KEY_HASH_FILE);
        addSignerSetting(KEY_SIGNATURE_TYPE);
        addSignerSetting(KEY_SW_VERSION);
        addSignerSetting(KEY_MAJOR_BUILD_VERSION);
        addSignerSetting(KEY_MINOR_BUILD_VERSION);
        addSignerSetting(KEY_FLAGS);
        addSignerSetting(COMMON_FOLDER_KEY_AND_PACKAGES);
        addSignerSetting(KEY_SIGN_KEY);

        addSignerSetting(KEY_SIGN_SERVICE_SERVERS);
        addSignerSetting(KEY_SIGN_PACKAGE_ROOT);
        addSignerSetting(KEY_LOCAL_SIGN_PACKAGE_ROOT);
        addSignerSetting(KEY_LOCAL_KEY_ROOT);
        addSignerSetting(KEY_LOCAL_ENCRYPTION_KEY_ROOT);

        setSignerSetting(KEY_ROOT_KEY_HASH_TYPE, HashType.SHA256_HASH);
        setSignerSetting(KEY_PAYLOAD_HASH_TYPE, HashType.SHA256_HASH);
        setSignerSetting(KEY_SIGNATURE_HASH_TYPE, HashType.SHA256_HASH);
        setSignerSetting(KEY_MAJOR_BUILD_VERSION, Short.valueOf((short) 0));
        setSignerSetting(KEY_MINOR_BUILD_VERSION, Short.valueOf((short) 0));
        setSignerSetting(KEY_FLAGS, Integer.valueOf(0));
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
        checkAndSet(settings, KEY_SIGN_PACKAGE_TYPE);
        checkAndSet(settings, KEY_CUT_ID);
        checkAndSet(settings, KEY_SW_TYPE);
    }

    private void checkAndSet(ISignerSettings settings, String key) throws SignerSettingsException {
        if (settings.getSignerSetting(key) != null) {
            setSignerSetting(key, settings.getSignerSetting(key));
        }
    }
}
