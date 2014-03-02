package com.stericsson.sdk.common.gdfs;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.log4j.Logger;

/**
 * The GdfParser provides functionality to read and extract complete GDFS data blocks from a gdfs
 * file.
 */
public class GdfParser implements IGDDataParser {

    private Logger logger = Logger.getLogger(GdfParser.class.getName());

    static final int INDEX = 0;

    static final int LENGTH = 1;

    static final int BLOCK = 2;

    static final int NUM_VARS = 3;

    BufferedReader mReader;

    String mLine;

    /**
     * Constructor, creates an instance of this and opens the given file for gdfs data reading.
     * 
     * @param f
     *            The file to read GDFS data from.
     * @throws IOException
     *             If the parser fails to open the file.
     */
    public GdfParser(File f) throws IOException {
        mReader = new BufferedReader(new InputStreamReader(new FileInputStream(f), "UTF-8"));
        mLine = mReader.readLine();
    }

    /**
     * Reports if there is more GDFS data available in the file.
     * 
     * @return true if more data is available, false otherwise.
     */
    public boolean hasMoreData() {
        return mLine != null;
    }

    /**
     * Returns the next GDFS data block from the file.
     * 
     * @return The next data block from the file or null if no more blocks are available.
     * @throws IOException
     *             if the parser fails to read from the file.
     */
    public GDData nextData() throws IOException {
        if (!hasMoreData()) {
            return null;
        }
        GDData data = createData();
        if (data.getData().length == 0) {
            data = null;
        }
        return data;
    }

    /**
     * Reads data from the opened file and creates a complete GDFS data block.
     * 
     * @return The GDFS data block read from the file.
     * @throws IOException
     *             if the parser fails to read and construct a GDFS data block.
     */
    private GDData createData() throws IOException {
        int varsFound = 0;
        int[] variables = new int[NUM_VARS];
        List<Byte> bbData = new ArrayList<Byte>();
        firstWhile: do {
            readAndTrim();

            StringTokenizer st = new StringTokenizer(mLine, " ");
            while (st.hasMoreElements()) {
                String sVal = st.nextElement().toString();
                if (varsFound < NUM_VARS && (varsFound == INDEX || varsFound == BLOCK)) {
                    // Next is index, parse as hex
                    // or next is block, parse as
                    // hex.
                    sVal = "0x" + sVal;
                    varsFound = decodeAndSetVariable(varsFound, variables, sVal);
                } else if (varsFound < NUM_VARS && varsFound == LENGTH) {
                    sVal = sVal.replaceFirst("^[0]+", "");

                    varsFound = decodeAndSetVariable(varsFound, variables, sVal);
                } else if (varsFound < NUM_VARS) {
                    varsFound = decodeAndSetVariable(varsFound, variables, sVal);
                } else if (variables[LENGTH] > bbData.size()) {
                    // In the data part, parse as
                    // hex
                    sVal = "0x" + sVal;
                    int val = Integer.decode(sVal).intValue();
                    bbData.add((byte) (val & 0xFF));
                    if (variables[LENGTH] == bbData.size()) {
                        mLine = mReader.readLine();
                        break firstWhile; // This is instead of the boolean
                        // recordCompleted used
                        // before to break condition.
                    }
                }
            }
            mLine = mReader.readLine();
        } while (mLine != null);

        GDData gd = getFilledGDData(variables, bbData);
        return gd;
    }

    /**
     * Get GD data.
     * 
     * @param variables
     *            The GD variables.
     * @param bbData
     *            The GD data part.
     * @return a GDData structure
     */
    private GDData getFilledGDData(int[] variables, List<Byte> bbData) {
        GDData gd = new GDData();
        gd.setIndex(variables[INDEX]);
        gd.setBlock(variables[BLOCK]);
        byte[] data = new byte[bbData.size()];
        for (int i = 0; i < bbData.size(); i++) {
            data[i] = bbData.get(i);
        }
        gd.setData(data);
        return gd;
    }

    /**
     * Read and trim line.
     * 
     * @throws IOException
     */
    private void readAndTrim() throws IOException {
        while (mLine != null && mLine.trim().startsWith("/")) {
            mLine = mReader.readLine();
        }
        if (mLine != null) {
            mLine = mLine.trim();
            mLine = mLine.replace('\t', ' ');
        }
    }

    /**
     * Decode and set value
     * 
     * @param varsFound
     *            The variable index.
     * @param variables
     *            The variables container.
     * @param sVal
     *            The value.
     * @return Variable index.
     */
    private int decodeAndSetVariable(int varsFound, int[] variables, String sVal) {
        long val = 0;

        try {
            val = Long.decode(sVal);
        } catch (Exception e) {
            logger.debug("Failed to decode value: " + sVal);
        }
        variables[varsFound++] = (int) (val & 0xFFFFFFFF);
        return varsFound;

    }

    /**
     * Closes the opened GDFS file.
     * 
     * @throws IOException
     *             if the parser fails to close the file.
     */
    public void close() throws IOException {
        mReader.close();
    }
}
