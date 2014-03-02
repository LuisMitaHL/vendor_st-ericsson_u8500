package com.stericsson.sdk.signing.mockup;

import junit.framework.TestCase;

import org.junit.Test;

/**
 * 
 * @author ezaptom
 *
 */
public class MockProtocolMessageTest extends TestCase{

    /**
     * 
     */
    @Test
    public void testMockProtocolMessage() {
        FakeMessage clientMessage = new FakeMessage("clientMessage");
        FakeMessage responseMessage = new FakeMessage("responseMessage");
        boolean initByServer = false;
        long timeOut = 100;

        MockProtocolMessage message1 = new MockProtocolMessage(clientMessage, responseMessage, initByServer, timeOut);

        assertEquals(clientMessage, message1.getClientMessage());
        assertEquals(responseMessage, message1.getServerResponse());
        assertEquals(initByServer, message1.isServerInitiated());
        assertEquals(timeOut, message1.getTimeout());

        MockProtocolMessage message2 = new MockProtocolMessage(clientMessage, responseMessage, timeOut);
        message2.setServerInitiated(true);

        assertFalse(message1.isServerInitiated() == message2.isServerInitiated());
    }
}
