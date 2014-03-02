package com.stericsson.sdk.assembling.internal.preflash;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.text.ParseException;
import java.util.logging.Logger;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.u8500.U8500ConfigurationEntry;
import com.stericsson.sdk.assembling.utilities.StreamCloser;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.moto.SRecord;

/**
 * @author xtspepa
 * 
 */
public class SRecordConverter {

    /**
     * 
     */
    public static final long ATTRIBUTE_NOT_SET = -1;

    /** Temporary preflash image */
    private static final String PREFLASH_TEMP_NAME = "preflash_bin";

    /**
     * 
     */
    private static Logger logger = Logger.getLogger(SRecordConverter.class.getName());

    /**
     * 
     */
    private long mRangeStart;

    /**
     * 
     */
    private long mNextPossibleStartAddress;

    /**
     * 
     */
    private long mFirstLoadedAddress = -1; // Keeps track of the first (lowest)

    /**
     * 
     */
    private long mLastLoadedAddress = -1; // Keeps track of the last (highest)

    /**
     * Creates an ImageConverter spanning over entire addressable memory.
     */
    public SRecordConverter() {
        this(0x00000000);
    }

    /**
     * Creates an Converter starting at a specified address of the addressable memory.
     * 
     * @param startAddress
     *            The first address to place data on.
     */
    public SRecordConverter(long startAddress) {
        mRangeStart = startAddress;
        mNextPossibleStartAddress = mRangeStart;
    }

    /**
     * Resets information about addressed loaded during latest "load" call.
     * 
     */
    public void resetLoadingData() {
        mFirstLoadedAddress = -1;
        mLastLoadedAddress = -1;
    }

    /**
     * Finds the first address in specified Motorola S-record file, that is also within the
     * specified address range.
     * 
     * @param fileName
     *            the S3 file
     * @param rangeStart
     *            the start of the range
     * @param rangeEnd
     *            the end of the range
     * @return the first address
     * @throws IOException
     *             If the file cant be read
     * @throws AssemblerException
     *             If the syntax of the S3 file is incorrect
     */
    public long getFirstAddressInS3File(String fileName, long rangeStart, long rangeEnd) throws IOException,
        AssemblerException {

        BufferedReader reader = null;
        int loopCount = 0;
        SRecord record = null;
        try {
            reader = new BufferedReader(new InputStreamReader(new FileInputStream(fileName), "UTF-8"));
            String line = null;
            while ((line = reader.readLine()) != null) {
                loopCount++;
                try {
                    record = new SRecord(line);
                    if (record.isDataRecord() && record.adjustToRange(rangeStart, rangeEnd)) {
                        return record.getAddress();
                    }
                } catch (Exception e) {
                    throw new AssemblerException("Cannot convert row " + loopCount + "." + e.getMessage());
                }
            }

        } finally {
            StreamCloser.close(reader);
        }

        return -1;
    }

    /**
     * Writes the binary to file. Also checks/updates information keeping track of used addresses.
     * 
     * @param startAddress
     *            The address to write the data to.
     * @param data
     *            The data to write.
     * @param offset
     *            The offset to use.
     * @param length
     *            The amount of data to write.
     * @throws IOException
     *             If the given address is already reserved in heap.
     */
    private void writeBinary(long startAddress, byte[] data, int offset, int length, OutputStream output)
        throws IOException {

        output.write(data);

        // Update information about used and loaded addresses
        long endAddress = startAddress + length - 1;

        if ((mFirstLoadedAddress == -1) || (startAddress < mFirstLoadedAddress)) {
            mFirstLoadedAddress = startAddress;
        }

        if ((mLastLoadedAddress == -1) || (endAddress > mLastLoadedAddress)) {
            mLastLoadedAddress = endAddress;
        }
    }

