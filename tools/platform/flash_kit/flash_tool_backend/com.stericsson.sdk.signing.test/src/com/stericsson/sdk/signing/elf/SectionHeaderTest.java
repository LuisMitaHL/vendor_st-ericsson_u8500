package com.stericsson.sdk.signing.elf;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.test.Activator;

/**
 * @author emicroh
 * 
 */
public class SectionHeaderTest extends TestCase {

    private int getRandomInt(Random r) {
        return (Math.abs(r.nextInt()) % 256);
    }

    private String getResourceFilePath(String filepath) {
        return Activator.getResourcesPath() + filepath;
    }

    /**
     * 
     */
    @Test
    public void testGetSet() {
        SectionHeader sh = new SectionHeader();

        Random r = new Random(System.currentTimeMillis());

        byte[] data = new byte[20];
        r.nextBytes(data);

        int random = getRandomInt(r);
        long v = Math.abs(r.nextLong() % 256);
        String str = new String("hello");

        sh.setName(random);
        assertTrue(sh.getName() == random);

        sh.setType(random);
        assertTrue(sh.getType() == random);

        sh.setFlags(random);
        assertTrue(sh.getFlags() == random);

        sh.setAddress(random);
        assertTrue(sh.getAddress() == random);

        sh.setOffset(v);
        assertTrue(sh.getOffset() == v);

        sh.setSize(v);
        assertTrue(sh.getSize() == v);

        sh.setLink(random);
        assertTrue(sh.getLink() == random);

        sh.setInfo(random);
        assertTrue(sh.getInfo() == random);

        sh.setAddressAlign(random);
        assertTrue(sh.getAddressAlign() == random);

        sh.setEntrySize(random);
        assertTrue(sh.getEntrySize() == random);

        sh.setStringName(str);
        assertTrue(sh.getStringName().equals(str));
    }

    /**
     * 
     */
    @Test
    public void testSectionHeader() {

        FileChannel channel = null;
        FileInputStream fis = null;
        FileHeader fileHeader;
        List<SectionHeader> sectionHeader;

        try {
            fis = new FileInputStream(getResourceFilePath("/a2/module.elf"));
            channel = fis.getChannel();
            fileHeader = new FileHeader();
            if (channel == null) {
                throw new IOException("channel is null");
            }
            fileHeader.read(channel);
            sectionHeader = SectionHeaderTable.create(channel, fileHeader);

            assertTrue(sectionHeader.size() > 0);
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
    public void testGetEntriesFromNames() {
        FileChannel channel = null;
        FileInputStream fis = null;
        FileHeader fileHeader;
        List<SectionHeader> sectionHeaderTable;

        try {
            fis = new FileInputStream(getResourceFilePath("/a2/module.elf"));
            channel = fis.getChannel();
            if (channel == null) {
                throw new IOException("channel is null");
            }
            fileHeader = new FileHeader();
            fileHeader.read(channel);
            sectionHeaderTable = SectionHeaderTable.create(channel, fileHeader);
            assertNotNull(sectionHeaderTable);
            assertTrue(sectionHeaderTable.size() > 0);

            Iterator<SectionHeader> sections = SectionHeaderTable.getEntriesFromNames(sectionHeaderTable, new String[] {
                ".note"});
            assertNotNull(sections);
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
}
