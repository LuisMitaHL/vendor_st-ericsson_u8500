package com.stericsson.sdk.signing.cops;

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.local.encryption.EncryptionAlgorithm;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * Loader payload implementation of the IPayload interface.
 * 
 * @author xtomlju
 * @author ezaptom
 */
public class LoaderPayload implements ICOPSPayload {

    /** Size in number of bytes for one session key (ChipID + actual key) */
    private static final int SESSION_KEY_SIZE = 20;

    private static final long OFFSET_START_ADDRESS = 26L;

    private static final long OFFSET_PAYLOAD = 34L;

    private static final int ADDRESS_SIZE = 4;

    /** Size of sliding window when compressing */
    private static final int SLIDING_WINDOW_SIZE = 4096;

    /** Size of look ahead buffer when compressing */
    private static final int LOOK_AHEAD_SIZE = 18;

    /** Number of iterations for encryption */
    private static final int ENCRYPTION_ITERATION_COUNT = 32;

    /** Encryption can only be done on ENRYPTION_ALIGNMENT byte blocks */
    private static final int ENCRYPTION_ALIGNMENT = 8;

    /** A buffer used for both untransformed and transformed loader data */
    private byte[] fileBuffer;

    /** Offset to start compression or encryption from */
    long compressEncryptStart;

    /** Specifies if this is a Access CPU pre-loader */
    boolean isAccPreloader;

    /** Size of untransformed loader */
    int loaderSize;

    private long startAddress;

    private ByteBuffer payloadBuffer;

    private Logger logger;

    /** Buffer for the compression algorithm */
    private byte[] slidingWindow = new byte[SLIDING_WINDOW_SIZE];

    /** Buffer for the compression algorithm */
    private byte[] lookAhead = new byte[LOOK_AHEAD_SIZE];

    /** A bit buffer for output of bits */
    private int bitBuffer;

    /** A counter for the bitBuffer field */
    private int bitCount;

    /** Number of bytes matched during compression */
    private int matchedDataLength;

    /** Offset to matched data during compression */
    private int matchedDataOffset;

    /** Number of session keys */
    private int sessionKeyCount;

    private boolean oldAccPreLoader;

    private long payloadLength;

