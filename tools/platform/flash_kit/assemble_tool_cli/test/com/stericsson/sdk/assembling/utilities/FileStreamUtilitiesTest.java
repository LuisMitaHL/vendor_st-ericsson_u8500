package com.stericsson.sdk.assembling.utilities;

import static org.junit.Assert.fail;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;

/**
 * 
 * @author pkutac01
 * 
 */
public class FileStreamUtilitiesTest {

    private static final String EMPTY_FILE = ResourcePathManager.getResourceFilePath("res/empty.bin");

    private File file;

    private FileInputStream input;

    /**
     * 
     * @throws Exception
     *             Exception.
     */
    @Before
    public void setUp() throws Exception {
        file = new File(EMPTY_FILE);
        input = new FileInputStream(file);
    }

    /**
     * 
     */
    @After
    public void tearDown() {
        StreamCloser.close(input);
    }

    /**
     * 
     */
    @Test
    public void testRead16BitUnsignedIntegerException() {
        try {
            int value = FileStreamUtilities.read16BitUnsignedInteger(input);
            fail("Should not get here (read " + value + ").");
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testRead32BitUnsignedIntegerException() {
        try {
            long value = FileStreamUtilities.read32BitUnsignedInteger(input);
            fail("Should not get here (read " + value + ").");
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testReadByteArrayFullyException() {
        try {
            byte[] byteArray = new byte[2];
            FileStreamUtilities.readByteArrayFully(input, byteArray);
            fail("Should not get here (read " + Arrays.toString(byteArray) + ").");
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testSkipException() {
        FileInputStream inputStream = null;

        try {
            inputStream = new FileInputStream(file) {
                @Override
                public long skip(long n) throws IOException {
                    return 0;
                }
            };

            FileStreamUtilities.skip(inputStream, 1024);
            fail("Should not get here.");
        } catch (IOException e) {
            System.out.println(e.getMessage());
        } finally {
            StreamCloser.close(inputStream);
        }
    }

}
