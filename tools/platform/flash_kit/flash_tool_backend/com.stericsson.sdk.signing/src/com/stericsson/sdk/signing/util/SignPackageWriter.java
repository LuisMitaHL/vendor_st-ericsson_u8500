package com.stericsson.sdk.signing.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Date;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.signing.signerservice.ISignPackage;

/**
 * 
 * @author xolabju
 * 
 */
public final class SignPackageWriter {

    private SignPackageWriter() {
    }

    /**
     * Writes a sign package
     * 
     * @param outputFile
     *            path to the sign package file
     * @param indexMesp
     *            entries to be written in index.mesp
     * @param certificateFile
     *            the certificate to be included in the package
     * @throws IOException
     *             on errors
     */
    public static void writeSignPackage(String outputFile, MESPConfiguration indexMesp, String certificateFile)
        throws IOException {
        byte[] certificateData = readFromFile(certificateFile);
        writeSignPackage(outputFile, indexMesp, certificateData);
    }

    /**
     * Writes a sign package
     * 
     * @param outputFile
     *            path to the sign package file
     * @param indexMesp
     *            entries to be written in index.mesp
     * @param certificateData
     *            the certificate data
     * @throws IOException
     *             on errors
     */
    public static void writeSignPackage(String outputFile, MESPConfiguration indexMesp, byte[] certificateData)
        throws IOException {
        FileOutputStream fos = null;
        ZipOutputStream zos = null;
        try {
            fos = new FileOutputStream(new File(outputFile));
            zos = new ZipOutputStream(fos);

            ZipEntry mespEntry = new ZipEntry(ISignPackage.INDEX_MESP_FILENAME);
            ZipEntry certEntry = new ZipEntry(ISignPackage.CERTIFICATE_FILE_NAME);

            zos.putNextEntry(mespEntry);
            MESPConfigurationRecord[] records = indexMesp.getAllRecords();
            if (records != null) {
                for (MESPConfigurationRecord record : records) {
                    zos.write(record.toString().getBytes("UTF-8"));
                    zos.write("\n".getBytes("UTF-8"));
                }
            }
            zos.closeEntry();
            zos.putNextEntry(certEntry);
            zos.write(certificateData);
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

    /**
     * 
     * @param outputFile
     *            path to the sign package file
     * @param alias
     *            package alias
     * @param type
     *            package type
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
    public static void writeSignPackage(String outputFile, String alias, String type, String parent, String revision,
        String description, String certificateFile) throws IOException {
        byte[] certificateData = readFromFile(certificateFile);
        writeSignPackage(outputFile, alias, type, parent, revision, description, certificateData);
    }

    /**
     * 
     * @param outputFile
     *            path to the sign package file
     * @param alias
     *            package alias
     * @param type
     *            package type
     * @param parent
     *            package parent, or null if no parent
     * @param revision
     *            package revision
     * @param description
     *            package description
     * @param certificateData
     *            certificate data
     * @throws IOException
     *             one errors
     */
    public static void writeSignPackage(String outputFile, String alias, String type, String parent, String revision,
        String description, byte[] certificateData) throws IOException {
        MESPConfiguration mesp = new MESPConfiguration();
        MESPConfigurationRecord pkgRecord = new MESPConfigurationRecord();
        pkgRecord.setName(ISignPackage.PACKAGE_MESP_NAME);
        pkgRecord.setValue(ISignPackage.PACKAGE_ALIAS_NAME, alias);
        pkgRecord.setValue(ISignPackage.PACKAGE_TYPE_NAME, type);
        pkgRecord.setValue(ISignPackage.PACKAGE_CREATED_NAME, new Date().toString());
        pkgRecord.setValue(ISignPackage.PACKAGE_CREATED_BY_NAME, System.getProperty("user.name"));
        pkgRecord.setValue(ISignPackage.PACKAGE_REVISION_NAME, revision);
        pkgRecord.setValue(ISignPackage.PACKAGE_DESCRIPTION_NAME, description);
        if (parent != null && !parent.trim().equals("")) {
            pkgRecord.setValue(ISignPackage.PACKAGE_PARENT_NAME, parent);
        }
        MESPConfigurationRecord certRecord = new MESPConfigurationRecord();
        certRecord.setName(ISignPackage.CERTIFICATE_MESP_NAME);
        certRecord.setValue(ISignPackage.CERTIFICATE_NAME_NAME, ISignPackage.CERTIFICATE_FILE_NAME);

        mesp.addConfigurationRecord(pkgRecord);
        mesp.addConfigurationRecord(certRecord);

        writeSignPackage(outputFile, mesp, certificateData);

    }

    private static byte[] readFromFile(String fileName) throws IOException {
        File file = new File(fileName);
        byte[] bytes = null;
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(file);
            bytes = new byte[(int) file.length()];
            int read = fis.read(bytes);
            if (read < 0) {
                throw new IOException("Failed to read certificate from " + fileName);
            }
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return bytes;
    }
}
