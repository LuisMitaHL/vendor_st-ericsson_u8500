package com.stericsson.sdk.signing.cops;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.security.PublicKey;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.interfaces.RSAKey;
import java.util.Enumeration;
import java.util.Vector;

import org.bouncycastle.asn1.ASN1InputStream;
import org.bouncycastle.asn1.DERObject;
import org.bouncycastle.asn1.DEROctetString;
import org.bouncycastle.asn1.DEROutputStream;
import org.bouncycastle.asn1.DERSequence;

/**
 * Certificate list container used in COPS
 * 
 * @author tomas
 */
public class COPSCertificateList {

    private static final String EMP_EXTENSION_OID = "1.3.6.1.4.1.21885";

    private static final int TAG_SUBJECT_CID = 0x81;

    private static final int TAG_KEY_USAGE = 0x83;

    private ByteBuffer buffer;

    private X509Certificate[] certificates;

    /**
     * @param input
     *            Readable byte channel to read certificate list data from
     * @param copsControlFields
     *            Control fields
     * @throws IOException
     *             If an I/O related error occurred.
     */
    public COPSCertificateList(ReadableByteChannel input, COPSControlFields copsControlFields) throws IOException {
        buffer = ByteBuffer.allocate(copsControlFields.getCertificateListLength());
        input.read(buffer);
    }

    /**
     * Constructor
     * 
     * @param data
     *            Certificate list data
     */
    public COPSCertificateList(byte[] data) {

        buffer = ByteBuffer.wrap(data);

        try {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            ByteArrayInputStream bis = new ByteArrayInputStream(data);
            Vector<X509Certificate> certs = new Vector<X509Certificate>();
            while (bis.available() > 0) {
                certs.addElement((X509Certificate) cf.generateCertificate(bis));
            }
            certificates = certs.toArray(new X509Certificate[0]);
        } catch (Exception e) {
            throw new IllegalArgumentException("Could not parse certificate chain. Exception message: "
                + e.getMessage());
        }
    }

    /**
     * @return Certificate list as a byte array
     */
    public byte[] getBytes() {
        return buffer.array();
    }

    /**
     * @param output
     *            Writeable byte channel to write to
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel output) throws IOException {
        buffer.rewind();
        output.write(buffer);
    }

    /**
     * @param headerID
     *            Header ID
     * @return Protection type based on header id and key length
     */
    public int getProtectionType(int headerID) {

        int result = 0;
        int length = getModulusBitLength();

        if (headerID == COPSControlFields.HID_BABE0200) {
            if (COPSControlFields.RSA_1024_BIT_LENGTH == length) {
                result = COPSControlFields.PROT_TYPE_RSA_1024_SIG_WITH_SHA_1;
            } else if (COPSControlFields.RSA_2048_BIT_LENGTH == length) {
                result = COPSControlFields.PROT_TYPE_RSA_2048_SIG_WITH_SHA_1;
            }
        } else if (COPSControlFields.HID_BABE0300 == headerID) {
            if (COPSControlFields.RSA_1024_BIT_LENGTH == length) {
                result = COPSControlFields.PROT_TYPE_RSA_1024_SIG_WITH_SHA_256;
            } else if (COPSControlFields.RSA_2048_BIT_LENGTH == length) {
                result = COPSControlFields.PROT_TYPE_RSA_2048_SIG_WITH_SHA_256;
            }
        }

        return result;
    }

    /**
     * @return Subject customer id
     */
    public int getSubjectCID() {

        byte[] subjectCID;
        X509Certificate certificate = certificates[0];

        byte[] extensionValue = certificate.getExtensionValue(EMP_EXTENSION_OID);

        try {
            subjectCID = getExtensionParameter(extensionValue, TAG_SUBJECT_CID);
        } catch (IOException e) {
            subjectCID = null;
        }

        return ByteBuffer.wrap(subjectCID).order(ByteOrder.BIG_ENDIAN).getShort(0);
    }

    /**
     * @return Key usage as a byte array
     */
    public byte[] getKeyUsage() {

        byte[] keyUsage;
        X509Certificate certificate = certificates[0];

        byte[] extensionValue = certificate.getExtensionValue(EMP_EXTENSION_OID);

        try {
            keyUsage = getExtensionParameter(extensionValue, TAG_KEY_USAGE);
        } catch (IOException e) {
            keyUsage = null;
        }

        return keyUsage;
    }

    /**
     * @return The number of certificates
     */
    public int getNumberOfCertificates() {
        return certificates.length;
    }

    /**
     * @return Length in number of bytes
     */
    public int getLength() {
        return buffer.limit();
    }

    /**
     * @return Bit length of the public RSA key
     */
    public int getModulusBitLength() {
        PublicKey publicKey = certificates[certificates.length - 1].getPublicKey();
        if (publicKey != null && publicKey instanceof RSAKey) {
            return ((RSAKey) publicKey).getModulus().bitLength();
        } else {
            return -1;
        }
    }

    /**
     * Looks up specified extension parameter inside an certificate extension.
     * 
     * @param extension
     *            The certificate extension to look in.
     * @param tag
     *            The parameter tag to look for.
     * @return A byte[] containing the parameter value; null if parameter is not found in extension.
     * @throw IOException TBD
     */
    private static byte[] getExtensionParameter(byte[] extension, int tag) throws IOException {

        byte[] parameterValue = null;
        ASN1InputStream asnInput = new ASN1InputStream(new ByteArrayInputStream(extension));
        boolean found = false;

        while ((asnInput.available() > 0) && !found) {
            DEROctetString ds = (DEROctetString) asnInput.readObject();
            ASN1InputStream asnInsideInput = new ASN1InputStream(new ByteArrayInputStream(ds.getOctets()));

            while ((asnInsideInput.available() > 0) && !found) {

                Enumeration<?> derObjects = ((DERSequence) asnInsideInput.readObject()).getObjects();

                while (derObjects.hasMoreElements() && !found) {

                    ByteArrayOutputStream byteOutput = new ByteArrayOutputStream();
                    DEROutputStream dos = new DEROutputStream(byteOutput);
                    dos.writeObject((DERObject) derObjects.nextElement());

                    byte[] derTag = byteOutput.toByteArray();

                    if ((derTag[0] & 0xFF) == tag) {
                        int[] lengths = getDERLength(derTag);
                        int byteCount = lengths[0];
                        int byteCountTagValue = lengths[1];

                        parameterValue = new byte[byteCountTagValue];
                        System.arraycopy(derTag, byteCount + 1, parameterValue, 0, byteCountTagValue);

                        found = true;
                    }
                }
            }
        }

        return parameterValue;
    }

    /**
     * Calculates the length of the value in the DERField, assuming length of less than integer(4
     * bytes)
     * 
     * @param dERField
     *            The DERField itself
     * @return int[] The number of bytes the length uses and the actual length of the value in the
     *         DERField - of the first field!!!!!! int[0] = nbr of bytes used for length int[1] =
     *         length in bytes of value
     */
    private static int[] getDERLength(byte[] dERField) {
        int length = 0;
        int bytesForLength = 0;

        byte formIndicator = dERField[1];

        if ((formIndicator & 0x80) == 0) {
            length = formIndicator;
            bytesForLength = 1;
        } else {
            byte nbrBytesForLength = (byte) (formIndicator & 0x7F);
            bytesForLength = 1 + nbrBytesForLength;
            for (int i = 0; i < nbrBytesForLength; i++) {
                length = (length << 8);
                length |= (dERField[1 + 1 + i] & 0xFF);
            }
        }
        return new int[] {
            bytesForLength, length};
    }
}
