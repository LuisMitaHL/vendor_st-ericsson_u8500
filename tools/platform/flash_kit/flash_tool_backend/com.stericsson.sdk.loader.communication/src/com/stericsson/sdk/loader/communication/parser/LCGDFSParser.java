package com.stericsson.sdk.loader.communication.parser;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.common.gdfs.GDData;
import com.stericsson.sdk.common.gdfs.GDFSBinaryFileWriter;
import com.stericsson.sdk.common.gdfs.GdfWriter;

/**
 * @author xdancho
 */
public final class LCGDFSParser {

    static String gdfsPath;

    private static Logger logger = Logger.getLogger(LCGDFSParser.class.getName());

    static String separator = System.getProperty("file.separator");

    static {
        gdfsPath = System.getProperty("java.io.tmpdir") + separator;
        new File(gdfsPath).mkdir();
    }

    private LCGDFSParser() {
    }

    /**
     * @return TBD
     */
    public static String getTempGDFSPath() {
        return gdfsPath + System.currentTimeMillis() + "Read_gdfs.bin";
    }

    /**
     * @param gf
     *            TBD
     * @return TBD
     */
    public static File convertToBinary(File gf) {
        File output = new File(gdfsPath + System.currentTimeMillis() + "Written_gdfs.bin");
        output.deleteOnExit();

        try {
            GDFSBinaryFileWriter.writeGDFSBinaryFile(output, gf);
        } catch (IOException e) {
            logger.error("Failed to create GDFS binary file " + output.getAbsolutePath() + " from GDFS text file "
                + gf.getAbsolutePath(), e);
            output = null;
        }

        return output;
    }

    /**
     * @param binary
     *            TBD
     * @param dest
     *            TBD
     */
    public static void convertToGDF(File binary, File dest) {

        InputStream input = null;
        try {
            input = new FileInputStream(binary);
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        ArrayList<GDData> gdfsUnitList = new ArrayList<GDData>();

        try {

            try {

                while (input.available() > 0) {
                    GDData data = parseGDFSUnit(input, 2, 4, 2);
                    if (data != null) {
                        gdfsUnitList.add(data);
                    }

                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        } finally {
            if (input != null) {
                try {
                    input.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }

        GdfWriter gdfWriter = null;
        try {
            gdfWriter = new GdfWriter(dest);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        gdfWriter.setHeader("CSPSA dump file created by Flash Kit");
        gdfWriter.writeGdfsFile(gdfsUnitList);
        gdfWriter.close();

    }

    /**
     * It should be used for parsing one GDData unit from input stream. Because of possible usage of
     * different size for GDFS index, size and block, method deal with number of bytes for
     * representing these items size.
     * 
     * @param in
     *            - input stream for fetching bytes for GDFS unit
     * @param blockLength
     *            - number of bytes for representation of GDFS block
     * @param unitLength
     *            - number of bytes for representation of GDFS unit
     * @param sizeLength
     *            - number of bytes for representation of GDFS size
     * @return - GDFS unit if parsing was successfull
     * @throws IOException
     */
    private static GDData parseGDFSUnit(InputStream in, int blockLength, int unitLength, int sizeLength)
        throws IOException {
        byte[] blockBytes = new byte[blockLength];
        byte[] unitBytes = new byte[unitLength];
        byte[] sizeBytes = new byte[sizeLength];

        // block
        if (in.read(blockBytes) != blockLength) {
            System.out.println("Missing block number in GDFS read response. padd from bulk");
            return null;
        }

        int block = LittleEndianByteConverter.byteArrayToInt(blockBytes);

        // unit
        if (in.read(unitBytes) != unitLength) {
            System.out.println("Missing unit number in GDFS read response. padd from bulk");
            return null;
        }
        int unit = LittleEndianByteConverter.byteArrayToInt(unitBytes);

        // size
        if (in.read(sizeBytes) != sizeLength) {
            System.out.println("Missing unit size in GDFS read response. padd from bulk");
            return null;
        }
        int dataLength = LittleEndianByteConverter.byteArrayToInt(sizeBytes);

        if (dataLength == 0) {
            System.out.println("unit length is zero. padd from bulk"); // padd from bulk
            return null;
        }
        byte[] unitData = new byte[dataLength];

        int readLength = in.read(unitData);
        if (readLength != dataLength) {
            System.out.println("The GDFS block length was specifed to " + dataLength + " bytes but only got "
                + readLength + " bytes.");
        }
        return new GDData(block, unit, unitData);
    }

}
