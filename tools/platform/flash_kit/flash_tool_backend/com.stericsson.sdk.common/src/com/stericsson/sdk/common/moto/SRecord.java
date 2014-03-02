package com.stericsson.sdk.common.moto;

import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.util.Locale;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.HexUtilities;

/**
 * @author emicroh
 * 
 */
public class SRecord  {

    private static Logger logger = Logger.getLogger(SRecord.class.toString());

    private static final boolean IGNORE_INVALID_S9 = true;

    private static final String START_CHARACTER = "S";

    private static final int START_RECORD_TYPE = 0;

    private static final int END_RECORD_TYPE = 9;

    private static boolean useUpperCase = false;

    private int type;

    private long address;

    private byte[] data;

    /**
     * Set flag used for s-record representation
     * 
     * @param upperCase
     *            - true if s-record should be represented in upper-case, false otherwise
     */
    public static void setUpperCase(boolean upperCase) {
        SRecord.useUpperCase = upperCase;
    }

    /**
     * Serve to retrieve start record for Platform Assistant.
     * 
     * @return - S-record appropriate to start record of Platform Assistant
     */
    public static SRecord getStartRecord() {
        byte[] bytes;
        try {
            bytes = "Android ste-sign command".getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            bytes = new byte[0];
        }
        return new SRecord(START_RECORD_TYPE, 0L, bytes);
    }

    /**
     * Serve to retrieve end record for Platform Assistant.
     * 
     * @return - S-record appropriate to end record of Platform Assistant
     */
    public static SRecord getEndRecord() {
        return new SRecord(END_RECORD_TYPE, 0L, null);
    }

    /**
     * Creates new empty record with default vaules.
     */
    public SRecord() {
        reset();
    }

    /**
     * Creates s-record instance from input string.
     * 
     * @param line
     *            Input String which represents s-record.
     * @throws ParseException
     *             When parsing of input fails.
     */
    public SRecord(String line) throws ParseException {
        setLine(line);
    }

    /**
     * Creates S-Record instance of default type with given address and binary data.
     * 
     * @param addr
     *            Address for this S-Record.
     * @param sData
     *            Binary data for this record.
     */
    public SRecord(long addr, byte[] sData) {
        this();
        setAddress(addr);
        setData(sData);
    }

    /**
     * Creates s-record instance from given parameters.
     * 
     * @param t
     *            S-record type which has to be from range <0,9>.
     * @param addr
     *            S-record address.
     * @param sData
     *            S-record data.
     */
    public SRecord(int t, long addr, byte[] sData) {
        if ((t < 0) || (t > 9)) {
            throw new IllegalArgumentException("Invalid s-record type, it has to be from range (0,9)!");
        } else {
            this.type = t;
        }

        if (addr < 0) {
            throw new IllegalArgumentException("Invalid s-record address, it has to be grater then 0!");
        } else {
            this.address = addr;
        }

        if (isDataRecord() && ((sData == null) || !(sData.length > 0))) {
            throw new IllegalArgumentException("Invalid s-record data. Data has to contain at least one byte!");
        } else if (sData == null) {
            this.data = new byte[0];
        } else {
            this.data = sData;
        }
    }

    private boolean validateInputLine(String inputLine) throws ParseException {
        // verify minimum length of scanned line: S + T + LL
        if ((inputLine == null) || !inputLine.startsWith(START_CHARACTER) || (inputLine.length() < 2)) {
            throw new ParseException("Invalid S-record: " + inputLine, 0);
        }

        int t = inputLine.charAt(1) - '0';
        if ((t < 0) || (t > 9)) {
            throw new ParseException("Invalid S-record type '" + t + "' on line: " + inputLine, 0);
        } else {
            this.type = t;
        }

        if ((this.type == 9) && (inputLine.length() < 4) && IGNORE_INVALID_S9) {
            logger.warn("Invalid S9 record '" + inputLine + "' will be ignored!");
            address = 0;
            return false;
        }

        return true;
    }

