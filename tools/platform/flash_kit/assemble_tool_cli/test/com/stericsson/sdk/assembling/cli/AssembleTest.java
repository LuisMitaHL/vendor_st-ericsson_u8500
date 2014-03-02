package com.stericsson.sdk.assembling.cli;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.logging.Logger;

import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.AssemblerFactory;
import com.stericsson.sdk.assembling.AssemblerSettingsException;
import com.stericsson.sdk.assembling.IAssembler;
import com.stericsson.sdk.assembling.IAssemblerFactory;
import com.stericsson.sdk.assembling.IAssemblerSettings;
import com.stericsson.sdk.assembling.internal.u8500.U8500AssemblerSettings;

/**
 * 
 * @author xolabju
 * 
 */
public class AssembleTest {

    private static final Logger log = Logger.getLogger("com.stericsson.sdk.assembling");

    /** */
    public static final String RESOURCE_DIR = ResourcePathManager.getResourceFilePath("res/");

    /** */
    public static final String IMAGE_ADL = ResourcePathManager.getResourceFilePath("res/adl.bin");

    /** */
    public static final String BOOT_IMAGE = ResourcePathManager.getResourceFilePath("res/boot_image.bin");

    /** */
    public static final String IMAGE_IPL = ResourcePathManager.getResourceFilePath("res/ipl.bin");

    /** */
    public static final String IMAGE_MEM_INIT = ResourcePathManager.getResourceFilePath("res/mem_init.bin");

    /** */
    public static final String IMAGE_MODEM = ResourcePathManager.getResourceFilePath("res/mod.bin");

    /** */
    public static final String IMAGE_NORMAL = ResourcePathManager.getResourceFilePath("res/normal.bin");

    /** */
    public static final String IMAGE_PWR_MGT = ResourcePathManager.getResourceFilePath("res/power_management.bin");

    /** */
    public static final String IMAGE_PRODUCTION = ResourcePathManager.getResourceFilePath("res/production.bin");

    /** */
    public static final String EMPTY_FILE = ResourcePathManager.getResourceFilePath("res/empty.bin");

    /** */
    public static final String OUTPUT_IMAGE_FILE_NAME = "test_image.bin";

    /** */
    public static final String OUTPUT_FLASH_ARCHIVE_FILE_NAME = "test_archive.zip";

    /** */
    public static final String OUTPUT_PREFLASH_FILE_NAME = "test_preflash.bin";

    /** */
    public static final String CONFIG = ResourcePathManager.getResourceFilePath("res/config.xml");

    /** */
    public static final String CONFIG_INVALID = ResourcePathManager.getResourceFilePath("res/invalid_config.xml");

    private static final String FILE_LIST_LOCAL = ResourcePathManager.getResourceFilePath("res/files_local.txt");

    private static final String FILE_LIST_LOCAL_2 = ResourcePathManager.getResourceFilePath("res/files_local_2.txt");

    private static final String INVALID_FILE_LIST_1 =
        ResourcePathManager.getResourceFilePath("res/invalid_files_1.txt");

    private static final String INVALID_FILE_LIST_SERVER_1 =
        ResourcePathManager.getResourceFilePath("res/invalid_files_server_1.txt");

    private static final String FILE_LIST_BUILD_SERVER =
        ResourcePathManager.getResourceFilePath("res/files_build_server.txt");

    private static final String FILE_LIST_BUILD_SERVER_2 =
        ResourcePathManager.getResourceFilePath("res/files_build_server_2.txt");

    private static final String PREFLASH_CONFIG_LOCAL =
        ResourcePathManager.getResourceFilePath("res/config_preflash.xml");

    private static final String PREFLASH_CONFIG_SERVER =
        ResourcePathManager.getResourceFilePath("res/config_preflash_build_server.xml");

    private static final String PREFLASH_FILE_LIST_LOCAL =
        ResourcePathManager.getResourceFilePath("res/files_preflashimage_local.txt");

    private static final String PREFLASH_FILE_LIST_SERVER =
        ResourcePathManager.getResourceFilePath("res/files_preflashimage_build_server.txt");

