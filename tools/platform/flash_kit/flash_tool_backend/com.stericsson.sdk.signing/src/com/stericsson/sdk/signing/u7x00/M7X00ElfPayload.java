package com.stericsson.sdk.signing.u7x00;

import java.io.BufferedWriter;
import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.elf.FileHeader;
import com.stericsson.sdk.signing.elf.ProgramHeader;
import com.stericsson.sdk.signing.elf.ProgramHeaderTable;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * Class handling signing ELF files on Thorium
 * 
 * @author TSIKOR01
 * 
 */
public class M7X00ElfPayload implements IGenericSignablePayload {

    /** Logger */
    protected Logger logger;

    /** input file Unsigned ELF */
    protected File inputFile;

    private long payloadLength;

    private byte[] hashValue;

    private GenericCertificate certificate;

    private byte[] signature;

    private File payloadFile;

    private long ptLoadAdress;

    private String xmlName;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    // extract needed information and PT_LOAD from elf file
    public M7X00ElfPayload(GenericSignerSettings signerSettings) throws SignerException {
        logger = Logger.getLogger(M7X00ElfPayload.class);
        inputFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));

        if (!inputFile.exists()) {
            throw new SignerException("Failed to read file "
                + signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        } else if (inputFile.isDirectory()) {
            throw new SignerException(signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE)
                + " is a directory");
        }

        // creation of temp file
        try {
            payloadFile = File.createTempFile("tempfile", ".bin");
        } catch (IOException e) {
            throw new SignerException("Failed to create temp file: " + e.getMessage());
        }
        payloadFile.deleteOnExit();
        // output XML file name, this name value is same as the filename with xml extension,
        // if xml parameter was not set to proper value.
        xmlName = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_LOAD_ADDRESS_XML);
        if (!xmlName.endsWith(".xml")) {
            xmlName += ".xml";
        }
        // creating tempfile
        createTMPOutput(signerSettings);
        payloadLength = payloadFile.length();

    }

    /**
     * Creates TMP output file containing only PT_LOAD chung of the file, this chunk will be used as
     * content of sign file. There is also
     * 
     * @param signerSettings
     *            signer settings
     * @throws SignerException
     */
    private void createTMPOutput(GenericSignerSettings signerSettings) throws SignerException {
        FileChannel input = null;
        FileChannel output = null;
        FileInputStream fis = null;
        FileOutputStream fos = null;
        try {
            fis = new FileInputStream(inputFile);
            input = fis.getChannel();
            fos = new FileOutputStream(payloadFile);
            output = fos.getChannel();

            // Read ELF header
            FileHeader elfHeader = new FileHeader();
            elfHeader.read(input);

            // Create ProgramHeaderTable
            List<ProgramHeader> entries = ProgramHeaderTable.create(input, elfHeader);

            // Find PT_LOAD Entry
            for (ProgramHeader entry : entries) {
                // PT_LOAD type == 1
                if (entry.getType() == 1) {
                    ptLoadAdress = entry.getVirtualAddress();
                    // Truncate output file and transfer original PT_LOAD to output
                    output.truncate(0);
                    long ptLoadOffset = entry.getOffset();
                    // PT_LOAD size
                    long remaining = entry.getFileSize();
                    while (remaining != 0) {
                        remaining = remaining - input.transferTo(ptLoadOffset, remaining, output);
                    }
                    break;
                }
            }
            // setting load address to virtual address of PT_LOAD
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_LOAD_ADDRESS, ptLoadAdress);

            // setting Entry as the start address of the new header
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_START_ADDRESS, elfHeader.getEntry());

        } catch (Exception e) {
            throw new SignerException("Failed to process payload file: " + e.getMessage());
        } finally {
            closeStreams(input, output, fos, fis);
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

    /**
     * @return false
     * 
     * */
    public boolean hasPreBuiltHeader() {
        // there is no possibility do have already signed ELF for Thorium
        return false;
    }

    /**
     * @param cert
     *            the certificate to set
     */
    public void setCertificate(GenericCertificate cert) {
        this.certificate = cert;
    }

    /**
     * 
     * @param isignature
     *            the signature data
     */
    public void setSignature(byte[] isignature) {
        this.signature = isignature;
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
     * @return Payload destination address
     */
    public long getDestinationAddress() {
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
        if (hashValue == null || hashValue.length <= 0) {
            try {
                DigestDecorator digester = new DigestDecorator(hashAlgorithm);
                ByteBuffer payloadContent = readPayloadContent();
                digester.update(payloadContent.array());
                hashValue = digester.digest();
            } catch (IOException ioe) {
                logger.error("Failed to generate payload hash value for " + payloadFile.getAbsolutePath() + ": "
                    + ioe.getMessage());
            }
        }
        return hashValue;
    }

    /**
     * 
     * @return the payload as a byte buffer
     * @throws IOException
     *             on errors
     */
    protected ByteBuffer readPayloadContent() throws IOException {
        FileChannel channel = null;
        FileInputStream fis = null;
        ByteBuffer payloadBuffer = null;
        try {
            fis = new FileInputStream(payloadFile);
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
        try {
            ByteBuffer certificateContent = readCertificateContent();
            ByteBuffer payloadContent = readPayloadContent();
            ByteBuffer signatureContent = readSignatureContent();

            if (certificateContent != null) {
                channel.write(certificateContent);
            }

            if (signatureContent != null) {
                channel.write(signatureContent);
            }

            channel.write(payloadContent);
            if (signatureContent != null && certificateContent != null) {
                writeXMLFile(Long.valueOf(signatureContent.capacity() + certificateContent.capacity()));
            }

        } catch (IOException ioe) {
            logger.error("Failed to write payload contents for " + inputFile.getAbsolutePath() + ": "
                + ioe.getMessage());
        }
    }

    /**
     * Method writing output XML.
     * 
     * @param headerSize
     *            size of signed header
     * @throws IOException
     */
    private void writeXMLFile(Long headerSize) throws IOException {
        BufferedWriter out = null;
        try {
            out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(xmlName, false), "UTF-8"));
            out.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?>");
            out.write(System.getProperty("line.separator"));

            if (ptLoadAdress != 0) {
                out.write("<TOC-LoadAddess>" + HexUtilities.toHexString(ptLoadAdress - headerSize, false)
                    + "</TOC-LoadAddess>");
            } else {
                out.write("<TOC-LoadAddess>0x0</TOC-LoadAddess>");
            }
        } catch (IOException e) {
            throw new IOException("Failed to create output file: " + e.getMessage());
        } finally {
            if (out != null) {
                out.close();
            }
        }
    }
}
