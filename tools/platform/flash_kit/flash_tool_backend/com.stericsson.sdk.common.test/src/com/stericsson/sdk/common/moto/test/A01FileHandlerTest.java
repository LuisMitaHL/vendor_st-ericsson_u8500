package com.stericsson.sdk.common.moto.test;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.memory.MemoryIOException;
import com.stericsson.sdk.common.moto.A01FileHandler;

/**
 * @author xtomlju
 */
public class A01FileHandlerTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testLoad() {

        File softwareFile = new File("sw.a01");
        BufferedWriter writer = null;
        try {
            writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(softwareFile), "UTF-8"));
            writer
                .write("S02B00002E5C6378633030303030305F4150504C49434154494F4E5F50484F4E455C4941522D41524D2D4E417F\n");
            writer.write("S3154842F0000000000000000000000000000000000070\n");
            writer.write("S3154842F0100000000000000000000000000000000060\n");
            writer.write("S3154842F02000000000000000000000E0FFFFFFEF4143\n");
            writer.write("S3154842F0300000000000000000000000000000000040\n");
            writer.flush();

        } catch (IOException e1) {
            fail(e1.getMessage());
        } finally {
            try {
                if (writer != null) {
                    writer.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        A01FileHandler fileHandler;

        fileHandler = new A01FileHandler(softwareFile);
        try {
            fileHandler.load();
        } catch (IOException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testGetPayload() {

        A01FileHandler fileHandler;

        File softwareFile = new File("sw.a01");
        BufferedWriter writer = null;
        try {
            writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(softwareFile), "UTF-8"));
            writer
                .write("S02B00002E5C6378633030303030305F4150504C49434154494F4E5F50484F4E455C4941522D41524D2D4E417F\n");
            writer.write("S3154842F0000000000000000000000000000000000070\n");
            writer.write("S3154842F0100000000000000000000000000000000060\n");
            writer.write("S3154842F02000000000000000000000E0FFFFFFEF4143\n");
            writer.write("S3154842F0300000000000000000000000000000000040\n");
            writer.flush();
        } catch (IOException e1) {
            fail(e1.getMessage());
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        fileHandler = new A01FileHandler(softwareFile);
        try {
            fileHandler.load();
        } catch (IOException e) {
            fail(e.getMessage());
        }

        try {
            assertTrue(fileHandler.getPayload() != null);
        } catch (MemoryIOException e) {
            fail(e.getMessage());
        }
    }

}
