package com.stericsson.sdk.signing.signerservice.local;

import java.io.File;
import java.math.BigInteger;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.a2.A2AccessControlList;
import com.stericsson.sdk.signing.a2.A2Signer;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.a2.A2SoftwareVersion;
import com.stericsson.sdk.signing.a2.IA2HeaderDestinationAddressConstants;
import com.stericsson.sdk.signing.a2.IA2MacModeConstants;
import com.stericsson.sdk.signing.a2.IA2PayloadTypeConstants;
import com.stericsson.sdk.signing.cops.ICOPSSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.local.keys.COPSRSAPrivateKey;
import com.stericsson.sdk.signing.test.Activator;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.u8500.U8500SignerSettings;

/**
 * @author xolabju
 */
public class LocalSignerServiceTest extends TestCase {

    private static final String KEY_ROOT = "/localroot/keys";

    private static final String ENCRYPTION_ROOT = "/localroot/encryptionkeys";

    private static final String PACKAGE_ROOT = "/localroot/packages";

    private static final String KEYSANDPACKAGES_ROOT = "/localroot/keysandpackages";

    private static final String SIGN_PACKAGE_U5500_ROOT = "xolabju_u5500";

    private static final String UNSIGNED_U8500_XLOADER_FILENAME = "/u8500/unsigned_xloader.bin";

    private static final String UNSIGNED_U8500_FLASH_ARCHIVE_FILENAME = "/u8500/unsigned_flasharchive.zip";

    private static final String SIGNED_XLOADER_FILENAME = "signed_xloader.bin";

    private static final String SIGNED_U8500_FLASH_ARCHIVE_OUTPUT_FILENAME = "signed_u8500_archive_output.zip";

    private static final String RESIGNED_U8500_FLASH_ARCHIVE_OUTPUT_FILENAME = "resigned_u8500_archive.zip";

    private static final String SIGNED_U8500_FLASH_ARCHIVE_INPUT_FILENAME = "/u8500/flasharchive.zip";

    private static final String UNSIGNED_U8500_ISSW_FILENAME = "/u8500/unsigned_cust_u8500_issw.bin";

    private static final String SIGNED_ISSW_FILENAME = "signed_issw.bin";

    private static final String UNSIGNED_A2_LOADER_FILENAME = "/a2/CXC1725333_R1E.bin";

    private static final String SIGNED_A2_LOADER_FILENAME = "CXC1725333_R1E.ldr";

    private static final String SIGN_PACKAGE_A2 = "CUST_19_(0)_DB3210_COMMERCIAL";

    private static final String SIGNED_U5500_FLASH_ARCHIVE_INPUT_FILENAME = "/u5500/flasharchive.zip";

    private static final String RESIGNED_U5500_FLASH_ARCHIVE_OUTPUT_FILENAME = "re-signed_flash_archive.zip";

    private static final String SIGNED_U5500_FLASH_ARCHIVE_OUTPUT_FILENAME = "signed_flash_archive.zip";

    private static final String UNSIGNED_U5500_FLASH_ARCHIVE_FILENAME = "/u5500/unsigned_flasharchive.zip";

    private static final String UNSIGNED_U5500_XLOADER_FILENAME = "/u5500/unsigned_xloader.bin";

    private static final String UNSIGNED_U5500_ISSW_FILENAME = "/u5500/unsigned_cust_u5500_issw.bin";

    private static final String UNSIGNED_TRUSTED_FILENAME = "/u5500/unsigned_trusted.bin";

    private static final String SIGNED_TRUSTED_FILENAME = "signed_trusted.bin";

    private static final String KEY_HASH_FILE = "key_hash.bin";

    // private static final String UNSIGNED_TVP_LICENSE_FILENAME = "/tvp/unsigned_license.bin";

    // private static final String SIGNED_TVP_LICENSE_FILENAME = "signed_tvp_license.bin";

    // private static final String SIGN_PACKAGE_TVP = "xolabju_tvp";

    private static final String SIGN_PACKAGE_CRK = "crktest";

    private static final String UNSIGNED_U5500_CRKC_FILENAME = "/u5500/unsigned_ProductRootKey.priv.pem";

    private static final String SIGNED_U5500_CRKC_FILENAME = "signed_crkc.bin";

    private static final String UNSIGNED_U5500_ELF_FILENAME = "/u5500/unsigned_elf.elf";

    private static final String SIGNED_U5500_ELF_FILENAME = "signed_elf.elf";

    private static final String UNSIGNED_DNT = "/u8500/unsigned_dnt.bin";

    private static final String SIGNED_DNT_FRAC = "signed_dnt_frac.bin";

    private static final String KEY_SIGN_KEY_PATH = "/localroot/keys/xolabju_u8500_root/root.pem";