    /**
     * Converts a S3 file to a binary file
     * 
     * @param fileName
     *            The file to load
     * @param rangeStart
     *            Range start
     * @param rangeEnd
     *            Range end
     * @param preserveOrg
     *            Preserve org
     * @param org
     *            Org
     * @param align
     *            Alignment
     * @param output
     *            The outoutstream to write the binary file to
     * @throws IOException
     *             I/O exception
     * @throws AssemblerException
     *             Syntax error exception
     */
    public void writeS3AsBinary(String fileName, long rangeStart, long rangeEnd, boolean preserveOrg, long org,
        long align, OutputStream output) throws IOException, AssemblerException {
        resetLoadingData();

        logger.fine("Converting Motorola S-record file: " + fileName + "...");
        long addressOffset = 0;

        addressOffset = calculateStartAddress(fileName, rangeStart, rangeEnd, preserveOrg, org, align, addressOffset);

        BufferedReader reader = null;

        int binData = 0;
        int loopCount = 0;
        String line;
        String lookahead;
        SRecord record = new SRecord();
        SRecord lookaheadrecord = new SRecord();

        try {
            reader = new BufferedReader(new InputStreamReader(new FileInputStream(fileName), "UTF-8"));
            line = reader.readLine();
            if (line != null) {
                loopCount++;
                try {
                    record.setLine(line);
                } catch (ParseException e1) {
                    throw new AssemblerException("Cannot convert row " + loopCount + "." + e1.getMessage());
                }
                while ((lookahead = reader.readLine()) != null) {
                    loopCount++;
                    try {

                        lookaheadrecord.setLine(lookahead);
                    } catch (ParseException e1) {
                        throw new AssemblerException("Cannot convert row " + loopCount + "." + e1.getMessage());
                    }

                    if (record.isDataRecord() && record.adjustToRange(rangeStart, rangeEnd)) {
                        long firstAddressInRecord = record.getAddress();
                        long lookaheadfirstAddressInRecord = lookaheadrecord.getAddress();
                        byte[] data = record.getData();
                        long lastAddressInRecord = firstAddressInRecord + data.length - 1;
                        try {
                            if (!((firstAddressInRecord > rangeEnd) || (lastAddressInRecord < rangeStart))) {
                                // Find the part of the record (possibly whole) that is
                                // inside range
                                long firstAddressToRead = Math.max(firstAddressInRecord, rangeStart);
                                long lastAddressToRead = Math.min(lastAddressInRecord, rangeEnd);
                                int offset = (int) (firstAddressToRead - firstAddressInRecord);
                                int length = (int) (lastAddressToRead - firstAddressToRead + 1);

                                long destAddress = firstAddressToRead + addressOffset;
                                writeBinary(destAddress, data, offset, length, output);
                                checkAndFillGap(output, lookaheadfirstAddressInRecord, lastAddressInRecord);

                                mNextPossibleStartAddress = destAddress + length;
                            }
                        } catch (ArrayIndexOutOfBoundsException e) {
                            throw new IOException("Failed load and convert software: " + e.getMessage());
                        }
                        binData += data.length;
                    }
                    line = lookahead;
                    try {
                        record.setLine(line);
                    } catch (ParseException e1) {
                        throw new AssemblerException("Cannot convert row " + loopCount + "." + e1.getMessage());
                    }
                }
                // Check the last line as well
                checkLastRecord(rangeStart, rangeEnd, output, addressOffset, record);
            }
        } finally {
            StreamCloser.close(reader);
        }

        logger.fine("Motorola S-record converted to binary!");
    }

