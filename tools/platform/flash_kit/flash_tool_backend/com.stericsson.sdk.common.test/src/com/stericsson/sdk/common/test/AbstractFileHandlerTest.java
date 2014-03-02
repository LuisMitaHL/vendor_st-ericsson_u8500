package com.stericsson.sdk.common.test;

import static org.easymock.EasyMock.createMock;
import static org.easymock.EasyMock.expect;
import static org.easymock.EasyMock.replay;
import static org.easymock.EasyMock.verify;

import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.common.AbstractFileHandler;
import com.stericsson.sdk.common.memory.IPayloadBlock;
import com.stericsson.sdk.common.memory.MemoryIOException;

/**
 * 
 * 
 */
public class AbstractFileHandlerTest extends TestCase{

    Extender mClassUnderTest;

    /**
     * 
     * @throws Exception
     *             e
     */
    @Before
    public void setUp() throws Exception {
        mClassUnderTest = new Extender();
    }

    /**
     * 
     * @throws Exception
     *             e
     */
    @After
    public void tearDown() throws Exception {
        mClassUnderTest = null;
    }

    /**
     * 
     * @throws IOException
     *             e
     */
    @Test
    public final void testWriteDumpFile() throws IOException {
        PipedInputStream pis = new PipedInputStream();
        PipedOutputStream pos = new PipedOutputStream(pis);
        mClassUnderTest.writeDumpFile(pos);
        assertEquals("First byte not the same", 01, pis.read());
        assertEquals("Second byte not the same", 02, pis.read());
        assertEquals("Third byte not the same", 03, pis.read());
        assertEquals("Last byte not the same", 04, pis.read());
        verify(mClassUnderTest.block1);
    }

    /**
     * 
     * @throws IOException
     *             e
     */
    @Test
    public final void testWriteTwoDumpFiles() throws IOException {
        mClassUnderTest.amountOfBlocks = 2;
        PipedInputStream pis = new PipedInputStream();
        PipedOutputStream pos = new PipedOutputStream(pis);
        mClassUnderTest.writeDumpFile(pos);
        for (int i = 0; i < 2; ++i) {
            assertEquals("First byte not the same", 01, pis.read());
            assertEquals("Second byte not the same", 02, pis.read());
            assertEquals("Third byte not the same", 03, pis.read());
            assertEquals("Last byte not the same", 04, pis.read());
        }
        verify(mClassUnderTest.block1);
    }

    /**
     * 
     * @throws IOException
     *             e
     */
    @Test
    public final void testWriteTenDumpFiles() throws IOException {
        mClassUnderTest.amountOfBlocks = 10;
        PipedInputStream pis = new PipedInputStream();
        PipedOutputStream pos = new PipedOutputStream(pis);
        mClassUnderTest.writeDumpFile(pos);
        for (int i = 0; i < 10; ++i) {
            assertEquals("First byte not the same", 01, pis.read());
            assertEquals("Second byte not the same", 02, pis.read());
            assertEquals("Third byte not the same", 03, pis.read());
            assertEquals("Last byte not the same", 04, pis.read());
        }
        verify(mClassUnderTest.block1);
    }

    /**
     * 
     * 
     */
    public class Extender extends AbstractFileHandler {

        /** */
        private IPayloadBlock block1;

        /** */
        private int amountOfBlocks = 1;

        /**
         * 
         */
        public Extender() {
            super(null);
        }

        /**
         * @throws IOException
         *             e
         * @return list
         */
        @Override
        public byte[] getPayload() throws IOException {
            assertTrue(false);
            return null;
        }

        /**
         * @return list
         */
        @Override
        public List<IPayloadBlock> getPayloadBlocks() {
            ArrayList<IPayloadBlock> list = new ArrayList<IPayloadBlock>();
            block1 = createMock(IPayloadBlock.class);

            for (int i = 0; i < amountOfBlocks; ++i) {
                list.add(block1);

                expect(block1.getPositionInfo()).andReturn(new byte[] {
                    01, 02});

                try {
                    expect(block1.getData()).andReturn(new byte[] {
                        03, 04});
                } catch (MemoryIOException e) {
                    assertTrue(false);
                }
            }
            replay(block1);

            return list;
        }

        /**
         * @throws IOException
         *             e
         */
        @Override
        public void load() throws IOException {
            assertTrue(false);
        }
    }
}
