package com.stericsson.sdk.signing.mockup;

import java.io.UnsupportedEncodingException;
import java.util.Arrays;

/**
 * Helper class which serves for the testing purpose to be able to mock protocol with server.
 * 
 * @author emicroh
 */
public class FakeMessage {

    private byte[] msg;

    /**
     * Creates and init instance of fake message with given byte array.
     * 
     * @param message
     *            Byte array representation for this message
     */
    public FakeMessage(byte[] message) {
        this.msg = message;
    }

    /**
     * Creates and init instance of fake message with given string.
     * 
     * @param str
     *            String message.
     */
    public FakeMessage(String str) {
        if (str == null) {
            this.msg = null;
        } else {
            try {
                this.msg = str.getBytes("UTF-8");
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean equals(Object obj) {
        if ((obj == null) || !(obj instanceof FakeMessage)) {
            return false;
        } else if (this == obj) {
            return true;
        }

        FakeMessage tmp = (FakeMessage) obj;
        if (msg.length != tmp.getMsg().length) {
            return false;
        } else {
            return Arrays.equals(msg, tmp.getMsg());
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int hashCode() {
        int hash = 0;
        if (msg != null) {
            for (int i = 0; i < msg.length; i++) {
                hash ^= msg[i];
            }
        }
        return hash;
    }

    /**
     * @return Byte array representation of this fake message.
     */
    public byte[] getMsg() {
        return msg;
    }

}
