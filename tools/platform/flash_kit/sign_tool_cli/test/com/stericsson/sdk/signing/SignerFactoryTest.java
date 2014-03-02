package com.stericsson.sdk.signing;

import static org.easymock.EasyMock.anyObject;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;

import org.apache.commons.cli.ParseException;
import org.easymock.EasyMock;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.cli.ICommandLineProxy;
import com.stericsson.sdk.signing.cli.SignerFactory;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesSignerSettings;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.SignPackageFactory;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.u7x00.M7X00SignerSettings;
import com.stericsson.sdk.signing.u8500.U8500SignerSettings;
import com.stericsson.sdk.signing.util.ListPackagesSignerSettings;

/**
 * @author xtomlju
 */
public class SignerFactoryTest {

    private static final String EXISTING_FILE_FILENAME = "test_files/dummy.bin";

    private static final String FILE_OUT_FILENAME = "signed_software.ssw_out";

    private static final String A2_SIGN_PACKAGE_FILENAME = "test_files/a2.pkg";

    private static final String U5500_SIGN_PACKAGE_FILENAME = "test_files/xolabju_u5500_ROOT.pkg";

    private static final String LOAD_MODULES_PATH = "test_files/loadmodules/loadmodules.xml";

    private static final String EXISTING_FILE_LIST = "test_files/loadmodules/filelist.txt";

