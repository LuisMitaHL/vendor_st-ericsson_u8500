package com.stericsson.sdk.signing.cli;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import org.apache.commons.cli.ParseException;
import org.junit.Assert;
import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptions;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesCommandLineOptions;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesSignerSettings;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.u8500.U8500SignerSettings;

/**
 * 
 * @author mielcluk
 * 
 */
public class CommandLineSignerSettingsSetupTest {

    private static final String UNSIGNED_LOADER_FILENAME = "test_files/dummy.bin";

    private static final String OUTPUT_FILE = "test_files/signedsoftware.ssw_out";

    private static final String FILE_LIST_FILENAME = "test_files/loadmodules/filelist.txt";

    private static final String OUTPUT_FOLDER_NAME = "test_files/loadmodules/";

    private static final String CONFIG_FILE_FILENAME = "test_files/loadmodules/loadmodules.xml";

    CommandLineProxyMock commandLineProxyA2 = new CommandLineProxyMock();

    CommandLineProxyMock commandLineProxyU8500 = new CommandLineProxyMock();

    CommandLineProxyMock commandLineProxyU5500 = new CommandLineProxyMock();

    CommandLineProxyMock commandLineProxySignLoadModules = new CommandLineProxyMock();

    // A2 tests
    /**
     */
    @Test
    public void setupA2SettingsTestWithoutTarget() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.TARGET.getShortCMD());
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
        } catch (SignerException e) {
            assertEquals("You must specify a target CPU", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupA2SettingsTestInvalidTarget() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.TARGET.getShortCMD(), "INVALID");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Target CPU 'INVALID' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedTargetCPU(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestInvalidSoftwareType() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_TYPE.getShortCMD(), "INVALID");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Software type 'INVALID' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedSoftwareTypes(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutInteractive() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.INTERACTIVE.getLongCMD());
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("You must specify a interactive load bit", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithInvalidInteractive() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.INTERACTIVE.getLongCMD(), "INVALID");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Interactive Load Bit 'INVALID' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedInteractiveLoadBits(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutAppSec() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.APP_SEC.getLongCMD());
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("You must specify a application security", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithInvalidAppSec() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.APP_SEC.getLongCMD(), "INVALID");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Application security type 'INVALID' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedApplicationSecurity(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutAddrFormat() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.ADDR_FORMAT.getLongCMD());
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("You must specify a address format", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithInvalidAddrFormat() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.ADDR_FORMAT.getLongCMD(), "INVALID");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Address format 'INVALID' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedAddressFormat(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutHdrSec() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.HDR_SEC.getLongCMD());

            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("You must specify a header security", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithInvalidPType() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.PTYPE.getLongCMD(), "INVALID");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Payload type 'INVALID' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedPayloadTypes(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutMacMode() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.MAC_MODE.getShortCMD());
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("You must specify a mac mode", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithInvalidMacMode() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.MAC_MODE.getShortCMD(), "INVALID");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Mac mode 'INVALID' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedMacMode(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutSwVersion() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.SW_VERSION.getLongCMD());
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("You must specify a software version", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutSwType() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.SW_TYPE.getShortCMD());
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("You must specify a software type", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithoutPType() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.removeOption(A2CommandLineOptions.Option.PTYPE.getLongCMD());
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_VERSION.getLongCMD(), "1");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
        } catch (SignerException e) {
            assertEquals("You must specify a software type", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithBadSwVersion() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Software version '0.1' is not supported. Supported values: [decimal number: 0..255]", e
                .getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithBadSwVersion2() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_VERSION.getLongCMD(), "-1");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Software version '-1' is not supported. Supported values: [decimal number: 0..255]", e
                .getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithBadSwVersion3() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_VERSION.getLongCMD(), "256");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
            fail("Should throw SignerException");
        } catch (SignerException e) {
            assertEquals("Software version '256' is not supported. Supported values: [decimal number: 0..255]", e
                .getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithSwTypeGeneric() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_VERSION.getLongCMD(), "255");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_TYPE.getShortCMD(), "generic");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
        } catch (SignerException e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    /**
     */
    @Test
    public void setupA2SettingsTestWithSwTypeArchive() {
        A2SignerSettings settings = null;
        initA2Parameters();
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_VERSION.getLongCMD(), "255");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_TYPE.getShortCMD(), "archive");
            settings = new A2SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyA2, settings);
        } catch (SignerException e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    private void initA2Parameters() {
        try {
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.TARGET.getShortCMD(), "app");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_TYPE.getShortCMD(), "image");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.INTERACTIVE.getLongCMD(), "static");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.APP_SEC.getLongCMD(), "on");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.ADDR_FORMAT.getLongCMD(), "pages");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.HDR_SEC.getLongCMD(), "asIs=0x123456");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.PTYPE.getLongCMD(), "nand");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.MAC_MODE.getShortCMD(), "Config");
            commandLineProxyA2.setOptionValue(A2CommandLineOptions.Option.SW_VERSION.getLongCMD(), "0.1");

        } catch (ParseException e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    // U5500
    /**
     */
    @Test
    public void setupU5500SettingsTestWithoutSwType() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500.removeOption(GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD());
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            assertEquals("You must specify software type: -s, --sw-type", e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU5500SettingsTestWithInvalidSwType() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD(),
                "INVALID");
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            assertEquals("Software type 'INVALID' is not supported. Supported values: "
                + GenericCommandLineOptions.getSupportedSoftwareTypes(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU5500SettingsTestWithLoadAddress() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU5500SettingsTestWithEmptyHashBlockSize() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.HASH_BLOCK_SIZE.getShortCMD(),
                "");
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU5500SettingsTestWithNullBufferSize() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500
                .setOptionValue(GenericCommandLineOptions.GenericOption.BUFFER_SIZE.getLongCMD(), null);
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU5500SettingsTestRootKeyHashTypeSHA1() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.ROOT_KEY_HASH_TYPE
                .getShortCMD(), "SHA-1");
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupU5500SettingsTestRootKeyHashTypeSHA256() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.ROOT_KEY_HASH_TYPE
                .getShortCMD(), "SHA-256");
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupU5500SettingsTestRootKeyHashTypeSHA384() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.ROOT_KEY_HASH_TYPE
                .getShortCMD(), "SHA-384");
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupU5500SettingsTestRootKeyHashTypeSHA512() {
        ISignerSettings settings = null;
        initU5500Parameters();
        try {
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.ROOT_KEY_HASH_TYPE
                .getShortCMD(), "SHA-512");
            settings = new U5500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU5500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupU8500SettingsTestWithNullBufferSize() {
        ISignerSettings settings = null;
        initU8500Parameters();
        try {
            commandLineProxyU8500
                .setOptionValue(GenericCommandLineOptions.GenericOption.BUFFER_SIZE.getLongCMD(), null);
            settings = new U8500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU8500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU8500SettingsTestWithInvalidSwType() {
        ISignerSettings settings = null;
        initU8500Parameters();
        try {
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD(),
                "INVALID");
            settings = new U8500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU8500, settings);
        } catch (SignerException e) {
            assertEquals("Software type 'INVALID' is not supported. Supported values: "
                + GenericCommandLineOptions.getSupportedSoftwareTypes(), e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU8500SettingsTestWithEmptyHashBlockSize() {
        ISignerSettings settings = null;
        initU8500Parameters();
        try {
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.HASH_BLOCK_SIZE.getShortCMD(),
                "");
            settings = new U8500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU8500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }

    }

    /**
     */
    @Test
    public void setupU8500SettingsTestWithSwTypePwrMgt() {
        ISignerSettings settings = null;
        initU8500Parameters();
        try {
            commandLineProxyU8500
                .setOptionValue(GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD(), "prcmu");
            settings = new U8500SignerSettings(UNSIGNED_LOADER_FILENAME, OUTPUT_FILE);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxyU8500, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupSignLoadModulesSettingsTest() {
        LoadModulesSignerSettings settings = null;
        initLoadModulesParameters();
        try {
            settings = new LoadModulesSignerSettings(FILE_LIST_FILENAME, OUTPUT_FOLDER_NAME);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxySignLoadModules, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /**
     */
    @Test
    public void setupSignLoadModulesSettingsTestWithoutConfigFile() {
        LoadModulesSignerSettings settings = null;
        initLoadModulesParameters();
        try {
            commandLineProxySignLoadModules.setOptionValue(LoadModulesCommandLineOptions.Option.CONFIGURATION
                .getShortCMD(), null);
            settings = new LoadModulesSignerSettings(FILE_LIST_FILENAME, OUTPUT_FOLDER_NAME);
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxySignLoadModules, settings);
        } catch (SignerException e) {
            Assert.assertTrue(true);
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    /***/
    @Test
    public void setupNonSignLoadModulesSettingsTest() {
        LoadModulesSignerSettings settings = null;
        initLoadModulesParameters();
        try {
            settings = new LoadModulesSignerSettings(FILE_LIST_FILENAME, OUTPUT_FOLDER_NAME) {

                public void setSignerSetting(String pKey, Object pValue) throws SignerSettingsException {
                }

                public void setFrom(ISignerSettings pSettings) throws SignerSettingsException {
                }

                public Object getSignerSetting(String pKey) {
                    return null;
                }
            };
            new CommandLineSignerSettingsSetup().setupSettings(commandLineProxySignLoadModules, settings);
        } catch (SignerException e) {
            fail("Bad SignerException exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        } catch (Exception e) {
            fail("Bad exception:" + e.getClass().getSimpleName() + " " + e.getMessage());
        }
    }

    private void initU8500Parameters() {
        try {
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD(), "issw");
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.LOAD_ADDRESS.getShortCMD(),
                "0x123456");
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.START_ADDRESS.getShortCMD(),
                "0x123456");
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.HASH_BLOCK_SIZE.getShortCMD(),
                "1");
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.ENABLE_DMA.getLongCMD(), null);
            commandLineProxyU8500.setOptionValue(GenericCommandLineOptions.GenericOption.ENABLE_PKA.getLongCMD(), null);
        } catch (ParseException e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    private void initLoadModulesParameters() {
        try {
            commandLineProxySignLoadModules.setOptionValue(LoadModulesCommandLineOptions.Option.CONFIGURATION
                .getShortCMD(), "config_file_path");
            commandLineProxySignLoadModules.setOptionValue(LoadModulesCommandLineOptions.Option.CONFIGURATION
                .getShortCMD(), CONFIG_FILE_FILENAME);
        } catch (ParseException e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }

    private void initU5500Parameters() {
        try {
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD(), "issw");
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.LOAD_ADDRESS.getShortCMD(),
                "0x123456");
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.START_ADDRESS.getShortCMD(),
                "0x123456");
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.HASH_BLOCK_SIZE.getShortCMD(),
                "1");
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.ENABLE_DMA.getLongCMD(), null);
            commandLineProxyU5500.setOptionValue(GenericCommandLineOptions.GenericOption.ENABLE_PKA.getLongCMD(), null);
        } catch (ParseException e) {
            fail("Bad exception:" + e.getClass().getSimpleName());
        }
    }
}
