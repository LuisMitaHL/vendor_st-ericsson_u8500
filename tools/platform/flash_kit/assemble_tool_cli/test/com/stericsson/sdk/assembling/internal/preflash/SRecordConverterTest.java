package com.stericsson.sdk.assembling.internal.preflash;

import static org.junit.Assert.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.utilities.StreamCloser;

/**
 * 
 * @author pkutac01
 * 
 */
public class SRecordConverterTest {

    private static final String S3_FILE = ResourcePathManager.getResourceFilePath("res/image.a01");

    /**
     * 
     */
    @Test
    public void testGetFirstAddressInS3File() {
        File file = null;
        FileOutputStream output = null;
        String contents = null;
        SRecordConverter converter = null;

        try {
            file = File.createTempFile("temp", null);

            // Test to get first address in S3 file containing data record.
            output = new FileOutputStream(file);
            contents = "S1130000000200002C32000000000000FFFFFFFF90";
            output.write(contents.getBytes("UTF-8"));
            output.flush();
            output.close();
            converter = new SRecordConverter();

            assertEquals(0, converter.getFirstAddressInS3File(file.getAbsolutePath(), 0, 1));

            // Test to get -1 after attempt to get first address in S3 file containing only non-data
            // record.
            file.delete();
            output = new FileOutputStream(file);
            contents = "S00600004844521B";
            output.write(contents.getBytes("UTF-8"));
            output.flush();
            output.close();
            converter = new SRecordConverter();

            assertEquals(-1, converter.getFirstAddressInS3File(file.getAbsolutePath(), 0, 1));

            // Test to get -1 after attempt to get first address in S3 file with invalid range
            // specified.
            file.delete();
            output = new FileOutputStream(file);
            contents = "S1130000000200002C32000000000000FFFFFFFF90";
            output.write(contents.getBytes("UTF-8"));
            output.flush();
            output.close();
            converter = new SRecordConverter();

            assertEquals(-1, converter.getFirstAddressInS3File(file.getAbsolutePath(), 1, 0));
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.getMessage());
        } catch (AssemblerException e) {
            e.printStackTrace();
            fail(e.getMessage());
        } finally {
            StreamCloser.close(output);
            if (file != null) {
                file.delete();
            }
        }
    }

    /**
     * 
     */
    @Test
    public void testGetFirstAddressInS3FileWithInvalidFile() {
        File file = null;
        FileOutputStream output = null;
        String contents = null;
        SRecordConverter converter = null;

        try {
            file = File.createTempFile("temp", null);

            output = new FileOutputStream(file);
            contents = "S1";
            output.write(contents.getBytes("UTF-8"));
            output.flush();
            output.close();
            converter = new SRecordConverter();

            assertEquals(0, converter.getFirstAddressInS3File(file.getAbsolutePath(), 0, 1));
            fail("Should not get here.");
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.getMessage());
        } catch (AssemblerException e) {
            System.out.println(e.getMessage());
        } finally {
            StreamCloser.close(output);
            if (file != null) {
                file.delete();
            }
        }
    }

    /**
     * Test to write S3 file as a binary file with various address alignment options set.
     */
    @Test
    public void testWriteS3AsBinary() {
        File file = null;
        File outputFile = null;
        FileOutputStream output = null;
        SRecordConverter converter = null;

        try {
            file = new File(S3_FILE);
            outputFile = File.createTempFile("temp", null);

            output = new FileOutputStream(outputFile);
            converter = new SRecordConverter();
            converter.writeS3AsBinary(file.getAbsolutePath(), 0, 1, false, 0, 0, output);
            output.flush();
            output.close();

            assertTrue(outputFile.exists() && outputFile.length() > 0);

            outputFile.delete();
            output = new FileOutputStream(outputFile);
            converter = new SRecordConverter();
            converter.writeS3AsBinary(file.getAbsolutePath(), 0, 1, true, 0, 0, output);
            output.flush();
            output.close();

            assertTrue(outputFile.exists() && outputFile.length() > 0);

            outputFile.delete();
            output = new FileOutputStream(outputFile);
            converter = new SRecordConverter();
            converter.writeS3AsBinary(file.getAbsolutePath(), 0, 1, true, 0, 1, output);
            output.flush();
            output.close();

            assertTrue(outputFile.exists() && outputFile.length() > 0);

            outputFile.delete();
            output = new FileOutputStream(outputFile);
            converter = new SRecordConverter();
            converter.writeS3AsBinary(file.getAbsolutePath(), 0, 1, false, SRecordConverter.ATTRIBUTE_NOT_SET, 0,
                output);
            output.flush();
            output.close();

            assertTrue(outputFile.exists() && outputFile.length() > 0);

            outputFile.delete();
            output = new FileOutputStream(outputFile);
            converter = new SRecordConverter();
            converter
                .writeS3AsBinary(file.getAbsolutePath(), 0, 1, true, SRecordConverter.ATTRIBUTE_NOT_SET, 1, output);
            output.flush();
            output.close();

            assertTrue(outputFile.exists() && outputFile.length() > 0);

            outputFile.delete();
            output = new FileOutputStream(outputFile);
            converter = new SRecordConverter();
            converter
                .writeS3AsBinary(file.getAbsolutePath(), 1, 1, true, SRecordConverter.ATTRIBUTE_NOT_SET, 2, output);
            output.flush();
            output.close();

            assertTrue(outputFile.exists() && outputFile.length() > 0);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.getMessage());
        } catch (AssemblerException e) {
            e.printStackTrace();
            fail(e.getMessage());
        } finally {
            StreamCloser.close(output);
            if (outputFile != null) {
                outputFile.delete();
            }
        }
    }

}
