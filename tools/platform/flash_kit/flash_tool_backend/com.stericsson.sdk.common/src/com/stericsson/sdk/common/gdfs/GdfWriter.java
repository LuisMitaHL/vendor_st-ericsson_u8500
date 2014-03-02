package com.stericsson.sdk.common.gdfs;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

/**
 * GdfWriter provides functionality to write single or multiple GDFS data blocks to a GDFS file.
 * 
 * @author qkapers (Karl-Olof Persson)
 */
public class GdfWriter {
    PrintWriter mWriter;

    String mFileHeader = "CSPSA dump file created by Flash Kit";

    /**
     * Creates a GdfWriter for writing to the specified file.
     * 
     * @param f
     *            The file to write to.
     * @throws IOException
     *             if file can not be created.
     */
    public GdfWriter(File f) throws IOException {
        mWriter = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(f), "UTF-8")));
    }

    /**
     * Sets the header line to be written in the beginning of the GDFS file. Default is:
     * "CSPSA dump file created by Flash Kit"
     * 
     * @param header
     *            The header in the GDFS file.
     */
    public void setHeader(String header) {
        mFileHeader = header;
    }

    /**
     * Writes all GDFS data blocks in specified list to file.
     * 
     * @param gdfsUnitList
     *            A List with GDFS data blocks (GDData).
     */
    public void writeGdfsFile(List<GDData> gdfsUnitList) {
        Collections.sort(gdfsUnitList);

        mWriter.println("//*********************************************");
        mWriter.println("// " + mFileHeader);
        mWriter.println("// Creation date: " + new Date());
        mWriter.println("// User: " + System.getProperty("user.name"));
        mWriter.println("//*********************************************");
        mWriter.println("");
        mWriter.println("// Data format:");
        mWriter.println("// <UNIT>(hex) <DATASIZE>(dec) <BLOCK>(hex) [<DATA>(hex)]");
        mWriter.println("");
        mWriter.println("");

        for (Iterator<?> gdfsIterator = gdfsUnitList.iterator(); gdfsIterator.hasNext();) {
            mWriter.println(gdfsIterator.next().toString());
        }
    }

    /**
     * Writes the specified GDFS data block to file.
     * 
     * @param gdData
     *            GDFS data block to write.
     */
    public void writeGdfsFile(GDData gdData) {
        ArrayList<GDData> gdfsUnitList = new ArrayList<GDData>();
        gdfsUnitList.add(gdData);

        writeGdfsFile(gdfsUnitList);
    }

    /**
     * Closes the opened GDFS file.
     * 
     */
    public void close() {
        mWriter.close();
    }
}
