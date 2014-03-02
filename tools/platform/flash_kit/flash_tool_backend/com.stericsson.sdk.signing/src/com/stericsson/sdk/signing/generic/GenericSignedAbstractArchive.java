package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;
import java.util.zip.CRC32;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * @author xtomzap
 * 
 */
public abstract class GenericSignedAbstractArchive implements IGenericSignablePayload {

    private static final int DEFAULT_HASH_BLOCK_SIZE = 128 * 1024;

    private static final int OFFSET_LENGTH = 8;

    private static final int CHUNK_SIZE_LENGTH = 8;

    private int bufferSize = 256 * 1024;

    private int hashBlockSize;

    private CRC32 crc32;

    private Logger logger;

    private File payloadFile;

    private byte[] hashListHash;

    private byte[] hashList;

    private GenericCertificate certificate;

    private byte[] signature;

    private String outputFilename;

    private String archiveName = "";

    private String signatureName = "";

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @param pArchiveName
     *            Archive name
     * @param pSignatureName
     *            Signature name
     * @throws SignerException
     *             on errors
     */
    public GenericSignedAbstractArchive(GenericSignerSettings signerSettings, String pArchiveName, String pSignatureName)
        throws SignerException {
        this.archiveName = pArchiveName;
        this.signatureName = pSignatureName;
        logger = Logger.getLogger(getClass().getName());
        payloadFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        Object bufferSetting = signerSettings.getSignerSetting(IGenericSignerSettings.KEY_BUFFER_SIZE);

        if (bufferSetting != null && !String.valueOf(bufferSetting).equalsIgnoreCase("")) {
            int bufferSettingInt = Integer.parseInt(String.valueOf(bufferSetting));
            logger.info("Using buffer size: " + bufferSettingInt + "bytes.");
            bufferSize = bufferSettingInt;
        }

        if (!payloadFile.exists()) {
            throw new SignerException("Failed to read file "
                + signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        } else if (payloadFile.isDirectory()) {
            throw new SignerException(signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE)
                + " is a directory");
        }
        outputFilename = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_OUTPUT_FILE);
        if (isSigned()) {
            logger.info("Archive already signed. Extracting signable parts...");
            payloadFile = extractContents();
        }

