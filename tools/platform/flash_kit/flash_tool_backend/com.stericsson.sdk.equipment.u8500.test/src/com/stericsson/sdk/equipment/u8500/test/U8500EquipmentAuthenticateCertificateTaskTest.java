package com.stericsson.sdk.equipment.u8500.test;

import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_HEADERPACKAGE_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_LOGIN_ACCEPT;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_LOGIN_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_RANDOMNUMBER_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_SECRET_REQUEST;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacket.COMMAND_SECRET_RESPONSE;
import static com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketFactory.createPacket;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Random;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentAuthenticateCertificateTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;
import com.stericsson.sdk.signing.mockup.FakeMessage;
import com.stericsson.sdk.signing.mockup.FakeSignServer;
import com.stericsson.sdk.signing.mockup.MockProtocolMessage;

/**
 * @author xtomzap
 * @author Vit Sykala
 */
public class U8500EquipmentAuthenticateCertificateTaskTest extends TestCase {

    private U8500Equipment eq;

    private EquipmentAuthenticateCertificateTask authenticationTask;

    private FakeSignServer signServer;

    private File secFile;

    private static String[] pArgumentsPermanent = new String[] {
        "SYSTEM_AUTHENTICATION_CERTIFICATE", "<equipment-id>", "xolabju_u5500_auth", "true"};

    private static String[] pArgumentsTemporary = new String[] {
        "SYSTEM_AUTHENTICATION_CERTIFICATE", "<equipment-id>", "xolabju_u5500_auth", "false"};

    private static final int RESULT_SUCCESS = 0;

    private static final int RESULT_FAIL = 5;

    private static final String SIGN_PACKAGE_U5500_ROOT = "xolabju_u5500";

    private static final byte[] SECRET = new byte[] {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    private static final FakeMessage PACKET_HELLO = new FakeMessage(new byte[] {
        -86, -86, -86, -86, 9, 0, 0, 0, 0, -53, 50, -107, 108});

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

    /**
     * @throws Exception .
     */
    @Before
    public void setUp() throws Exception {
        eq = new U8500Equipment(Activator.getPort(), null);
        authenticationTask = new EquipmentAuthenticateCertificateTask(eq);

        Random random = new Random();
        int size = 64;
        byte[] authChallange = new byte[size];
        random.nextBytes(authChallange);

        // prepare Fake signServer
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

        ServiceReference serviceReference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        IConfigurationService service =
            (IConfigurationService) Activator.getBundleContext().getService(serviceReference);
        IConfigurationRecord record = service.getRecord("SignTool");
        if (record == null) {
            throw new IOException("Failed to read configuration record: SignTool");
        }
        record.setValue("UseLocalSigning", "false");
    }

    /**
     *
     */
    @After
    public void tearDown() {
        signServer.close();
        secFile.delete();
        eq = null;
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateSuccesPermanentFlag() throws Throwable {

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsPermanent);

        setFakeSignProtocolForAuthSigning();
        signServer.startServer();

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals("Operation successful. Operation finished successfully.", result.getResultMessage());
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateSuccesTemporary() throws Throwable {

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsTemporary);

        setFakeSignProtocolForAuthSigning();
        signServer.startServer();

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals("Operation successful. Operation finished successfully.", result.getResultMessage());
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateFail() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        authenticationTask.setArguments(pArgumentsPermanent);

        setFakeSignProtocolForAuthSigning();
        signServer.startServer();

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());

        assertNull(result.getResultObject());

        assertFalse(authenticationTask.isCancelable());

        assertEquals(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name(), authenticationTask.getId());

        assertEquals(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name() + "@"
            + authenticationTask.getEquipment().toString(), authenticationTask.toString());

        assertFalse(result.isFatal());
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateFailUnregisterSignTool() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsPermanent);

        ServiceReference serviceReference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        IConfigurationService service =
            (IConfigurationService) Activator.getBundleContext().getService(serviceReference);
        IConfigurationRecord record = service.getRecord("SignTool");

        if (record != null) {
            EquipmentTaskResult result = new EquipmentTaskResult(0, "", null, false);
            try {
                record.setName("SignTool2");

                result = authenticationTask.execute();
            } finally {
                record.setName("SignTool");
            }

            assertTrue("result should't be ok", RESULT_SUCCESS != result.getResultCode());
        } else {
            fail("SignTool record was null.");
        }
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateFailBadSignProtocol() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsPermanent);

        signServer.setMockMessages(new MockProtocolMessage[] {
            new MockProtocolMessage(PACKET_HELLO, PACKET_HELLO, true),
            new MockProtocolMessage(new FakeMessage(createPacket(COMMAND_LOGIN_REQUEST,
                SIGN_PACKAGE_U5500_ROOT.getBytes("UTF-8")).toArray()), new FakeMessage(createPacket(
                COMMAND_RANDOMNUMBER_REQUEST, new byte[] {}).toArray()), 10)});

        signServer.startServer();

        EquipmentTaskResult result = authenticationTask.execute();

        assertTrue("result should't be ok", RESULT_SUCCESS != result.getResultCode());
    }

    /**
     * Cancel has empty implementation.
     */
    @Test
    public void testCancel() {
        authenticationTask.cancel();
    }

    /**
     * getControlKeys has empty implementation.
     */
    @Test
    public void testGetControlKeys() {
        authenticationTask.getControlKeys();
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
