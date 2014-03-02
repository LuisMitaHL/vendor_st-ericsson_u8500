package com.stericsson.sdk.signing.cli.loadmodules;

import static com.stericsson.sdk.ResourcePathManager.getResourceFilePath;
import static org.junit.Assert.fail;

import java.io.File;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.ParseException;
import org.junit.Assert;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.signing.ISigner;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.cli.CommandLineOptionsFactory;
import com.stericsson.sdk.signing.cli.CommandLineOptionsValidator;
import com.stericsson.sdk.signing.cli.ICommandLineProxy;
import com.stericsson.sdk.signing.cli.Sign;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesCommandLineOptions.Validator;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import org.apache.log4j.Logger;

/**
 * @author xtomzap
 * 
 */
public class LoadModulesTest {

    private static final String FILE_LIST_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/loadmodules/filelist.txt");

    private static final String OUTPUT_FOLDER_NAME = ResourcePathManager.getResourceFilePath("test_files/loadmodules/");

    private static final String CONFIG_FILE_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/loadmodules/loadmodules.xml");

    private static final String NON_EXISTING_OUTPUT_FOLDER_NAME = "dummy";

    /***/
    @Test
    public void testSignLoadModules() {
        File fileListFile = new File(getResourceFilePath(FILE_LIST_FILENAME));
        File outputFolder = new File(getResourceFilePath(OUTPUT_FOLDER_NAME));
        File configFile = new File(getResourceFilePath(CONFIG_FILE_FILENAME));

        LoadModulesSignerSettings settings;
        try {
            settings = new LoadModulesSignerSettings(fileListFile.getAbsolutePath(), outputFolder.getAbsolutePath());
            settings.setSignerSetting(LoadModulesSignerSettings.KEY_CONFIGURATION, configFile.getAbsolutePath());

            DummyLoadModulesFacade facade0 = new DummyLoadModulesFacade(0);
            facade0.sign(settings, null, false);
            Assert.assertTrue(true);

            DummyLoadModulesFacade facade1 = new DummyLoadModulesFacade(1);
            facade1.sign(settings, null, false);
            Assert.assertTrue(true);
        } catch (SignerException e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /***/
    @Test
    public void testNonExistingOutputFolder() {
        File fileListFile = new File(getResourceFilePath(FILE_LIST_FILENAME));
        File outputFolder = new File(fileListFile.getParent() + File.separator + NON_EXISTING_OUTPUT_FOLDER_NAME);
        File configFile = new File(getResourceFilePath(CONFIG_FILE_FILENAME));

        LoadModulesSignerSettings settings;
        try {
            settings = new LoadModulesSignerSettings(fileListFile.getAbsolutePath(), outputFolder.getAbsolutePath());
            settings.setSignerSetting(LoadModulesSignerSettings.KEY_CONFIGURATION, configFile.getAbsolutePath());

            DummyLoadModulesFacade facade = new DummyLoadModulesFacade(0);
            facade.sign(settings, null, false);
            Assert.assertTrue(true);
        } catch (SignerException e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } finally {
            if (outputFolder.exists()) {
                outputFolder.delete();
            }
        }

    }

    /***/
    @Test
    public void testFacade() {
        ISignerServiceListener listener = new ISignerServiceListener() {
            public void signerServiceMessage(ISignerService pSignerService, String pMessage) {
            }
        };

        LoadModulesFacade facade = new LoadModulesFacade();
        try {
            facade.getSignPackage("", listener, true, false, (ISignerSettings) null);
        } catch (SignerServiceException e) {
            Assert.assertTrue(false);
        }

        try {
            facade.getSignPackages(listener, true, false, (ISignerSettings) null);
        } catch (SignerException e) {
            Assert.assertTrue(false);
        }

        ISignerListener signerListener = new ISignerListener() {

            public void signerServiceMessage(ISignerService pSignerService, String pMessage) {
            }

            public void signerMessage(ISigner pSigner, String pMessage) {
            }
        };
        ISignerSettings settings = null;
        try {
            facade.sign(settings, signerListener, true);
        } catch (SignerException e) {
            Assert.assertTrue(true);
        }

        signerListener = null;
        settings = new ISignerSettings() {
            public void setSignerSetting(String pKey, Object pValue) throws SignerSettingsException {
            }

            public void setFrom(ISignerSettings pSettings) throws SignerSettingsException {
            }

            public Object getSignerSetting(String pKey) {
                return null;
            }
        };

        try {
            facade.sign(settings, signerListener, true);
        } catch (SignerException e) {
            Assert.assertTrue(true);
        }

        signerListener = new ISignerListener() {

            public void signerServiceMessage(ISignerService pSignerService, String pMessage) {
            }

            public void signerMessage(ISigner pSigner, String pMessage) {
            }
        };

        try {
            facade.sign(settings, signerListener, true);
        } catch (SignerException e) {
            Assert.assertTrue(true);
        }

    }

    /***/
    @Test
    public void testSignEntry() {
        String platform = "u5500";
        LoadModuleSignEntry signEntry = new LoadModuleSignEntry(platform);
        Assert.assertTrue(platform.equals(signEntry.getPlatform()));

        signEntry.setPlatform(platform);
        Assert.assertTrue(platform.equals(signEntry.getPlatform()));

        signEntry.addAttribute("-attribute", "value");
        Assert.assertTrue(signEntry.getArguments().get(2).equals("-attributevalue"));

        signEntry.addAttribute("-attribute", null);
        signEntry.getArguments();

        signEntry.addAttribute("-attribute", "");
        signEntry.getArguments();
    }

    /***/
    @Test
    public void testLoadModulesCommandLineOptions() {
        LoadModulesCommandLineOptions.Validator validator = new Validator();

        try {
            validator.validShortCMD("");
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            validator.validShortCMD(null);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            validator.validLongCMD("");
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            validator.validLongCMD(null);
        } catch (Exception e) {
            Assert.assertTrue(true);
        }

        try {
            validator.validDescription("");
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            validator.validDescription(null);
        } catch (Exception e) {
            Assert.assertTrue(true);
        }

        Assert.assertFalse(LoadModulesCommandLineOptions.Option.HELP.isRequired());
    }

    /**
     */
    @Test
    public void testLoadModulesCommandLineOptionsFactory() {
        try {
            CommandLineOptionsFactory.createCommandLineOptions("");
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }
    }

    /**
     */
    @Test
    public void testLoadModulesCommandLineOptionsValidator() {
        Logger logger = Logger.getLogger(LoadModulesTest.class.getName());
        try {
            CommandLineOptionsValidator.validateArguments(getCommandLineProxy(Sign.SIGN_LOAD_MODULES), logger);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            CommandLineOptionsValidator.validateArguments(getCommandLineProxy(Sign.SIGN_U5500), logger);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            CommandLineOptionsValidator.validateArguments(getCommandLineProxy(Sign.SIGN_U8500), logger);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            CommandLineOptionsValidator.validateArguments(getCommandLineProxy(Sign.SIGN_L9540), logger);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            CommandLineOptionsValidator.validateArguments(getCommandLineProxy(Sign.SIGN_A2), logger);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            CommandLineOptionsValidator.validateArguments(getCommandLineProxy(Sign.SIGN_LOAD_MODULES), null);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }

        try {
            CommandLineOptionsValidator.validateArguments(getCommandLineProxy("fake"), null);
        } catch (ParseException e) {
            Assert.assertTrue(true);
        }
    }

    private ICommandLineProxy getCommandLineProxy(final String pString) {

        return new ICommandLineProxy() {

            public void setProxiedObject(CommandLine pCl) {
            }

            public boolean hasOption(String pShortCMD) {
                return false;
            }

            public String getOptionValue(String pShortCMD) {
                return null;
            }

            public String[] getArgs() {
                return new String[]{pString};
            }
        };
    }
}
