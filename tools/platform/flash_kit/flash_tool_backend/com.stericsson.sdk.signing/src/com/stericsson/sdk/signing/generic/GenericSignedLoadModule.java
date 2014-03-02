package com.stericsson.sdk.signing.generic;

import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SoftwareAlreadySignedException;
import com.stericsson.sdk.signing.elf.ELFFileHandler;
import com.stericsson.sdk.signing.elf.FileHeader;
import com.stericsson.sdk.signing.elf.ProgramHeader;
import com.stericsson.sdk.signing.elf.ProgramHeaderTable;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * Class for handling signing of fairbanks load modules
 * 
 * @author xolabju
 * 
 */
public class GenericSignedLoadModule implements IGenericSignablePayload {

    /** */
    public static final int MACHINE_ARM = 40;

    /** */
    public static final int SEGMENT_TYPE_LOAD = 1;

    /** */
    public static final int SEGMENT_TYPE_DYNAMIC = 2;

    /** */
    public static final int SEGMENT_TYPE_SIGNATURE = 0x656D7000;

    /** */
    public static final int SEGMENT_TYPE_HASH = 0x656D7001;

    /** */
    public static final int SEGMENT_TYPE_HASH_4K = 0x656D7002;

    /** */
    public static final int SEGMENT_TYPE_HASH_16G = 0x656D7018;

    /** */
    public static final int SEGMENT_TYPE_MANIFEST = 0x656D704D;

    private List<ProgramHeader> entries;

    /** */
    protected Logger logger;

    /** */
    protected File payloadFile;

    private long payloadLength;

    private byte[] hashValue;

    private GenericCertificate certificate;

    private byte[] signature;

    private File tempFile;

    private FileHeader elfHeader;

    private GenericSignerSettings settings;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public GenericSignedLoadModule(GenericSignerSettings signerSettings) throws SignerException {
        logger = Logger.getLogger(getClass().getName());
        payloadFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        if (!payloadFile.exists()) {
            throw new SignerException("Failed to read file "
                + signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        } else if (payloadFile.isDirectory()) {
            throw new SignerException(signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE)
                + " is a directory");
        }

        settings = signerSettings;
        payloadLength = payloadFile.length();
        try {
            tempFile = File.createTempFile("tempElf", ".elf");
            tempFile.deleteOnExit();
        } catch (IOException e) {
            throw new SignerException("Failed to create temp file: " + e.getMessage());
        }
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
     * @throws SignerException
     *             If an signer related error occured
     */
    public byte[] getHashValue(int hashAlgorithm) throws SignerException {
        if (hashValue == null || hashValue.length <= 0) {
            try {
                createHashListSegments();
                checkSignedHeader();
                createPayloadHash(hashAlgorithm);
            } catch (SoftwareAlreadySignedException e) {
                throw e;
            } catch (Exception ioe) {
                logger.error("Failed to generate payload hash value for " + payloadFile.getAbsolutePath() + ": "
                    + ioe.getMessage());
                throw new SignerException("Failed to generate payload hash value for " + payloadFile.getAbsolutePath()
                    + " " + ioe.getMessage());
            }
        }
        return hashValue;
    }

    private void checkSignedHeader() throws SoftwareAlreadySignedException {
        if (hasPreBuiltHeader()) {
            throw new SoftwareAlreadySignedException("Elf file '" + payloadFile.getAbsolutePath()
                + "' is already signed.");
        }
    }

    private void createPayloadHash(int hashAlgorithm) throws Exception {
        FileChannel output = null;
        RandomAccessFile raf = null;
        try {
            raf = new RandomAccessFile(tempFile, "rw");
            output = raf.getChannel();

            int signatureSegmentSize =
                (Short) settings.getSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE)
                    + certificate.getLength() + DigestDecorator.getHashSize(hashAlgorithm);

            ProgramHeader entry = new ProgramHeader();
            entry.setType(SEGMENT_TYPE_SIGNATURE);
            entry.setFlags(4);
            entry.setAlign(1);
            entry.setFileSize(signatureSegmentSize);
            entry.setOffset(tempFile.length());
            entries.add(entry);

            // Update ELF header
            elfHeader.setProgramHeaderNumOfEntries(entries.size());
            elfHeader.setProgramHeaderOffset(tempFile.length() + signatureSegmentSize);

            // Calculate payload hash and also the payload length
            payloadLength = 0;

            DigestDecorator payloadDigest = new DigestDecorator(hashAlgorithm);

            logger.debug("ELF file header data: " + HexUtilities.toHexString(elfHeader.getBytes()));

            payloadDigest.update(/* ELF file header */elfHeader.getBytes(), 0, elfHeader.getBytes().length);

            payloadLength = payloadLength + elfHeader.getBytes().length;

            ListIterator<ProgramHeader> i = entries.listIterator();
            while (i.hasNext()) {
                entry = i.next();
                int type = entry.getType();
                if ((type >= SEGMENT_TYPE_HASH) && (type <= SEGMENT_TYPE_HASH_16G)) {
                    logger.debug("Hash segment table entry data: "
                        + HexUtilities.toHexString(entry.getBytes(elfHeader.getELFClass())));

                    payloadDigest.update(entry.getBytes(elfHeader.getELFClass()), 0, entry.getBytes(elfHeader
                        .getELFClass()).length);
                    payloadLength = payloadLength + entry.getBytes(elfHeader.getELFClass()).length;
                } else if (shouldBeHashed(type)) {

                    if (entry.getFileSize() == 0) {
                        logger.debug("Skipped empty load segment from hashing");
                        continue;
                    }
                    logger.debug("Segment table entry data: "
                        + HexUtilities.toHexString(entry.getBytes(elfHeader.getELFClass())));
                    payloadDigest.update(entry.getBytes(elfHeader.getELFClass()), 0, entry.getBytes(elfHeader
                        .getELFClass()).length);
                    payloadLength = payloadLength + entry.getBytes(elfHeader.getELFClass()).length;
                }
            }
            // Now update hash digest with segment data from selected entries
            i = entries.listIterator();
            while (i.hasNext()) {
                entry = i.next();
                if ((entry.getType() >= SEGMENT_TYPE_HASH) && (entry.getType() <= SEGMENT_TYPE_HASH_16G)) {
                    if (entry.getData() == null) {
                        throw new SignerException("Invalid ELF file. No data element found.");
                    }
                    logger.debug("Hash segment data: " + HexUtilities.toHexString(entry.getData()));
                    payloadDigest.update(entry.getData(), 0, entry.getData().length);

                    payloadLength = payloadLength + entry.getData().length;
                }
            }
            hashValue = payloadDigest.digest();
            logger.debug("Payload hash: " + HexUtilities.toHexString(hashValue));
            logger.debug("Payload length: " + payloadLength);

        } finally {
            closeStreams(output, raf);
        }

    }

