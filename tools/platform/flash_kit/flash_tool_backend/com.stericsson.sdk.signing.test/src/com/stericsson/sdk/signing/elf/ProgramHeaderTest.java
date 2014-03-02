package com.stericsson.sdk.signing.elf;

import java.io.IOException;
import java.util.Random;

import junit.framework.TestCase;

import org.junit.Test;

/**
 * @author emicroh
 *
 */
public class ProgramHeaderTest extends TestCase{

    private int getRandomInt(Random r) {
        return (Math.abs(r.nextInt()) % 256);
    }

    /**
     * @throws IOException TBD
     */
    @Test
    public void testGetSet() throws IOException {
        ProgramHeader ph = new ProgramHeader();
        Random r = new Random(System.currentTimeMillis());

        byte[] data = new byte[20];
        r.nextBytes(data);

        int random = getRandomInt(r);

        ph.setType(random);
        assertTrue(ph.getType() == random);

        ph.setType(random);
        assertTrue(ph.getType() == random);

        ph.setOffset(random);
        assertTrue(ph.getOffset() == random);

        ph.setVirtualAddress(random);
        assertTrue(ph.getVirtualAddress() == random);

        ph.setPhysicalAddress(random);
        assertTrue(ph.getPhysicalAddress() == random);

        ph.setFileSize(random);
        assertTrue(ph.getFileSize() == random);

        ph.setMemorySize(random);
        assertTrue(ph.getMemorySize() == random);

        ph.setFlags(random);
        assertTrue(ph.getFlags() == random);

        ph.setAlign(random);
        assertTrue(ph.getAlign() == random);

        ph.setData(data);
        assertTrue(ph.getData().length == data.length);

    }

}
