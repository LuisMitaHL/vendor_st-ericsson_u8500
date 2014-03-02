package com.stericsson.sdk.signing.elf;

import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.Random;

import junit.framework.TestCase;

import org.junit.Assert;
import org.junit.Test;

import com.stericsson.sdk.signing.test.Activator;

/**
 * @author xtomlju
 */
public class FileHeaderTest extends TestCase {

    private static final String ELF_32_CREATED_FILE_FILENAME = "created_module.elf";

    private static final String ELF_64_CREATED_FILE_FILENAME = "created_module_64.elf";

    private static final String ELF_32_FILE_FILENAME = "/a2/module.elf";

    private String getResourceFilePath(String filepath) {
        return Activator.getResourcesPath() + filepath;
    }

    /**
     * 
     */
    @Test
    public void testRead() {

        FileChannel channel = null;
        FileInputStream fis = null;
        FileHeader header = null;

        try {
            fis = new FileInputStream(getResourceFilePath(ELF_32_FILE_FILENAME));
            channel = fis.getChannel();
            header = new FileHeader(FileHeader.CLASS_32BIT);
            header.read(channel);

            assertTrue(header.getELFClass() == FileHeader.CLASS_32BIT);
            assertTrue(header.getElfHeaderSize() == 52);
            assertTrue(header.getBytes().length == header.getElfHeaderSize());
        } catch (IOException e) {
            fail(e.getMessage());
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * 
     */
    @Test
    public void testWrite() {

        FileChannel writeChannel = null;
        FileChannel readChannel = null;
        FileOutputStream fos = null;
        FileInputStream fis = null;
        FileHeader header = null;
        File outputFile = null;

        try {
            outputFile =
                new File((new File(getResourceFilePath(ELF_32_FILE_FILENAME))).getParent() + File.separator
                    + ELF_32_CREATED_FILE_FILENAME);
            fos = new FileOutputStream(outputFile);
            writeChannel = fos.getChannel();
            if (writeChannel == null) {
                throw new IOException("channel is null");
            }
            header = new FileHeader(FileHeader.CLASS_32BIT);
            header.write(writeChannel);

            assertTrue(outputFile.exists());
            fis = new FileInputStream(outputFile);
            readChannel = fis.getChannel();
            if (readChannel == null) {
                throw new IOException("channel is null");
            }
            header = new FileHeader(FileHeader.CLASS_32BIT);
            header.read(readChannel);
        } catch (IOException e) {
            fail(e.getMessage());
        } finally {
            cleanup(readChannel, writeChannel, fis, fos);
            if (outputFile != null) {
                outputFile.delete();
            }
        }
    }

    /**
     * 
     */
    @Test
    public void testWrite64() {
        FileChannel writeChannel = null;
        FileChannel readChannel = null;
        FileOutputStream fos = null;
        FileInputStream fis = null;
        FileHeader header = null;
        File outputFile = null;

        try {
            outputFile =
                new File((new File(getResourceFilePath(ELF_32_FILE_FILENAME))).getParent() + File.separator
                    + ELF_64_CREATED_FILE_FILENAME);
            fos = new FileOutputStream(outputFile);
            writeChannel = fos.getChannel();
            header = new FileHeader(FileHeader.CLASS_64BIT);
            if (writeChannel == null) {
                throw new IOException("channel is null");
            }
            header.write(writeChannel);

            assertTrue(outputFile.exists());
            fis = new FileInputStream(outputFile);
            readChannel = fis.getChannel();
            if (readChannel == null) {
                throw new IOException("channel is null");
            }
            header = new FileHeader(FileHeader.CLASS_64BIT);
            header.read(readChannel);
        } catch (IOException e) {
            fail(e.getMessage());
        } finally {
            cleanup(readChannel, writeChannel, fis, fos);
            if (outputFile != null) {
                outputFile.delete();
            }
        }
    }

    private void cleanup(Closeable... closeables) {
        if (closeables != null) {
            for (Closeable c : closeables) {
                if (c != null) {
                    try {
                        c.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    private int getRandomInt(Random r) {
        return (Math.abs(r.nextInt()) % 256);
    }

    /**
     * @throws ELFException
     *             TBD
     */
    @Test
    public void smokeTest() throws ELFException {
        FileHeader fh = new FileHeader();

        Random r = new Random(System.currentTimeMillis());

        byte[] data = new byte[20];
        r.nextBytes(data);

        int random = getRandomInt(r);

        fh.setELFByteOrder(ByteOrder.BIG_ENDIAN);
        Assert.assertTrue(fh.getELFByteOrder() == ByteOrder.BIG_ENDIAN);

        fh.setType(random);
        Assert.assertTrue(fh.getType() == random);

        fh.setMachine(random);
        Assert.assertTrue(fh.getMachine() == random);

        fh.setVersion(random);
        Assert.assertTrue(fh.getVersion() == random);

        long v = Math.abs(r.nextLong());
        fh.setEntry(v);
        Assert.assertTrue(fh.getEntry() == v);

        fh.setProgramHeaderOffset(random);
        Assert.assertTrue(fh.getProgramHeaderOffset() == random);

        fh.setSectionHeaderOffset(random);
        Assert.assertTrue(fh.getSectionHeaderOffset() == random);

        fh.setFlags(random);
        Assert.assertTrue(fh.getFlags() == random);

        fh.setProgramHeaderNumOfEntries(random);
        Assert.assertTrue(fh.getProgramHeaderNumOfEntries() == random);

        fh.setSectionHeaderNumOfEntries(random);
        Assert.assertTrue(fh.getSectionHeaderNumOfEntries() == random);

        fh.setSectionHeaderStringTableIndex(random);
        Assert.assertTrue(fh.getSectionHeaderStringTableIndex() == random);

    }

}