    private boolean shouldBeHashed(int type) {
        if ((type == SEGMENT_TYPE_LOAD) || (type == SEGMENT_TYPE_MANIFEST) || (type == SEGMENT_TYPE_SIGNATURE)
            || (type == SEGMENT_TYPE_DYNAMIC)) {
            return true;
        }
        return false;

    }

    private void createHashListSegments() throws IOException {
        FileChannel input = null;
        FileChannel output = null;
        FileInputStream fis = null;
        RandomAccessFile raf = null;
        try {
            fis = new FileInputStream(payloadFile);
            input = fis.getChannel();
            raf = new RandomAccessFile(tempFile, "rw");
            output = raf.getChannel();
            output.truncate(0);

            // Read ELF header
            elfHeader = new FileHeader();
            elfHeader.read(input);

            entries = ProgramHeaderTable.create(input, elfHeader);

            // Truncate output file and transfer original ELF to output
            output.truncate(0);
            long remaining = input.size();
            while (remaining > 0) {
                remaining = remaining - input.transferTo(0, input.size(), output);

            }

            createHashListSegments(input, output, SEGMENT_TYPE_HASH);
        } finally {
            closeStreams(input, output, fis, raf);
        }
    }

    private void closeStreams(Closeable... closeable) {
        if (closeable != null) {
            for (Closeable c : closeable) {
                if (c != null) {
                    try {
                        c.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    private void createHashListSegments(final FileChannel input, final FileChannel output, int hashSegmentType)
        throws IOException {

        LinkedList<ProgramHeader> hashEntries = new LinkedList<ProgramHeader>();
        ListIterator<ProgramHeader> i = entries.listIterator();
        while (i.hasNext()) {
            ProgramHeader entry = i.next();
            if (entry.getFileSize() == 0) {
                continue;
            }
            if ((entry.getType() == SEGMENT_TYPE_LOAD) || (entry.getType() == SEGMENT_TYPE_MANIFEST)
                || (entry.getType() == SEGMENT_TYPE_DYNAMIC)) {

                if (entry.getFileSize() == 0) {
                    continue;
                }

                ProgramHeader hashEntry = new ProgramHeader();
                byte[] data = getSegmentData(input, entry.getOffset(), (int) entry.getFileSize());

                byte[] hashListData = null;

                hashListData = ELFFileHandler.createHashListData(data, data.length);
                hashSegmentType = SEGMENT_TYPE_HASH;

                hashEntry.setType(hashSegmentType);
                hashEntry.setAlign(1);
                hashEntry.setVirtualAddress(entry.getVirtualAddress());
                hashEntry.setPhysicalAddress(entry.getPhysicalAddress());
                hashEntry.setMemorySize(entry.getFileSize());
                hashEntry.setFlags(4);
                hashEntry.setFileSize(hashListData.length);
                hashEntry.setOffset(output.position());
                hashEntry.setData(hashListData);

                hashEntries.add(hashEntry);
                // Write segment data to output
                ByteBuffer buffer = ByteBuffer.wrap(hashListData);
                output.write(buffer);
            }
        }

        entries.addAll(hashEntries);
    }

    private static byte[] getSegmentData(final FileChannel input, final long offset, final int size) throws IOException {

        ByteBuffer buffer = ByteBuffer.allocate(size);

        input.position(offset);
        while (buffer.remaining() > 0) {
            input.read(buffer);
        }

        return buffer.array();
    }

    /**
     * @return Payload length
     */
    public long getLength() {
        return payloadLength;
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
        FileChannel tempinput = null;
        FileChannel tempoutput = null;
        FileInputStream fis = null;
        RandomAccessFile raf = null;
        try {

            // it is not possible to use FileOutputStream because of java bug (4950302) on linux,
            // so RandomAccessFile has to used as workaround.
            raf = new RandomAccessFile(tempFile, "rw");
            tempoutput = raf.getChannel();
            tempoutput.position(tempFile.length());
            ByteBuffer certificateContent = readCertificateContent();
            ByteBuffer signatureContent = readSignatureContent();

            if (certificateContent != null) {
                tempoutput.write(certificateContent);
            }

            if (signatureContent != null) {
                tempoutput.write(signatureContent);
            }
            elfHeader.write(tempoutput);
            ListIterator<ProgramHeader> i = entries.listIterator();
            while (i.hasNext()) {
                i.next().write(tempoutput, elfHeader.getELFClass(), elfHeader.getELFByteOrder());
            }
            try {
                tempoutput.close();
            } catch (Exception e) {
                throw new IOException("Failed to close output");
            }
            fis = new FileInputStream(tempFile);
            tempinput = fis.getChannel();
            long fileSize = tempinput.size();

            channel.write(tempinput.map(FileChannel.MapMode.READ_ONLY, 0, fileSize));

        } catch (IOException ioe) {
            logger.error("Failed to write payload contents for " + payloadFile.getAbsolutePath() + ": "
                + ioe.getMessage());
        } finally {
            closeStreams(tempinput, tempoutput, fis, raf);
        }
    }

    /**
     * 
     * @return the read signature
     */
    protected ByteBuffer readSignatureContent() {
        ByteBuffer buf = null;

        if (signature != null) {
            buf = ByteBuffer.allocate(signature.length);
            buf.put(signature, 0, signature.length);
            buf.rewind();
        }

        return buf;
    }

    /**
     * 
     * @return the certificate content
     */
    protected ByteBuffer readCertificateContent() {
        ByteBuffer buf = null;

        if (certificate != null) {
            buf = ByteBuffer.allocate(certificate.getLength());
            buf.put(certificate.getData(), 0, certificate.getLength());
            buf.rewind();
        }

        return buf;
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasPreBuiltHeader() {
        for (ProgramHeader header : entries) {
            if (header.getType() == SEGMENT_TYPE_SIGNATURE) {
                return true;
            }
        }
        return false;
    }

    /**
     * 
     * @return the pre built cert or null if the payload was built without a cer
     */
    public GenericCertificate getPreBuiltHeader() {
        return null;
    }
}