    /**
     * Check if the last record is a data record and writes this as well.
     * 
     * @param rangeStart
     * @param rangeEnd
     * @param output
     * @param addressOffset
     * @param record
     * @throws IOException
     */
    private void checkLastRecord(long rangeStart, long rangeEnd, OutputStream output, long addressOffset, SRecord record)
        throws IOException {
        if (record.isDataRecord() && record.adjustToRange(rangeStart, rangeEnd)) {
            logger.fine("Last line was also a data record. Adding it to the binary output");
            long firstAddressInRecord = record.getAddress();
            byte[] data = record.getData();
            long lastAddressInRecord = firstAddressInRecord + data.length - 1;
            try {
                if (!((firstAddressInRecord > rangeEnd) || (lastAddressInRecord < rangeStart))) {
                    // Find the part of the record (possibly whole) that is
                    // inside range
                    long firstAddressToRead = Math.max(firstAddressInRecord, rangeStart);
                    long lastAddressToRead = Math.min(lastAddressInRecord, rangeEnd);
                    int offset = (int) (firstAddressToRead - firstAddressInRecord);
                    int length = (int) (lastAddressToRead - firstAddressToRead + 1);

                    long destAddress = firstAddressToRead + addressOffset;
                    writeBinary(destAddress, data, offset, length, output);
                    mNextPossibleStartAddress = destAddress + length;
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                throw new IOException("Failed load and convert software: " + e.getMessage());
            }
        }
    }

    /**
     * Check if there is a gap and fills it.
     * 
     * @param output
     * @param lookaheadfirstAddressInRecord
     * @param lastAddressInRecord
     * @throws IOException
     */
    private void checkAndFillGap(OutputStream output, long lookaheadfirstAddressInRecord, long lastAddressInRecord)
        throws IOException {
        if (lookaheadfirstAddressInRecord != -1 && (lookaheadfirstAddressInRecord != lastAddressInRecord + 1)) {
            long size = lookaheadfirstAddressInRecord - lastAddressInRecord - 1;

            if (size > 0) {
                logger.info("Fill binary gap of size " + size + " at address " + lastAddressInRecord + 1);

                fillBinaryGap(output, size, (byte) 0xff);
            }

        }
    }

    /**
     * Calculate the start address.
     * 
     * @param fileName
     * @param rangeStart
     * @param rangeEnd
     * @param preserveOrg
     * @param org
     * @param align
     * @param addressOffset
     * @return
     * @throws IOException
     * @throws AssemblerException
     */
    private long calculateStartAddress(String fileName, long rangeStart, long rangeEnd, boolean preserveOrg, long org,
        long align, long addressOffset) throws IOException, AssemblerException {
        if ((!preserveOrg) || (align > 0)) {
            // We must look up the first address in the input file to be able to
            // calculate offset to
            // use
            long firstUsedAddress = getFirstAddressInS3File(fileName, rangeStart, rangeEnd);

            if (org != ATTRIBUTE_NOT_SET) {
                // Use address offset to move first addres to "org" address
                addressOffset = org - firstUsedAddress;
                logger.fine("First address " + firstUsedAddress + ", moved to address " + org);
                mNextPossibleStartAddress = org;

            } else if (!preserveOrg) {
                // Use address offset to move first addres to "rangeStart"
                // address
                addressOffset = mNextPossibleStartAddress - firstUsedAddress;
                logger.fine("First address " + firstUsedAddress + ", moved to address " + mNextPossibleStartAddress);
            }

            if (align > 0) {
                // Change to (possibly) new start address, but not yet aligned
                firstUsedAddress += addressOffset;

                if ((firstUsedAddress % align) != 0) {
                    long alingmentOffset = align - (firstUsedAddress % align);
                    logger.fine("First address " + firstUsedAddress + ", aligned to "
                        + (firstUsedAddress + alingmentOffset));

                    // Compensate for alignment too in address offset
                    addressOffset += alingmentOffset;
                } else {
                    logger.fine("First address " + firstUsedAddress + " already aligned!");
                }
            }
        }
        return addressOffset;
    }

    /**
     * Writes a gap in the data to output steam, using the specified fill char. If the gap spans
     * over several memory blocks, the gap is written as separate areas.
     * 
     * @param output
     *            The OutputStream to write fill bytes to.
     * @param gapSize
     *            The size of the gap.
     * @param fillChar
     *            Character to fill the gap with.
     * @throws IOException
     *             If the writing to output stream fails.
     */
    private void fillBinaryGap(OutputStream output, long gapSize, byte fillChar) throws IOException {
        for (int i = 0; i < gapSize; i++) {
            output.write(fillChar);
        }
    }

    /**
     * Converts and a01 to a bin file and returns the complete path to the created binary file
     * 
     * @param entry
     *            the entry containing the preflash information
     * @param filename
     *            the filename for the a01 file (the preflash file)
     * @return the binary file created from the a01
     * 
     * @throws AssemblerException
     *             if the operation couldn't be done for some reason
     */
    public String convertA01ToBin(U8500ConfigurationEntry entry, String filename) throws AssemblerException {
        logger.info("Converting " + filename + " to binary. Type is " + entry.getType());
        OutputStream os = null;
        try {

            // Convert the Motorola S-record to a binary
            File preflashbin;
            File placeHolder = File.createTempFile("placeholder", null);
            placeHolder.deleteOnExit();
            preflashbin =
                File.createTempFile(PREFLASH_TEMP_NAME, String.valueOf(System.currentTimeMillis()), placeHolder
                    .getParentFile());
            preflashbin.deleteOnExit();

            SRecordConverter converter = new SRecordConverter();

            // Get the start address of the pre-flash image
            String startAddress =
                HexUtilities.toHexString(converter.getFirstAddressInS3File(filename, 0x00000000, 0xffffffffL), true);
            logger.info("Start address of the pre-flash image :" + startAddress);

            os = new FileOutputStream(preflashbin);
            converter.writeS3AsBinary(filename, 0x00000000, 0xffffffffL, false, 0, 0, os);
            logger.fine("Temporary pre-flash binary created : " + preflashbin.getPath());

            // Determine the size of the generated binary file
            logger.info("Size of the pre-flash image :" + preflashbin.length());

            return preflashbin.getPath();

        } catch (Exception e) {
            throw new AssemblerException(e.getMessage());
        } finally {
            StreamCloser.close(os);
        }

    }

}