    private static final String PREFLASH_ARCHIVE_FILE_LIST_LOCAL =
        ResourcePathManager.getResourceFilePath("res/files_preflasharchive_local.txt");

    private static final String PREFLASH_ARCHIVE_FILE_LIST_SERVER =
        ResourcePathManager.getResourceFilePath("res/files_preflasharchive_build_server.txt");

    private static final String PREFLASH_ARCHIVE_CONFIG =
        ResourcePathManager.getResourceFilePath("res/config_preflash_archive.xml");

    private static final String PREFLASH_CONFIG_LOCAL_INVALID =
        ResourcePathManager.getResourceFilePath("res/config_preflash_invalid.xml");

    private static final String PREFLASH_CONFIG_SERVER_INVALID =
        ResourcePathManager.getResourceFilePath("res/config_preflash_build_server_invalid.xml");

    private static final String PREFLASH_CONFIG_LOCAL_INVALID_2 =
        ResourcePathManager.getResourceFilePath("res/config_preflash_invalid_2.xml");

    private static final String PREFLASH_CONFIG_SERVER_INVALID_2 =
        ResourcePathManager.getResourceFilePath("res/config_preflash_build_server_invalid_2.xml");

    private static boolean loggerConfigured = false;

    /**
     * @return the loggerConfigured
     */
    private static boolean isLoggerConfigured() {
        return loggerConfigured;
    }

    /**
     * @param value
     *            the loggerConfigured to set
     */
    private static void setLoggerConfigured(boolean value) {
        AssembleTest.loggerConfigured = value;
    }

    /**
     * 
     * @return the file list to use
     */
    public static String getFileList() {
        if (new File("res/files_local.txt").exists()) {
            return FILE_LIST_LOCAL;
        } else {
            return FILE_LIST_BUILD_SERVER;
        }
    }

    /**
     * 
     * @return the file list to use
     */
    public static String getBinaryImageFileList() {
        if (new File("res/files_local.txt").exists()) {
            return FILE_LIST_LOCAL_2;
        } else {
            return FILE_LIST_BUILD_SERVER_2;
        }
    }

    /**
     * 
     * @return the config to use
     */
    public static String getPreflashConfig() {
        if (new File("res/files_local.txt").exists()) {
            return PREFLASH_CONFIG_LOCAL;
        } else {
            return PREFLASH_CONFIG_SERVER;
        }
    }

    /**
     * 
     * @return the config to use
     */
    public static String getPreflashConfigInvalid() {
        if (new File("res/files_local.txt").exists()) {
            return PREFLASH_CONFIG_LOCAL_INVALID;
        } else {
            return PREFLASH_CONFIG_SERVER_INVALID;
        }
    }

    /**
     * 
     * @return the config to use
     */
    public static String getPreflashConfigInvalid2() {
        if (new File("res/files_local.txt").exists()) {
            return PREFLASH_CONFIG_LOCAL_INVALID_2;
        } else {
            return PREFLASH_CONFIG_SERVER_INVALID_2;
        }
    }

    /**
     * 
     * @return the file list to use
     */
    public static String getPreflashFileList() {
        if (new File("res/files_local.txt").exists()) {
            return PREFLASH_FILE_LIST_LOCAL;
        } else {
            return PREFLASH_FILE_LIST_SERVER;
        }
    }

