package com.stericsson.sdk.signing.cops;

import java.io.IOException;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

/**
 * Signed data container used in COPS
 * 
 * @author tomas
 */
public class COPSSignedData {

    /** */
    private COPSControlFields copsControlFields;

    /** */
    private COPSCertificateList copsCertificateList;

    /** */
    private COPSSignature copsSignature;

    /** */
    private COPSHashList copsHashList;

    /** */
    private COPSPadding copsPadding;

    /**
     * @param input
     *            Readable byte channel
     * @throws IOException
     *             If an I/O related error occurred
     */
    public COPSSignedData(ReadableByteChannel input) throws IOException {
        copsControlFields = new COPSControlFields(input);
        copsCertificateList = new COPSCertificateList(input, copsControlFields);
        copsSignature = new COPSSignature(input, copsControlFields);
        copsHashList = new COPSHashList(input, copsControlFields);
        copsPadding = new COPSPadding(input, copsControlFields);
    }

    /**
     * @param controlFields
     *            Control fields
     * @param certificateList
     *            Certificate list
     * @param signature
     *            Signature
     * @param hashList
     *            Hash list
     * @param padding
     *            Padding
     */
    public COPSSignedData(COPSControlFields controlFields, COPSCertificateList certificateList,
        COPSSignature signature, COPSHashList hashList, COPSPadding padding) {
        copsControlFields = controlFields;
        copsCertificateList = certificateList;
        copsSignature = signature;
        copsHashList = hashList;
        copsPadding = padding;
    }

    /**
     * @param output
     *            Writable byte channel to write to
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel output) throws IOException {
        copsControlFields.write(output);
        copsCertificateList.write(output);
        copsSignature.write(output);
        copsHashList.write(output);
        copsPadding.write(output);
    }

    /**
     * @return Control fields
     */
    public COPSControlFields getControlFields() {
        return copsControlFields;
    }

    /**
     * @return Certificate list
     */
    public COPSCertificateList getCertificateList() {
        return copsCertificateList;
    }

    /**
     * @return Signature list
     */
    public COPSSignature getSignature() {
        return copsSignature;
    }

    /**
     * @return Hash list
     */
    public COPSHashList getHashList() {
        return copsHashList;
    }

    /**
     * @return Padding
     */
    public COPSPadding getPadding() {
        return copsPadding;
    }

}
