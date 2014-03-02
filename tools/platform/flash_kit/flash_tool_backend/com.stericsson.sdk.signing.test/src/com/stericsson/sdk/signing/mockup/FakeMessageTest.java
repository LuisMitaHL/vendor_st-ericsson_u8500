package com.stericsson.sdk.signing.mockup;

import java.io.UnsupportedEncodingException;

import junit.framework.TestCase;

import org.junit.Test;

/**
 * 
 * @author ezaptom
 * 
 */
public class FakeMessageTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testfakeMessage() {
        String message = "fakeMessageText";

        FakeMessage fakeMessage1 = null;
        FakeMessage fakeMessage2 = null;
        FakeMessage fakeMessage3 = null;
        FakeMessage fakeMessage4 = null;

        try {
            fakeMessage1 = new FakeMessage(message.getBytes("UTF-8"));
            fakeMessage2 = new FakeMessage(message.getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
            return;
        }

        int hash1 = fakeMessage1.hashCode();
        int hash2 = fakeMessage2.hashCode();
        assertTrue(hash1 == hash2);

        assertFalse(fakeMessage1.equals(null));
        assertTrue(fakeMessage1.equals(fakeMessage1));
        assertTrue(fakeMessage1.equals(fakeMessage2));

        message = "a";
        try {
            fakeMessage3 = new FakeMessage(message.getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
            return;
        }
        assertFalse(fakeMessage1.equals(fakeMessage3));

        message = null;
        fakeMessage4 = new FakeMessage(message);
        assertEquals(null, fakeMessage4.getMsg());
    }
}