    private static final String EXISTING_OUTPUT_FOLDER = "test_files/loadmodules/";
    /**
     * 
     */
    @Test
    public void testCreateA2SignerSettings() {
        try {
            final String packageAlias = "3350";
            SignerFactory factory = new SignerFactory() {
                public String getSignPackageAlias(ICommandLineProxy commandLine) throws ParseException, SignerException {
                    return packageAlias;
                }
            };
            ISignPackage signPackage =
                SignPackageFactory.createSignPackage(new File(ResourcePathManager
                    .getResourceFilePath(A2_SIGN_PACKAGE_FILENAME)));
            ISignerService service = EasyMock.createMock(ISignerService.class);
            service.configure();
            EasyMock.expectLastCall();
            EasyMock.expect(service.getSignPackage(packageAlias, true)).andReturn(signPackage);
            EasyMock.expect(service.getSignPackagesInformation(false, false)).andReturn(new ISignPackageInformation[] {
                new ISignPackageInformation() {

                    public boolean isEncryptRequired() {
                        return false;
                    }

                    public String getType() {
                        return "COPS_A2";
                    }

                    public String getParent() {
                        return null;
                    }

                    public String getName() {
                        return packageAlias;
                    }

                    public String[] getChipIdentifiers() {
                        return new String[] {
                            ""};
                    }
                }});

            ICommandLineProxy proxy = EasyMock.createMock(ICommandLineProxy.class);
            EasyMock.expect(proxy.getArgs()).andReturn(new String[] {
                "a2"});
            EasyMock.expect(proxy.getOptionValue("t")).andReturn("app").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("s")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("s")).andReturn("loader").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("interactive")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("interactive")).andReturn("static").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("app-sec")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("app-sec")).andReturn("on").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("addr-format")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("addr-format")).andReturn("pages").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("hdr-sec")).andReturn("verifyOnly").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("ptype")).andReturn("nand").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("short-mac")).andReturn("abc").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("m")).andReturn("2ndSBC").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("anti-rollback")).andReturn("TBD").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("sw-version")).andReturn("2").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("ss")).andReturn("127.0.0.1:4444").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("spr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("lspr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("lkr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("lekr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("list-packages")).andReturn(false);
            EasyMock.expect(proxy.hasOption("debug")).andReturn(false);
            EasyMock.expect(proxy.hasOption("t")).andReturn(true);
            EasyMock.expect(proxy.hasOption("s")).andReturn(true);
            EasyMock.expect(proxy.hasOption("disable-etx-real")).andReturn(true);
            EasyMock.expect(proxy.hasOption("disable-etx-header")).andReturn(true);
            EasyMock.expect(proxy.hasOption("interactive")).andReturn(true);
            EasyMock.expect(proxy.hasOption("app-sec")).andReturn(true);
            EasyMock.expect(proxy.hasOption("addr-format")).andReturn(true);
            EasyMock.expect(proxy.hasOption("hdr-sec")).andReturn(true);
            EasyMock.expect(proxy.hasOption("ptype")).andReturn(true);
            EasyMock.expect(proxy.hasOption("short-mac")).andReturn(true);
            EasyMock.expect(proxy.hasOption("m")).andReturn(true);
            EasyMock.expect(proxy.hasOption("a")).andReturn(true);
            EasyMock.expect(proxy.hasOption("sw-version")).andReturn(true);
            EasyMock.expect(proxy.hasOption("c")).andReturn(true);
            EasyMock.expect(proxy.hasOption("e")).andReturn(true);

            EasyMock.replay(service);
            EasyMock.replay(proxy);

            try {
                String existingFileName = ResourcePathManager.getResourceFilePath(EXISTING_FILE_FILENAME);
                String existingFileOutName =
                    (new File(existingFileName)).getParent() + File.separator + FILE_OUT_FILENAME;

                assertTrue(factory.createSignerSettings(new Object[] {
                    existingFileName, existingFileOutName}, proxy) instanceof A2SignerSettings);
            } catch (NullPointerException e) {
                assertTrue(false);
            } catch (ParseException e) {
                assertTrue(false);
            }
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateListPackagesSignerSettings() {
        try {
            SignerFactory factory = new SignerFactory();
            ISignerService service = EasyMock.createMock(ISignerService.class);

            ICommandLineProxy proxy = EasyMock.createMock(ICommandLineProxy.class);

            EasyMock.expect(proxy.hasOption((String) anyObject())).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("ss")).andReturn("127.0.0.1:4444").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("spr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("lspr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("lkr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("lekr")).andReturn("/tmp").atLeastOnce();

            EasyMock.replay(service);
            EasyMock.replay(proxy);

            try {
                assertTrue(factory.createSignerSettings(new Object[] {}, proxy) instanceof ListPackagesSignerSettings);
            } catch (NullPointerException e) {
                assertTrue(false);
            } catch (ParseException e) {
                assertTrue(false);
            }
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateU5500SignerSettingsWithoutAdditionalArguments() {
        try {
            final String packageAlias = "xolabju_u5500";
            SignerFactory factory = new SignerFactory() {
                public String getSignPackageAlias(ICommandLineProxy commandLine) throws ParseException, SignerException {
                    return packageAlias;
                }
            };
            ISignPackage signPackage =
                SignPackageFactory.createSignPackage(new File(ResourcePathManager
                    .getResourceFilePath(U5500_SIGN_PACKAGE_FILENAME)));
            ISignerService service = EasyMock.createMock(ISignerService.class);
            service.configure();
            EasyMock.expectLastCall();
            EasyMock.expect(service.getSignPackage(packageAlias, true)).andReturn(signPackage);
            EasyMock.expect(service.getSignPackagesInformation(false, false)).andReturn(new ISignPackageInformation[] {
                new ISignPackageInformation() {

                    public boolean isEncryptRequired() {
                        return false;
                    }

                    public String getType() {
                        return "U5500_ROOT";
                    }

                    public String getParent() {
                        return null;
                    }

                    public String getName() {
                        return packageAlias;
                    }

                    public String[] getChipIdentifiers() {
                        return new String[] {
                            ""};
                    }
                }});

            ICommandLineProxy proxy = EasyMock.createMock(ICommandLineProxy.class);
            EasyMock.expect(proxy.getArgs()).andReturn(new String[] {"u5500"}).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("t")).andReturn("app").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("s")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("s")).andReturn("xloader").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("interactive")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("interactive")).andReturn("static").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("app-sec")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("app-sec")).andReturn("on").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("addr-format")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("addr-format")).andReturn("pages").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("hdr-sec")).andReturn("verifyOnly").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("ptype")).andReturn("nand").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("short-mac")).andReturn("abc").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("m")).andReturn("2ndSBC").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("anti-rollback")).andReturn("TBD").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("sw-version")).andReturn("2").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("b")).andReturn("123").atLeastOnce();
            EasyMock.expect(proxy.hasOption("ss")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("ss")).andReturn("127.0.0.1:4444").atLeastOnce();
            EasyMock.expect(proxy.hasOption("spr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("spr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lspr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lspr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lkr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lkr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lekr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lekr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("list-packages")).andReturn(false);
            EasyMock.expect(proxy.hasOption("debug")).andReturn(false);
            EasyMock.expect(proxy.hasOption("t")).andReturn(true);
            EasyMock.expect(proxy.hasOption("s")).andReturn(true);
            EasyMock.expect(proxy.hasOption("la")).andReturn(false);
            EasyMock.expect(proxy.hasOption("sa")).andReturn(false);
            EasyMock.expect(proxy.hasOption("enable-dma")).andReturn(false);
            EasyMock.expect(proxy.hasOption("enable-pka")).andReturn(false);
            EasyMock.expect(proxy.hasOption("b")).andReturn(false);
            EasyMock.expect(proxy.hasOption("r")).andReturn(false);
            EasyMock.expect(proxy.hasOption("xml")).andReturn(false);
            EasyMock.expect(proxy.hasOption("key-hash")).andReturn(false);
            EasyMock.expect(proxy.hasOption("buffer-size")).andReturn(false);
            EasyMock.expect(proxy.hasOption("sv")).andReturn(false);
            EasyMock.expect(proxy.hasOption("major-build-version")).andReturn(false);
            EasyMock.expect(proxy.hasOption("minor-build-version")).andReturn(false);
            EasyMock.expect(proxy.hasOption("flags")).andReturn(false);
            EasyMock.expect(proxy.hasOption("ph")).andReturn(false);
            EasyMock.expect(proxy.hasOption("sh")).andReturn(false);

            EasyMock.replay(service);
            EasyMock.replay(proxy);

            try {
                String existingFileName = ResourcePathManager.getResourceFilePath(EXISTING_FILE_FILENAME);
                String existingFileOutName =
                    (new File(existingFileName)).getParent() + File.separator + FILE_OUT_FILENAME;

                assertTrue(factory.createSignerSettings(new Object[] {
                    existingFileName, existingFileOutName}, proxy) instanceof U5500SignerSettings);
            } catch (NullPointerException e) {
                assertTrue(false);
            } catch (ParseException e) {
                assertTrue(false);
            }
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateU8500SignerSettingsWithAdditionalArguments() {
        try {
            final String packageAlias = "xolabju_u5500";
            SignerFactory factory = new SignerFactory() {
                public String getSignPackageAlias(ICommandLineProxy commandLine) throws ParseException, SignerException {
                    return packageAlias;
                }
            };
            ISignPackage signPackage =
                SignPackageFactory.createSignPackage(new File(ResourcePathManager
                    .getResourceFilePath(U5500_SIGN_PACKAGE_FILENAME)));
            ISignerService service = EasyMock.createMock(ISignerService.class);
            service.configure();
            EasyMock.expectLastCall();
            EasyMock.expect(service.getSignPackage(packageAlias, true)).andReturn(signPackage);
            EasyMock.expect(service.getSignPackagesInformation(false, false)).andReturn(new ISignPackageInformation[] {
                new ISignPackageInformation() {

                    public boolean isEncryptRequired() {
                        return false;
                    }

                    public String getType() {
                        return "U5500_ROOT";
                    }

                    public String getParent() {
                        return null;
                    }

                    public String getName() {
                        return packageAlias;
                    }

                    public String[] getChipIdentifiers() {
                        return new String[] {
                            ""};
                    }
                }});

            ICommandLineProxy proxy = EasyMock.createMock(ICommandLineProxy.class);
            EasyMock.expect(proxy.getArgs()).andReturn(new String[] {"u8500"}).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("t")).andReturn("app").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("s")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("s")).andReturn("xloader").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("interactive")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("interactive")).andReturn("static").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("app-sec")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("app-sec")).andReturn("on").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("addr-format")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("addr-format")).andReturn("pages").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("hdr-sec")).andReturn("verifyOnly").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("ptype")).andReturn("nand").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("short-mac")).andReturn("abc").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("m")).andReturn("2ndSBC").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("anti-rollback")).andReturn("TBD").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("sw-version")).andReturn("2").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("b")).andReturn("123").atLeastOnce();
            EasyMock.expect(proxy.hasOption("ss")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("ss")).andReturn("127.0.0.1:4444").atLeastOnce();
            EasyMock.expect(proxy.hasOption("spr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("spr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lspr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lspr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lkr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lkr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lekr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lekr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("list-packages")).andReturn(false);
            EasyMock.expect(proxy.hasOption("debug")).andReturn(false);
            EasyMock.expect(proxy.hasOption("t")).andReturn(true);
            EasyMock.expect(proxy.hasOption("s")).andReturn(true);
            EasyMock.expect(proxy.hasOption("la")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("la")).andReturn("10").atLeastOnce();
            EasyMock.expect(proxy.hasOption("sa")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("sa")).andReturn("20").atLeastOnce();
            EasyMock.expect(proxy.hasOption("enable-dma")).andReturn(true);
            EasyMock.expect(proxy.hasOption("enable-pka")).andReturn(true);
            EasyMock.expect(proxy.hasOption("b")).andReturn(false);
            EasyMock.expect(proxy.hasOption("r")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("r")).andReturn("SHA-384").atLeastOnce();
            EasyMock.expect(proxy.hasOption("xml")).andReturn(false);
            EasyMock.expect(proxy.hasOption("key-hash")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("key-hash")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("buffer-size")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("buffer-size")).andReturn("6").atLeastOnce();
            EasyMock.expect(proxy.hasOption("sv")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("sv")).andReturn("2").atLeastOnce();
            EasyMock.expect(proxy.hasOption("major-build-version")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("major-build-version")).andReturn("5").atLeastOnce();
            EasyMock.expect(proxy.hasOption("minor-build-version")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("minor-build-version")).andReturn("3").atLeastOnce();
            EasyMock.expect(proxy.hasOption("flags")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("flags")).andReturn("1111").atLeastOnce();
            EasyMock.expect(proxy.hasOption("ph")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("ph")).andReturn("SHA-384").atLeastOnce();
            EasyMock.expect(proxy.hasOption("sh")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("sh")).andReturn("SHA-384").atLeastOnce();

            EasyMock.replay(service);
            EasyMock.replay(proxy);

            try {
                String existingFileName = ResourcePathManager.getResourceFilePath(EXISTING_FILE_FILENAME);
                String existingFileOutName =
                    (new File(existingFileName)).getParent() + File.separator + FILE_OUT_FILENAME;

                assertTrue(factory.createSignerSettings(new Object[] {
                    existingFileName, existingFileOutName}, proxy) instanceof U8500SignerSettings);
            } catch (NullPointerException e) {
                assertTrue(false);
            } catch (ParseException e) {
                assertTrue(false);
            }
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateU7400SignerSettingsWithoutAdditionalArguments() {
        try {
            final String packageAlias = "xolabju_u5500";
            SignerFactory factory = new SignerFactory() {
                public String getSignPackageAlias(ICommandLineProxy commandLine) throws ParseException, SignerException {
                    return packageAlias;
                }
            };
            ISignPackage signPackage =
                SignPackageFactory.createSignPackage(new File(ResourcePathManager
                    .getResourceFilePath(U5500_SIGN_PACKAGE_FILENAME)));
            ISignerService service = EasyMock.createMock(ISignerService.class);
            service.configure();
            EasyMock.expectLastCall();
            EasyMock.expect(service.getSignPackage(packageAlias, true)).andReturn(signPackage);
            EasyMock.expect(service.getSignPackagesInformation(false, false)).andReturn(new ISignPackageInformation[] {
                new ISignPackageInformation() {

                    public boolean isEncryptRequired() {
                        return false;
                    }

                    public String getType() {
                        return "U5500_ROOT";
                    }

                    public String getParent() {
                        return null;
                    }

                    public String getName() {
                        return packageAlias;
                    }

                    public String[] getChipIdentifiers() {
                        return new String[] {
                            ""};
                    }
                }});

            ICommandLineProxy proxy = EasyMock.createMock(ICommandLineProxy.class);
            EasyMock.expect(proxy.getArgs()).andReturn(new String[] {"m7x00"}).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("t")).andReturn("app").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("s")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("s")).andReturn("xloader").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("interactive")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("interactive")).andReturn("static").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("app-sec")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("app-sec")).andReturn("on").atLeastOnce();
            // EasyMock.expect(proxy.hasOption("addr-format")).andReturn(true).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("addr-format")).andReturn("pages").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("hdr-sec")).andReturn("verifyOnly").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("ptype")).andReturn("nand").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("short-mac")).andReturn("abc").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("m")).andReturn("2ndSBC").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("anti-rollback")).andReturn("TBD").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("sw-version")).andReturn("2").atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("b")).andReturn("123").atLeastOnce();
            EasyMock.expect(proxy.hasOption("ss")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("ss")).andReturn("127.0.0.1:4444").atLeastOnce();
            EasyMock.expect(proxy.hasOption("spr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("spr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lspr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lspr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lkr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lkr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("lekr")).andReturn(true);
            EasyMock.expect(proxy.getOptionValue("lekr")).andReturn("/tmp").atLeastOnce();
            EasyMock.expect(proxy.hasOption("list-packages")).andReturn(false);
            EasyMock.expect(proxy.hasOption("debug")).andReturn(false);
            EasyMock.expect(proxy.hasOption("t")).andReturn(true);
            EasyMock.expect(proxy.hasOption("s")).andReturn(true);
            EasyMock.expect(proxy.hasOption("la")).andReturn(false);
            EasyMock.expect(proxy.hasOption("sa")).andReturn(false);
            EasyMock.expect(proxy.hasOption("enable-dma")).andReturn(false);
            EasyMock.expect(proxy.hasOption("enable-pka")).andReturn(false);
            EasyMock.expect(proxy.hasOption("b")).andReturn(false);
            EasyMock.expect(proxy.hasOption("r")).andReturn(false);
            EasyMock.expect(proxy.hasOption("xml")).andReturn(false);
            EasyMock.expect(proxy.hasOption("key-hash")).andReturn(false);
            EasyMock.expect(proxy.hasOption("buffer-size")).andReturn(false);
            EasyMock.expect(proxy.hasOption("sv")).andReturn(false);
            EasyMock.expect(proxy.hasOption("major-build-version")).andReturn(false);
            EasyMock.expect(proxy.hasOption("minor-build-version")).andReturn(false);
            EasyMock.expect(proxy.hasOption("flags")).andReturn(false);
            EasyMock.expect(proxy.hasOption("ph")).andReturn(false);
            EasyMock.expect(proxy.hasOption("sh")).andReturn(false);

            EasyMock.replay(service);
            EasyMock.replay(proxy);

            try {
                String existingFileName = ResourcePathManager.getResourceFilePath(EXISTING_FILE_FILENAME);
                String existingFileOutName =
                    (new File(existingFileName)).getParent() + File.separator + FILE_OUT_FILENAME;

                assertTrue(factory.createSignerSettings(new Object[] {
                    existingFileName, existingFileOutName}, proxy) instanceof M7X00SignerSettings);
            } catch (NullPointerException e) {
                assertTrue(false);
            } catch (ParseException e) {
                assertTrue(false);
            }
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateLoadModuleSignerSettingsWithoutAdditionalArguments() {
        try {
            SignerFactory factory = new SignerFactory();
            ISignerService service = EasyMock.createMock(ISignerService.class);
            service.configure();
            EasyMock.expectLastCall();

            ICommandLineProxy proxy = EasyMock.createMock(ICommandLineProxy.class);
            EasyMock.expect(proxy.getArgs()).andReturn(new String[] {"load-modules"}).atLeastOnce();
            EasyMock.expect(proxy.getOptionValue("c")).andReturn(LOAD_MODULES_PATH).atLeastOnce();
            EasyMock.expect(proxy.hasOption("list-packages")).andReturn(false);


            EasyMock.replay(service);
            EasyMock.replay(proxy);

            try {

                String existingFileList = ResourcePathManager.getResourceFilePath(EXISTING_FILE_LIST);
                String existingFileOutputFolder = ResourcePathManager.getResourceFilePath(EXISTING_OUTPUT_FOLDER);

                assertTrue(factory.createSignerSettings(new Object[] {
                        existingFileList, existingFileOutputFolder}, proxy) instanceof LoadModulesSignerSettings);
            } catch (NullPointerException e) {
                assertTrue(false);
            } catch (ParseException e) {
                assertTrue(false);
            }
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }
}
