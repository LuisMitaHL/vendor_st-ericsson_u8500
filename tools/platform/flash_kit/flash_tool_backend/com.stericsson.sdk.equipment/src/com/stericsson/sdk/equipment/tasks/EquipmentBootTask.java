package com.stericsson.sdk.equipment.tasks;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.Locale;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.Convert;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.VersionParser;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.image.BinaryImage;
import com.stericsson.sdk.equipment.image.TableOfContents;
import com.stericsson.sdk.equipment.image.TableOfContentsItem;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.port.PortUtilities;

/**
 * The U8500 Equipment Boot Task is used to load a binary flash and customize loader image to target
 * equipment.
 * 
 * @author xtomlju
 */
public class EquipmentBootTask extends AbstractEquipmentTask {

    /** */
    public static final String UART_BAUDRATE = "UARTbaudrate";

    private static final String WITH_DMA_ON_USB_RECORD_VALUE = "with_dma_on_usb";

    private static final String XLOADER_DEBUG_TRACE_ON_UART_RECORD_VALUE = "xloader_debug_trace_on_uart";

    private static final String DEBUG_TRACE_ON_UART_RECORD_VALUE = "debug_trace_on_uart";

    private static final String BOOT_INDICATION_FLAGS_RECORD_NAME = "bootIndicationFlags";

    private static final String BOOT_INDICATION_RECORD_NAME = "bootIndication";

    private static final String BOOT_INDICATION_PRODUCTION_NAME = "Production";

    private static final String BOOT_INDICATION_ALT_NAME = "ALT";

    private static final String BOOT_INDICATION_ADL_NAME = "ADL";

    private static final String BOOT_INDICATION_NORMAL_NAME = "Normal";

    private static final String BOOT_INDICATION_PROGRAMMING_NAME = "Programming";

    /** ASIC ID request */
    private static final byte[] ASIC_ID_REQUEST = new byte[] {
        (byte) 0x15, (byte) 0x88, (byte) 0x54, (byte) 0x53};

    private static final int BOOT_INDICATION_NORMAL = 0xF0030001;

    private static final int BOOT_INDICATION_PROGRAMMING = 0xF0030002;

    private static final int BOOT_INDICATION_ADL = 0xF0030003;

    private static final int BOOT_INDICATION_ALT = 0xF0030004;

    private static final int BOOT_INDICATION_PRODUCTION = 0xF0030005;

    private static final int DEBUG_TRACE_ON_UART = 0x00000100;

    private static final int XLOADER_DEBUG_TRACE_ON_UART = 0x00000100;

    private static final int WITH_DMA_ON_USB = 0x00001000;

    /** Default baud rate */
    public static final int UART_BAUD_RATE_DEFAULT = 115200;

    /** Timeout value for getting next token */
    private static final long TOKEN_TIMEOUT = 10000;

    /** Timeout value for reading of initial byte */
    private static final long INITIAL_BYTE_READ_TIMEOUT = 60000;

    private static final long READ_SLEEP = 40;

    /** Equipment instance */
    private final AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private static Logger logger = Logger.getLogger(EquipmentBootTask.class);

    private boolean changeBaudRateACK = false;

    private StringBuilder info;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentBootTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store U8500 equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "No error";
        info = new StringBuilder();
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {

        AbstractPort port = (AbstractPort) mobileEquipment.getPort();

        byte[] tokenBuffer = new byte[4];

        try {
            notifyTaskStart();

            // Read boot image

            if (PortUtilities.isUSBPort(port)) {
                // Write some synchronization bytes (only used for USB)
                port.write(ASIC_ID_REQUEST);
            }

            // Read and parse ASIC ID into equipment properties
            parseASICID(port);

            // Change UART baud rate in ROM.
            changeUARTBaudRateInROM(port);

            // Write boot indication message to continue boot on the same interface
            createBootIndication(tokenBuffer);

            notifyTaskMessage(HexUtilities.toHexString(tokenBuffer, 0, 4, 4, true) + " sent as boot indication");

            port.write(tokenBuffer);

            if (programmingBoot() || adlBoot()) {

                if (programmingBoot()) { // just for programming boot !!! ADL should take loader
                    // from ME !!!
                    BinaryImage bootImage =
                        BinaryImage.createFromFile(mobileEquipment.getProfile().getSofwarePath("Loader"));
                    notifyTaskMessage(new File(mobileEquipment.getProfile().getSofwarePath("Loader")).getAbsolutePath());

                    sendTOCAndISSW(port, bootImage);

                    // The ISSW and X-LOADER will read the TOC and ask for data by sending tokens.
                    // Loop
                    // until the Flash and customize loader binary has been loaded

                    readAndHandleTokens(port, tokenBuffer, bootImage);
                    parseAndNotifyBootImageVersions(bootImage);
                }
            }
        } catch (IOException e) {
            resultCode = 1;
            resultMessage = e.getMessage();
        } catch (EquipmentBootException e) {
            resultCode = 1;
            resultMessage = e.getMessage();
        }
        mobileEquipment.setInfo(info.toString());
        return new EquipmentTaskResult(resultCode, resultMessage, null, (resultCode != 0));
    }

