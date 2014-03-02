package com.stericsson.sdk.assembling;

import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.ParseException;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.assembling.cli.Assemble;
import com.stericsson.sdk.assembling.cli.ICommandLineProxy;
import com.stericsson.sdk.assembling.internal.u8500.U8500CommandLineOptions;

/**
 * 
 * @author xolabju
 * 
 */
public class AssemblerFactoryTest {

    /** */
    private static final String CONFIGURATION = ResourcePathManager.getResourceFilePath("res/config.xml");

    /** */
    private static final String FILE_LIST = ResourcePathManager.getResourceFilePath("res/files_local.txt");

    /**
     * 
     */
    @Test
    public void testCreateAssembler() {
        AssemblerFactory factory = new AssemblerFactory();
        AbstractAssemblerSettings settings = new AbstractAssemblerSettings() {
        };
        try {
            settings.put(IAssemblerSettings.KEY_PLATFORM_TYPE, "invalidPlatform");
            assertNull(factory.createAssembler(settings));
        } catch (AssemblerSettingsException e) {
            fail(e.getMessage());
        }

        try {
            settings.put(IAssemblerSettings.KEY_PLATFORM_TYPE, Assemble.PLATFORM_U8500);
            settings.put(IAssemblerSettings.KEY_OUTPUT_TYPE, "invalidOutputType");
            assertNull(factory.createAssembler(settings));
        } catch (AssemblerSettingsException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateAssemblerSettings() {
        AssemblerFactory factory = new AssemblerFactory();
        ICommandLineProxy proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                return "Invalid output";
            }

            public String[] getArgs() {
                return new String[] {
                    "Invalid platform"};
            }
        };
        try {
            assertNull(factory.createAssemblerSettings("outFile.test", proxy));
        } catch (Exception e) {
            fail(e.getMessage());
        }

        proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                return "Invalid output";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            factory.createAssemblerSettings("outFile.test", proxy);
            fail("should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testNullOutputTypeSetting() {
        AssemblerFactory factory = new AssemblerFactory();
        ICommandLineProxy proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                try {
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD())) {
                        return null;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD())) {
                        return CONFIGURATION;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD())) {
                        return FILE_LIST;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
                        return "512";
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
                        return "512";
                    }
                } catch (ParseException e) {
                    fail(e.getMessage());
                }
                return "Invalid value";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            assertNull(factory.createAssemblerSettings("output.test", proxy));
            fail("Should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testNullConfigurationSetting() {
        AssemblerFactory factory = new AssemblerFactory();
        ICommandLineProxy proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                try {
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD())) {
                        return U8500CommandLineOptions.TYPE_FLASH_ARCHIVE;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD())) {
                        return null;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD())) {
                        return FILE_LIST;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
                        return "512";
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
                        return "512";
                    }
                } catch (ParseException e) {
                    fail(e.getMessage());
                }
                return "Invalid value";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            assertNull(factory.createAssemblerSettings("output.test", proxy));
            fail("Should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testNullFileListSetting() {
        AssemblerFactory factory = new AssemblerFactory();
        ICommandLineProxy proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                try {
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD())) {
                        return U8500CommandLineOptions.TYPE_FLASH_ARCHIVE;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD())) {
                        return CONFIGURATION;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD())) {
                        return null;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
                        return "512";
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
                        return "512";
                    }
                } catch (ParseException e) {
                    fail(e.getMessage());
                }
                return "Invalid value";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            assertNull(factory.createAssemblerSettings("output.test", proxy));
            fail("Should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testBufferAndAlignmentSizeSettings() {
        final int bufferSize = 8 * 1024;
        final int alignmentSize = 1024;
        AssemblerFactory factory = new AssemblerFactory();
        ICommandLineProxy proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                try {
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD())) {
                        return U8500CommandLineOptions.TYPE_FLASH_ARCHIVE;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD())) {
                        return CONFIGURATION;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD())) {
                        return FILE_LIST;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
                        return String.valueOf(bufferSize);
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
                        return String.valueOf(alignmentSize);
                    }
                } catch (ParseException e) {
                    fail(e.getMessage());
                }
                return "Invalid value";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            IAssemblerSettings settings = factory.createAssemblerSettings("output.test", proxy);
            Integer bufferSizeValue = (Integer) settings.get(IAssemblerSettings.KEY_BUFFER_SIZE);
            assertTrue((bufferSizeValue != null) ? bufferSizeValue.intValue() == bufferSize : false);
            Integer alignmentSizeValue = (Integer) settings.get(IAssemblerSettings.KEY_ALIGNMENT_SIZE);
            assertTrue((alignmentSizeValue != null) ? alignmentSizeValue.intValue() == alignmentSize : false);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidBufferSizeSetting() {
        AssemblerFactory factory = new AssemblerFactory();
        ICommandLineProxy proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                try {
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD())) {
                        return U8500CommandLineOptions.TYPE_FLASH_ARCHIVE;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD())) {
                        return CONFIGURATION;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD())) {
                        return FILE_LIST;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
                        return null;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
                        return "512";
                    }
                } catch (ParseException e) {
                    fail(e.getMessage());
                }
                return "Invalid value";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            assertNull(factory.createAssemblerSettings("output.test", proxy));
            fail("Should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidAlignmentSizeSetting() {
        AssemblerFactory factory = new AssemblerFactory();
        ICommandLineProxy proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                try {
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD())) {
                        return U8500CommandLineOptions.TYPE_FLASH_ARCHIVE;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD())) {
                        return CONFIGURATION;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD())) {
                        return FILE_LIST;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
                        return "512";
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
                        return null;
                    }
                } catch (ParseException e) {
                    fail(e.getMessage());
                }
                return "Invalid value";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            assertNull(factory.createAssemblerSettings("output.test", proxy));
            fail("Should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }

        proxy = new ICommandLineProxy() {

            public void setProxiedObject(CommandLine cl) {

            }

            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String getOptionValue(String shortCMD) {
                try {
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD())) {
                        return U8500CommandLineOptions.TYPE_FLASH_ARCHIVE;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD())) {
                        return CONFIGURATION;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD())) {
                        return FILE_LIST;
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
                        return "512";
                    }
                    if (shortCMD.equalsIgnoreCase(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
                        return String.valueOf(Integer.MAX_VALUE);
                    }
                } catch (ParseException e) {
                    fail(e.getMessage());
                }
                return "Invalid value";
            }

            public String[] getArgs() {
                return new String[] {
                    Assemble.PLATFORM_U8500};
            }
        };
        try {
            assertNull(factory.createAssemblerSettings("output.test", proxy));
            fail("Should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
