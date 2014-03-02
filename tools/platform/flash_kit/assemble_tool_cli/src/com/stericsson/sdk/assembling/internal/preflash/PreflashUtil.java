package com.stericsson.sdk.assembling.internal.preflash;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.LinkedList;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.u8500.U8500AssemblerHelper;
import com.stericsson.sdk.assembling.utilities.StreamCloser;
import com.stericsson.sdk.common.moto.SRecord;

/**
 * Class for various preflash utilities.
 * 
 * @author xtomlju
 */
public final class PreflashUtil {
    /*
     * Private constructor
     */
    private PreflashUtil() {

    }

    /**
     * Process a Motorola S-record file and create a new file for every gap detected in address
     * range.
     * 
     * @param inputFilename
     *            Source Motorola S-record file with possible gaps
     * @return An array of created filenames
     * @throws IOException
     *             If an I/O error occurred
     * @throws AssemblerException
     *             If an I/O error occurred
     */
    public static String[] splitA01(String inputFilename) throws IOException, AssemblerException {

        LinkedList<String> outputFileNameList = new LinkedList<String>();

        String bareInputFileName = new File(inputFilename).getName();

        int p = bareInputFileName.lastIndexOf('.');
        if (p != -1) {
            bareInputFileName = bareInputFileName.substring(0, p);
        }

        FileInputStream input = null;
        FileOutputStream output = null;
        BufferedReader reader = null;
        BufferedWriter writer = null;

        SRecord record = new SRecord();

        int fileCount = 0;
        long oldAddress = -1;
        long oldDataLength = 0;

        File outputFile =
            U8500AssemblerHelper.createTempFile(new byte[0], bareInputFileName + "_" + fileCount + ".a01");
        outputFile.deleteOnExit();
        String outputFilename = outputFile.getAbsolutePath();
        outputFileNameList.add(outputFilename);

        try {
            input = new FileInputStream(inputFilename);
            output = new FileOutputStream(outputFilename);

            reader = new BufferedReader(new InputStreamReader(input, "UTF-8"));
            writer = new BufferedWriter(new OutputStreamWriter(output));
            String line = reader.readLine();
            while (line != null) {
                record.setLine(line);
                if (record.isDataRecord()) {
                    long newAddress = record.getAddress();
                    long gap = newAddress - (oldAddress + oldDataLength);
                    if (oldAddress != -1 && (gap > 0)) {
                        writer.flush();
                        output.close();
                        writer.close();
                        fileCount++;
                        outputFile =
                            U8500AssemblerHelper.createTempFile(new byte[0], bareInputFileName + "_" + fileCount
                                + ".a01");
                        outputFile.deleteOnExit();
                        outputFilename = outputFile.getAbsolutePath();
                        outputFileNameList.add(outputFilename);
                        output = new FileOutputStream(outputFilename);
                        writer = new BufferedWriter(new OutputStreamWriter(output));
                    }
                    oldAddress = newAddress;
                    byte[] binaryData = record.getData();
                    long dataLength = binaryData == null ? 0 : binaryData.length;
                    oldDataLength = dataLength;
                    writer.write(line + "\r\n");
                }
                line = reader.readLine();
            }
            writer.flush();
        } catch (Exception e) {
            throw new IOException(e.getMessage());
        } finally {
            StreamCloser.close(reader, writer, output, input);
        }

        return outputFileNameList.toArray(new String[outputFileNameList.size()]);
    }

}