        Object inputHashBlockSize = signerSettings.getSignerSetting(IGenericSignerSettings.KEY_HASH_BLOCK_SIZE);
        if (inputHashBlockSize != null && !String.valueOf(inputHashBlockSize).equalsIgnoreCase("")) {
            int hbSize = Integer.parseInt(String.valueOf(inputHashBlockSize));
            if (hbSize > 0) {
                hashBlockSize = hbSize;
            } else {
                hashBlockSize = DEFAULT_HASH_BLOCK_SIZE;
            }
        } else {
            hashBlockSize = DEFAULT_HASH_BLOCK_SIZE;
        }
    }

    private File extractContents() throws SignerException {
        byte[] buf = new byte[bufferSize];
        ZipInputStream zipInput = null;
        FileOutputStream output = null;
        File file = null;
        try {
            zipInput = new ZipInputStream(new FileInputStream(payloadFile));

            file = File.createTempFile("contents_", ".zip");
            file.deleteOnExit();
            output = new FileOutputStream(file);

            ZipEntry entry = null;
            while ((entry = zipInput.getNextEntry()) != null) {
                if (entry.getName().equals(archiveName)) {
                    int n = 0;
                    while ((n = zipInput.read(buf, 0, buf.length)) > -1) {
                        output.write(buf, 0, n);
                    }
                    zipInput.closeEntry();
                }
            }

        } catch (Exception e) {
            throw new SignerException(e);
        } finally {
            if (output != null) {
                try {
                    output.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
            if (zipInput != null) {
                try {
                    zipInput.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }
        return file;
    }

    private boolean isSigned() throws SignerException {
        boolean isSigned = false;
        ZipFile zipFile = null;
        try {
            zipFile = new ZipFile(payloadFile);
            if (containsEntry(zipFile, archiveName) && containsEntry(zipFile, signatureName)) {
                isSigned = true;
            }
        } catch (Exception e) {
            throw new SignerException(e);
        } finally {
            if (zipFile != null) {
                try {
                    zipFile.close();
                } catch (Exception e) {
                    logger.warn(e.getMessage());
                }
            }
        }
        return isSigned;
    }

    private boolean containsEntry(ZipFile zipFile, String entryName) {
        return zipFile.getEntry(entryName) != null;
    }

    /**
     * 
     * @param data
     *            the signature data
     */
    public void setSignature(byte[] data) {
        this.signature = data;
    }

    /**
     * 
     * @return the signature
     */
    public byte[] getSignature() {
        return signature;
    }

    /**
     * @return the certificate
     */
    public GenericCertificate getCertificate() {
        return certificate;
    }

    /**
     * @param cert
     *            the certificate to set
     */
    public void setCertificate(GenericCertificate cert) {
        this.certificate = cert;
    }

    /**
     * @return Payload destination address
     */
    public long getDestinationAddress() {
        // not used
        return 0;
    }

    /**
     * Calculates and returns the hash value for the payload
     * 
     * @see DigestDecorator#SHA_1
     * @see DigestDecorator#SHA_256
     * @param hashAlgorithm
     *            Algorithm to use for hashing
     * @return Payload hash
     */
    public byte[] getHashValue(int hashAlgorithm) {
        if (hashListHash == null || hashList.length <= 0 || hashListHash == null) {
            try {
                hashList = calculateHashList(DigestDecorator.SHA_256);
                DigestDecorator digester = new DigestDecorator(hashAlgorithm);
                digester.update(hashList);
                hashListHash = digester.digest();
            } catch (IOException ioe) {
                logger.error("Failed to generate payload hash value for " + payloadFile.getAbsolutePath() + ": "
                    + ioe.getMessage());
            }
        }
        return hashListHash;
    }

    /**
     * @return Payload length
     */
    public long getLength() {
        return hashList == null ? 0 : hashList.length;
    }

    /**
     * Writes the payload to specified channel
     * 
     * @param channel
     *            Writable byte channel
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel channel) throws IOException {

        // special treatment for flash archive
        // close the original channel and create a zip output to the same file
        channel.close();
        channel = null;
        ZipOutputStream zipOutput = null;

        try {
            // write archive

            zipOutput = new ZipOutputStream(new FileOutputStream(outputFilename));

            writeSignatureEntry(zipOutput);
            writeArchiveEntry(zipOutput, payloadFile);
        } finally {
            if (zipOutput != null) {
                try {
                    zipOutput.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }
    }

    private ByteBuffer readFileContent(File file) throws IOException {
        FileChannel channel = null;
        ByteBuffer payloadBuffer = null;
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(file);
            channel = fis.getChannel();
            payloadBuffer = ByteBuffer.allocate((int) channel.size());
            channel.read(payloadBuffer);
            payloadBuffer.rewind();
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
        return payloadBuffer;
    }

    private byte[] calculateHashList(int algorithm) throws IOException {
        File sourceFile = payloadFile;
        File hashListFile = File.createTempFile("hlTemp", ".bin");
        hashListFile.deleteOnExit();

        crc32 = new CRC32();
        int numberOfBlocks = 0;
        FileChannel input = null;
        FileChannel output = null;
        FileInputStream fis = null;
        FileOutputStream fos = null;
        DigestDecorator digester = null;
        ByteBuffer buffer = ByteBuffer.allocate(hashBlockSize);
        ByteBuffer entryBuffer =
            ByteBuffer.allocate(OFFSET_LENGTH + CHUNK_SIZE_LENGTH + DigestDecorator.getHashSize(algorithm));
        entryBuffer.order(ByteOrder.LITTLE_ENDIAN);

        logger.info("Creating hashlist and CRC on payload. Using hash block size: " + hashBlockSize);

        long offset = 0;
        long chunkSize = hashBlockSize;

        try {
            fis = new FileInputStream(sourceFile);
            fos = new FileOutputStream(hashListFile);
            input = fis.getChannel();
            output = fos.getChannel();
            buffer.rewind();

            while (input.read(buffer) != -1) {
                if (buffer.remaining() == 0) {
                    crc32.update(buffer.array());

                    digester = new DigestDecorator(algorithm);
                    digester.update(buffer.array());

                    entryBuffer.rewind();
                    entryBuffer.putLong(offset);
                    entryBuffer.putLong(chunkSize);
                    entryBuffer.put(digester.digest());
                    entryBuffer.rewind();

                    output.write(entryBuffer);

                    numberOfBlocks += 1;

                    buffer.rewind();

                    offset = offset + chunkSize;
                }
            }

            if (buffer.remaining() > 0) {
                crc32.update(buffer.array(), 0, buffer.position());

                digester = new DigestDecorator(algorithm);
                digester.update(buffer.array(), 0, buffer.position());

                entryBuffer.rewind();
                entryBuffer.putLong(offset);
                entryBuffer.putLong(buffer.position());
                entryBuffer.put(digester.digest());
                entryBuffer.rewind();

                output.write(entryBuffer);

                numberOfBlocks += 1;
            }
        } finally {
            if (input != null) {
                input.close();
            }
            if (output != null) {
                output.close();
            }
            if (fis != null) {
                fis.close();
            }
            if (fos != null) {
                fos.close();
            }
        }
        return readFileContent(hashListFile).array();

    }

    private void writeArchiveEntry(ZipOutputStream zipOutput, File file) throws IOException {

        ZipEntry entry = null;
        FileChannel input = null;
        FileInputStream fis = null;
        ByteBuffer buffer = ByteBuffer.allocate(bufferSize);

        try {
            entry = new ZipEntry(archiveName);
            entry.setMethod(ZipEntry.STORED);
            entry.setSize(file.length());
            entry.setCompressedSize(file.length());
            entry.setCrc(crc32.getValue());
            zipOutput.putNextEntry(entry);
            fis = new FileInputStream(file);
            input = fis.getChannel();
            buffer.rewind();

            logger.info("Writing entry: " + archiveName);

            while (input.read(buffer) != -1) {
                if (buffer.remaining() == 0) {
                    zipOutput.write(buffer.array());
                    buffer.rewind();
                }
            }

            if (buffer.remaining() > 0) {
                zipOutput.write(buffer.array(), 0, buffer.position());
            }
        } finally {
            if (input != null) {
                try {
                    input.close();
                } catch (Exception e) {
                    e.getMessage();
                }
            }
            if (zipOutput != null) {
                try {
                    zipOutput.close();
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

    private void writeSignatureEntry(ZipOutputStream zipOutput) throws IOException {

        CRC32 crc = new CRC32();
        byte[] certificateData = certificate.getData();
        crc.update(certificateData);
        crc.update(signature);
        crc.update(hashList);

        ZipEntry entry = null;

        entry = new ZipEntry(signatureName);
        entry.setMethod(ZipEntry.STORED);
        entry.setSize(certificateData.length + hashList.length + signature.length);
        entry.setCompressedSize(certificateData.length + hashList.length + signature.length);
        entry.setCrc(crc.getValue());

        logger.info("Writing signature entry: " + signatureName);

        zipOutput.putNextEntry(entry);
        zipOutput.write(certificateData, 0, certificateData.length);
        zipOutput.write(signature, 0, signature.length);
        zipOutput.write(hashList, 0, hashList.length);
        zipOutput.closeEntry();
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasPreBuiltHeader() {
        return false;
    }

}