    private byte numberOfKeyBlocks = 0;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            signer settings
     * @param controlFields
     *            control fields
     * @throws IOException
     *             If an I/O error occurred
     * @throws SignerException
     *             s
     */
    public LoaderPayload(A2SignerSettings signerSettings, COPSControlFields controlFields) throws IOException,
        SignerException {

        File file = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        boolean loaderCompression =
            (Boolean) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_LOADER_COMPRESSION);
        boolean loaderEncryption = (Boolean) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_LOADER_ENCRYPTION);

        logger = Logger.getLogger(getClass().getName());
        File transformedFile = null;

        if (loaderCompression || loaderEncryption) {
            if (!(Boolean) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ENCRYPT_REQUIRED)) {
                throw new SignerException("Cannot compress or encrypt with package.");
            }

            transformedFile = File.createTempFile(file.getName(), ".temp");
            transformedFile.deleteOnExit();
            tranformLoader(file, transformedFile, loaderCompression, loaderEncryption, controlFields, signerSettings);
            createPayload(transformedFile, loaderCompression, loaderEncryption);
        } else {
            createPayload(file, loaderCompression, loaderEncryption);
        }
    }

    private void createPayload(File file, boolean loaderCompression, boolean loaderEncryption) throws IOException {
        FileChannel channel = null;
        FileInputStream fis = null;

        try {
            fis = new FileInputStream(file);
            channel = fis.getChannel();
            channel.position(OFFSET_START_ADDRESS);
            ByteBuffer addressBuffer = ByteBuffer.allocate(ADDRESS_SIZE);
            channel.read(addressBuffer);
            startAddress = addressBuffer.getInt(0);
            channel.position(OFFSET_PAYLOAD);
            payloadBuffer = ByteBuffer.allocate((int) channel.size() - (int) OFFSET_PAYLOAD);
            if (!loaderCompression && !loaderEncryption) {
                payloadLength = payloadBuffer.limit();
            }
            channel.read(payloadBuffer);
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }
    }

    /**
     * This method transfer loader file, compress it, encrypt it or both.
     * 
     * @param loaderInFile
     *            input file
     * @param loaderOutFile
     *            output transfered file
     * @param loaderCompression
     *            boolean flag if loader should be compressed
     * @param loaderEncryption
     *            boolean flag if loader should be encrypted
     * @param controlFields
     *            control fields
     * @param signerSettings
     *            signer settings
     * @throws IOException
     */
    private void tranformLoader(File loaderInFile, File loaderOutFile, boolean loaderCompression,
        boolean loaderEncryption, COPSControlFields controlFields, A2SignerSettings signerSettings) throws IOException {

        ByteArrayInputStream input = null;
        ByteArrayOutputStream output = null;
        ByteArrayOutputStream loaderOutput = null;
        int data = 0;
        int outputSize = 0;
        int paddingByteCount = 0;

        fileBuffer = createBufferFromFile(loaderInFile);
        loaderOutput = new ByteArrayOutputStream();
        compressEncryptStart = locatePayloadOffset(fileBuffer);
        isAccPreloader = (compressEncryptStart != OFFSET_PAYLOAD);
        loaderSize = (int) (fileBuffer.length - compressEncryptStart);
        outputSize = loaderSize;

        if (loaderCompression) {
            // Create buffer to compress;
            byte[] bufferToCompress = new byte[outputSize];
            System.arraycopy(fileBuffer, (int) compressEncryptStart, bufferToCompress, 0, bufferToCompress.length);
            // Compress buffer
            input = new ByteArrayInputStream(bufferToCompress);
            output = new ByteArrayOutputStream();
            compress(input, output);
            outputSize = output.size(); // Store new output size
            // Reconstruct loaderInput
            loaderOutput.write(fileBuffer, 0, (int) compressEncryptStart);
            loaderOutput.write(output.toByteArray());
            fileBuffer = loaderOutput.toByteArray();
        }

        if (loaderEncryption) {
            // Create buffer to encrypt and fix padding
            paddingByteCount = ENCRYPTION_ALIGNMENT - (outputSize % ENCRYPTION_ALIGNMENT);
            if (paddingByteCount == 8) {
                paddingByteCount = 0;
            }
            byte[] bufferToEncrypt = new byte[outputSize + paddingByteCount];
            System.arraycopy(fileBuffer, (int) compressEncryptStart, bufferToEncrypt, 0, outputSize);
            for (int i = 0; i < paddingByteCount; i++) {
                bufferToEncrypt[outputSize + i] = 0;
            }
            // Encrypt buffer
            input = new ByteArrayInputStream(bufferToEncrypt);
            output = new ByteArrayOutputStream();
            int[] randomKeyValue = (int[]) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_RANDOM_KEY_VALUE);
            encrypt(input, output, randomKeyValue);
            outputSize = output.size();
            // Reconstruct loaderInput
            loaderOutput.reset();
            loaderOutput.write(fileBuffer, 0, (int) compressEncryptStart);
            // If not an ACC pre-loader create space for specified number of
            // session keys.
            if (!isAccPreloader) {
                createSessionKeySpace(loaderOutput, signerSettings);
            }
            loaderOutput.write(output.toByteArray());
            fileBuffer = loaderOutput.toByteArray();
            try {
                setKeyBlock(signerSettings);
            } catch (Exception e) {
                throw new IOException("Method setKeyBlock() failed");
            }
        }

        if (loaderEncryption) {
            data = outputSize + sessionKeyCount * SESSION_KEY_SIZE;
            data = data + (sessionKeyCount << 24);
            if (loaderCompression) {
                data = (data + (paddingByteCount << 28)) | 0x80000000;
            } else {
                data = (data + (paddingByteCount << 28));
            }
        } else {
            data = outputSize | 0x80000000;
        }

        if (isAccPreloader) {
            updateAccessPreLoaderFileBuffer(data, controlFields);
            payloadLength = outputSize + compressEncryptStart - 34;
        } else {
            numberOfKeyBlocks = (byte) (data >> 24);
            updatePayloadLength(loaderCompression, outputSize);
        }

        if (isAccPreloader && oldAccPreLoader) {
            String[] chipIDs = (String[]) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_CHIPIDS);
            numberOfKeyBlocks = (byte) chipIDs.length;
        }

        // Save fileBuffer to new file
        createFileFromBuffer(loaderOutFile, fileBuffer);
    }

    private void updatePayloadLength(boolean loaderCompression, int outputSize) {
        if (loaderCompression) {
            payloadLength = loaderSize;
        } else {
            payloadLength = outputSize + sessionKeyCount * SESSION_KEY_SIZE;
        }
    }

    private void createSessionKeySpace(ByteArrayOutputStream loaderOutput, A2SignerSettings signerSettings) {
        String[] chipIDs = (String[]) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_CHIPIDS);
        sessionKeyCount = chipIDs.length;
        // sessionKeyCount = clientContainer.getChipIDS().length;
        for (int i = 0; i < sessionKeyCount; i++) {
            for (int j = 0; j < SESSION_KEY_SIZE; j++) {
                loaderOutput.write(0x22);
            }
        }
    }

    /**
     * @return number of key blocks in payload
     */
    public byte getNumberOfKeyblocks() {
        return numberOfKeyBlocks;
    }

    /**
     * @param file
     *            File to create byte buffer from.
     * @return A data buffer filled with all contents from file.
     * @throws IOException
     *             If I/O error occured.
     */
    private byte[] createBufferFromFile(File file) throws IOException {
        BufferedInputStream input = null;
        byte[] resultBuffer = null;

        try {
            input = new BufferedInputStream(new FileInputStream(file));
            resultBuffer = new byte[(int) file.length()];
            int read = input.read(resultBuffer, 0, resultBuffer.length);
            if (read < 1) {
                throw new IOException("Failed to read file " + file.getAbsolutePath() + ".");
            }
        } finally {
            if (input != null) {
                try {
                    input.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return resultBuffer;
    }

    /**
     * @param file
     *            File to create.
     * @param buffer
     *            Data to be written to file.
     */
    private void createFileFromBuffer(File file, byte[] buffer) {
        FileOutputStream output = null;
        try {
            output = new FileOutputStream(file);
            output.write(buffer);
        } catch (IOException e) {
            System.err.println(e);
        } finally {
            if (output != null) {
                try {
                    output.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * This method will try to locate the payload offset inside a loader binary file buffer. If
     * found it will also calculate number of session keys (used for access cpu pre-loader).
     * 
     * @param buffer
     *            Buffer to search in for payload offset.
     * @return An offset to tranformation start.
     */
    private long locatePayloadOffset(byte[] buffer) {
        long result = OFFSET_PAYLOAD;
        int i = 0;
        int c = 0;
        while (i < buffer.length) {
            while (buffer[i] == 0x22) {
                c++;
                i++;
            }
            if ((c != 0) && ((c % SESSION_KEY_SIZE) == 0)) {
                sessionKeyCount = c / SESSION_KEY_SIZE;
                result = i;
            } else {
                i++;
            }
            c = 0;
        }
        return result;
    }

    /**
     * This methods compress the contents in input and stores result in output
     * 
     * @param input
     *            Input to read uncompressed data from.
     * @param output
     *            Output to write compressed data to.
     * @throws IOException
     *             If I/O error occured.
     */
    private void compress(final ByteArrayInputStream input, final ByteArrayOutputStream output) throws IOException {
        int wHead = 0;
        int uHead = 0;
        int lookLength;
        int c;

        bitBuffer = 0;
        bitCount = 0;

        // Fill sliding window
        for (int i = 0; i < SLIDING_WINDOW_SIZE; i++) {
            slidingWindow[i] = ' ';
        }
        int read = input.read(lookAhead);
        if (read < 1) {
            throw new IOException("Failed to read stream.");
        }
        lookLength = lookAhead.length;
        findMatch(wHead, uHead);
        while (lookLength > 0) {
            if (output.size() > 500000) {
                break;
            }

            if (matchedDataLength > lookLength) {
                matchedDataLength = lookLength;
            }

            if (matchedDataLength <= 2) {
                bitFilePutBit(1, output);
                bitFilePutChar(lookAhead[uHead], output);
                matchedDataLength = 1;
            } else {
                bitFilePutBit(0, output);
                c = ((matchedDataOffset & 0xFFF) >> 4);
                bitFilePutChar(c, output);
                c = (((matchedDataOffset & 0xF) << 4) | (matchedDataLength - 3));
                bitFilePutChar(c, output);
            }

            int i = 0;
            while (i < matchedDataLength) {
                c = input.read();
                if (c == -1) {
                    // End of input
                    break;
                }
                slidingWindow[wHead] = lookAhead[uHead];
                lookAhead[uHead] = (byte) c;
                wHead = (wHead + 1) % SLIDING_WINDOW_SIZE;
                uHead = (uHead + 1) % LOOK_AHEAD_SIZE;
                i++;
            }
            while (i < matchedDataLength) {
                slidingWindow[wHead] = lookAhead[uHead];
                wHead = (wHead + 1) % SLIDING_WINDOW_SIZE;
                uHead = (uHead + 1) % LOOK_AHEAD_SIZE;
                lookLength--;
                i++;
            }
            findMatch(wHead, uHead);
        }

        if (bitCount != 0) {
            bitBuffer <<= 8 - bitCount;
            output.write(bitBuffer);
        }
    }

    /**
     * Encrypts input and places the result in output.
     * 
     * @param input
     *            Length of input must be a multiple of 8
     * @param output
     * @param randomKeyValue
     * @throws IOException
     */
    private void encrypt(final ByteArrayInputStream input, final ByteArrayOutputStream output, int[] randomKeyValue)
        throws IOException {
        EncryptionAlgorithm.encryptPayload(ENCRYPTION_ITERATION_COUNT, input, randomKeyValue, output);
    }

    /**
     * Fills the key block placeholders with encrypted keys.
     * 
     * @param signerSettings
     * 
     * @throws IOException
     *             If an I/O error occured.
     */
    private void setKeyBlock(A2SignerSettings signerSettings) throws Exception {

        String[] chipIDS = (String[]) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_CHIPIDS);

        byte[] keyBlocks = new byte[SESSION_KEY_SIZE * chipIDS.length];

        for (int i = 0; i < chipIDS.length; ++i) {
            Integer numberAsInteger;
            if (chipIDS[i].substring(0, 2).equalsIgnoreCase("0x")) {
                numberAsInteger = Integer.valueOf(chipIDS[i].substring(2, chipIDS[i].length()), 16);
            } else {
                numberAsInteger = Integer.valueOf(chipIDS[i]);
            }
            ISignerService service =
                (ISignerService) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGNER_SERVICE);
            int[] randomKeyValue = (int[]) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_RANDOM_KEY_VALUE);
            byte[] encryptedSessionKey = service.encryptSessionKey(numberAsInteger.intValue(), randomKeyValue);
            if (encryptedSessionKey == null) {
                throw new Exception("Failed to encrypt session key.");
            }

            for (int j = 0; j < SESSION_KEY_SIZE; ++j) {
                if (j < 4) {
                    keyBlocks[20 * i + j] = (byte) (0xff & (numberAsInteger.intValue() >>> j * 8));
                } else {
                    keyBlocks[20 * i + j] = encryptedSessionKey[j - 4];
                }
            }
        }

        int offset = (int) compressEncryptStart;
        if (isAccPreloader) {
            offset = offset - sessionKeyCount * SESSION_KEY_SIZE;
        }

        for (int i = 0; i < keyBlocks.length; i++) {
            fileBuffer[offset + i] = keyBlocks[i];
        }
    }

    /**
     * @param w
     * @param u
     */
    private void findMatch(int w, int u) {
        int i = w;
        int j = 0;

        matchedDataLength = 0;
        while (true) {
            if (slidingWindow[i] == lookAhead[u]) {
                j = 1;
                while (slidingWindow[(i + j) % SLIDING_WINDOW_SIZE] == lookAhead[(u + j) % LOOK_AHEAD_SIZE]) {
                    if (j >= LOOK_AHEAD_SIZE) {
                        break;
                    }
                    j++;
                }
                if (j > matchedDataLength) {
                    matchedDataLength = j;
                    matchedDataOffset = i;
                }
            }
            if (j >= LOOK_AHEAD_SIZE) {
                matchedDataLength = LOOK_AHEAD_SIZE;
                return;
            }
            i = (i + 1) % SLIDING_WINDOW_SIZE;
            if (i == w) {
                return;
            }
        }
    }

    /**
     * @param c
     *            Character to write.
     * @param output
     *            Output to write data to.
     * @throws IOException
     *             If I/O error occured.
     */
    private void bitFilePutChar(int c, OutputStream output) throws IOException {
        int temp;

        if (bitCount == 0) {
            output.write(c);
            return;
        }

        temp = (c & 0xFF) >> bitCount;
        temp = temp | ((bitBuffer << (8 - bitCount)) & 0xFF);
        output.write(temp);
        bitBuffer = c;
    }

    /**
     * @param c
     * @param output
     * @throws IOException
     */
    private void bitFilePutBit(int c, OutputStream output) throws IOException {
        bitCount++;
        bitBuffer = (bitBuffer << 1) & 0xFF;

        if (c != 0) {
            bitBuffer = bitBuffer | 1;
        }

        if (bitCount == 8) {
            output.write(bitBuffer);
            bitCount = 0;
            bitBuffer = 0;
        }
    }

    private void updateAccessPreLoaderFileBuffer(int data, COPSControlFields controlFields) {

        int offset = (int) (compressEncryptStart - (sessionKeyCount * SESSION_KEY_SIZE + 12));

        oldAccPreLoader = true;
        if ((fileBuffer[offset + 8] == 0x33) && (fileBuffer[offset + 9] == 0x33) && (fileBuffer[offset + 10] == 0x33)
            && (fileBuffer[offset + 8] == 0x33)) {
            oldAccPreLoader = false;
        } else {
            offset = offset + 4;
        }

        if (!oldAccPreLoader) {
            fileBuffer[offset++] = (byte) (data & 0xFF);
            fileBuffer[offset++] = (byte) ((data >> 8) & 0xFF);
            fileBuffer[offset++] = (byte) ((data >> 16) & 0xFF);
            fileBuffer[offset++] = (byte) (data >> 24);
        } else {
            data = fileBuffer.length - offset - 8;
            fileBuffer[offset++] = (byte) (data & 0xFF);
            fileBuffer[offset++] = (byte) ((data >> 8) & 0xFF);
            fileBuffer[offset++] = (byte) ((data >> 16) & 0xFF);
            fileBuffer[offset++] = (byte) (data >> 24);
        }

        // Write CID

        int cid = controlFields.getCustomerID();

        fileBuffer[offset++] = (byte) (cid & 0xFF);
        fileBuffer[offset++] = (byte) ((cid >> 8) & 0xFF);
        fileBuffer[offset++] = (byte) ((cid >> 16) & 0xFF);
        fileBuffer[offset++] = (byte) (cid >> 24);

        if (!oldAccPreLoader) {
            // Write loader size
            fileBuffer[offset++] = (byte) (loaderSize & 0xFF);
            fileBuffer[offset++] = (byte) ((loaderSize >> 8) & 0xFF);
            fileBuffer[offset++] = (byte) ((loaderSize >> 16) & 0xFF);
            fileBuffer[offset++] = (byte) (loaderSize >> 24);
        }

    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.internal.payload.IPayload#getLength()
     */
    /**
     * @return Payload length
     */
    public long getLength() {
        // if (transformed) {
        return payloadLength;
        // }
        // return payloadBuffer.limit();
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * com.stericsson.sdk.signing.internal.payload.IPayload#write(java.nio.channels.WritableByteChannel
     * )
     */
    /**
     * Writes the payload to specified channel
     * 
     * @param channel
     *            Writable byte channel
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel channel) throws IOException {
        payloadBuffer.rewind();
        channel.write(payloadBuffer);
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.internal.payload.IPayload#getHashValue(int)
     */
    /**
     * Calculates and returns the hash value for the payload
     * 
     * @param hashAlgorithm
     *            Algorithm to use for hashing
     * @return Payload hash
     */
    public byte[] getHashValue(int hashAlgorithm) {
        DigestDecorator digester = new DigestDecorator(hashAlgorithm);
        digester.update(payloadBuffer.array());
        return digester.digest();
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.internal.payload.IPayload#getDestinationAddress()
     */
    /**
     * Returns the payload destination address extracted from the unsigned loader file
     * 
     * @return Payload destination address
     */
    public long getDestinationAddress() {
        return startAddress;
    }

    /**
     * @param fields
     *            TBD
     */
    public void updateCOPSControlFields(COPSControlFields fields) {
        fields.setNumberOfKeyblocks(getNumberOfKeyblocks());
    }

}
