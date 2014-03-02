package com.stericsson.sdk.signing.signerservice.protocol;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xtomlju
 */
public class SignerProtocolPacketFactoryTest extends TestCase {

    private static final byte INVALID_COMMAND = (byte) 0xFF;

    /**
     * 
     */
    @Test
    public void testCreatePacket() {

        SignerProtocolPacket packet = null;

        try {
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_HELLO, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_HELLO));
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_LOGIN_REQUEST, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_LOGIN_REQUEST));
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_LOGIN_ACCEPT, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet, SignerProtocolPacket.COMMAND_LOGIN_ACCEPT));
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_HEADERPACKAGE_REQUEST, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet,
                SignerProtocolPacket.COMMAND_HEADERPACKAGE_REQUEST));
            packet =
                SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_HEADERPACKAGE_RESPONSE, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet,
                SignerProtocolPacket.COMMAND_HEADERPACKAGE_RESPONSE));
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_RANDOMNUMBER_REQUEST, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet,
                SignerProtocolPacket.COMMAND_RANDOMNUMBER_REQUEST));
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet,
                SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE));
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_SECRET_REQUEST, null);
            assertTrue(SignerProtocolPacketFactory
                .isPackageCommand(packet, SignerProtocolPacket.COMMAND_SECRET_REQUEST));
            packet = SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_SECRET_RESPONSE, null);
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packet,
                SignerProtocolPacket.COMMAND_SECRET_RESPONSE));
        } catch (SignerServiceException e) {
            fail(e.getMessage());
        }

        try {
            packet = SignerProtocolPacketFactory.createPacket(INVALID_COMMAND, null);
            assertTrue(false);
        } catch (SignerServiceException e) {
            assertTrue(true);
        }
    }

}
