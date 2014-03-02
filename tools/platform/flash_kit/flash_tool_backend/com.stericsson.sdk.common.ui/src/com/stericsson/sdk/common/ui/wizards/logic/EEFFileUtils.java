package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.StringTokenizer;
import java.util.Vector;

import com.stericsson.sdk.common.LittleEndianByteConverter;

/**
 * EEF file generation utilities
 * 
 * @author kapalpaw
 * 
 */
public final class EEFFileUtils {

    private EEFFileUtils() {
    }

    private static void finalizeCreateEEFFile(String src, boolean deleteSource, InputStreamReader instream,
        FileInputStream fis, OutputStream ostream, BufferedReader in) {
        try {
            if (fis != null) {
                fis.close();
            }
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
        try {
            if (in != null) {
                in.close();
            }
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
        try {
            if (instream != null) {
                instream.close();
            }
        } catch (IOException ioe) {
            System.err.println("Error: " + ioe.getMessage());
        }
        try {
            if (ostream != null) {
                ostream.close();
            }
        } catch (IOException ioe) {
            System.err.println("Error: " + ioe.getMessage());
        }
        // remove tmp file
        if (deleteSource) {
            File f = new File(src);
            boolean res = f.delete();

            // if delete file failed, try to delete it when exiting
            if (!res) {
                f.deleteOnExit();
            }
        }
    }

    private static void parseEEFData(Unit u, BufferedReader in, String firstLine) throws IOException {
        String strLine = firstLine;

        if (u == null) {
            throw new IOException("Unit is null");
        }
        u.unitData = new byte[u.length];
        int index = 0;

        while (true) {

            StringTokenizer tk = new StringTokenizer(strLine);

            while (tk.hasMoreElements()) {
                u.unitData[index] = (byte) Integer.parseInt(String.valueOf(tk.nextElement()), 16);
                index++;
            }

            if (index <= (u.length - 1)) {
                strLine = in.readLine(); // we have more data
            } else {
                break;
            }

        }

    }

    private static Unit parseEEFHeader(String strLine) {
        String[] unitStr = strLine.split(" ");
        Unit u = new Unit();

        u.number = Integer.valueOf(unitStr[0], 16).intValue();
        u.length = Integer.valueOf(unitStr[1]).intValue();
        return u;
    }

    /**
     * Method that converts a GDF file to an EEF file
     * 
     * @param src
     *            is the path to the .tmp GDF file
     * @param dest
     *            is the path to the Early Enumeration Customization data file (EEF)
     * @param deleteSource
     *            if the sourcefile should be deleted after completion
     * @return false if not OK
     */
    static boolean createEEFFile(String src, String dest, boolean deleteSource) {

        // ver 2
        /* the format is 4 hex(unit) 2 hex(length) 4 hex (block) 2 hex (flags) */
        String regex = ".?[0-9a-fA-F]{8}.?[0-9a-fA-F]{1,4}.?[0-9a-fA-F]{4}.?";

        int paramsCounter = 0;
        // ver 2
        // String[] unitStr = new String[3]; // header contains field number,
        // length, block and
        // flags
        Vector<Unit> units = new Vector<Unit>();
        Unit u = null;
        InputStreamReader instream = null;
        FileInputStream fis = null;
        OutputStream ostream = null;
        BufferedReader in = null;

        ErrorInfoUtils.logn("Creating EE file");

        try {

            fis = new FileInputStream(src);
            instream = new InputStreamReader(fis, "UTF-8");

            // read everything to a buffer
            in = new BufferedReader(instream);

            ostream = new FileOutputStream(dest);

            String strLine;

            while ((strLine = in.readLine()) != null) {

                // get rid of comments and empty lines
                if ((strLine.compareTo("") != 0) && !strLine.contains("/")) {

                    if (strLine.matches(regex)) {
                        // HEADER
                        u = parseEEFHeader(strLine);
                        paramsCounter++;

                    } else {
                        // DATA
                        parseEEFData(u, in, strLine);
                        units.add(u);
                    }
                }

            }

            ostream.write(LittleEndianByteConverter.intToByteArray(IDefaultDataSheetConstants.EEF_HEADER_LENGTH, 4)); // for
            // now
            // always
            // 12
            // bytes???
            ostream.write(LittleEndianByteConverter.intToByteArray(IDefaultDataSheetConstants.EEF_HEADER_VERSION_1, 4)); // for
            // now
            // always
            // version
            // 1???
            ostream.write(LittleEndianByteConverter.intToByteArray(paramsCounter, 4));

            for (Unit unit : units) {
                ostream.write(LittleEndianByteConverter.intToByteArray(unit.number, 2));
                ostream.write(LittleEndianByteConverter.intToByteArray(unit.length, 2));
                ostream.write(unit.unitData);
            }

        } catch (Exception e) {// if exception remove eef file
            File f = new File(dest);
            f.delete();
            System.err.println("Error: " + e.getMessage());
            return false;

        } finally {

            finalizeCreateEEFFile(src, deleteSource, instream, fis, ostream, in);

        }

        ErrorInfoUtils.logn(IDefaultDataSheetConstants.USERINFODONESTRING);

        return true;

    }

}