    private void sendTOCAndISSW(AbstractPort port, BinaryImage bootImage) throws IOException {
        // Write CRKC if present in TOC
        boolean writeCRKC = bootImage.getTOC().getItem(TableOfContentsItem.FILENAME_CRKC) != null;

        // Write size of TOC and ISSW binary and then the TOC and ISSW binary payloads

        if (writeCRKC) {
            notifyTaskMessage("Sending TOC, ISSW and CRKC");
        } else {
            notifyTaskMessage("Sending TOC and ISSW");
        }

        writeInteger(port, TableOfContents.SIZE
            + bootImage.getTOC().getItem(TableOfContentsItem.FILENAME_ISSW).getSize()
            + ((writeCRKC) ? bootImage.getTOC().getItem(TableOfContentsItem.FILENAME_CRKC).getSize() : 0));

        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        bos.write(bootImage.getTOC().getData(), 0, TableOfContents.SIZE);
        bos.write(bootImage.getImageData(TableOfContentsItem.FILENAME_ISSW), 0, bootImage.getTOC().getItem(
            TableOfContentsItem.FILENAME_ISSW).getSize());
        if (writeCRKC) {
            bos.write(bootImage.getImageData(TableOfContentsItem.FILENAME_CRKC), 0, bootImage.getTOC().getItem(
                TableOfContentsItem.FILENAME_CRKC).getSize());
        }

        byte[] bootbuffer = bos.toByteArray();
        writeFully(port, bootbuffer);
    }

    private void parseAndNotifyBootImageVersions(BinaryImage bootImage) {

        // Store versions as equipment properties

        String version;

        version =
            VersionParser.getInstance().getISSWVersion(bootImage.getImageData(TableOfContentsItem.FILENAME_ISSW), true)
                .trim();
        if ((version != null) && version.length() > 0) {
            mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_ISSW_VERSION.getPropertyName(),
                EquipmentBootTaskConstants.PROP_ISSW_VERSION.getPropertyName(), version, false);
        }

