package com.stericsson.sdk.common.moto;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.common.AbstractFileHandler;
import com.stericsson.sdk.common.memory.IPayloadBlock;
import com.stericsson.sdk.common.memory.MemoryIOException;
import com.stericsson.sdk.common.memory.VirtualMemory;

/**
 * @author emicroh
 */
public class A01FileHandler extends AbstractFileHandler {

    private VirtualMemory virtualMemory;

    /**
     * @param inputFile
     *            TBD
     */
    public A01FileHandler(File inputFile) {
        super(inputFile);
        virtualMemory = new VirtualMemory(256 * VirtualMemory.KILO_BYTE, (byte) 0xFF);
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.common.moto.AbstractFileHandler#getPayload()
     */
    @Override
    public byte[] getPayload() throws MemoryIOException {
        List<IPayloadBlock> payloadBlocks = getPayloadBlocks();
        int payloadLength = (int) (virtualMemory.getLastUsedAddress() - virtualMemory.getFirstUsedAddress() + 1);

        byte[] res = new byte[payloadLength];
        int pos = 0;
        for (IPayloadBlock block : payloadBlocks) {
            System.arraycopy(block.getData(), 0, res, pos, block.getLength());
        }
        return res;
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.common.AbstractFileHandler#getPayloadBlocks()
     */
    @Override
    public List<IPayloadBlock> getPayloadBlocks() {
        virtualMemory.reset();
        List<IPayloadBlock> res = new ArrayList<IPayloadBlock>();
        int payloadLength = 0;
        while (virtualMemory.hasMorePayloadBlocks()) {
            IPayloadBlock block = virtualMemory.getNextPayloadBlock();
            if (block != null) {
                payloadLength += block.getLength();
                res.add(block);
            }
        }
        return res;
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.common.moto.AbstractFileHandler#load()
     */
    @Override
    public void load() throws IOException {
        LineNumberReader reader = null;
        int lineNumber = 0;
        String line;

        long offset = 0;

        try {
            reader = new LineNumberReader(new InputStreamReader(new FileInputStream(inputFile), "UTF-8"));
            while ((line = reader.readLine()) != null) {
                lineNumber++;
                line = line.trim();
                if (line.length() > 0) {
                    SRecord sRec = new SRecord(line);
                    if (sRec.isDataRecord()) {
                        virtualMemory.write(sRec.getAddress() + offset, sRec.getData());
                    }
                }
            }
        } catch (ParseException e) {
            throw new IOException("Line " + lineNumber + " parsing error: " + e.getMessage());
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
