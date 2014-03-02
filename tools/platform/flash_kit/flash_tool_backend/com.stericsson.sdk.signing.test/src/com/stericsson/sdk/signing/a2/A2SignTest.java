package com.stericsson.sdk.signing.a2;

import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_HEADERPACKAGE_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_KEYCATALOGUE_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_KEYCATALOGUE_RESPONSE;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_LOGIN_ACCEPT;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_LOGIN_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_RANDOMNUMBER_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_SECRET_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_SECRET_RESPONSE;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketFactory.createPacket;

import java.io.File;
import java.io.FileOutputStream;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.cops.ICOPSSignerSettings;
import com.stericsson.sdk.signing.mockup.FakeMessage;
import com.stericsson.sdk.signing.mockup.FakeSignServer;
import com.stericsson.sdk.signing.mockup.MockProtocolMessage;
import com.stericsson.sdk.signing.test.Activator;

/**
 * @author xtomlju
 */
public class A2SignTest extends TestCase {

    private static final String SIGN_PACKAGE_ALIAS = "CUST_19_(0)_TietoEnatorKarlstad_DB3210_DEVELOPMENT";

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String EMPTY_FILE_FILENAME = "/a2/empty.bin";

    private static final String DIRECTORY_FILE_FILENAME = "/";

    private static final String UNSIGNED_LOADER_FILENAME = "/a2/CXC1725333_R1E.bin";

    private static final String SIGNED_LOADER_FILENAME = "CXC1725333_R1E.ldr";

    private static final String UNSIGNED_TAR_FILENAME = "/a2/tartest.tar";

    private static final String SIGNED_TAR_FILENAME = "tartest.ssw";

    private static final String UNSIGNED_ELF_FILENAME = "/a2/module.elf";

    private static final String UNSIGNED_ELF_WITH_SECTIONS_FILENAME = "/a2/module_with_sections.elf";

    private static final String SIGNED_ELF_FILENAME = "module.ssw";

    private static final String SIGNED_ELF_WITH_SECTIONS_FILENAME = "module_with_sections.ssw";

    private static final String UNSIGNED_SRECORD_FILENAME = "/a2/software.a01";

    private static final String SIGNED_SRECORD_FILENAME = "software.ssw";

