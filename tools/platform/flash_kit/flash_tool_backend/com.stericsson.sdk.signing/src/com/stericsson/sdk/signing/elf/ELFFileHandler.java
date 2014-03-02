package com.stericsson.sdk.signing.elf;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.AbstractFileHandler;
import com.stericsson.sdk.common.memory.IPayloadBlock;
import com.stericsson.sdk.common.memory.VirtualMemory;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * 
 * This class represents elf file loaded into memory.
 * 
 * @author emicroh
 */
public class ELFFileHandler extends AbstractFileHandler {

    private static final Logger log = Logger.getLogger(ELFFileHandler.class.getName());

    private static final int TYPE_LOAD = 0x1;

    private static final int TYPE_PROGBITS = 0x1;

    private static final int TYPE_LOUSER = 0x80000100;

    private static final int TYPE_HIUSER = 0x8FFFFFFF;

    /** Identifier for binary format of loading. */
    public static final int FORMAT_BINARY = 0;

    /** Identifier for elf format of loading. */
    public static final int FORMAT_ELF = 1;

    /** */
    private VirtualMemory virtualMemory;

    /**
     * @see AbstractFileHandler
     * 
     * @param input
     *            Input file for super constructor.
     */
    public ELFFileHandler(File input) {
        super(input);
        virtualMemory = new VirtualMemory(256 * VirtualMemory.KILO_BYTE, (byte) 0xFF);
    }

    /**
     * @return Payload in desired form.
     * 
     * @throws IOException
     *             When I/O operation fails during fetching payload.
     */
    public byte[] getPayload() throws IOException {
        throw new IOException("getPayload() method not implemented");
    }

    /**
     * Perform input file loading.
     * 
     * @throws IOException
     *             When I/O operation fails during input file loading.
     */
    public void load() throws IOException {
        load(FORMAT_BINARY);
    }

    /**
     * @return Return software payload blocks for the software image loaded into memory.
     */
    public List<IPayloadBlock> getPayloadBlocks() {
        virtualMemory.reset();
        List<IPayloadBlock> res = new ArrayList<IPayloadBlock>();
        int payloadLength = 0;
        while (virtualMemory.hasMorePayloadBlocks()) {
            IPayloadBlock block = virtualMemory.getNextPayloadBlock();
            if (block == null) {
                throw new RuntimeException("payload block is null.");
            }
            payloadLength += block.getLength();
            res.add(block);
        }
        return res;
    }

    /**
     * Calculates list of hashes of given raw data.
     * 
     * @param data
     *            raw data.
     * @param hashBlockSize
     *            size of hash block.
     * @return list of hashes.
     * @throws IOException if an I/O error occurs.
     */
    public static byte[] createHashListData(final byte[] data, final int hashBlockSize) throws IOException {

        if (hashBlockSize == 0) {
            return new byte[0];
        }
        int hashCount = data.length / hashBlockSize + 1;

        if ((data.length % hashBlockSize) == 0) {
            hashCount--;
        }

        ByteArrayOutputStream output = new ByteArrayOutputStream();

        DigestDecorator digest = new DigestDecorator(DigestDecorator.SHA_256);

        int offset = 0;
        long length = hashBlockSize;
        for (int j = 0; j < hashCount; j++) {
            length = Math.min(data.length - offset, (int) length);
            digest.update(data, offset, (int) length);
            offset = offset + (int) length;
        }
        byte[] buffer = digest.digest();
        output.write(buffer);

        return output.toByteArray();
    }

    /**
     * Perform loading according given format.
     * 
     * @param format
     *            Only {@link ELFFileHandler#FORMAT_BINARY} or {@link ELFFileHandler#FORMAT_ELF}
     *            will be accepted. Binary loading is used as default.
     * 
     * @throws IOException
     *             When I/O operation fails during input file loading.
     */
    public void load(int format) throws IOException {
        switch (format) {
            case FORMAT_BINARY:
                loadBinary();
                break;
            case FORMAT_ELF:
                loadELF();
                break;
            default:
                loadBinary();
        }
    }

    private void loadBinary() throws IOException {
        FileChannel input = null;
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(inputFile);
            input = fis.getChannel();
            ByteBuffer buf = ByteBuffer.allocate((int) input.size());
            input.read(buf);
            virtualMemory.write(0, buf.array());
        } finally {
            if (input != null) {
                try {
                    input.close();
                } catch (Exception e) {
                    e.getMessage();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (Exception e) {
                    e.getMessage();
                }
            }
        }
    }

    private void loadELF() throws IOException {
        FileChannel channel = null;
        FileInputStream fis = null;
        FileHeader header;

        try {
            fis = new FileInputStream(inputFile);
            channel = fis.getChannel();
            header = new FileHeader();
            header.read(channel);

            if (header.getProgramHeaderOffset() != 0) {
                loadDataFromSegments(channel, header);
            } else {
                loadDataFromSections(channel, header);
            }
        } catch (ELFException e) {
            throw new IOException(e.getMessage());
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

    private void loadDataFromSegments(FileChannel channel, FileHeader header) throws IOException {
        log.info("Loading image data from ELF segments");

        List<ProgramHeader> programHeaderTable = ProgramHeaderTable.create(channel, header);

        for (ProgramHeader programHeader : programHeaderTable) {
            if (programHeader.getType() == TYPE_LOAD) {

                channel.position(programHeader.getOffset());

                ByteBuffer buffer;

                if (programHeader.getFileSize() == 0) {
                    buffer = ByteBuffer.allocate((int) programHeader.getMemorySize());
                } else {
                    buffer = ByteBuffer.allocate((int) programHeader.getFileSize());
                }

                while (buffer.remaining() > 0) {
                    channel.read(buffer);
                }

                virtualMemory.write(programHeader.getVirtualAddress(), buffer.array());
            }
        }
    }

    private void loadDataFromSections(FileChannel channel, FileHeader header) throws IOException {
        log.info("Loading image data from ELF sections");

        List<SectionHeader> sectionHeaderTable = SectionHeaderTable.create(channel, header);

        for (SectionHeader sectionHeader : sectionHeaderTable) {

            if (sectionHeader.getStringName().startsWith(".")) {
                if ((sectionHeader.getType() == TYPE_PROGBITS)
                    || ((sectionHeader.getType() >= TYPE_LOUSER) && (sectionHeader.getType() <= TYPE_HIUSER))) {

                    channel.position(sectionHeader.getOffset());

                    ByteBuffer buffer = ByteBuffer.allocate((int) sectionHeader.getSize());

                    while (buffer.remaining() > 0) {
                        channel.read(buffer);
                    }

                    virtualMemory.write(sectionHeader.getAddress(), buffer.array());
                }
            }
        }
    }

}