    /**
     * 
     */
    public void setUp() {
        System.setProperty(ISignerService.ENV_LOCAL_ENCRYPTION_KEY_ROOT, getResourceFilePath(ENCRYPTION_ROOT));
        System.setProperty(ISignerService.ENV_LOCAL_SIGN_KEY_ROOT, getResourceFilePath(KEY_ROOT));
        System.setProperty(ISignerService.ENV_LOCAL_SIGN_PACKAGE_ROOT, getResourceFilePath(PACKAGE_ROOT));
        System.setProperty(ISignerService.ENV_LOCAL_SIGN_KEYS_AND_PACKAGE_ROOT, getResourceFilePath(KEYSANDPACKAGES_ROOT));
    }

    private String getResourceFilePath(String filepath) {
        return Activator.getResourcesPath() + filepath;
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500XLoader() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U8500_XLOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_XLOADER_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.XLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        testSigning(settings);

    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500XLoaderCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U8500_XLOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_XLOADER_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.XLOADER);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.XLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        testSigning(settings);

    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500FRAC() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_DNT));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_DNT_FRAC);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, "TEST_FRAC");
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FRAC);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500FRACCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_DNT));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_DNT_FRAC);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, "TEST_FRAC");
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FRAC);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500ISSW() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U8500_ISSW_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ISSW_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ISSW);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500ISSWCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U8500_ISSW_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ISSW_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ISSW);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500TrustedApp() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TRUSTED_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TRUSTED_FILENAME);
        File keyHash = new File(unsignedFile.getParent() + File.separator + KEY_HASH_FILE);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.TRUSTED);
        settings.setSignerSetting(ISignerSettings.KEY_LOCAL, true);
        settings.setSignerSetting(ISignerSettings.KEY_HASH_FILE, keyHash.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500TrustedAppCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TRUSTED_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TRUSTED_FILENAME);
        File keyHash = new File(unsignedFile.getParent() + File.separator + KEY_HASH_FILE);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.TRUSTED);
        settings.setSignerSetting(ISignerSettings.KEY_LOCAL, true);
        settings.setSignerSetting(ISignerSettings.KEY_HASH_FILE, keyHash.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignA2Loader() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_A2_LOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_A2_LOADER_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setA2Defaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_A2);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
            A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_APP_SEC,
            A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, A2AccessControlList.TARGET_CPU_APPLICATION);
        settings
            .setSignerSetting(A2SignerSettings.KEY_ACL_FORMAT, A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_HDR_SECURITY, A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);

        setA2SWType("loader", settings);
        testSigning(settings);

    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignA2ECLoader() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_A2_LOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_A2_LOADER_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setA2Defaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_A2);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
            A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_APP_SEC,
            A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, A2AccessControlList.TARGET_CPU_APPLICATION);
        settings
            .setSignerSetting(A2SignerSettings.KEY_ACL_FORMAT, A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_HDR_SECURITY, A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_COMPRESSION, true);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_ENCRYPTION, true);

        setA2SWType("loader", settings);
        testSigning(settings);

    }

    private void setA2SWType(String softwareType, A2SignerSettings settings) throws Exception {
        if (softwareType.equalsIgnoreCase("loader")) {
            settings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE, A2AccessControlList.SW_TYPE_LOADER);
            settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE_TYPE, A2Signer.INPUT_FILE_TYPE_BIN);
        } else {
            settings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE,
                A2AccessControlList.SW_TYPE_BOOT_CODE_OR_SW_IMAGE);

            if (softwareType.equalsIgnoreCase("image")) {
                settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_SW_IMAGE);
            } else if (softwareType.equalsIgnoreCase("elf")) {
                settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_ELF);
            } else if (softwareType.equalsIgnoreCase("generic")) {
                settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_GENERIC);
            } else if (softwareType.equalsIgnoreCase("archive")) {
                settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_ARCHIVE);
            } else if (softwareType.equals("tar")) {
                settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_TAR);
            }
        }

    }

    private void setA2Defaults(A2SignerSettings settings) throws Exception {
        settings.setSignerSetting(A2SignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_CONFIG);
        settings.setSignerSetting(A2SignerSettings.KEY_SW_VERSION, 0);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_DEBUG,
            A2AccessControlList.DEBUG_ON_ACCESS_APPLICATION_SIDE_ENABLE);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_EXT_LEVEL, A2AccessControlList.ETX_LEVEL_ENABLE);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_ETX_LEVEL_IN_HEADER,
            A2AccessControlList.ETX_LEVEL_IN_HEADER_ENABLE);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_PTYPE, A2AccessControlList.PAYLOAD_TYPE_PHYSICAL_ADDRESS);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SHORT_MAC, A2AccessControlList.SHORT_MAC_HEADER_ENABLE);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_ANTI_ROLL_REQUIRED,
            A2SoftwareVersion.ANTI_ROLLBACK_NOT_REQUIRED);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE, 0);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_HEADER_DESTINATION_ADDRESS,
            IA2HeaderDestinationAddressConstants.HEADER_DEST_ADDRESS);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500FlashArchive() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U8500_FLASH_ARCHIVE_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + SIGNED_U8500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU8500FlashArchiveCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U8500_FLASH_ARCHIVE_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + SIGNED_U8500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testReSignU8500FlashArchive() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(SIGNED_U8500_FLASH_ARCHIVE_INPUT_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + RESIGNED_U8500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testReSignU8500FlashArchiveCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(SIGNED_U8500_FLASH_ARCHIVE_INPUT_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + RESIGNED_U8500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500FlashArchive() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_FLASH_ARCHIVE_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + SIGNED_U5500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500FlashArchiveCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_FLASH_ARCHIVE_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + SIGNED_U5500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testReSignU5500FlashArchive() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(SIGNED_U5500_FLASH_ARCHIVE_INPUT_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + RESIGNED_U5500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testReSignU5500FlashArchiveCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(SIGNED_U5500_FLASH_ARCHIVE_INPUT_FILENAME));
        File signedFile =
            new File(unsignedFile.getParent() + File.separator + RESIGNED_U5500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500XLoader() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_XLOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_XLOADER_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.XLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500XLoaderCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_XLOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_XLOADER_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.XLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500CRKCDefaultHash() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_CRKC_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_U5500_CRKC_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_CRK);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.CRKC);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500CRKCSHA256() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_CRKC_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_U5500_CRKC_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_CRK);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.CRKC);
        settings.setSignerSetting(U5500SignerSettings.KEY_ROOT_KEY_HASH_TYPE, "SHA-256");
        settings.setSignerSetting(U5500SignerSettings.KEY_PAYLOAD_HASH_TYPE, "SHA-256");
        settings.setSignerSetting(U5500SignerSettings.KEY_SIGNATURE_HASH_TYPE, "SHA-256");
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500ISSW() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_ISSW_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ISSW_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ISSW);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500ISSWCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_ISSW_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ISSW_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ISSW);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSigning(settings);
    }

    /**
     * Test for the old way of signing TVP license.
     * 
     * @throws Throwable
     *             TBD
     */
    /*
     * @Test public void testSignTVPLicense() throws Throwable { setUp(); File unsignedFile = new
     * File(getResourceFilePath(UNSIGNED_TVP_LICENSE_FILENAME)); File signedFile = new
     * File(unsignedFile.getParent() + File.separator + SIGNED_TVP_LICENSE_FILENAME);
     * ISignerSettings settings = new U5500SignerSettings(unsignedFile.getAbsolutePath(),
     * signedFile.getAbsolutePath());
     * settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_TVP);
     * settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.TVP_LICENSE);
     * 
     * testSigning(settings); }
     */

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500ELF() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_ELF_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_U5500_ELF_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ELF);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignU5500ELFCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U5500_ELF_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_U5500_ELF_FILENAME);
        ISignerSettings settings =
            new U5500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ELF);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testKeySignKeyNotNull() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_U8500_FLASH_ARCHIVE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_U8500_FLASH_ARCHIVE_OUTPUT_FILENAME);
        U8500SignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        settings.setSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY, getResourceFilePath(KEY_SIGN_KEY_PATH));
        settings.setSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE, SignatureType.RSASSA_PKCS_V1_5);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testKeySignKeyNotNullTrusted() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TRUSTED_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TRUSTED_FILENAME);
        U8500SignerSettings settings =
            new U8500SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, GenericSoftwareType.TRUSTED);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        settings.setSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY, getResourceFilePath(KEY_SIGN_KEY_PATH));
        settings.setSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE, SignatureType.RSASSA_PKCS_V1_5);
        testSigning(settings);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testListSignPackages() throws Throwable {
        setUp();
        assertNotNull(new SignerFacade().getSignPackages(null, true, false,(ISignerSettings) null));
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testListSignPackagesCommonFolder() throws Throwable {
        setUp();
        assertNotNull(new SignerFacade().getSignPackages(null, true, true,(ISignerSettings) null));
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    private void testSigning(ISignerSettings settings) throws Throwable {
        SignerFacade facade = new SignerFacade();
        String unsignedFileName = (String) settings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE);
        assertNotNull(unsignedFileName);
        String signedFileName = (String) settings.getSignerSetting(ISignerSettings.KEY_OUTPUT_FILE);
        assertNotNull(signedFileName);

        File unsignedFile = new File(unsignedFileName);
        File signedFile = new File(signedFileName);

        assertTrue(unsignedFile.exists());
        try {
            facade.sign(settings, null, true);
        } catch (Exception e) {
            fail(e.getMessage());
            e.printStackTrace();
        }

        assertTrue(signedFile.exists());
        assertTrue(signedFile.length() > 0);

        signedFile.delete();
        assertFalse(signedFile.exists());
    }
}
