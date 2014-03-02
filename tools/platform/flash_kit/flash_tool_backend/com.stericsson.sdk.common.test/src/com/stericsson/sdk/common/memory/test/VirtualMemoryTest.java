package com.stericsson.sdk.common.memory.test;

import static com.stericsson.sdk.common.memory.VirtualMemory.KILO_BYTE;
import static com.stericsson.sdk.common.memory.VirtualMemory.PAYLOAD_BLOCK_SIZE;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.common.memory.IPayloadBlock;
import com.stericsson.sdk.common.memory.MemoryIOException;
import com.stericsson.sdk.common.memory.VirtualMemory;

/**
 * Testing class for operations performed over Virtual Memory. Tests are mostly oriented to not
 * corrupt memory integrity after I/O operations and there are couple more sophisticated test for
 * fetching software payload blocks from memory.
 * 
 * @author emicroh
 */
public class VirtualMemoryTest extends TestCase {

    private static byte[] data;

    static {
        data = new byte[255];
        for (int i = 0; i < data.length; i++) {
            data[i] = (byte) i;
        }
    }

    private VirtualMemory vm;

    /** Create fixture for testing methods. */
    @Before
    public void createVMInstance() {
        vm = new VirtualMemory(KILO_BYTE, (byte) 0xFF);
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testCapacity() throws MemoryIOException {
        // Junit3 does not use Before statements
        if (vm == null) {
            createVMInstance();
        }
        if (vm == null) {
            throw new MemoryIOException("vm is null");
        }
        Assert.assertTrue(vm.getCapacity() == (vm.getHighestAddress() + 1));
        Assert.assertTrue(vm.getReserved() == 0);

        int loopCnt = 10;
        for (int i = 0; i < loopCnt; i++) {
            vm.write(KILO_BYTE / 2 + i * KILO_BYTE, data);
        }

        long reserved = vm.getReserved();
        long capacity = vm.getCapacity();

        Assert.assertTrue("Written amount of data doesn't match to reserved byte count!", reserved == loopCnt
            * data.length);
        Assert.assertTrue("Remaining capacity doesn't match expected one!", capacity == vm.getHighestAddress() + 1
            - loopCnt * data.length);

        long payloadSize = 0L;
        while (vm.hasMorePayloadBlocks()) {
            IPayloadBlock nextPayloadBlock = vm.getNextPayloadBlock();
            if (nextPayloadBlock != null) {
                payloadSize += nextPayloadBlock.getLength();
            }
        }

        Assert.assertTrue("Payload size doesn't match to sum of blocks length!", payloadSize == vm.getPayloadSize());

    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    public void testWritingBeforeMemoryStart() throws MemoryIOException {
        // try-catch block for JUnit3 since it does not support "expected" tag
        try {
            // Junit3 does not use Before statements
            if (vm == null) {
                createVMInstance();
            }
            if (vm == null) {
                throw new Exception("vm is null");
            }
            vm.write(-1, data);
            fail("Should not get there.");
        } catch (MemoryIOException ioe) {
            // ignore since this one is expected
            assertTrue(true);
        } catch (Exception e) {
            fail("Expected MemoryIOException. Got " + e.getClass().getName());
        }
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    public void testWritingAfterMemoryEnd() throws MemoryIOException {
        try {
            // Junit3 does not use Before statements
            if (vm == null) {
                createVMInstance();
            }
            if (vm == null) {
                throw new Exception("vm is null");
            }
            vm.write(vm.getHighestAddress() + 1, data);
            fail("Should not get there.");
        } catch (MemoryIOException ioe) {
            // ignore since this one is expected
            assertTrue(true);
        } catch (Exception e) {
            fail("Expected MemoryIOException. Got " + e.getClass().getName());
        }
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testWriteOperationTest() throws MemoryIOException {
        // Junit3 does not use Before statements
        if (vm == null) {
            createVMInstance();
        }
        if (vm == null) {
            throw new MemoryIOException("vm is null");
        }
        vm.write(0, data);

        byte[] singleByte = new byte[] {
            0};

        try {
            vm.write(0, singleByte);
            Assert.assertTrue("Exception should be thrown when rewriting same address!", false);
        } catch (MemoryIOException e) {
            Assert.assertTrue(true);
        }

        try {
            vm.write(data.length - 1, singleByte);
            Assert.assertTrue("Exception should be thrown when rewriting same address!", false);
        } catch (MemoryIOException e) {
            Assert.assertTrue(true);
        }

        long lastUsed = vm.getLastUsedAddress();

        vm.write(lastUsed + 2, data);
        vm.write(lastUsed + 1, singleByte);
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testFreeOperation() throws MemoryIOException {
        byte gap = (byte) 0xFE;
        int unitSize = 10;

        vm = new VirtualMemory(unitSize, gap);

        byte[] writeBuf = new byte[unitSize];

        for (int i = 0; i < writeBuf.length; i++) {
            writeBuf[i] = (byte) i;
        }

        vm.write(0, writeBuf);

        Assert.assertTrue("Memory reservation failed", vm.getReserved() == unitSize);
        Assert.assertTrue("Usage efficiency should be 100%!", vm.usageEfficiency() == 1.0);

        vm.free(0, unitSize / 2);
        Assert.assertTrue("Freeing memory failed", vm.getReserved() == unitSize / 2);
        Assert.assertTrue("Usage efficiency should be 50%!", vm.usageEfficiency() == 0.5);

        vm.free();
        Assert.assertTrue("Usage efficiency should be 100%!", vm.getReserved() == 0 && vm.usageEfficiency() == 1.0);

        vm.write(unitSize / 2, writeBuf);

        Assert.assertTrue("Memory reservation failed", vm.getReserved() == unitSize);
        Assert.assertTrue("Usage efficiency should be 50%!", vm.usageEfficiency() == 0.5);

        vm.free(unitSize, unitSize / 2);
        Assert.assertTrue("Freeing memory failed", vm.getReserved() == unitSize / 2);
        Assert.assertTrue("Usage efficiency should be 50%!", vm.usageEfficiency() == 0.5);

        vm.free();
        Assert.assertTrue("Usage efficiency should be 100%!", vm.getReserved() == 0 && vm.usageEfficiency() == 1.0);

        vm.write(unitSize + unitSize / 2, writeBuf);

        Assert.assertTrue("Memory reservation failed", vm.getReserved() == unitSize);
        Assert.assertTrue("Usage efficiency should be 50%!", vm.usageEfficiency() == 0.5);

        vm.free(0, unitSize * 2);
        Assert.assertTrue("Freeing memory failed", vm.getReserved() == unitSize / 2);
        Assert.assertTrue("Usage efficiency should be 50%!", vm.usageEfficiency() == 0.5);
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testPayloadBlockSingleOddByte() throws MemoryIOException {
        vm = new VirtualMemory(KILO_BYTE, (byte) 0xFF);
        byte[] pattern = new byte[] {
            0};
        writeWithHoles(pattern, 256 * KILO_BYTE, true);
        checkWrittenBlocks();
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testPayloadBlockSingleEvenByte() throws MemoryIOException {
        vm = new VirtualMemory(KILO_BYTE, (byte) 0xFF);
        byte[] pattern = new byte[] {
            0};
        writeWithHoles(pattern, 256 * KILO_BYTE, false);
        checkWrittenBlocks();
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testPayloadBlock8BOdd() throws MemoryIOException {
        vm = new VirtualMemory(KILO_BYTE, (byte) 0xFF);
        byte[] pattern = new byte[] {
            0, 1, 2, 3, 4, 5, 6, 7};
        writeWithHoles(pattern, 32 * KILO_BYTE, true);
        checkWrittenBlocks();
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testPayloadBlock8BEven() throws MemoryIOException {
        vm = new VirtualMemory(KILO_BYTE, (byte) 0xFF);
        byte[] pattern = new byte[] {
            0, 1, 2, 3, 4, 5, 6, 7};
        writeWithHoles(pattern, 32 * KILO_BYTE, false);
        checkWrittenBlocks();
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testPayloadBlock256BOdd() throws MemoryIOException {
        vm = new VirtualMemory(KILO_BYTE, (byte) 0xFF);
        byte[] pattern = new byte[256];
        for (int i = 0; i < pattern.length; i++) {
            pattern[i] = (byte) i;
        }
        writeWithHoles(pattern, 8 * KILO_BYTE, true);
        checkWrittenBlocks();
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testPayloadBlock256BEven() throws MemoryIOException {
        vm = new VirtualMemory(KILO_BYTE, (byte) 0xFF);
        byte[] pattern = new byte[256];
        for (int i = 0; i < pattern.length; i++) {
            pattern[i] = (byte) i;
        }
        writeWithHoles(pattern, 8 * KILO_BYTE, false);
        checkWrittenBlocks();
    }

    /**
     * Test method.
     * 
     * @throws MemoryIOException
     *             When I/O error occurs while testing.
     */
    @Test
    public void testEdgedBlock() throws MemoryIOException {
        int blockSize = 64;
        int loopCnt = 4;

        vm = new VirtualMemory(blockSize, (byte) 0xFF);

        byte[] buf = new byte[blockSize / loopCnt];
        for (int i = 0; i < buf.length; i++) {
            buf[i] = (byte) i;
        }

        writeWithHoles(buf, loopCnt, true);

        List<IPayloadBlock> blocks = new ArrayList<IPayloadBlock>();
        while (vm.hasMorePayloadBlocks()) {
            blocks.add(vm.getNextPayloadBlock());
        }

        IPayloadBlock last = blocks.get(blocks.size() - 1);

        Assert.assertTrue("Blocks count doesn't match expectation!", blocks.size() == loopCnt - 1);
        Assert.assertTrue("Last block alignment doesn't match expectation!", blockSize == last.getOffset()
            + last.getLength());
    }

    private void writeWithHoles(byte[] pattern, int loopCnt, boolean odd) throws MemoryIOException {
        for (int i = 0; i < loopCnt; i++) {
            if (odd && (i % 2 == 1)) {
                vm.write(i * pattern.length, pattern);
            } else if (!odd && (i % 2 == 0)) {
                vm.write(i * pattern.length, pattern);
            }
        }
    }

    private void checkWrittenBlocks() throws MemoryIOException {
        long firstUsed = vm.getFirstUsedAddress();
        long lastUsed = vm.getLastUsedAddress();

        boolean reserved = true;

        long pointer = firstUsed;
        long blockStart = pointer;

        while (pointer < lastUsed) {
            reserved = vm.isReserved(pointer);
            pointer = getNextPointer(pointer, !reserved);

            Assert.assertTrue("More payload blocks expected in VM!", vm.hasMorePayloadBlocks());
            IPayloadBlock nextBlock = vm.getNextPayloadBlock();

            if (nextBlock == null) {
                fail("Block is null");
                return;
            }
            Assert.assertTrue("Reserved flag doesn't match!", reserved == nextBlock.containsData());
            Assert.assertTrue("Block start doesn't match!", nextBlock.getOffset() == blockStart);
            if (!(nextBlock.getLength() == (pointer - blockStart))) {
                Assert.assertTrue("Block length doesn't match!", nextBlock.getLength() == (pointer - blockStart));
            }

            blockStart = pointer;
        }

    }

    private long getNextPointer(long pointer, boolean reserved) {
        long lastUsed = vm.getLastUsedAddress();
        long searchFrom = pointer;

        while ((pointer <= lastUsed) && (reserved ^ vm.isReserved(pointer))) {
            pointer++;
            if ((pointer % PAYLOAD_BLOCK_SIZE == 0) || ((pointer - searchFrom + 1) >= PAYLOAD_BLOCK_SIZE)) {
                break;
            }
        }
        return pointer;
    }
}