    /**
     * Recreate this s-record to match parsed input line.
     * 
     * @param inputLine
     *            Input line which represents s-record.
     * @throws ParseException
     *             When parsing input line fails.
     */
    public void setLine(String inputLine) throws ParseException {
        reset();
        inputLine = inputLine.trim().toUpperCase(Locale.getDefault());

        if (!validateInputLine(inputLine)) {
            return;
        }

        int recordLength;
        int checksum;

        try {
            // type is already set, continue parsing with third character
            int pos = 2;
            try {
                // read and convert LL value
                recordLength = AsciiConverter.toInt(inputLine, pos, 2);
                pos += 2;
                // line length validation regarding record length
                if (inputLine.length() != pos + recordLength * 2) {
                    throw new ParseException("Declared record length doesn't correspond to line length!", pos);
                }
                // line length validation, every line should contain some data so has to be greater
                // then
                // actual pos
                // plus char counts reserved for address plus 2 chars for checksum
                if (isDataRecord() && (inputLine.length() <= pos + getAddressCharCount() + 2)) {
                    throw new ParseException("Scanned line doesn't contain valid s-record!", pos);
                }
            } catch (IllegalArgumentException e) {
                throw new ParseException("Error parsing length in s-record: " + inputLine + "\nError: "
                    + e.getMessage(), pos);
            }

            try {
                address = AsciiConverter.toLong(inputLine, pos, getAddressCharCount());
                pos += getAddressCharCount();
            } catch (IllegalArgumentException e) {
                throw new ParseException("Error parsing address in s-record: " + inputLine + "\nError: "
                    + e.getMessage(), pos);
            }

            // checksum also takes 2 chars
            int dataLength = recordLength * 2 - getAddressCharCount() - 2;
            data = new byte[dataLength / 2 + dataLength % 2];
            try {
                AsciiConverter.convert(inputLine, pos, dataLength, data);
                pos += dataLength;
            } catch (IllegalArgumentException ie) {
                throw new ParseException("Error converting data in s-record: " + inputLine + "\nError: "
                    + ie.getMessage(), pos);
            }

            // checksum verification
            checksum = AsciiConverter.toInt(inputLine, pos, 2);
            if (checksum != getChecksum(inputLine.substring(2, pos))) {
                logger.warn("Checksum " + checksum + " in s-record is incorrect.");
            }
        } catch (ParseException pe) {
            if ((this.type == 9) && IGNORE_INVALID_S9) {
                logger.warn("Invalid S9 record '" + inputLine + "' will be ignored!");
                address = 0;
            } else {
                throw pe;
            }
        }
    }

    private int getAddressCharCount() {
        return getAddressCharCount(this.type);
    }

    private int getAddressCharCount(int t) {
        switch (t) {
            case 0:
                return 4;
            case 1:
                return 4;
            case 2:
                return 6;
            case 3:
                return 8;
            case 5:
                return 4;
            case 7:
                return 8;
            case 8:
                return 6;
            case 9:
                return 4;
            default:
                return 4;
        }
    }

    private void reset() {
        type = 3; // default s-record type
        address = 0;
        data = new byte[0];
    }

    private static int getChecksum(String str) {
        int sum = 0;
        int len = str.length() - str.length() % 2; // ensure divisibility by 2
        for (int i = 0; i < len; i += 2) {
            sum += Integer.parseInt(str.substring(i, i + 2), 16);
        }
        // correction for string not divisible by 2
        if (len % 2 > 0) {
            sum += Integer.parseInt(str.substring(str.length() - 1), 16);
        }

        sum ^= 0xFF;
        return sum & 0xFF;
    }

    /**
     * Getter for s-record type.
     * 
     * @return S-record type.
     */
    public int getType() {
        return type;
    }

    /**
     * Getter for data holded by s-record.
     * 
     * @return S-record data.
     */
    public byte[] getData() {
        return data;
    }

    /**
     * S-record data setter.
     * 
     * @param inputData
     *            Binary data to set for this s-record.
     */
    public void setData(byte[] inputData) {
        this.data = inputData;
    }

    /**
     * Getter for s-record address.
     * 
     * @return S-record address.
     */
    public long getAddress() {
        return address;
    }

    /**
     * S-record address setter.
     * 
     * @param addr
     *            Address to set.
     */
    public void setAddress(long addr) {
        this.address = addr;
    }
    /**
     * Serves to find out if it's allowed to this S-Record to carry any data.
     * 
     * @return True for records which are consider to be data records, false otherwise.
     */
    public boolean isDataRecord() {
        // only this record types comes with data
        return (type == 1) || (type == 2) || (type == 3);
    }

    /**
     * Find out if this record is of type start record.
     * 
     * @return True if this record is of type start record, false otherwise.
     */
    public boolean isStartRecord() {
        return type == START_RECORD_TYPE;
    }

    /**
     * Find out if this record is of type end record.
     * 
     * @return True if this record is of type end record, false otherwise.
     */
    public boolean isEndRecord() {
        return type == END_RECORD_TYPE;
    }

