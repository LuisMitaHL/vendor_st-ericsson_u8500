package com.stericsson.sdk.signing.l9540;

import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_HEADERPACKAGE_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_KEYCATALOGUE_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_LOGIN_ACCEPT;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_LOGIN_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_RANDOMNUMBER_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_SECRET_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_SECRET_RESPONSE;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_SUB_PACKAGE_BY_KEY_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketFactory.createPacket;

import java.io.File;
import java.io.FileOutputStream;
import java.util.LinkedList;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.mockup.FakeMessage;
import com.stericsson.sdk.signing.mockup.FakeSignServer;
import com.stericsson.sdk.signing.mockup.MockProtocolMessage;
import com.stericsson.sdk.signing.test.Activator;

/**
 * @author - Goran Gjorgoski(xxvs0005)  <Goran.Gjorgoski@seavus.com>
 *
 */
public class L9540SignTest extends TestCase {

    private static final String SIGN_PACKAGE_U5500_ROOT = "xolabju_u5500";

    private static final String SIGN_PACKAGE_U5500_AUTH = "xolabju_u5500_auth";

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String EMPTY_FILE_FILENAME = "/a2/empty.bin";

    private static final String DIRECTORY_FILE_FILENAME = "/";

    private static final String UNSIGNED_ISSW_FILENAME = "/u8500/unsigned_cust_u8500_issw.bin";

    private static final String SIGNED_ISSW_FILENAME = "signed_issw.bin";

    private static final String UNSIGNED_PRCMU_FILENAME = "/u8500/unsigned_prcmu.bin";

    private static final String SIGNED_PRCMU_FILENAME = "signed_prcmu.bin";

    private static final String UNSIGNED_MEM_INIT_FILENAME = "/u8500/unsigned_mem_init.bin";

    private static final String SIGNED_MEM_INIT_FILENAME = "signed_mem_init.bin";

    private static final String UNSIGNED_XLOADER_FILENAME = "/u8500/unsigned_xloader.bin";

    private static final String SIGNED_XLOADER_FILENAME = "signed_xloader.bin";

    private static final String UNSIGNED_OSLOADER_FILENAME = "/u8500/unsigned_osloader.bin";

    private static final String SIGNED_OSLOADER_FILENAME = "signed_osloader.bin";

    private static final String UNSIGNED_APE_NORMAL_FILENAME = "/u8500/unsigned_ape_normal.bin";

    private static final String SIGNED_APE_NORMAL_FILENAME = "signed_ape_normal.bin";

    private static final String UNSIGNED_LDR_FILENAME = "/u8500/unsigned_ldr.bin";

    private static final String SIGNED_LDR_FILENAME = "signed_ldr.bin";

    private static final String UNSIGNED_MODEM_ELF_FILENAME = "/u8500/unsigned_modem.elf";

    private static final String SIGNED_MODEM_ELF_FILENAME = "signed_modem.elf";

    private static final String UNSIGNED_FOTA_FILENAME = "/u8500/unsigned_fota.zip";

    private static final String SIGNED_FOTA_FILENAME = "signed_fota.zip";

    private static final String UNSIGNED_DNT_FILENAME = "/u8500/unsigned_dnt.bin";

    private static final String SIGNED_DNT_FILENAME = "signed_dnt.bin";

    private static final String UNSIGNED_IPL_FILENAME = "/u8500/unsigned_ipl.bin";

    private static final String SIGNED_TRUSTED_FILENAME = "signed_trusted.bin";

    private static final String UNSIGNED_TRUSTED_FILENAME = "/u8500/unsigned_trusted.bin";

    private static final String SIGNED_FLASH_ARCHIVE_FILENAME = "signed_flash_archive.zip";

    private static final String UNSIGNED_FLASH_ARCHIVE_FILENAME = "/u8500/unsigned_flasharchive.zip";

    private static final String PRESIGNED_FLASH_ARCHIVE_FILENAME = "/u8500/flasharchive.zip";

    private static final String SIGNED_IPL_FILENAME = "signed_ipl.bin";

    private static final String UNSIGNED_AUTH_CERT_FILENAME = "/u8500/unsigned_authcert.bin";

    private static final String SIGNED_AUTH_CERT_FILENAME = "signed_authcert.bin";

