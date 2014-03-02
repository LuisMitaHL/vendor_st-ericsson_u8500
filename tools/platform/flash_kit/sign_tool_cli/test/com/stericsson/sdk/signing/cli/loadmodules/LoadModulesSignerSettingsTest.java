package com.stericsson.sdk.signing.cli.loadmodules;

import org.junit.Assert;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.signing.SignerException;

/**
 * @author xtomzap
 * 
 */
public class LoadModulesSignerSettingsTest {

    private static final String FILE_LIST_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/loadmodules/filelist.txt");

    private static final String OUTPUT_FOLDER_NAME = ResourcePathManager.getResourceFilePath("test_files/loadmodules/");

    /***/
    @Test
    public void testLoadModulesSignerSettings() {
        try {
            LoadModulesSignerSettings settings = new LoadModulesSignerSettings(FILE_LIST_FILENAME, OUTPUT_FOLDER_NAME);
            settings.setFrom(null);
            Assert.assertTrue(true);
        } catch (SignerException e) {
            Assert.assertTrue(false);
        }
    }
}
