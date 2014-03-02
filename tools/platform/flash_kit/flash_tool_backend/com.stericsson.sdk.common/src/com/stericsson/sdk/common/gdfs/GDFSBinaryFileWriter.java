package com.stericsson.sdk.common.gdfs;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.common.LittleEndianByteConverter;

/**
 * Contains methods to write GDFS binary files.
 * 
 * @author pkutac01
 * 
 */
public final class GDFSBinaryFileWriter {

    private GDFSBinaryFileWriter() {

    }

    /**
     * Writes GDFS binary file from the list of GDFS data units.
     * 
     * @param pGDFSBinaryFile
     *            Output GDFS binary file.
     * @param pGDFSDataList
     *            List of GDFS data units.
     * @throws IOException
     *             I/O Exception.
     */
    public static void writeGDFSBinaryFile(File pGDFSBinaryFile, List<GDData> pGDFSDataList) throws IOException {
        int size = 0;
        for (GDData gdfsData : pGDFSDataList) {
            // size = block (2 bytes) + index (4 bytes) + length (2 bytes) + data (length)
            size += 8 + gdfsData.getData().length;
        }

        ByteBuffer byteBuffer = ByteBuffer.allocate(size);

        for (GDData gdfsData : pGDFSDataList) {
            byteBuffer.put(LittleEndianByteConverter.valueToByteArray(gdfsData.getBlock(), 2));
            byteBuffer.put(LittleEndianByteConverter.valueToByteArray(gdfsData.getIndex(), 4));
            byteBuffer.put(LittleEndianByteConverter.valueToByteArray(gdfsData.getLength(), 2));
            byteBuffer.put(gdfsData.getData());
        }

        FileOutputStream outputStream = new FileOutputStream(pGDFSBinaryFile);

        try {
            outputStream.write(byteBuffer.array());
            outputStream.flush();
        } finally {
            outputStream.close();
        }
    }

    /**
     * Writes GDFS binary file containing data from the existing GDFS text file.
     * 
     * @param pGDFSBinaryFile
     *            Output GDFS binary file.
     * @param pGDFSTextFile
     *            Input GDFS text file.
     * @throws IOException
     *             I/O Exception.
     */
    public static void writeGDFSBinaryFile(File pGDFSBinaryFile, File pGDFSTextFile) throws IOException {
        GdfParser parser = new GdfParser(pGDFSTextFile);
        List<GDData> gdfsDataList = new ArrayList<GDData>();

        try {
            while (parser.hasMoreData()) {
                GDData gdfsData = parser.nextData();

                if (gdfsData != null) {
                    gdfsDataList.add(gdfsData);
                }
            }
        } finally {
            parser.close();
        }

        GDFSBinaryFileWriter.writeGDFSBinaryFile(pGDFSBinaryFile, gdfsDataList);
    }

}