    /**
     * 
     * @return the file list to use
     */
    public static String getPreflashArchiveFileList() {
        if (new File("res/files_local.txt").exists()) {
            return PREFLASH_ARCHIVE_FILE_LIST_LOCAL;
        } else {
            return PREFLASH_ARCHIVE_FILE_LIST_SERVER;
        }
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Before
    public void setUp() throws Exception {
        if (!isLoggerConfigured()) {
            CommandLineLogging.setupLogger();
            setLoggerConfigured(true);
        }
    }

    /**
     * 
     */
    @Test
    public void testAssembleBinaryImage() {
        String output = RESOURCE_DIR + OUTPUT_IMAGE_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        try {
            Assemble assemble = new Assemble(null, null);
            String[] args = new String[] {
                "u8500", "-v", "--buffer-size", "8k", "-t", "ram_image", "-c", CONFIG, "-l", getBinaryImageFileList(), output};
            int result = assemble.execute(new AssemblerFactory(), args, log);
            assertEquals(0, result);

            file = new File(output);
            assertTrue(file.exists());
            assertTrue(file.length() > 0);

            // test overwriting without --force flag
            try {
                result = assemble.execute(new AssemblerFactory(), args, log);
                assertFalse(result == 0);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }

            // test with dir as output file
            args =
                new String[] {
                    "u8500", "--buffer-size", "8192", "-f", "-t", "ram_image", "-c", CONFIG, "-l", getBinaryImageFileList(),
                    RESOURCE_DIR};
            try {
                result = assemble.execute(new AssemblerFactory(), args, log);
                assertFalse(result == 0);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }

            // test with dir as input file
            args = new String[] {
                "u8500", "-f", "-v", "-t", "ram_image", "-c", RESOURCE_DIR, "-l", getBinaryImageFileList(), output};
            try {
                result = assemble.execute(new AssemblerFactory(), args, log);
                assertFalse(result == 0);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }

            // test with empty file input
            args = new String[] {
                "u8500", "-f", "-v", "-t", "ram_image", "-c", EMPTY_FILE, "-l", getBinaryImageFileList(), output};
            try {
                result = assemble.execute(new AssemblerFactory(), args, log);
                assertFalse(result == 0);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }

            // test with non existing file input
            args = new String[] {
                "u8500", "-f", "-v", "-t", "ram_image", "-c", "idontexist.testfile", "-l", getBinaryImageFileList(), output};
            try {
                result = assemble.execute(new AssemblerFactory(), args, log);
                assertFalse(result == 0);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testAssembleFlashArchive() {
        String output = RESOURCE_DIR + OUTPUT_FLASH_ARCHIVE_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        try {
            Assemble assemble = new Assemble(null, null);
            String[] args =
                new String[] {
                    "u8500", "--buffer-size", "1M", "--alignment-size", "1024", "-t", "flash_archive", "-c", CONFIG,
                    "-l", getFileList(), output};
            int result = assemble.execute(new AssemblerFactory(), args, log);
            assertEquals(0, result);

            file = new File(output);
            assertTrue(file.exists());
            assertTrue(file.length() > 0);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testAssembleFlashArchive1() {
        String output = RESOURCE_DIR + OUTPUT_FLASH_ARCHIVE_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }
        try {
            Assemble assemble = new Assemble(null, null);
            String[] args =
                new String[] {
                    "u8500", "-r", "--buffer-size", "1M", "--alignment-size", "1024", "-t", "flash_archive", "-c",
                    CONFIG, "-l", getFileList(), output};
            int result = assemble.execute(new AssemblerFactory(), args, log);
            assertEquals(0, result);

            file = new File(output);
            assertTrue(file.exists());
            assertTrue(file.length() > 0);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     *
     */
    @Test
    public void testAssemblePreflashImage() {
        String output = RESOURCE_DIR + OUTPUT_PREFLASH_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        try {
            Assemble assemble = new Assemble(null, null);
            String[] args = new String[] {
                "u8500", "-t", "preflash_image", "-c", getPreflashConfig(), "-l", getPreflashFileList(), output};
            int result = assemble.execute(new AssemblerFactory(), args, log);
            assertEquals(0, result);

            file = new File(output);
            assertTrue(file.exists());
            assertTrue(file.length() > 0);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
    *
    */
    @Test
    public void testAssemblePreflashImageWithInvalidConfig() {
        String output = RESOURCE_DIR + OUTPUT_PREFLASH_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        try {
            Assemble assemble = new Assemble(null, null);
            String[] args = new String[] {
                "u8500", "-t", "preflash_image", "-c", getPreflashConfigInvalid(), "-l", getPreflashFileList(), output};
            int result = assemble.execute(new AssemblerFactory(), args, log);

            fail("Should not get here (result is " + result + ").");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
    *
    */
    @Test
    public void testAssemblePreflashImageWithInvalidConfig2() {
        String output = RESOURCE_DIR + OUTPUT_PREFLASH_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        try {
            Assemble assemble = new Assemble(null, null);
            String[] args = new String[] {
                "u8500", "-t", "preflash_image", "-c", getPreflashConfigInvalid2(), "-l", getPreflashFileList(), output};
            int result = assemble.execute(new AssemblerFactory(), args, log);

            fail("Should not get here (result is " + result + ").");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testAssemblePreflashArchive() {
        String output = RESOURCE_DIR + OUTPUT_PREFLASH_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        try {
            Assemble assemble = new Assemble(null, null);
            String[] args =
                new String[] {
                    "u8500", "-t", "flash_archive", "-c", PREFLASH_ARCHIVE_CONFIG, "-l", getPreflashArchiveFileList(),
                    output};
            int result = assemble.execute(new AssemblerFactory(), args, log);
            assertEquals(0, result);

            file = new File(output);
            assertTrue(file.exists());
            assertTrue(file.length() > 0);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidFileList1() {
        String output = RESOURCE_DIR + OUTPUT_FLASH_ARCHIVE_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        try {
            String fileList = new File("res/config.xml").exists() ? INVALID_FILE_LIST_1 : INVALID_FILE_LIST_SERVER_1;

            Assemble assemble = new Assemble(null, null);
            String[] args = new String[] {
                "u8500", "-t", "flash_archive", "-c", CONFIG, "-l", fileList, output};
            try {
                int result = assemble.execute(new AssemblerFactory(), args, log);
                assertNotSame(0, result);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }

            file = new File(output);
            assertFalse(file.exists());
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidConfig() {
        String output = RESOURCE_DIR + OUTPUT_IMAGE_FILE_NAME;
        File file = new File(output);
        if (file.exists()) {
            file.delete();
        }

        Assemble assemble = new Assemble(null, null);
        String[] args = new String[] {
            "u8500", "-t", "ram_image", "-c", CONFIG_INVALID, "-l", getFileList(), output};
        try {
            int result = assemble.execute(new AssemblerFactory(), args, log);
            assertFalse(result == 0);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testPrintHelp() {
        try {
            ByteArrayOutputStream outStream = new ByteArrayOutputStream();
            Assemble assemble;
            try {
                assemble = new Assemble(null, new PrintStream(outStream, false, "UTF-8"));
            } catch (UnsupportedEncodingException e1) {
                fail(e1.getMessage());
                return;
            }
            String[] args = new String[] {
                "u8500", "-h"};
            int result = assemble.execute(new AssemblerFactory(), args, log);
            assertTrue(result == 0);
            try {
                outStream.close();
            } catch (IOException e) {
                e.getMessage();
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidSubCommand() {
        try {
            ByteArrayOutputStream outStream = new ByteArrayOutputStream();
            Assemble assemble = null;
            try {
                assemble = new Assemble(null, new PrintStream(outStream, false, "UTF-8"));
            } catch (UnsupportedEncodingException e1) {
                fail(e1.getMessage());
                return;
            }
            String[] args = new String[] {
                "wrong_platform"};
            try {
                int result = assemble.execute(new AssemblerFactory(), args, log);
                assertFalse(result == 0);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
            try {
                outStream.close();
            } catch (IOException e) {
                e.getMessage();
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testMissingArguments() {
        ByteArrayOutputStream outStream = new ByteArrayOutputStream();
        Assemble assemble = null;

        ICommandLineProxy commandLine = new CommandLineProxy();
        Options commandLineOptions;
        CommandLineParser argumentParser = new PosixParser();
        String[] arguments = new String[1];
        arguments[0] = Assemble.PLATFORM_U8500;

        try {
            assemble = new Assemble(null, new PrintStream(outStream, false, "UTF-8"));
            commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(arguments[0]);
            commandLine.setProxiedObject(argumentParser.parse(commandLineOptions, arguments));
            assemble.missingArguments(commandLine, log);
            fail("Parse exception should be thrown: 'Missing one or more required parameters.'");
        } catch (UnsupportedEncodingException e1) {
            fail(e1.getMessage());
            return;
        } catch (ParseException e) {
            assertEquals("Missing one or more required parameters.", e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testNullSettings() {
        try {
            Assemble assemble = new Assemble(null, null);
            IAssemblerFactory fact = new IAssemblerFactory() {

                public IAssemblerSettings createAssemblerSettings(String outFile, ICommandLineProxy cmdLineProxy)
                    throws AssemblerSettingsException, AssemblerException, ParseException {
                    return null;
                }

                public IAssembler createAssembler(IAssemblerSettings settings) {
                    return null;
                }
            };
            try {
                int execute =
                    assemble.execute(fact, new String[] {
                        "u8500", "-t", "flash_archive", "-c", CONFIG, "-l", getFileList(),
                        OUTPUT_FLASH_ARCHIVE_FILE_NAME}, log);
                assertEquals(1, execute);
                System.out.println("Should not get here.");
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testNullAssembler() {
        try {
            Assemble assemble = new Assemble(null, null);
            IAssemblerFactory fact = new IAssemblerFactory() {

                public IAssemblerSettings createAssemblerSettings(String outFile, ICommandLineProxy cmdLineProxy)
                    throws AssemblerSettingsException, AssemblerException, ParseException {
                    return new U8500AssemblerSettings(CONFIG, getFileList(), OUTPUT_FLASH_ARCHIVE_FILE_NAME);
                }

                public IAssembler createAssembler(IAssemblerSettings settings) {
                    return null;
                }
            };
            try {
                int execute =
                    assemble.execute(fact, new String[] {
                        "u8500", "-t", "flash_archive", "-c", CONFIG, "-l", getFileList(),
                        OUTPUT_FLASH_ARCHIVE_FILE_NAME}, log);
                assertEquals(1, execute);
                System.out.println("Should not get here.");
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testNoOutput() {
        try {
            Assemble assemble = new Assemble(null, null);
            IAssemblerFactory fact = new IAssemblerFactory() {

                public IAssemblerSettings createAssemblerSettings(String outFile, ICommandLineProxy cmdLineProxy)
                    throws AssemblerSettingsException, AssemblerException, ParseException {
                    return new U8500AssemblerSettings(CONFIG, getFileList(), OUTPUT_FLASH_ARCHIVE_FILE_NAME);
                }

                public IAssembler createAssembler(IAssemblerSettings settings) {
                    return null;
                }
            };
            try {
                int execute = assemble.execute(fact, new String[] {
                    "u8500", "-t", "flash_archive", "-c", CONFIG, "-l", getFileList()}, log);
                assertEquals(1, execute);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testMissingSettings() {
        try {
            Assemble assemble = new Assemble(null, null);
            IAssemblerFactory fact = new IAssemblerFactory() {

                public IAssemblerSettings createAssemblerSettings(String outFile, ICommandLineProxy cmdLineProxy)
                    throws AssemblerSettingsException, AssemblerException, ParseException {
                    return new U8500AssemblerSettings(CONFIG, getFileList(), OUTPUT_FLASH_ARCHIVE_FILE_NAME);
                }

                public IAssembler createAssembler(IAssemblerSettings settings) {
                    return null;
                }
            };
            try {
                int execute = assemble.execute(fact, new String[] {
                    "u8500"}, null);
                assertEquals(1, execute);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testAssemblerMessage() {
        try {
            Logger logger = Logger.getLogger(Assemble.class.getName());
            PrintStream ps = new PrintStream(System.out, false, "UTF-8");
            Assemble assemble = new Assemble(logger, ps);
            assemble.assemblerMessage(null, "Test message.");
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testGetErrorLogFilePath() {
        try {
            System.out.println(Assemble.getErrorLogFilePath());
        } catch (SecurityException e) {
            System.out.println(e.getMessage());
        }
    }
}