    private static final byte[] SECRET = new byte[] {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    private static final String UNSIGNED_AUTH_CHALLANGE_FILENAME = "/u8500/unsigned_auth_challange.bin";

    private static final String SIGNED_AUTH_CHALLANGE_FILENAME = "signed_auth_challange.bin";

    private static final FakeMessage PACKET_ROOT_SIGN_PACKAGE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 20, 0, 0, 3, 60, 80, 75, 3, 4, 20, 0, 0, 0, 8, 0, -126, -125, 112, 61, -116, -113, 59,
            -12, -55, 1, 0, 0, 76, 3, 0, 0, 15, 0, 0, 0, 67, 101, 114, 116, 105, 102, 105, 99, 97, 116, 101, 46, 98,
            105, 110, 11, 15, 14, -10, -28, 96, 100, 96, -40, -61, -128, 0, 74, 86, -107, 98, 12, -93, 0, 14, 24, 105,
            104, 46, 2, -125, -56, 6, -122, -73, 110, -21, 60, 54, -36, 121, -9, 126, -10, -125, -77, -115, -33, -114,
            10, -26, 51, -52, 119, -56, 16, -36, 18, -10, -12, -37, 10, -105, -43, 44, 66, 119, 31, 60, -72, 58, 123,
            -37, 59, -10, 119, 75, -38, -70, -71, -10, -20, 13, -113, -53, 8, -111, -116, -105, -68, 110, 60, 51, -47,
            81, 103, -2, 52, -122, -52, -117, -62, 83, -9, -78, 58, -89, 94, 88, -53, 108, 121, 98, -10, -19, -117, 59,
            34, -81, -80, -90, -82, -101, -50, -25, 59, 83, -101, -31, -50, -67, -24, 5, -81, -1, -36, -112, -100, -5,
            -64, 120, -111, 118, -75, -87, -41, 26, -114, 58, -99, -117, 51, 84, 121, -54, -51, 31, -86, -35, 14, 127,
            126, -24, -41, -114, -89, -97, 85, 75, 66, 87, 109, -114, 56, -106, -70, -20, -79, -87, 71, 110, 43, -1, 5,
            -31, -101, -101, -42, -122, -58, 48, 78, -8, -4, -89, 35, 111, -65, -60, -7, -118, -45, 57, 77, 2, 70,
            -116, 34, 87, 95, -66, 21, -42, -33, 119, -49, -15, -106, -27, -110, 73, 63, -99, -45, -78, 67, -49, -12,
            -91, 48, -1, -79, 62, 90, -41, -34, -14, 118, -41, -49, -34, -60, -5, 103, 46, 108, 43, 122, -11, -104, 95,
            -15, 55, 67, -124, -73, -106, -74, -34, -122, -106, 6, -121, 93, -27, 75, -25, -65, -84, -99, 127, 35, -10,
            -5, -25, 95, 6, 77, 71, -2, -15, 31, 93, -42, -86, -58, -45, 32, 99, -80, 121, -113, -37, -14, -116, -71,
            -116, -23, 38, -113, -12, 39, 125, -1, 121, -48, 57, 52, 56, 100, 2, 60, -52, 17, 33, -49, -120, 12, -96,
            98, 29, 40, 97, -25, -64, 48, 121, -18, 23, -93, 70, -26, -118, -113, -4, 90, -27, 74, -54, 14, -99, 34,
            -13, -103, 47, 100, 55, 79, -19, 114, -99, -53, -70, -59, -3, -7, -83, 35, -102, -78, -59, -85, -93, 79,
            108, -34, -65, 115, -38, -99, -87, -119, 14, -26, 13, -5, -110, 126, 87, 101, -83, 12, -47, -67, -75, -2,
            -54, -53, -121, 86, 37, -110, 5, -127, 101, 83, 56, 123, -125, 111, 116, -85, -19, -6, -71, -29, -99, 109,
            -20, -2, -98, -26, -36, 73, 107, -41, -55, -90, 63, 113, 12, -32, -117, 125, 120, 101, -35, -20, 109, 111,
            77, 38, -2, 59, -72, 87, -67, -4, 104, -45, -45, 41, -33, 23, -80, 74, 47, 76, 84, -30, -71, 45, -59, 126,
            -119, -35, 57, -87, 107, -26, 49, 0, 80, 75, 3, 4, 20, 0, 0, 0, 8, 0, 119, 86, -41, 60, -55, 127, -24, -60,
            -109, 0, 0, 0, -75, 0, 0, 0, 10, 0, 0, 0, 105, 110, 100, 101, 120, 46, 109, 101, 115, 112, 77, -115, 65,
            14, -126, 48, 20, 68, -9, -100, -30, -25, 31, -64, -76, 104, 99, 66, -46, -123, 18, 55, 110, 32, -120, 113,
            73, 74, -7, -110, 10, 22, -46, 22, 35, -73, 23, 93, 16, -105, -13, 50, 111, 38, 87, -70, 83, 45, -127, -22,
            -115, -14, 18, -33, 67, -81, -22, -57, 84, 77, 66, 48, -122, 16, -26, -111, 36, 94, -65, -95, 42, -78, -84,
            68, -48, -114, 84, -96, 70, -30, -115, 26, 56, 79, 22, -30, 45, 112, -106, 8, -98, -20, -10, -112, -98, 46,
            37, -60, -116, -77, -75, 119, -100, -41, 77, 4, 71, 47, -29, -51, 96, 37, 22, -4, -128, -48, -112, -41,
            -50, -116, -31, 71, 74, -14, 1, -14, -82, -59, 40, 37, 23, -52, -35, -24, 69, 7, -85, -98, -53, -1, 31,
            -39, -44, -58, 98, -12, 1, 80, 75, 1, 2, 20, 0, 20, 0, 0, 0, 8, 0, -126, -125, 112, 61, -116, -113, 59,
            -12, -55, 1, 0, 0, 76, 3, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 67, 101, 114, 116,
            105, 102, 105, 99, 97, 116, 101, 46, 98, 105, 110, 80, 75, 1, 2, 20, 0, 20, 0, 0, 0, 8, 0, 119, 86, -41,
            60, -55, 127, -24, -60, -109, 0, 0, 0, -75, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 1, 0, -128, 0, 0, 0, -10, 1,
            0, 0, 105, 110, 100, 101, 120, 46, 109, 101, 115, 112, 80, 75, 5, 6, 0, 0, 0, 0, 2, 0, 2, 0, 117, 0, 0, 0,
            -79, 2, 0, 0, 0, 0, -62, 17, 102, 108});

    private static final FakeMessage PACKET_U5500_ENCRYPTION_RESPONSE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 40, 0, 0, 1, 0, 119, -24, -70, 18, -23, 120, -89, 109, 84, -107, -58, 126, 24, -99, 1,
            18, -8, 13, -36, -50, 80, 100, 68, -105, 82, -108, 14, 67, -65, -83, 34, -96, 53, 127, 92, 41, -100, -54,
            -12, 84, 115, 61, -27, 76, 6, 103, -59, -112, 113, -98, 73, 26, -102, -123, -106, 107, -69, 102, 105, -118,
            83, 96, -42, -34, -73, -47, 110, -90, 57, 80, 112, 14, 17, -5, 34, -71, 76, 39, -24, 88, -116, 82, -56,
            100, 114, -110, 5, -84, -32, -100, -85, 40, 25, -124, -31, 110, -57, -4, -76, 85, 23, -124, 108, 71, -48,
            97, -15, -47, 120, 119, 114, -110, -82, -118, 29, -56, -36, -79, -58, 0, 122, -74, -110, 83, -77, 89, 21,
            123, 89, 59, -36, 102, -1, -53, 122, -90, 125, -3, -102, -43, 88, -45, 125, -14, 56, 61, -43, -49, -87,
            -70, -34, 85, -37, -118, -92, 50, -56, 70, 51, -89, 7, 20, 49, 75, 47, 51, 57, -112, 112, -106, -26, -84,
            58, 26, 101, -84, 123, 17, -8, -83, -19, -98, 82, -84, 19, -106, 105, -83, -83, 43, 126, 118, 82, -70, 60,
            -64, 96, -12, -83, 55, -18, -24, 112, -86, -97, 104, 41, 67, 86, -34, 28, -46, 117, -90, -49, -61, 74, 87,
            91, 36, 59, -42, 120, -44, -45, 112, 17, -13, -68, 42, -15, 75, 77, 8, -17, 107, -128, -70, 45, -47, -96,
            61, 32, -10, -52, 69, -90, -66, -103, 63, -27, 57, 103, -26, -69, -4, -57, -30, 44, 75});

    private static final FakeMessage PACKET_U5500_ENCRYPTION_REQUEST =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 35, 0, 0, 5, -88, 87, 83, 83, 73, 8, 1, 0, 0, 24, 3, 0, 0, -108, 5, 0, 0, 0, 1, 0, 0,
            34, 58, 121, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -96, -35, 100, -67, -83, 111,
            -48, -42, -67, 76, -53, 15, 118, -74, 54, 35, 110, -68, 122, -80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -48, 28, 0, 0,
            36, 0, 0, 0, -108, 1, 0, 0, 0, -128, 0, 64, -95, -123, 0, 64, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0,
            0, 0, 0, -128, 0, 17, -127, -60, 46, -102, -96, -54, 3, 124, 37, -30, 57, -98, -45, 39, 87, -10, -125, -40,
            118, 121, -33, -97, 47, -45, -77, 10, -111, 16, 61, -87, 39, -15, -112, -102, -90, 18, -26, -13, -56, 6,
            62, -72, 73, 51, -30, 119, -94, -26, 82, 22, -64, -92, 99, -11, 104, -27, 29, -127, -32, 117, 63, -16, 88,
            -65, -30, 92, 34, -7, 111, 22, -1, -61, 15, 86, -67, -41, 111, 40, -44, -104, -22, -111, -35, 85, -112, 62,
            -112, -43, 37, 0, -11, -25, 69, -87, -61, 44, 83, -12, 46, 2, 106, -56, -11, 54, -41, -126, -65, -56, 101,
            -57, -68, -6, -41, 33, 97, -9, 124, 64, 80, 93, 40, -71, 60, -126, 112, 59, 31, 58, -52, -46, -26, -61, 34,
            81, 53, 95, -67, 116, -90, 105, 66, 126, -25, 61, 101, -56, 94, -120, -104, 85, 60, 26, -92, 121, -120,
            -119, -57, 26, 102, 115, 51, -79, 110, 105, -84, -70, 5, 111, 55, 46, 125, -63, 99, -9, -22, -104, -45,
            -98, 45, -32, 109, 80, -20, -46, 93, 3, 14, 46, 41, 51, 118, 84, 20, 25, 6, -127, -68, -1, -116, -55, 21,
            109, -78, -28, -50, -78, 30, 73, 32, 120, -23, 86, -122, 63, 60, 115, 106, -23, -19, -69, -65, 20, -23, 11,
            -56, 9, -19, -77, 99, -118, -33, 109, -128, 13, 95, -94, 118, -112, -122, -40, -63, -87, -29, -93, -62, -5,
            109, -89, -98, -64, 28, -48, -15, -7, -28, 67, 85, 83, 84, 3, 0, 0, 0, 1, 0, -123, 0, 0, 0, 0, 0, 0, 0, 4,
            7, 2, 1, 0, 8, 8, 1, 0, 0, -120, 0, 0, 0, -120, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, -128, 0, 9, -8,
            -91, -72, 29, 24, 19, 17, -51, -98, 79, 42, -48, 26, 101, 90, 43, 110, 108, -31, 45, -71, 95, -108, 77,
            -91, 14, -26, -126, 26, -30, -56, -104, 99, -80, -119, -31, -126, -115, 109, 44, -60, -105, -23, 3, -20,
            -82, -32, -21, 115, -98, -74, 48, 86, -54, -25, 109, 59, 66, -107, -96, -103, 62, -118, -25, -7, -21, -33,
            109, 53, -4, -113, -61, -19, -68, 125, -79, -57, 123, -57, 12, 64, -112, -89, 0, 62, -34, 48, -21, -32,
            -115, -13, 38, -66, -44, 106, -106, -68, -29, -17, -47, -55, 76, -116, -8, 101, 112, 13, -55, 1, -73, 105,
            -46, -117, 81, -95, -80, -35, -78, -67, 27, 29, -42, 79, -33, 2, -45, -74, 69, -79, 119, 60, 0, 116, 78,
            -97, 36, -102, -107, -77, 17, 12, -42, -32, 70, -54, 56, -74, -57, 19, -99, 105, -59, -58, 49, -37, -125,
            -124, -12, -18, 51, 63, 37, -118, 66, 73, -83, -59, 59, 75, -52, 13, -42, -4, 111, 108, 94, 25, -119, -33,
            -99, 104, 8, 35, -49, 0, -55, -83, 118, -34, -114, 123, -102, -19, 80, 123, -86, -96, 17, 100, -58, 117,
            24, 74, -1, -117, -109, 0, 8, -47, 10, 113, -39, -18, 46, 24, -44, -38, -2, 52, 52, 63, 7, 7, 78, 119, 21,
            -50, 92, -42, 41, -33, 70, -39, -126, 29, 81, 29, 45, -118, -18, -39, 66, 50, 116, -60, 83, -99, 61, 76,
            111, 89, -37, -73, -97, -62, 3, 0, 0, 0, 0, 0, 64, 0, 5, -51, -66, 71, 12, 70, -67, -1, -61, -72, -9, -99,
            92, -9, -44, 9, 109, 55, -56, -117, -90, 19, -14, -71, -2, -71, -67, 77, -45, -19, -94, -27, -3, -21, -76,
            -87, 21, 89, -119, -19, -76, -2, 36, 79, -62, -7, -123, 73, -127, -52, 119, 5, 118, 43, -72, -127, 75, 108,
            114, -10, 13, -116, 38, -30, -64, 121, -128, 19, 62, -82, -91, 86, -88, -113, 66, -57, -46, 59, -64, 69,
            -86, -15, -3, -108, -33, 45, 0, -112, 114, 69, -118, -118, 29, 91, -89, -15, 120, 0, -45, -117, 69, 36, 86,
            46, 76, 38, 47, -9, 10, 13, -66, -126, -90, 41, -12, -1, 49, -99, 92, -93, -85, 25, -34, -75, 95, -74, 34,
            -66, 3, 0, 0, 0, 0, 0, 64, 0, -7, 47, 59, 53, -40, -50, 103, -1, -5, 57, 11, 24, 90, -59, 94, 71, -85, -11,
            -87, -109, 68, 58, -79, -1, 97, -54, 100, 82, -117, -48, 103, -46, -36, 115, -28, -47, 61, -105, 6, 80,
            -48, 70, 120, 70, 33, -6, -17, 41, 99, 106, 89, -80, 37, 2, -83, 119, 120, -52, -24, -86, 55, 92, 87, -16,
            76, 44, -2, 81, 113, -106, -36, -103, -18, -70, 18, -85, -123, -38, 9, 9, 62, 91, 53, 120, -108, -43, 25,
            -54, -69, -12, 105, -85, 12, 89, 114, -64, 22, -38, 24, -35, 62, -18, 32, 48, 16, 13, -105, -61, -118, 41,
            90, -103, 100, 114, 124, -120, -58, 73, -11, 126, 87, -80, -9, -25, 70, 69, 8, -17, 126, 56, 114, -97});

    private static final FakeMessage PACKET_HELLO = new FakeMessage(new byte[] {
        -86, -86, -86, -86, 9, 0, 0, 0, 0, -53, 50, -107, 108});

    private static final FakeMessage PACKET_CATALOGUE_RESPONSE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 14, 0, 0, 2, -68, 69, 77, 80, 72, 101, 97, 100, 101, 114, 80, 97, 99, 107, 97, 103,
            101, 32, 97, 108, 105, 97, 115, 61, 34, 120, 111, 108, 97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 34, 32,
            112, 97, 116, 104, 61, 34, 92, 92, 119, 119, 48, 49, 51, 55, 52, 52, 92, 83, 105, 103, 110, 83, 101, 114,
            118, 101, 114, 83, 116, 111, 114, 97, 103, 101, 92, 112, 97, 99, 107, 97, 103, 101, 115, 92, 120, 111, 108,
            97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 95, 114, 111, 111, 116, 34, 32, 69, 78, 67, 82, 89, 80, 84, 95,
            82, 69, 81, 85, 73, 82, 69, 68, 61, 34, 102, 97, 108, 115, 101, 34, 32, 114, 101, 118, 105, 115, 105, 111,
            110, 61, 34, 82, 49, 65, 34, 32, 100, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 61, 34, 85, 56, 53,
            48, 48, 32, 114, 111, 111, 116, 32, 115, 105, 103, 110, 32, 112, 97, 99, 107, 97, 103, 101, 34, 32, 116,
            121, 112, 101, 61, 34, 85, 56, 53, 48, 48, 95, 82, 79, 79, 84, 34, 32, 116, 121, 112, 101, 61, 34, 85, 56,
            53, 48, 48, 95, 82, 79, 79, 84, 34, 13, 10, 69, 77, 80, 72, 101, 97, 100, 101, 114, 80, 97, 99, 107, 97,
            103, 101, 32, 97, 108, 105, 97, 115, 61, 34, 120, 111, 108, 97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 95,
            111, 101, 109, 49, 34, 32, 112, 97, 116, 104, 61, 34, 92, 92, 119, 119, 48, 49, 51, 55, 52, 52, 92, 83,
            105, 103, 110, 83, 101, 114, 118, 101, 114, 83, 116, 111, 114, 97, 103, 101, 92, 112, 97, 99, 107, 97, 103,
            101, 115, 92, 120, 111, 108, 97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 95, 111, 101, 109, 49, 34, 32, 69,
            78, 67, 82, 89, 80, 84, 95, 82, 69, 81, 85, 73, 82, 69, 68, 61, 34, 102, 97, 108, 115, 101, 34, 32, 114,
            101, 118, 105, 115, 105, 111, 110, 61, 34, 82, 49, 65, 34, 32, 100, 101, 115, 99, 114, 105, 112, 116, 105,
            111, 110, 61, 34, 85, 56, 53, 48, 48, 32, 79, 69, 77, 49, 32, 112, 97, 99, 107, 97, 103, 101, 34, 32, 116,
            121, 112, 101, 61, 34, 85, 56, 53, 48, 48, 95, 79, 69, 77, 49, 34, 32, 112, 97, 114, 101, 110, 116, 61, 34,
            120, 111, 108, 97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 34, 32, 116, 121, 112, 101, 61, 34, 85, 56, 53,
            48, 48, 95, 79, 69, 77, 49, 34, 13, 10, 69, 77, 80, 72, 101, 97, 100, 101, 114, 80, 97, 99, 107, 97, 103,
            101, 32, 97, 108, 105, 97, 115, 61, 34, 120, 111, 108, 97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 95, 111,
            101, 109, 50, 34, 32, 112, 97, 116, 104, 61, 34, 92, 92, 119, 119, 48, 49, 51, 55, 52, 52, 92, 83, 105,
            103, 110, 83, 101, 114, 118, 101, 114, 83, 116, 111, 114, 97, 103, 101, 92, 112, 97, 99, 107, 97, 103, 101,
            115, 92, 120, 111, 108, 97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 95, 111, 101, 109, 50, 34, 32, 69, 78,
            67, 82, 89, 80, 84, 95, 82, 69, 81, 85, 73, 82, 69, 68, 61, 34, 102, 97, 108, 115, 101, 34, 32, 114, 101,
            118, 105, 115, 105, 111, 110, 61, 34, 82, 49, 65, 34, 32, 100, 101, 115, 99, 114, 105, 112, 116, 105, 111,
            110, 61, 34, 85, 56, 53, 48, 48, 32, 79, 69, 77, 50, 32, 112, 97, 99, 107, 97, 103, 101, 34, 32, 116, 121,
            112, 101, 61, 34, 85, 56, 53, 48, 48, 95, 79, 69, 77, 50, 34, 32, 112, 97, 114, 101, 110, 116, 61, 34, 120,
            111, 108, 97, 98, 106, 117, 95, 117, 56, 53, 48, 48, 34, 32, 116, 121, 112, 101, 61, 34, 85, 56, 53, 48,
            48, 95, 79, 69, 77, 50, 34, 13, 10, 8, -48, -86, 114});

    private static final FakeMessage PACKET_SUB_SIGN_PACKAGE_BY_KEY_RESPONSE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 38, 0, 0, 0, 23, 120, 111, 108, 97, 98, 106, 117, 95, 117, 53, 53, 48, 48, 95, 103,
            101, 110, 101, 114, 105, 99, 95, 49, -13, 124, 61, -59});

    private static final FakeMessage PACKET_SUB_SIGN_PACKAGE_BY_KEY_RESPONSE_COMMON_FOLDER =
            new FakeMessage(new byte[]{-86, -86, -86, -86, 38, 0, 0, 0, 4, 116, 114, 117, 101, 62, -38, -38, -42});

    private static final FakeMessage PACKET_SUB_SIGN_PACKAGE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 20, 0, 0, 1, -98, 80, 75, 3, 4, 20, 0, 0, 0, 8, 0, -128, 91, -41, 60, 31, 54, 41, -22,
            -91, 0, 0, 0, -37, 0, 0, 0, 10, 0, 0, 0, 105, 110, 100, 101, 120, 46, 109, 101, 115, 112, 93, -114, -63,
            10, -126, 64, 20, 69, -9, 65, -1, -16, 120, 31, 16, 51, -106, 4, -62, 44, 74, 36, 106, 17, 98, 70, 75,
            -103, -58, -105, 76, -38, 40, 51, 99, -28, -33, 39, 46, 36, -38, 30, 14, -9, -98, 84, -86, 90, 86, 4, -78,
            -47, -46, 9, -4, -76, -115, -68, 63, -5, -94, 15, 67, -58, -118, -118, 12, 89, -83, 10, -114, -32, -121,
            -114, 4, 94, 39, 124, 72, -50, 73, 118, -116, 17, -108, 37, -23, -87, 20, 120, -93, 18, 78, -67, -127, 96,
            13, -100, 69, 33, -113, 54, 91, -120, -109, 75, 14, 1, -29, 108, -10, -10, -61, 124, -128, 96, -23, -83,
            -99, 110, -115, -64, -116, -17, 16, 74, 114, -54, -22, -50, 79, 36, 39, -25, 33, -83, 43, -124, 78, 90, 50,
            -2, -81, 11, -105, -117, -104, -84, -41, 15, -83, -58, 89, 48, -14, 53, -90, -3, -112, -43, 93, -101, -47,
            -7, 2, 80, 75, 3, 4, 20, 0, 8, 0, 8, 0, 119, 86, -41, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0,
            67, 101, 114, 116, 105, 102, 105, 99, 97, 116, 101, 46, 98, 105, 110, 11, 114, -15, 8, 102, 32, 1, 0, 0,
            80, 75, 7, 8, 77, 45, -24, -13, 9, 0, 0, 0, 48, 0, 0, 0, 80, 75, 1, 2, 20, 0, 20, 0, 0, 0, 8, 0, -128, 91,
            -41, 60, 31, 54, 41, -22, -91, 0, 0, 0, -37, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 0,
            0, 105, 110, 100, 101, 120, 46, 109, 101, 115, 112, 80, 75, 1, 2, 20, 0, 20, 0, 8, 0, 8, 0, 119, 86, -41,
            60, 77, 45, -24, -13, 9, 0, 0, 0, 48, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -51, 0, 0, 0, 67,
            101, 114, 116, 105, 102, 105, 99, 97, 116, 101, 46, 98, 105, 110, 80, 75, 5, 6, 0, 0, 0, 0, 2, 0, 2, 0,
            117, 0, 0, 0, 19, 1, 0, 0, 0, 0, -85, 113, 81, 89});

    private static final FakeMessage PACKET_AUTH_SIGN_PACKAGE =
        new FakeMessage(new byte[] {
            -86, -86, -86, -86, 20, 0, 0, 2, 17, 80, 75, 3, 4, 20, 0, 0, 0, 8, 0, 69, 123, -35, 60, -44, 7, 71, -73,
            -109, 0, 0, 0, -70, 0, 0, 0, 10, 0, 0, 0, 105, 110, 100, 101, 120, 46, 109, 101, 115, 112, 77, -51, -53,
            10, -126, 64, 20, 6, -32, 125, 79, 113, 56, 15, 16, 51, -126, 65, -62, 44, 76, -126, 104, 37, 53, -82, 101,
            28, 79, 54, 105, -93, -52, 37, -14, -19, 19, 23, -46, -10, -29, -65, -108, 74, -9, -86, 35, 80, -125, 81,
            94, -32, 119, 28, 84, -13, -118, 117, 76, 83, -58, 106, 21, -61, 19, 33, -52, 19, 9, -84, 86, -55, 43, 121,
            65, -48, -114, 84, -96, 86, -96, -116, 4, -41, 104, 33, 57, 2, 79, -77, -28, -112, 113, 14, -59, -7, 46,
            33, 97, -100, 109, -71, -45, -68, 13, 35, 56, -6, 24, 111, 70, 43, -16, -58, 115, -124, -106, -68, 118,
            102, 10, -85, 72, -14, 1, -54, -66, -61, 93, 65, 46, -104, -121, -47, 75, 29, -84, 122, 47, -1, 127, -78,
            111, -116, -59, -35, 15, 80, 75, 3, 4, 20, 0, 0, 0, 8, 0, -80, 125, -6, 60, 48, -91, 106, 34, -98, 0, 0, 0,
            -76, 0, 0, 0, 15, 0, 0, 0, 67, 101, 114, 116, 105, 102, 105, 99, 97, 116, 101, 46, 98, 105, 110, 11, 9,
            114, 118, 100, 64, 2, -105, 88, 16, 108, -104, 68, 7, 16, 51, -126, 49, -120, 116, 96, -104, 60, -9, -117,
            81, 35, 115, -59, 71, 126, -83, 114, 37, 101, -121, 78, -111, -7, -52, 23, -78, -101, -89, 118, -71, -50,
            101, -35, -30, -2, -4, -42, 17, 77, -39, -30, -43, -47, 39, 54, -17, -33, 57, -19, -50, -44, 68, 7, -13,
            -122, 125, 73, -65, -85, -78, 86, -122, -24, -34, 90, 127, -27, -27, 67, -85, 18, -55, -126, -64, -78, 41,
            -100, -67, -63, 55, -70, -43, 118, -3, -36, -15, -50, 54, 118, 127, 79, 115, -18, -92, -75, -21, 100, -45,
            -97, 56, 6, -16, -59, 62, -68, -78, 110, -10, -74, -73, 38, 19, -1, 29, -36, -85, 94, 126, -76, -23, -23,
            -108, -17, 11, 88, -91, 23, 38, 42, -15, -36, -106, 98, -65, -60, -18, -100, -44, 53, -13, 24, 0, 80, 75,
            1, 2, 20, 0, 20, 0, 0, 0, 8, 0, 69, 123, -35, 60, -44, 7, 71, -73, -109, 0, 0, 0, -70, 0, 0, 0, 10, 0, 0,
            0, 0, 0, 0, 0, 1, 0, -128, 0, 0, 0, 0, 0, 0, 0, 105, 110, 100, 101, 120, 46, 109, 101, 115, 112, 80, 75, 1,
            2, 20, 0, 20, 0, 0, 0, 8, 0, -80, 125, -6, 60, 48, -91, 106, 34, -98, 0, 0, 0, -76, 0, 0, 0, 15, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 32, 0, 0, 0, -69, 0, 0, 0, 67, 101, 114, 116, 105, 102, 105, 99, 97, 116, 101, 46, 98,
            105, 110, 80, 75, 5, 6, 0, 0, 0, 0, 2, 0, 2, 0, 117, 0, 0, 0, -122, 1, 0, 0, 0, 0, -26, -117, 85, -28});

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
     *             exception
     *
     */
    @Test
    public void testValidateInputNonExistingFile() throws Throwable {

        try {
            new L9540SignerSettings(NON_EXISITNG_FILE_FILENAME, "outfile");
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
            new L9540SignerSettings(getResourceFilePath(EMPTY_FILE_FILENAME), "outfile");
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
            new L9540SignerSettings(getResourceFilePath(DIRECTORY_FILE_FILENAME), "outfile");
            fail("Should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignISSW() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ISSW_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ISSW_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ISSW);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        setFakeSignProtocol();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignISSWCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ISSW_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ISSW_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.ISSW);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        setFakeSignProtocolCommonFolder();
        testSigning(settings);
        endUp();
    }

    /**
     *
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignAuthChallenge() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_AUTH_CHALLANGE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_AUTH_CHALLANGE_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_AUTH);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.AUTH_CHALLENGE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        setFakeSignProtocolForAuthSigning();
        testSigning(settings);
        endUp();
    }

    /**
     *
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignAuthChallengeCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_AUTH_CHALLANGE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_AUTH_CHALLANGE_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_AUTH);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.AUTH_CHALLENGE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        setFakeSignProtocolForAuthSigning();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     *
     */
    @Test
    public void testSignGenericSoftwareFormat() throws Throwable {
        File unsignedFile;
        File signedFile;
        LinkedList<L9540SignerSettings> testSettingsList = new LinkedList<L9540SignerSettings>();
        L9540SignerSettings settings = null;

        unsignedFile = new File(getResourceFilePath(UNSIGNED_PRCMU_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_PRCMU_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.PRCMU);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_MEM_INIT_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_MEM_INIT_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.MEM_INIT);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_XLOADER_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_XLOADER_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.XLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_OSLOADER_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_OSLOADER_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.OSLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_APE_NORMAL_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_APE_NORMAL_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.APE_NORMAL);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_LDR_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_LDR_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.LDR);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_MODEM_ELF_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_MODEM_ELF_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.MODEM);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_FOTA_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_FOTA_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FOTA);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_DNT_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_DNT_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.DNT);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_IPL_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_IPL_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.IPL);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);
        testSettingsList.add(settings);

        for (L9540SignerSettings testSettings : testSettingsList) {
            setUp();
            signServer.joinServerSocketThread();
            setFakeSignProtocolForGenericSigning();
            testSigning(testSettings);
            endUp();
        }
    }

    /**
     * @throws Throwable
     *             TBD
     *
     */
    @Test
    public void testSignGenericSoftwareFormatCommonFolder() throws Throwable {
        File unsignedFile;
        File signedFile;
        LinkedList<L9540SignerSettings> testSettingsList = new LinkedList<L9540SignerSettings>();
        L9540SignerSettings settings = null;

        unsignedFile = new File(getResourceFilePath(UNSIGNED_PRCMU_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_PRCMU_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.PRCMU);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_MEM_INIT_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_MEM_INIT_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.MEM_INIT);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_XLOADER_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_XLOADER_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.XLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_OSLOADER_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_OSLOADER_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.OSLOADER);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_APE_NORMAL_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_APE_NORMAL_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.APE_NORMAL);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_LDR_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_LDR_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.LDR);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_MODEM_ELF_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_MODEM_ELF_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.MODEM);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_FOTA_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_FOTA_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FOTA);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_DNT_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_DNT_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.DNT);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        unsignedFile = new File(getResourceFilePath(UNSIGNED_IPL_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_IPL_FILENAME);
        settings = new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.IPL);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);
        testSettingsList.add(settings);

        for (L9540SignerSettings testSettings : testSettingsList) {
            setUp();
            signServer.joinServerSocketThread();
            setFakeSignProtocolForGenericSigningCommonFolder();
            testSigning(testSettings);
            endUp();
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignTrustedApplication() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TRUSTED_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TRUSTED_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.TRUSTED);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        setFakeSignProtocolForGenericSigning();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignTrustedApplicationCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TRUSTED_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TRUSTED_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.TRUSTED);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        setFakeSignProtocolForGenericSigningCommonFolder();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignAuthCert() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_AUTH_CERT_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_AUTH_CERT_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.AUTH_CERT);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        setFakeSignProtocolForGenericSigning();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignAuthCertCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_AUTH_CERT_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_AUTH_CERT_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.AUTH_CERT);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        setFakeSignProtocolForGenericSigningCommonFolder();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignFlashArchive() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_FLASH_ARCHIVE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_FLASH_ARCHIVE_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(L9540SignerSettings.KEY_BUFFER_SIZE, 8192);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        setFakeSignProtocolForGenericSigning();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignFlashArchiveCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_FLASH_ARCHIVE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_FLASH_ARCHIVE_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(L9540SignerSettings.KEY_BUFFER_SIZE, 8192);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        setFakeSignProtocolForGenericSigningCommonFolder();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testReSignFlashArchive() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(PRESIGNED_FLASH_ARCHIVE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + "re-" + SIGNED_FLASH_ARCHIVE_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

        setFakeSignProtocolForGenericSigning();
        testSigning(settings);
        endUp();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testReSignFlashArchiveCommonFolder() throws Throwable {
        setUp();
        File unsignedFile = new File(getResourceFilePath(PRESIGNED_FLASH_ARCHIVE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + "re-" + SIGNED_FLASH_ARCHIVE_FILENAME);
        L9540SignerSettings settings =
            new L9540SignerSettings(unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath());
        settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, SIGN_PACKAGE_U5500_ROOT);
        settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, GenericSoftwareType.FLASH_ARCHIVE);
        settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, true);

        setFakeSignProtocolForGenericSigningCommonFolder();
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
        assertNotNull(new SignerFacade().getSignPackages(null, false, false, null));
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
        assertNotNull(new SignerFacade().getSignPackages(null, false, true, null));
        endUp();
    }

    private void setFakeSignProtocolForListPackages() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_KEYCATALOGUE_REQUEST, new byte[0]).toArray()),
                PACKET_CATALOGUE_RESPONSE)});
    }

    private void setFakeSignProtocol() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_U5500_ROOT.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_ROOT_SIGN_PACKAGE, 500),
            new MockProtocolMessage(PACKET_U5500_ENCRYPTION_REQUEST, PACKET_U5500_ENCRYPTION_RESPONSE, 500)});
    }

    private void setFakeSignProtocolCommonFolder() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_U5500_ROOT.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_ROOT_SIGN_PACKAGE, 500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SUB_PACKAGE_BY_KEY_REQUEST, new byte[0])
                    .toArray()), PACKET_SUB_SIGN_PACKAGE_BY_KEY_RESPONSE),
            new MockProtocolMessage(PACKET_U5500_ENCRYPTION_REQUEST, PACKET_U5500_ENCRYPTION_RESPONSE, 500)});
    }

    private void setFakeSignProtocolForGenericSigning() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_U5500_ROOT.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_ROOT_SIGN_PACKAGE, 500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SUB_PACKAGE_BY_KEY_REQUEST, new byte[0])
                .toArray()), PACKET_SUB_SIGN_PACKAGE_BY_KEY_RESPONSE),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_U5500_ROOT.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_SUB_SIGN_PACKAGE, 500),
            new MockProtocolMessage(PACKET_U5500_ENCRYPTION_REQUEST, PACKET_U5500_ENCRYPTION_RESPONSE, 500)});
    }

    private void setFakeSignProtocolForGenericSigningCommonFolder() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_U5500_ROOT.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_ROOT_SIGN_PACKAGE, 500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SUB_PACKAGE_BY_KEY_REQUEST, new byte[0])
                .toArray()), PACKET_SUB_SIGN_PACKAGE_BY_KEY_RESPONSE_COMMON_FOLDER),
            new MockProtocolMessage(PACKET_U5500_ENCRYPTION_REQUEST, PACKET_U5500_ENCRYPTION_RESPONSE, 500)});
    }

    private void setFakeSignProtocolForAuthSigning() throws Throwable {
        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_U5500_ROOT.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_RANDOMNUMBER_RESPONSE, new byte[20]).toArray()), new FakeMessage(
                    createPacket(COMMAND_SECRET_REQUEST, secFile.getAbsolutePath().getBytes("UTF-8")).toArray()), true,
                500),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_SECRET_RESPONSE, SECRET).toArray()),
                new FakeMessage(createPacket(COMMAND_LOGIN_ACCEPT, new byte[] {}).toArray()), 500),
            new MockProtocolMessage(
                new FakeMessage(createPacket(COMMAND_HEADERPACKAGE_REQUEST, new byte[0]).toArray()),
                PACKET_AUTH_SIGN_PACKAGE, 500),
            new MockProtocolMessage(PACKET_U5500_ENCRYPTION_REQUEST, PACKET_U5500_ENCRYPTION_RESPONSE, 500)});
    }

}
