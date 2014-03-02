/**
 * 
 */
package com.stericsson.sdk.signing.ui.wizards.tvpsignpackage;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.Date;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.signing.tvp.X509CertificateChain;

/**
 * @author kapalpaw
 * 
 */
public final class TVPSignPackageWriter {

    private static final String INDEX_MESP_FILENAME = "index.mesp";

    // Package section
    private static final String PACKAGE_MESP_NAME = "Package";

    private static final String PACKAGE_ALIAS_NAME = "alias";

    private static final String PACKAGE_TYPE_X509 = "X.509";

    private static final String PACKAGE_PARENT_NAME = "parent";

    private static final String PACKAGE_TYPE_NAME = "type";

    private static final String PACKAGE_CREATED_NAME = "created";

    private static final String PACKAGE_CREATED_BY_NAME = "createdBy";

    private static final String PACKAGE_REVISION_NAME = "revision";

    private static final String PACKAGE_DESCRIPTION_NAME = "description";

    // Header section
    private static final String HEADER_MESP_NAME = "EMPHeader";

    private static final String HEADER_VERSION = "version";

    private static final String HEADER_NAME = "name";

    private static final String HEADER_FILENAME = "HeaderID.bin";

    private static final int HEADER_ID_MAGIC = 0xacdc0001;

    // Certificate section
    private static final String CERTIFICATE_MESP_NAME = "Certificate";

    private static final String CERTIFICATE_NAME_NAME = "name";

    private static final String CERTIFICATE_FILENAME = "CertificateChain.bin";

    private TVPSignPackageWriter() {

    }

    /**
     * 
     * @param outputFile
     *            path to the sign package file
     * @param alias
     *            package alias
     * @param parent
     *            package parent, or null if no parent
     * @param revision
     *            package revision
     * @param description
     *            package description
     * @param certificateFile
     *            path to certificate file
     * @throws IOException
     *             one errors
     */
    public static void writeSignPackage(final String outputFile, final String alias, final String parent,
        final String revision, final String description, final String certificateFile) throws IOException {

        // Create mesp data
        final MESPConfiguration mesp = new MESPConfiguration();

        // Package line
        final MESPConfigurationRecord pkgRecord = new MESPConfigurationRecord();
        pkgRecord.setName(PACKAGE_MESP_NAME);
        pkgRecord.setValue(PACKAGE_ALIAS_NAME, alias);
        pkgRecord.setValue(PACKAGE_TYPE_NAME, PACKAGE_TYPE_X509);
        pkgRecord.setValue(PACKAGE_CREATED_NAME, new Date().toString());
        pkgRecord.setValue(PACKAGE_CREATED_BY_NAME, System.getProperty("user.name"));
        pkgRecord.setValue(PACKAGE_REVISION_NAME, revision);
        pkgRecord.setValue(PACKAGE_DESCRIPTION_NAME, description);
        if (parent != null && !parent.trim().equals("")) {
            pkgRecord.setValue(PACKAGE_PARENT_NAME, parent);
        }

        // Header line
        final MESPConfigurationRecord headerRecord = new MESPConfigurationRecord();
        headerRecord.setName(HEADER_MESP_NAME);
        headerRecord.setValue(HEADER_VERSION, Integer.toHexString(HEADER_ID_MAGIC));
        headerRecord.setValue(HEADER_NAME, HEADER_FILENAME);

        // Certificate line
        final MESPConfigurationRecord certRecord = new MESPConfigurationRecord();
        certRecord.setName(CERTIFICATE_MESP_NAME);
        certRecord.setValue(CERTIFICATE_NAME_NAME, CERTIFICATE_FILENAME);

        // Add records to the mesp file
        mesp.addConfigurationRecord(pkgRecord);
        mesp.addConfigurationRecord(headerRecord);
        mesp.addConfigurationRecord(certRecord);

        // Create certificate chain data
        byte[] certificateChainData = null;

        FileInputStream fis = null;
        X509CertificateChain chain = null;

        try {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            fis = new FileInputStream(new File(certificateFile));

            final Certificate tempCert = cf.generateCertificate(fis);
            if (tempCert != null && tempCert instanceof X509Certificate) {
                final X509Certificate cert = (X509Certificate) tempCert;
                chain = new X509CertificateChain(new X509Certificate[] {
                    cert});
                certificateChainData = chain.getBytes();
            } else {
                // TODO:
                System.out.println("Error loading certificate");
            }
        } catch (Exception e) {
            // TODO:
            e.printStackTrace();
        } finally {
            if (fis != null) {
                fis.close();
            }
        }

        // Create header data
        final ByteBuffer bb = ByteBuffer.allocate(4);
        bb.putInt(HEADER_ID_MAGIC);
        final byte[] headerData = bb.array();

        writeSignPackage(outputFile, mesp, certificateChainData, headerData);
    }

    /**
     * Writes TVP sign package
     * 
     * @param outputFile
     *            path to the sign package file
     * @param indexMesp
     *            entries to be written in index.mesp
     * @param certificateData
     *            the certificate data
     * @param headerData
     *            header data
     * @throws IOException
     *             on errors
     */
    public static void writeSignPackage(String outputFile, MESPConfiguration indexMesp, byte[] certificateData,
        byte[] headerData) throws IOException {
        FileOutputStream fos = null;
        ZipOutputStream zos = null;
        try {
            fos = new FileOutputStream(new File(outputFile));
            zos = new ZipOutputStream(fos);

            // Write certificate chain data
            final ZipEntry certEntry = new ZipEntry(CERTIFICATE_FILENAME);
            zos.putNextEntry(certEntry);
            zos.write(certificateData);
            zos.closeEntry();

            // Write header data
            final ZipEntry headerEntry = new ZipEntry(HEADER_FILENAME);
            zos.putNextEntry(headerEntry);
            zos.write(headerData);
            zos.closeEntry();

            // Write mesp data
            final ZipEntry mespEntry = new ZipEntry(INDEX_MESP_FILENAME);
            zos.putNextEntry(mespEntry);
            MESPConfigurationRecord[] records = indexMesp.getAllRecords();
            if (records != null) {
                for (MESPConfigurationRecord record : records) {
                    zos.write(record.toString().getBytes("UTF-8"));
                    zos.write("\n".getBytes("UTF-8"));
                }
            }
            zos.closeEntry();

        } finally {
            if (zos != null) {
                try {
                    zos.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            if (fos != null) {
                try {
                    fos.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