        version =
            VersionParser.getInstance().getMemInitVersion(
                bootImage.getImageData(TableOfContentsItem.FILENAME_MEM_INIT), true).trim();
        if ((version != null) && version.length() > 0) {
            mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_MEM_INIT_VERSION.getPropertyName(),
                EquipmentBootTaskConstants.PROP_MEM_INIT_VERSION.getPropertyName(), version, false);
        }

        version =
            VersionParser.getInstance().getXLoaderVersion(bootImage.getImageData(TableOfContentsItem.FILENAME_XLOADER),
                true).trim();

        if ((version != null) && version.length() > 0) {
            mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_XLOADER_VERSION.getPropertyName(),
                EquipmentBootTaskConstants.PROP_XLOADER_VERSION.getPropertyName(), version, false);
        }

        version =
            VersionParser.getInstance().getPwrMgtVersion(bootImage.getImageData(TableOfContentsItem.FILENAME_PWR_MGT),
                true).trim();

        if ((version != null) && version.length() > 0) {
            mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_PWR_MGT_VERSION.getPropertyName(),
                EquipmentBootTaskConstants.PROP_PWR_MGT_VERSION.getPropertyName(), version, false);
        }

        notifyTaskMessage("Boot image versions : " + "\n"
            + "\nLISTED BOOT IMAGES ARE USED IN PERIPHERAL BOOT BY LOADER. PLATFORM MIGHT USE OTHER VERSIONS. \n"
            + "\n" + bootImage.parseVersions());
    }

    private boolean adlBoot() {
        return BOOT_INDICATION_ADL_NAME.equalsIgnoreCase(mobileEquipment.getProfile().getProperty(
            BOOT_INDICATION_RECORD_NAME));
    }

    private boolean programmingBoot() {
        return BOOT_INDICATION_PROGRAMMING_NAME.equalsIgnoreCase(mobileEquipment.getProfile().getProperty(
            BOOT_INDICATION_RECORD_NAME));
    }

    private void createBootIndication(byte[] buffer) {
        int indication = BOOT_INDICATION_PROGRAMMING;

        String bootIndication = mobileEquipment.getProfile().getProperty(BOOT_INDICATION_RECORD_NAME);

        if (BOOT_INDICATION_NORMAL_NAME.equalsIgnoreCase(bootIndication)) {
            indication = BOOT_INDICATION_NORMAL;
        } else if (BOOT_INDICATION_ADL_NAME.equalsIgnoreCase(bootIndication)) {
            indication = BOOT_INDICATION_ADL;
        } else if (BOOT_INDICATION_ALT_NAME.equalsIgnoreCase(bootIndication)) {
            indication = BOOT_INDICATION_ALT;
        } else if (BOOT_INDICATION_PRODUCTION_NAME.equalsIgnoreCase(bootIndication)) {
            indication = BOOT_INDICATION_PRODUCTION;
        } else if (BOOT_INDICATION_PROGRAMMING_NAME.equalsIgnoreCase(bootIndication)) {
            indication = BOOT_INDICATION_PROGRAMMING;
        } else if ((bootIndication != null) && (bootIndication.startsWith("0x"))) {
            indication = Long.decode(bootIndication.substring(2)).intValue();
        }

        String bootIndicationFlag = mobileEquipment.getProfile().getProperty(BOOT_INDICATION_FLAGS_RECORD_NAME);
        if (bootIndicationFlag != null) {

            if (bootIndicationFlag.toLowerCase(Locale.getDefault()).contains(DEBUG_TRACE_ON_UART_RECORD_VALUE)) {
                indication = indication | DEBUG_TRACE_ON_UART;
            }

            if (bootIndicationFlag.toLowerCase(Locale.getDefault()).contains(XLOADER_DEBUG_TRACE_ON_UART_RECORD_VALUE)) {
                indication = indication | XLOADER_DEBUG_TRACE_ON_UART;
            }

            if (bootIndicationFlag.toLowerCase(Locale.getDefault()).contains(WITH_DMA_ON_USB_RECORD_VALUE)) {
                indication = indication | WITH_DMA_ON_USB;
            }
        }

        // Convert integer to 4 byte array

        buffer[0] = (byte) (indication & 0xFF);
        buffer[1] = (byte) ((indication & 0xFF00) >> 8);
        buffer[2] = (byte) ((indication & 0xFF0000) >> 16);
        buffer[3] = (byte) ((indication & 0xFF000000) >> 24);
    }

    private void readAndHandleTokens(AbstractPort port, byte[] tokenBuffer, BinaryImage bootImage) throws IOException,
        EquipmentBootException {

        boolean readingTokens = true;
        while (readingTokens) {

            // Get a token

            TokenType token = readToken(port, tokenBuffer);
            token.handleToken(this, port, bootImage);
            if (token.isLastToken()) {
                readingTokens = false;
            }
        }
    }

    /**
     * Writes binary image to specified port.
     * 
     * @param binaryName
     *            binary image name
     * @param port
     *            instance of port
     * @param bootImage
     *            complete boot image (loader)
     * @throws IOException
     *             If an I/O error occurred
     */
    public void writeBinary(String binaryName, AbstractPort port, BinaryImage bootImage) throws IOException {
        writeInteger(port, bootImage.getTOC().getItem(binaryName).getSize());
        writeFully(port, bootImage.getImageData(binaryName));
    }

    /**
     * Changes baud rate in ROM and if the change is acknowledged it changes UART port baud rate in
     * the Flash Tool Backend too.
     * 
     * @param port
     *            Port.
     */
    private void changeUARTBaudRateInROM(AbstractPort port) {
        if (PortUtilities.isUSBPort(port)) {
            return;
        }

        EquipmentProperty uartBaudRateProperty =
            mobileEquipment.getProperty(EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName());
        if (uartBaudRateProperty == null) {
            return;
        }

        boolean changeBaudRateInROM = Boolean.parseBoolean(uartBaudRateProperty.getPropertyValue());
        if (changeBaudRateInROM) {
            String uartBaudRatePropertyValue = mobileEquipment.getProfile().getProperty(UART_BAUDRATE);
            if (uartBaudRatePropertyValue != null) {
                try {
                    int uartBaudRate = Integer.parseInt(uartBaudRatePropertyValue);
                    if (uartBaudRate == UART_BAUD_RATE_DEFAULT) {
                        return;
                    }

                    // Send command to change baud rate
                    sendChangeBaudRate(port, uartBaudRate);
                    // Read baud rate change acknowledgment
                    if (!readChangeBaudRateACK(port)) {
                        notifyTaskMessage("Baud rate change in ROM wasn't acknowledged, "
                            + "but due to possible cable problem UART port baud rate will be changed.");
                        // return;
                    }
                    // Short sleep
                    try {
                        Thread.sleep(1);
                    } catch (InterruptedException e) {
                        logger.info(e);
                    }

                    // Change UART port baud rate
                    notifyTaskMessage("UART port baud rate is being set to " + uartBaudRatePropertyValue);
                    port.setSpeed(uartBaudRatePropertyValue);
                    notifyTaskMessage("UART port baud rate has been changed");
                    // Short sleep
                    try {
                        Thread.sleep(1);
                    } catch (InterruptedException e) {
                        logger.info(e);
                    }
                } catch (Exception e) {
                    logger.error("UART port baud rate cannot be changed", e);
                }
            }
        }
    }

    /**
     * Sends command to change baud rate.
     * 
     * @param port
     *            Port.
     * @param uartBaudRate
     *            Baud rate.
     * @throws IOException
     *             I/O Exception.
     */
    private void sendChangeBaudRate(AbstractPort port, int uartBaudRate) throws IOException {
        byte[] buffer = new byte[4];
        buffer[0] = (byte) (uartBaudRate & 0xFF);
        buffer[1] = (byte) ((uartBaudRate) >> 8);
        buffer[2] = (byte) ((uartBaudRate) >> 16);
        buffer[3] = (byte) (0xBD);
        notifyTaskMessage("Sending new baud rate: " + uartBaudRate);
        writeFully(port, buffer);
        notifyTaskMessage(HexUtilities.toHexString(buffer, 0, 4, 4, true) + " sent as the new baud rate");
    }

    /**
     * Reads acknowledgment of baud rate change.
     * 
     * @param port
     *            Port.
     * @return True if the baud rate change has been acknowledged.
     * @throws IOException
     *             I/O Exception.
     */
    private boolean readChangeBaudRateACK(AbstractPort port) throws IOException {
        byte[] buffer = new byte[4];
        readToken(port, buffer);

        // CHANGE_BAUD_RATE_ACK agreed acknowledge message for thorium v2.0
        // CHANGE_BAUD_RATE_ACK_INCORRECT acknowledge message really received from thorium v2.0
        // CHANGE_BAUD_RATE_ACK_NEW acknowledge message for thorium v2.1
        notifyTaskMessage(HexUtilities.toHexString(buffer, 0, 4, 4, true)
            + " received as baud rate change acknowledgment, expected "
            + HexUtilities.toHexString(TokenType.CHANGE_BAUD_RATE_ACK_2_0.getTokenValue(), 0, 4, 4, true) + " or "
            + HexUtilities.toHexString(TokenType.CHANGE_BAUD_RATE_ACK_2_0_INCORRECT.getTokenValue(), 0, 4, 4, true)
            + " or " + HexUtilities.toHexString(TokenType.CHANGE_BAUD_RATE_ACK_2_1.getTokenValue(), 0, 4, 4, true));
        changeBaudRateACK =
            Arrays.equals(buffer, TokenType.CHANGE_BAUD_RATE_ACK_2_0.getTokenValue())
                || Arrays.equals(buffer, TokenType.CHANGE_BAUD_RATE_ACK_2_1.getTokenValue())
                || Arrays.equals(buffer, TokenType.CHANGE_BAUD_RATE_ACK_2_0_INCORRECT.getTokenValue());
        mobileEquipment.setChangeBaudRateROMAck(changeBaudRateACK);
        return changeBaudRateACK;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return getId() + "@" + mobileEquipment;
    }

    /**
     * Read a four byte token.
     * 
     * @param port
     *            Port interface
     * @param tokenBuffer
     *            Buffer to read into
     * @return
     * @throws IOException
     *             If an I/O error occurred
     */
    private TokenType readToken(AbstractPort port, byte[] tokenBuffer) throws IOException {
        long time = System.currentTimeMillis();
        int bytesRead = port.read(tokenBuffer, 0, 4);
        while (bytesRead != 4) {
            if ((System.currentTimeMillis() - time) > TOKEN_TIMEOUT) {
                throw new IOException("No response from ME");
            }
            bytesRead += port.read(tokenBuffer, bytesRead, 4 - bytesRead);
        }

        return TokenType.getTokenByValue(tokenBuffer);
    }

    /**
     * Write a (four byte) integer to specified port.
     * 
     * @param port
     *            Port interface
     * @param value
     *            Integer value
     * @throws IOException
     *             If an I/O error occurred
     */
    private void writeInteger(AbstractPort port, int value) throws IOException {
        final ByteBuffer intBuffer = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN);
        intBuffer.putInt(0, value);
        port.write(intBuffer.array());
    }

    /**
     * Reads specified number of bytes from the port. Uses multiple read operations if necessary.
     * 
     * @param port
     *            Port to read data from.
     * @param buffer
     *            Byte buffer to store read data.
     * @param offset
     *            Offset into buffer.
     * @param length
     *            Number of bytes to read.
     * @throws PortException
     *             Port exception.
     */
    void readFully(AbstractPort port, byte[] buffer, int offset, int length) throws PortException {
        int bytesLeft = length;
        while (bytesLeft > 0) {
            int bytesRead = port.read(buffer, offset, bytesLeft);
            if (bytesRead < 0) {
                throw new PortException("Could not read required number of bytes (" + length + ").");
            }
            offset = offset + bytesRead;
            bytesLeft = bytesLeft - bytesRead;
        }
    }

    /**
     * Writes specified number of bytes to the port. Uses multiple write operations if necessary.
     * 
     * @param port
     *            Port to write data to.
     * @param buffer
     *            Byte buffer to write data from.
     * @param offset
     *            Offset into buffer.
     * @param length
     *            Number of bytes to write.
     * @throws PortException
     *             Port exception.
     */
    private void writeFully(AbstractPort port, byte[] buffer, int offset, int length) throws PortException {
        int bytesLeft = length;
        while (bytesLeft > 0) {
            int bytesWritten = port.write(buffer, offset, bytesLeft);
            if (bytesWritten < 0) {
                throw new PortException("Could not write bytes");
            }
            offset = offset + bytesWritten;
            bytesLeft = bytesLeft - bytesWritten;
        }
    }

    /**
     * Writes all bytes to the port. Uses multiple write operations if necessary.
     * 
     * @param port
     *            Port to write data to.
     * @param buffer
     *            Byte buffer to write data from.
     * @throws PortException
     *             Port exception.
     */
    private void writeFully(AbstractPort port, byte[] buffer) throws PortException {
        writeFully(port, buffer, 0, buffer.length);
    }

    private static final int LIMIT_OF_ASIC_ID_SUB_BLOCKS = 10;

    private static final int ASIC_ID_SUB_BLOCK_ID = 0x01;

    private static final int ASIC_ID_SUB_BLOCK_PUBLIC_ID = 0x12;

    private static final int ASIC_ID_SUB_BLOCK_SECURE_MODE = 0x13;

    private static final int ASIC_ID_SUB_BLOCK_ROOT_KEY_HASH = 0x14;

    private static final int ASIC_ID_SUB_BLOCK_CHECKSUM = 0x15;

    /**
     * First 4 bytes of the M7400 ASIC ID. Used for synchronization.
     */
    private static final byte[] ASIC_ID_START = new byte[] {
        0x05, 0x01, 0x05, 0x01};

    /**
     * First 4 bytes of the M7400 V2 ASIC ID. Used for synchronization.
     */
    private static final byte[] ASIC_ID_START_V2 = new byte[] {
        0x05, 0x01, 0x0B, 0x01};

    private static final String ASIC_ID_MISSING_FIRST_BYTE_MESSAGE =
        "\n\nReceived ASIC ID data with missing first byte."
            + " The data have been synchronized so the boot procedure can continue."
            + " If the boot procedure is not successful, reset the ME and repeat the procedure.\n";

    /**
     * Synchronizes ASIC ID data. Removes initial garbage data or adds missing first byte.
     * 
     * @param pPort
     *            Port to read from.
     * @param pBuffer
     *            Data buffer to read into.
     * @return Number of read bytes.
     * @throws PortException
     *             Port exception.
     */
    private int synchronizeASICID(AbstractPort pPort, byte[] pBuffer) throws PortException {
        int index = 0;
        boolean matches = false;
        boolean missing = false;

        while (!matches) {
            int read = pPort.read();
            long time = System.currentTimeMillis();
            while (read < 0) {
                if ((System.currentTimeMillis() - time) > INITIAL_BYTE_READ_TIMEOUT) {
                    throw new PortException("No response from ME.");
                }
                read = pPort.read();
                try {
                    Thread.sleep(READ_SLEEP);
                } catch (InterruptedException e) {
                    logger.debug("Interrupted while sleeping.");
                }
            }

            if (read == ASIC_ID_START[index] || read == ASIC_ID_START_V2[index]) {
                pBuffer[index] = (byte) read;
                index++;
            } else {
                if ((index == 0) && (read == ASIC_ID_START[1])) {
                    // When UART connection is not reliable, first byte(s) of ASIC ID may be lost.
                    // This code tries to synchronize ASIC ID data with missing first byte.
                    pBuffer[0] = ASIC_ID_START[0];
                    pBuffer[1] = ASIC_ID_START[1];
                    index = 2;
                    missing = true;
                } else {
                    if (index == 2) {
                        logger.info(String.format(Locale.getDefault(), "Expected: 0x%02X or 0x%02X  Received: 0x%02X",
                            ASIC_ID_START[index], ASIC_ID_START_V2[index], read));
                    } else {
                        logger.info(String.format(Locale.getDefault(), "Expected: 0x%02X  Received: 0x%02X",
                            ASIC_ID_START[index], read));
                    }
                    index = 0;
                    missing = false;
                }
            }

            if (index >= ASIC_ID_START.length) {
                matches = true;
            }
        }

        if (missing) {
            logger.info(ASIC_ID_MISSING_FIRST_BYTE_MESSAGE);
        }

        return index;
    }

    private void parseASICID(AbstractPort pPort) throws PortException, EquipmentBootException {
        // Initial data size (4 bytes) + maximal size of sub-block (255 bytes).
        byte[] buffer = new byte[260];
        int count;
        int size;

        // Read number of sub-blocks (1 byte), header of the first sub-block (2 bytes) and one byte
        // of data of the first sub-block. These 4 bytes are used for synchronization if needed.
        readInitialData(pPort, buffer, 0, 4);

        count = Convert.uint8ToInt(buffer[0]);

        if (count > LIMIT_OF_ASIC_ID_SUB_BLOCKS) {
            throw new EquipmentBootException(String.format(Locale.getDefault(),
                "Unparsable ASIC ID. The number of sub-blocks is too high. The limit is %d sub-blocks.",
                LIMIT_OF_ASIC_ID_SUB_BLOCKS));
        }

        size = Convert.uint8ToInt(buffer[2]);
        // Read sub-block data of given size minus one byte that was read before.
        readFully(pPort, buffer, 4, size - 1);
        parseSubBlockASICID(buffer, 1);

        for (int i = 1; i < count; i++) {
            readFully(pPort, buffer, 0, 2); // Read header of sub-block (2 bytes).
            size = Convert.uint8ToInt(buffer[1]);
            readFully(pPort, buffer, 2, size); // Read sub-block data.
            parseSubBlockASICID(buffer, 0);
        }
    }

    /**
     * Reads initial ASIC ID data and synchronizes it if needed.
     * 
     * @param pPort
     *            Port to read from.
     * @param pBuffer
     *            Data buffer to read into.
     * @param pOffset
     *            Data buffer offset.
     * @param pLength
     *            Number of bytes to read.
     * @return Number of read bytes.
     * @throws PortException
     *             Port exception.
     */
    private int readInitialData(AbstractPort pPort, byte[] pBuffer, int pOffset, int pLength) throws PortException {
        int bytesRead = 0;

        if (mobileEquipment.getEquipmentType() == EquipmentType.M7X00) {
            bytesRead = synchronizeASICID(pPort, pBuffer);
        } else {
            int read = pPort.read();
            long time = System.currentTimeMillis();
            while (read < 0) {
                if ((System.currentTimeMillis() - time) > INITIAL_BYTE_READ_TIMEOUT) {
                    throw new PortException("No response from ME.");
                }
                read = pPort.read();
                try {
                    Thread.sleep(READ_SLEEP);
                } catch (InterruptedException e) {
                    logger.debug("Interrupted while sleeping.");
                }
            }
            pBuffer[0] = (byte) read;
            readFully(pPort, pBuffer, pOffset + 1, pLength - 1);
            bytesRead = pLength;
        }
        return bytesRead;
    }

    /**
     * Parse sub-block of the ASIC ID information and put them as equipment properties.
     * 
     * @param pBuffer
     *            Buffer to parse
     * @param pOffset
     *            offset address at buffer which contains a related data
     * @throws EquipmentBootException
     */
    private void parseSubBlockASICID(byte[] pBuffer, int pOffset) throws EquipmentBootException {
        switch (pBuffer[pOffset]) {
            case ASIC_ID_SUB_BLOCK_ID:
                parseSubBlockID(pBuffer, pOffset);
                break;
            case ASIC_ID_SUB_BLOCK_PUBLIC_ID:
                parseSubBlockPublicID(pBuffer, pOffset);
                break;
            case ASIC_ID_SUB_BLOCK_SECURE_MODE:
                parseSubBlockSecureMode(pBuffer, pOffset);
                break;
            case ASIC_ID_SUB_BLOCK_ROOT_KEY_HASH:
                parseSubBlockRootKeyHash(pBuffer, pOffset);
                break;
            case ASIC_ID_SUB_BLOCK_CHECKSUM:
                parseSubBlockChecksum(pBuffer, pOffset);
                break;
            default:
                logger.info("Unknown ASIC ID subblock found. Subblock type is "
                    + HexUtilities.toHexString(pBuffer[pOffset]));
                break;
        }
    }

    private void parseSubBlockChecksum(byte[] pBuffer, int pOffset) throws EquipmentBootException {
        String publicROMCRC = "0x" + HexUtilities.toHexString(pBuffer, pOffset + 3, 4, 4, false);
        String secureROMCRC = "0x" + HexUtilities.toHexString(pBuffer, pOffset + 7, 4, 4, false);

        mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_PUBLIC_ROM_CRC.getPropertyName(),
            EquipmentBootTaskConstants.PROP_PUBLIC_ROM_CRC.getPropertyName(), publicROMCRC, false);
        mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_SECURE_ROM_CRC.getPropertyName(),
            EquipmentBootTaskConstants.PROP_SECURE_ROM_CRC.getPropertyName(), secureROMCRC, false);

        notifyTaskMessage("Public ROM CRC:     " + publicROMCRC);
        notifyTaskMessage("Secure ROM CRC:     " + secureROMCRC);
    }

    private void parseSubBlockRootKeyHash(byte[] pBuffer, int pOffset) {
        // Size is identify variable (default: 1) + public id (default: 20).
        int rootKeyHashLength = Convert.uint8ToInt(pBuffer[1 + pOffset]) - 1;

        String rootKeyHash =
            "0x" + HexUtilities.toHexString(pBuffer, pOffset + 3, rootKeyHashLength, rootKeyHashLength, false);

        mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_ROOT_KEY_HASH.getPropertyName(),
            EquipmentBootTaskConstants.PROP_ROOT_KEY_HASH.getPropertyName(), rootKeyHash, false);

        notifyTaskMessage("Root Key Hash:      " + rootKeyHash);
    }

    private void parseSubBlockSecureMode(byte[] pBuffer, int pOffset) throws EquipmentBootException {
        String secureMode = null;

        if (pBuffer[pOffset + 3] == 1) {
            secureMode = "OPENED";
        } else {
            secureMode = "CLOSED";
        }

        mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_SECURE_MODE.getPropertyName(),
            EquipmentBootTaskConstants.PROP_SECURE_MODE.getPropertyName(), secureMode, false);

        notifyTaskMessage("Secure Mode:        " + secureMode);
        info.append("Secure Mode:        " + secureMode + "\n");
    }

    private void parseSubBlockPublicID(byte[] pBuffer, int pOffset) throws EquipmentBootException {
        // Size is identify variable (default: 1) + public id (default: 20).
        int publicKeyHashLength = Convert.uint8ToInt(pBuffer[1 + pOffset]) - 1;

        String publicID =
            "0x" + HexUtilities.toHexString(pBuffer, pOffset + 3, publicKeyHashLength, publicKeyHashLength, false);

        mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_PUBLIC_ID.getPropertyName(),
            EquipmentBootTaskConstants.PROP_PUBLIC_ID.getPropertyName(), publicID, false);

        notifyTaskMessage("Public ID:          " + publicID);
        info.append("Public ID:          " + publicID + "\n");
    }

    private void parseSubBlockID(byte[] pBuffer, int pOffset) throws EquipmentBootException {
        String nomadikID = "0x" + HexUtilities.toHexString(pBuffer, pOffset + 3, 3, 3, false);
        String asicVersion = "0x" + HexUtilities.toHexString(pBuffer, pOffset + 6, 1, 1, false);

        byte[] data = new byte[3];
        System.arraycopy(pBuffer, pOffset + 3, data, 0, 3);
        ChipID chipID = ChipID.getByData(data);
        if (chipID == null) {
            throw new EquipmentBootException("Unsupported chip ID: " + nomadikID);
        }
        int chipVersion = Convert.uint8ToInt(pBuffer[pOffset + 6]);

        mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_NOMADIK_ID.getPropertyName(),
            EquipmentBootTaskConstants.PROP_NOMADIK_ID.getPropertyName(), nomadikID, false);
        mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_ASIC_VERSION.getPropertyName(),
            EquipmentBootTaskConstants.PROP_ASIC_VERSION.getPropertyName(), asicVersion, false);

        notifyTaskMessage("Nomadik ID:         " + nomadikID);
        info.append("Nomadik ID:         " + nomadikID + "\n");
        notifyTaskMessage("ASIC Version:       " + asicVersion);
        info.append("ASIC Version:       " + asicVersion + "\n");

        // ASIC ID with chip ID 0x007400 and chip version 0xB0 contains additional 6 bytes with chip
        // option (2 bytes) and chip customer ID (4 bytes).
        if (chipID == ChipID.M7400 && chipVersion >= Convert.uint8ToInt((byte) 0xB0)) {
            String chipOption = "0x" + HexUtilities.toHexString(pBuffer, pOffset + 7, 2, 2, false);
            String chipCustomerID = "0x" + HexUtilities.toHexString(pBuffer, pOffset + 9, 4, 4, false);

            mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_CHIP_OPTION.getPropertyName(),
                EquipmentBootTaskConstants.PROP_CHIP_OPTION.getPropertyName(), chipOption, false);
            mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_CHIP_CUSTOMER_ID.getPropertyName(),
                EquipmentBootTaskConstants.PROP_CHIP_CUSTOMER_ID.getPropertyName(), chipCustomerID, false);

            notifyTaskMessage("Chip Option:        " + chipOption);
            info.append("Chip Option:        " + chipOption + "\n");
            notifyTaskMessage("Chip Customer ID:   " + chipCustomerID);
            info.append("Chip Customer ID:   " + chipCustomerID + "\n");

            mobileEquipment.setProperty(EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(),
                EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(), "TRUE", false);
        }
        if (chipID == ChipID.M7400) {
            info.append("Batch number:       " + getBatchNumber(asicVersion) + "\n");
        }
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.BOOT;
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] arguments) {
    }

    private String getBatchNumber(String version) {
        if (version.equalsIgnoreCase("0xA0")) {
            return "Thorium V1";
        } else if (version.equalsIgnoreCase("0xB0")) {
            return "Thorium V2";
        } else if (version.equalsIgnoreCase("0xB1")) {
            return "Thorium V2.1";
        }
        return "Unknown";
    }
}
