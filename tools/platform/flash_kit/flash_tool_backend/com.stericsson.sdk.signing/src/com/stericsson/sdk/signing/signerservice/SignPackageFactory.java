package com.stericsson.sdk.signing.signerservice;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationException;
import com.stericsson.sdk.signing.a2.A2SignPackage;
import com.stericsson.sdk.signing.cops.COPSCertificateList;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.generic.cert.GenericAuthenticationCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;
import com.stericsson.sdk.signing.tvp.TVPSignPackage;
import com.stericsson.sdk.signing.tvp.X509CertificateChain;

/**
 * Factory class for creating sign package instances from sign package files.
 * 
 * @author xtomlju
 */
public final class SignPackageFactory {

    /**
     * @param data
     *            Byte array for sign package file
     * @return An ISignPackage interface
     * @throws SignerServiceException
     *             If an signer service related error occurred
     */
    public static ISignPackage createSignPackage(byte[] data) throws SignerServiceException {

        if (data == null) {
            return null;
        }
        File zipTemp = null;
        FileOutputStream output = null;

        try {
            zipTemp = File.createTempFile("signpackage", "zip");
            output = new FileOutputStream(zipTemp);
            output.write(data);
            return createSignPackage(zipTemp);
        } catch (IOException e) {
            throw new SignerServiceException(e.getMessage());
        } finally {
            if (output != null) {
                try {
                    output.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            if (zipTemp != null) {
                zipTemp.delete();
            }
        }
    }

    /**
     * @param file
     *            Sign package file
     * @return An ISignPackage interface
     * @throws SignerServiceException
     *             If an signer service related error occurred.
     */
    public static ISignPackage createSignPackage(File file) throws SignerServiceException {
        ZipFile zipFile;
        try {
            zipFile = new ZipFile(file);
        } catch (Exception e) {
            throw new SignerServiceException(e.getMessage());
        }
        return createSignPackage(zipFile);
    }

    /**
     * @param zipFile
     *            Sign package file
     * @return An ISignPackage interface
     * @throws SignerServiceException
     *             If an signer service related error occurred.
     */
    public static ISignPackage createSignPackage(ZipFile zipFile) throws SignerServiceException {

        ISignPackage signPackage = null;
        String packageType = "";
        String certificateName = "";
        String packageAlias = "";
        String packageParent = "";

        Logger logger = Logger.getLogger(SignPackageFactory.class.getName());

        try {
            MESPConfiguration config =
                MESPConfiguration.read(new ByteArrayInputStream(readEntry(zipFile, ISignPackage.INDEX_MESP_FILENAME)));

            packageType = config.getRecords(new String[] {
                ISignPackage.PACKAGE_MESP_NAME})[0].getValue(ISignPackage.PACKAGE_TYPE_NAME);
            packageAlias = config.getRecords(new String[] {
                ISignPackage.PACKAGE_MESP_NAME})[0].getValue(ISignPackage.PACKAGE_ALIAS_NAME);
            packageParent = config.getRecords(new String[] {
                ISignPackage.PACKAGE_MESP_NAME})[0].getValue(ISignPackage.PACKAGE_PARENT_NAME);
            certificateName = config.getRecords(new String[] {
                ISignPackage.CERTIFICATE_MESP_NAME})[0].getValue(ISignPackage.CERTIFICATE_NAME_NAME);

            if (packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_COPS_A2)
                || packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_COPS_A2_AUTH_CA)
                || packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_COPS_A2_AUTH)) {

                String hidName = config.getRecords(new String[] {
                    ISignPackage.HEADER_MESP_NAME})[0].getValue(ISignPackage.HID_FILENAME_NAME);
                String chipClassName = config.getRecords(new String[] {
                    ISignPackage.HEADER_MESP_NAME})[0].getValue(ISignPackage.CHIP_CLASS_FILENAME_NAME);

                signPackage =
                    new A2SignPackage(packageAlias, packageType, ByteBuffer.wrap(readEntry(zipFile, hidName)).order(
                        ByteOrder.BIG_ENDIAN).getInt(0), ByteBuffer.wrap(readEntry(zipFile, chipClassName)).order(
                        ByteOrder.LITTLE_ENDIAN).getInt(0),
                        new COPSCertificateList(readEntry(zipFile, certificateName)));
            } else if (isGenericSignPackage(packageType)) {
                GenericCertificate u5500Cert = readGenericCertificate(packageType, readEntry(zipFile, certificateName));
                signPackage = new GenericSignPackage(packageAlias, packageParent, packageType, u5500Cert);
            } else if (packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_X509)) {
                String headerName = config.getRecords(new String[] {
                    ISignPackage.HEADER_MESP_NAME})[0].getValue(ISignPackage.HEADER_NAME_NAME);
                X509CertificateChain chain = new X509CertificateChain(readEntry(zipFile, certificateName));
                ByteBuffer headerBuffer = ByteBuffer.wrap(readEntry(zipFile, headerName));
                headerBuffer.order(ByteOrder.LITTLE_ENDIAN);
                signPackage = new TVPSignPackage(packageAlias, packageType, headerBuffer.getInt(0), chain);

            } else {
                throw new SignerServiceException("Unsupported sign package type: " + packageType);
            }
        } catch (IOException e) {
            throw new SignerServiceException(e.getMessage());
        } catch (MESPConfigurationException e) {
            throw new SignerServiceException(e.getMessage());
        } finally {
            if (zipFile != null) {
                try {
                    zipFile.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }

        return signPackage;
    }

    private static GenericCertificate readGenericCertificate(String packageType, byte[] certData)
        throws SignerServiceException {
        GenericCertificate cert = null;

        if (packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_ROOT)) {
            cert = new GenericISSWCertificate();
        } else if (packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_GENERIC)) {
            cert = new GenericSignedHeader();
        } else if (packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_AUTH)) {
            cert = new GenericAuthenticationCertificate();
        } else if (packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_CRK)) {
            cert = new GenericCustomerRootKeyCertificate();
        } else {
            throw new SignerServiceException("Unrecognized packageType: " + packageType);
        }
        try {
            cert.setData(certData);
        } catch (Exception e) {
            throw new SignerServiceException(e.getMessage());
        }
        return cert;
    }

    private static boolean isGenericSignPackage(String packageType) {
        return packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_ROOT)
            || packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_GENERIC)
            || packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_AUTH)
            || packageType.equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_CRK);
    }

    private SignPackageFactory() {
    }

    private static byte[] readEntry(ZipFile zipFile, String name) throws IOException {

        byte[] result = null;
        ZipEntry entry = zipFile.getEntry(name);
        result = new byte[(int) entry.getSize()];
        int read = zipFile.getInputStream(entry).read(result);
        if (read < 1) {
            throw new IOException("Failed to read zip entry " + name + ".");
        }

        return result;
    }
}