    /**
     * Verify if this record or at least it's part is in valid range. If record data are in range
     * only partly, data and address of this record will be adjusted to this range and return
     * true as well as when it's all included in valid range. Be always aware that call of this
     * method will adjust record as a side effect when recognize that it's in range only partly.
     * 
     * @param rangeStart
     *            - start for valid range
     * @param rangeEnd
     *            - end of valid range
     * @return - true if record is at least partly included in valid range, false otherwise
     */
    public boolean adjustToRange(long rangeStart, long rangeEnd) {
        if (rangeEnd < rangeStart) {
            return false; // exclude wrong input
        }

        long lastAddressInRecord = address + data.length - 1;
        if (address >= rangeEnd) {// start address outside valid range
            return false;
        } else if (address >= rangeStart) {// start address is inside valid range
            // whole record is inside range
            if (lastAddressInRecord <= rangeEnd) {
                return true;
            } else {// data exceeds valid range end -> trim data to (address, rangeEnd)
                int count = (int) (rangeEnd - address + 1);
                if (count > 0) {
                    byte[] tempData = new byte[count];
                    System.arraycopy(this.data, 0, tempData, 0, count);
                    this.data = tempData; // adjust data
                    return true; // new adjusted record is in valid range
                } else {// will occur only if rangeStart and rangeEnd are not defined correctly
                    return false;
                }
            }
        } else if (lastAddressInRecord >= rangeStart) {// start address is lower then rangeStart
            // but
            // some data are in valid range
            // trim data to (rangeStart, lastAddressInRecord
            int count = (int) (Math.min(lastAddressInRecord, rangeEnd) - rangeStart + 1);
            if (count > 0) {
                byte[] tempData = new byte[count];
                System.arraycopy(this.data, (int) (rangeStart - address), tempData, 0, count);
                this.data = tempData;
                this.address = rangeStart;
                return true; // new adjusted record is in valid range
            } else {// will occur only if rangeStart and rangeEnd are not defined correctly
                return false;
            }
        } else {
            return false;
        }
    }

    /**
     * @return String representation of this S-Record.
     */
    @Override
    public String toString() {
        return toString(3); // default
    }

    /**
     * String representation of this S-Record with given type regardless of its real type. This
     * method could represent in custom type only data records.
     * 
     * @param custType
     *            Type for custom S-Record representation.
     * @return String representation of this S-Record of given custom type.
     */
    public String toString(int custType) {
        // only data records could be represented in different type
        if (!isDataRecord()) {
            custType = this.type;
        }

        StringBuffer sb = new StringBuffer(START_CHARACTER + custType);

        // record length in bytes
        int recordLength = getAddressCharCount(custType) / 2 + data.length + 1;
        if (recordLength > 0xFF) {
            throw new IllegalArgumentException("S-record length exceeds 0xFF!");
        }

        // write record length
        String s = Integer.toHexString(recordLength);
        if (s.length() < 2) {
            sb.append("0");
        }
        sb.append(s);

        // write address
        long maxAddr = 0;
        for (int i = 0; i < getAddressCharCount(custType); i++) {
            maxAddr |= (long) 0xF << (i * 4);
        }
        if (address > maxAddr) {
            throw new IllegalArgumentException("S-record address " + HexUtilities.toHexString(address)
                + " exceeds highest usable address for this record type " + HexUtilities.toHexString(maxAddr));
        }
        s = Long.toHexString(address);
        for (int i = 0; i < getAddressCharCount(custType) - s.length(); i++) {
            sb.append("0");
        }
        sb.append(s);

        // write data
        sb.append(AsciiConverter.binaryToString(data));

        // write checksum
        s = Integer.toHexString(getChecksum(sb.substring(2)));

        if (s.length() < 2) {
            sb.append("0");
        }
        sb.append(s);

        if (useUpperCase) {
            return sb.toString().toUpperCase(Locale.getDefault());
        } else {
            return sb.toString();
        }
    }

    /**
     * Serves to get user friendly description of this S-Record.
     * 
     * @return User friendly string of this S-Record.
     */
    public String toUserFriendlyString() {
        StringBuffer sb = new StringBuffer("S-Record('type', 'address', 'data') = (");
        sb.append("'" + type + "', '0x");
        String addr = Long.toHexString(address);
        for (int i = 0; i < 8 - addr.length(); i++) {
            sb.append("0");
        }
        sb.append(addr + "', '");
        sb.append(AsciiConverter.binaryToString(data) + "')");
        return sb.toString();
    }
}
