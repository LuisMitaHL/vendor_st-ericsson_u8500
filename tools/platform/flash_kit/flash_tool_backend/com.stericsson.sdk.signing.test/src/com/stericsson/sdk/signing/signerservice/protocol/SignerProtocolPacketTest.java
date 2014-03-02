package com.stericsson.sdk.signing.signerservice.protocol;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.math.BigInteger;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xtomlju
 */
public class SignerProtocolPacketTest extends TestCase{

    private static final BigInteger CLIENT_NUMBER = new BigInteger("123456789");

    private static final BigInteger CLIENT_SECRET = new BigInteger("987654321");

    /**
     * 
     */
    @Test
    public void testGetPacket() {

        ByteArrayOutputStream byteOutput = new ByteArrayOutputStream();
        SignerProtocolPacketOutput packetOutput = new SignerProtocolPacketOutput(byteOutput);

        try {
            packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_HELLO,
                new byte[0]));
            packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(SignerProtocolPacket.COMMAND_LOGIN_REQUEST,
                "SIGN_PACKAGE".getBytes("UTF-8")));
            packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
                SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE, CLIENT_NUMBER.toByteArray()));
            packetOutput.putPacket(SignerProtocolPacketFactory.createPacket(
                SignerProtocolPacket.COMMAND_SECRET_RESPONSE, CLIENT_SECRET.toByteArray()));

            byteOutput.flush();
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            ByteArrayInputStream stream = new ByteArrayInputStream(byteOutput.toByteArray());
            SignerProtocolPacketInput packetInput = new SignerProtocolPacketInput(stream);

            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packetInput.getPacket(1000),
                SignerProtocolPacket.COMMAND_HELLO));
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packetInput.getPacket(1000),
                SignerProtocolPacket.COMMAND_LOGIN_REQUEST));
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packetInput.getPacket(1000),
                SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE));
            assertTrue(SignerProtocolPacketFactory.isPackageCommand(packetInput.getPacket(1000),
                SignerProtocolPacket.COMMAND_SECRET_RESPONSE));

            packetOutput.terminate();
        } catch (SignerServiceException e) {
            fail(e.getMessage());
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
