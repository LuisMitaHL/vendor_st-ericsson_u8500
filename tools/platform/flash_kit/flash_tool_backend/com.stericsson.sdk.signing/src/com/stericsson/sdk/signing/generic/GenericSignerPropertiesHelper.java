package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.SignerException;


/**
 * @author Xxvs0002
 *
 */
public final class GenericSignerPropertiesHelper {

    private static Logger logger = Logger.getLogger(GenericSignerHelper.class.getName());

    private GenericSignerPropertiesHelper() {
    }

    /**
     * 
     * @param uncheckedBlob
     *            path to unchecked blob
     * @return the blob as bytes
     * @throws SignerException
     *             on errors
     */
    public static byte[] readUncheckedBlob(String uncheckedBlob) throws SignerException {
        FileInputStream fis = null;
        FileChannel channel = null;
        ByteBuffer payloadBuffer = null;
        File file = new File(uncheckedBlob);
        try {
            fis = new FileInputStream(file);
            channel = fis.getChannel();
            payloadBuffer = ByteBuffer.allocate((int) file.length());
            channel.read(payloadBuffer);
            payloadBuffer.rewind();
        } catch (IOException ioe) {
            throw new SignerException(ioe);
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }
        return payloadBuffer.array();
    }
}