    private static final byte[] SECRET = new byte[] {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    private static final FakeMessage PACKET_SIGN_PACKAGE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 20, 0, 0, 5, 59, 80, 75, 3, 4, 20, 0, 8, 0, 8, 0, 97, 108, -47, 56, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 105, 110, 100, 101, 120, 46, 109, 101, 115, 112, 77, -50, 65, 75, -61, 64,
            16, 5, -32, 123, -95, -1, 97, -104, -109, 94, 100, -109, 32, 106, 33, -121, 116, -77, -112, 104, 106, -125,
            -119, 94, -105, 73, 50, -43, -91, 49, 9, -69, -37, 74, -1, -67, -95, 45, -30, -11, -15, -67, -57, 43, -87,
            -35, -45, 39, 3, -11, -122, 92, -116, -14, -67, -86, 117, -16, -92, 111, -60, -83, -82, 13, -5, 81, 13,
            -28, 71, -5, 66, -74, 119, -98, 58, -99, -82, -93, 48, 16, 58, 85, 31, -86, -40, -106, 27, -11, 90, 35, -8,
            -45, -60, 115, 115, 91, 86, 58, 9, 17, 90, -53, -28, -71, -117, -79, 62, 48, 60, 31, 6, 8, 30, 32, -120,
            86, -47, -3, 74, -124, 32, 85, 85, 67, 40, -60, -29, -97, 91, -97, 98, -28, -42, -3, -48, -98, 17, 44, 31,
            -115, 51, -29, 16, -29, 91, -112, 32, 116, -20, 90, 107, 38, 127, 78, 112, -71, 80, -101, 50, 99, -22, -40,
            -62, -111, -19, -59, 53, -44, -80, -104, 7, 17, -78, 60, -43, 59, -45, -13, 64, -33, -13, -99, -117, -53,
            -45, -69, -58, 12, 8, 50, -53, 75, 45, -117, -92, -86, -2, 17, -7, 101, 38, -39, -109, 115, 103, -77, 92,
            72, -74, -34, -20, 76, 59, -65, -126, -85, -104, -109, -62, 56, 127, 5, -65, 80, 75, 7, 8, 40, 96, -85, 87,
            -27, 0, 0, 0, 44, 1, 0, 0, 80, 75, 3, 4, 20, 0, 8, 0, 8, 0, 97, 108, -47, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 12, 0, 0, 0, 72, 101, 97, 100, 101, 114, 73, 68, 46, 98, 105, 110, -37, -75, -113, -119, 1, 0, 80,
            75, 7, 8, 122, -64, 125, -82, 6, 0, 0, 0, 4, 0, 0, 0, 80, 75, 3, 4, 20, 0, 8, 0, 8, 0, 97, 108, -47, 56, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 67, 104, 105, 112, 67, 108, 97, 115, 115, 46, 98, 105, 110,
            99, 80, 96, 96, 0, 0, 80, 75, 7, 8, -4, -103, 9, 25, 6, 0, 0, 0, 4, 0, 0, 0, 80, 75, 3, 4, 20, 0, 8, 0, 8,
            0, 97, 108, -47, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 67, 101, 114, 116, 76, 105, 115, 116,
            46, 98, 105, 110, 51, 104, 98, -54, 48, 104, 98, -68, -72, -128, -103, -119, -111, -119, -119, -47, -57,
            -128, -105, -115, 83, -85, -51, -93, -19, 59, 47, 35, 35, 43, 43, -125, 1, -65, 33, -81, 1, 55, 27, 115,
            40, 11, -77, 48, -117, -85, 111, -128, -95, -127, -100, 56, -81, -127, -123, -127, -103, -95, -71, -95,
            -95, -79, -87, -127, 97, -108, 56, -81, -119, -91, -95, -111, 49, 16, -103, 90, -102, 90, 70, 25, 88, 27,
            90, 26, -104, -125, 53, -120, 24, 56, 26, -59, -121, 100, -90, -106, -28, -69, -26, 37, -106, -28, 23, 121,
            39, 22, -27, 20, -105, 36, -90, -60, 27, 90, -58, 107, 24, 104, -58, -69, 56, 25, 27, 25, 26, -60, -69,
            -72, -122, -71, -6, -8, 7, -8, -70, -6, -123, 24, 52, -50, 69, -74, -97, -111, -107, -127, -71, -79, -101,
            -63, -96, -79, -99, -87, -79, -111, -95, -49, 95, 112, -95, -20, -101, 10, -103, -30, 48, 101, -101, -123,
            -95, 12, -81, 106, -8, -44, -65, -103, 111, 123, -70, 74, -14, -42, 21, -5, -51, 114, -126, 2, -74, -17,
            10, 119, 60, 105, 102, 124, 104, -1, -86, 79, 59, 69, -39, -43, -76, -8, -34, -114, -102, -55, -58, 103,
            74, 106, 5, -66, -40, -60, 25, 9, 45, -29, -7, 58, -63, -47, 110, 121, -6, -18, -96, -103, -11, 122, 19,
            30, -42, -73, -71, -58, -66, 103, 55, -70, -8, -14, 39, -121, -64, 51, 121, -87, -69, 41, -98, -62, 89,
            -73, -54, -10, 93, -99, -44, -27, -101, -56, -82, -11, 105, -19, 55, -114, -97, 39, -51, -59, -49, 104, 76,
            54, 20, -99, -85, -56, -52, -2, -127, -81, -110, -119, -111, 117, 113, -29, 74, -125, -58, 101, 6, -115,
            -117, -39, 56, -76, -39, 24, 89, 24, 27, 87, -43, -78, 52, 78, 51, 104, -100, -36, -48, -40, -64, 100,
            -105, -58, -16, -33, -12, 127, -18, -115, 27, 95, 5, -86, -117, -8, 77, 46, -40, 100, 74, -20, -1, 34, 88,
            -111, -102, 104, -35, -1, 98, -101, 94, 67, -23, 82, -99, -7, 79, -102, -52, 31, 37, 110, 99, 60, -84, 20,
            126, -5, 126, -51, -121, -43, 1, 62, -111, -69, 122, 55, -50, -12, 11, 12, -67, 48, 117, -87, -105, -72,
            -64, -89, -61, -99, -7, -58, -18, -18, -94, -71, 79, 78, 29, 83, 20, -81, -102, -6, -15, 2, -49, -51, 0,
            -9, -103, 1, -127, 44, 124, 54, -22, -68, -84, 51, 23, -103, 72, -43, 101, -117, -123, 44, -39, 86, 111,
            20, 20, 23, 113, -27, -7, 86, -58, -48, -64, 15, 87, -46, 79, -119, 45, 18, -38, -30, -46, -56, -60, 32,
            -36, -60, -60, -64, -48, -52, -60, -8, -72, 5, 72, -93, 69, 36, 51, 48, 4, -19, 95, 78, -29, -83, 88, -84,
            113, -94, 120, -41, -14, -45, -115, -109, -107, -4, 102, -20, -17, 93, -17, 101, -104, -89, 106, -15, 48,
            -20, -112, -43, -28, -84, -103, -5, 93, -46, 101, -41, 39, 126, 48, -28, 77, 92, 35, 119, -22, 83, -25, 69,
            19, -50, -52, 101, -89, 127, -87, -20, -37, -94, -43, 125, 98, -34, -81, -69, -107, 60, 107, 110, -98, -15,
            105, 95, -110, 116, -1, 96, -43, -99, 16, -13, -37, -1, 19, -82, -13, 75, -2, 121, 31, 62, 107, -49, -25,
            -82, 53, 92, 95, -54, 31, -84, 96, -46, 122, -57, -109, 126, -127, -97, 105, -7, 78, -83, 27, 43, 91, -114,
            -2, 76, -27, 120, 63, -1, 92, -65, -36, -33, -52, -108, 101, -58, 119, 27, -68, 0, 80, 75, 7, 8, -45, 24,
            56, 23, 102, 2, 0, 0, 108, 2, 0, 0, 80, 75, 1, 2, 20, 0, 20, 0, 8, 0, 8, 0, 97, 108, -47, 56, 40, 96, -85,
            87, -27, 0, 0, 0, 44, 1, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 110, 100, 101,
            120, 46, 109, 101, 115, 112, 80, 75, 1, 2, 20, 0, 20, 0, 8, 0, 8, 0, 97, 108, -47, 56, 122, -64, 125, -82,
            6, 0, 0, 0, 4, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 1, 0, 0, 72, 101, 97, 100, 101, 114,
            73, 68, 46, 98, 105, 110, 80, 75, 1, 2, 20, 0, 20, 0, 8, 0, 8, 0, 97, 108, -47, 56, -4, -103, 9, 25, 6, 0,
            0, 0, 4, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 1, 0, 0, 67, 104, 105, 112, 67, 108, 97,
            115, 115, 46, 98, 105, 110, 80, 75, 1, 2, 20, 0, 20, 0, 8, 0, 8, 0, 97, 108, -47, 56, -45, 24, 56, 23, 102,
            2, 0, 0, 108, 2, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -98, 1, 0, 0, 67, 101, 114, 116, 76, 105,
            115, 116, 46, 98, 105, 110, 80, 75, 5, 6, 0, 0, 0, 0, 4, 0, 4, 0, -25, 0, 0, 0, 62, 4, 0, 0, 0, 0, -17,
            -24, -22, -30});

    private static final FakeMessage PACKET_A2_ENCRYPTION_RESPONSE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 24, 0, 0, 0, -128, -128, -109, -23, 14, 70, 14, -29, 106, -91, -84, 91, 42, 103, -126,
            17, 112, -60, -101, 110, -115, 107, -101, -36, 41, -58, 71, -109, -82, 78, 125, 105, -113, -118, 25, 35,
            -41, -11, 5, 57, -91, -54, 51, 25, -46, -44, 112, -114, -23, -97, 101, 75, 98, 121, -75, -28, 51, -60, 29,
            -39, 122, 92, 100, -122, 78, -20, -116, -120, 81, 75, 26, 77, 75, 104, -102, -63, 48, 17, 69, -81, -56,
            -84, -34, -89, -89, 117, -6, -59, 76, 24, -86, 62, -125, -24, -1, -93, 125, -2, -48, 17, 6, 19, -61, 20,
            34, -111, 24, -115, 68, -94, -78, -11, 30, -106, 86, 65, -7, -98, -80, -82, -79, 53, 45, -3, 1, 80, -42,
            59, -22, 5, -117, -112, 100});

    private static final FakeMessage PACKET_A2_ENCRYPTION_REQUEST =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 23, 0, 0, 0, 60, 13, -3, 73, -54, 95, 26, 18, 51, -10, -68, -104, 35, 46, -12, -42, 88,
            -51, 4, -60, -45, 48, 14, -52, 107, -18, 68, 49, -71, -123, -83, 51, -61, 23, -105, -57, -77, 5, 8, 97, 27,
            67, -43, -29, -94, -84, -6, -122, -8, -87, 29, 60, -7, 42, -101, -119, 97, 24, 13, -34, -106, 100, -23,
            -35, 51});

    private static final FakeMessage PACKET_HELLO = new FakeMessage(new byte[] {
        -86, -86, -86, -86, 9, 0, 0, 0, 0, -53, 50, -107, 108});

    private FakeSignServer signServer;

    private File secFile;

    /**
     * @throws Exception
     *             TBD
     */
    public void setUp() throws Exception {
        signServer = new FakeSignServer();
        secFile = File.createTempFile("secret", ".sec");
        secFile.deleteOnExit();
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(secFile);
            fos.write(SECRET);
        } finally {
            fos.close();
        }

        System.setProperty("SIGNSERVER", "localhost:" + signServer.getServerSocket().getLocalPort());
    }

    /**
     * 
     */
    public void endUp() {
        signServer.close();
        secFile.delete();
    }

    private String getResourceFilePath(String filepath) {
        return Activator.getResourcesPath() + filepath;
    }

    /**
     * @throws Throwable
     *             d
     * 
     */
    @Test
    public void testValidateInputNonExistingFile() throws Throwable {

        try {
            new A2SignerSettings(NON_EXISITNG_FILE_FILENAME, "outfile");
            fail("Should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * @throws Throwable
     *             exception
     * 
     */
    @Test
    public void testValidateInputEmptyFile() throws Throwable {
        try {
            new A2SignerSettings(getResourceFilePath(EMPTY_FILE_FILENAME), "outfile");
            fail("Should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * @throws Throwable
     *             exception
     * 
     */
    @Test
    public void testValidateInputFolderFile() throws Throwable {
        try {
            new A2SignerSettings(getResourceFilePath(DIRECTORY_FILE_FILENAME), "outfile");
            fail("Should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    private void setA2SWType(String softwareType, ISignerSettings signerSettings) throws SignerSettingsException {
        if (softwareType.equalsIgnoreCase("loader")) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE, A2AccessControlList.SW_TYPE_LOADER);
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
            signerSettings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE_TYPE, A2Signer.INPUT_FILE_TYPE_BIN);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE,
                A2AccessControlList.SW_TYPE_BOOT_CODE_OR_SW_IMAGE);

            if (softwareType.equalsIgnoreCase("image")) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_SW_IMAGE);
            } else if (softwareType.equalsIgnoreCase("elf")) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_ELF);
            } else if (softwareType.equalsIgnoreCase("generic")) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_GENERIC);
            } else if (softwareType.equalsIgnoreCase("archive")) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_ARCHIVE);
            } else if (softwareType.equals("tar")) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_TAR);
            }
        }
    }

    private void setDefaults(A2SignerSettings settings) throws SignerSettingsException {
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
    public void testSignLoader() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_LOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_LOADER_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setDefaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_ALIAS);
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

        setFakeSignProtocol();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignECLoader() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_LOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_LOADER_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setDefaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_ALIAS);
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

        setFakeSignProtocolForLoaderEncryption();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignSRecordImage() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_SRECORD_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_SRECORD_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setDefaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_ALIAS);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
            A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_APP_SEC,
            A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, A2AccessControlList.TARGET_CPU_APPLICATION);
        settings
            .setSignerSetting(A2SignerSettings.KEY_ACL_FORMAT, A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_HDR_SECURITY, A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);

        setA2SWType("image", settings);

        setFakeSignProtocol();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignTARFile() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TAR_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TAR_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setDefaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_ALIAS);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
            A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_APP_SEC,
            A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, A2AccessControlList.TARGET_CPU_APPLICATION);
        settings
            .setSignerSetting(A2SignerSettings.KEY_ACL_FORMAT, A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_HDR_SECURITY, A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);

        setA2SWType("tar", settings);

        setFakeSignProtocol();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignELFFileAsImage() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ELF_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ELF_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setDefaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_ALIAS);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
            A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_APP_SEC,
            A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, A2AccessControlList.TARGET_CPU_APPLICATION);
        settings
            .setSignerSetting(A2SignerSettings.KEY_ACL_FORMAT, A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_HDR_SECURITY, A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);

        setA2SWType("image", settings);

        setFakeSignProtocol();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignELFFileAsELF() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ELF_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ELF_FILENAME);
        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setDefaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_ALIAS);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_ELF);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
            A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_APP_SEC,
            A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, A2AccessControlList.TARGET_CPU_APPLICATION);
        settings
            .setSignerSetting(A2SignerSettings.KEY_ACL_FORMAT, A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_HDR_SECURITY, A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);

        setA2SWType("elf", settings);

        setFakeSignProtocol();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignELFFileAsELFWithSections() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ELF_WITH_SECTIONS_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ELF_WITH_SECTIONS_FILENAME);

        A2SignerSettings settings = new A2SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        setDefaults(settings);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, "app");
        settings.setSignerSetting(A2SignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_ALIAS);
        settings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
            A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_APP_SEC,
            A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_TARGET_CPU, A2AccessControlList.TARGET_CPU_APPLICATION);
        settings
            .setSignerSetting(A2SignerSettings.KEY_ACL_FORMAT, A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        settings.setSignerSetting(A2SignerSettings.KEY_ACL_HDR_SECURITY, A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);

        setA2SWType("elf", settings);

        setFakeSignProtocol();
        testSigning(settings);
        endUp();
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
        signServer.startServer();
        try {
            facade.sign(settings, null, false);
        } catch (Exception e) {
            fail(e.getMessage());
            e.printStackTrace();
        }

        assertTrue(signedFile.exists());
        assertTrue(signedFile.length() > 0);

        signedFile.delete();
        assertFalse(signedFile.exists());
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testListSignPackages() throws Throwable {
        setUp();
        setFakeSignProtocolForListPackages();
        signServer.startServer();
        assertNotNull(new SignerFacade().getSignPackages(null, false, false, (ISignerSettings) null));
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testListSignPackagesCommonFolder() throws Throwable {
        setUp();
        setFakeSignProtocolForListPackages();
        signServer.startServer();
        assertNotNull(new SignerFacade().getSignPackages(null, false, true, (ISignerSettings) null));
        endUp();
    }

    private void setFakeSignProtocolForListPackages() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_KEYCATALOGUE_REQUEST, new byte[0]).toArray()),
                new FakeMessage(createPacket(COMMAND_KEYCATALOGUE_RESPONSE, getSignPackageMESP().getBytes("UTF-8"))
                    .toArray()), 100)});
    }

    private void setFakeSignProtocol() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_ALIAS.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 100),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                100),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 100),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_SIGN_PACKAGE, 100),
            new MockProtocolMessage(PACKET_A2_ENCRYPTION_REQUEST, PACKET_A2_ENCRYPTION_RESPONSE, 100),});
    }

    private void setFakeSignProtocolForLoaderEncryption() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_ALIAS.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 100),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                100),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 100),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_SIGN_PACKAGE, 100),

            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_KEYCATALOGUE_REQUEST, new byte[0]).toArray()),
                new FakeMessage(createPacket(COMMAND_KEYCATALOGUE_RESPONSE, getSignPackageMESP().getBytes("UTF-8"))
                    .toArray()), 100),

            new MockProtocolMessage(PACKET_A2_ENCRYPTION_REQUEST, PACKET_A2_ENCRYPTION_RESPONSE, 100),});
    }

    private String getSignPackageMESP() {
        StringBuffer buf = new StringBuffer();
        buf.append("EMPHeaderPackage alias=\"CUST_19_(0)_TietoEnatorKarlstad_DB3210_DEVELOPMENT\" path=");
        buf.append("\"" + secFile.getAbsolutePath() + "\" ");
        buf.append("ENCRYPT_REQUIRED=\"true\" revision=\"R1A\" type=\"COPS_A2\" version=\"-1161952768\" "
            + "ETX_ENABLED=\"true\" JTAG_ENABLED_ACC=\"true\" JTAG_ENABLED_APP=\"true\" SIGN_ACCESS_SW=\"true\" "
            + "colors[]={\"BLUE\" \"BROWN\"}");
        return buf.toString();
    }
}
