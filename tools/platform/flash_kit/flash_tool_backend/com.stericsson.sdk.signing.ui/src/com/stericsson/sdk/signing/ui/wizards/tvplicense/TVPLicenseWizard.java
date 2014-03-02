package com.stericsson.sdk.signing.ui.wizards.tvplicense;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.ui.wizards.AbstractSignWizard;

/**
 * Wizard for creating unsigned TVP license file;
 */
public class TVPLicenseWizard extends AbstractSignWizard {

    private static final String CERTIFICATE_TYPE = "X.509";

    private static final String WIZARD_NAME = "TVP License Wizard";

    private static final String MAIN_PAGE = "Select Files";

    private static final String SIGN_PAGE = "Sign License";

    private TVPLicensePathsPage pathsPage = new TVPLicensePathsPage(MAIN_PAGE);

    Logger logger = Logger.getLogger(TVPLicenseWizard.class.getName());

    /**
     * Constructor
     */
    public TVPLicenseWizard() {
        super(SIGN_PAGE);
        setHelpAvailable(true);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        setWindowTitle(WIZARD_NAME);
        addPage(pathsPage);
        super.addPages();
    }

    /**
     * {@inheritDoc}
     */
    public boolean performFinish() {

        X509Certificate cert = null;

        byte[] xmlBytes = readBytesFromFile(pathsPage.getXMLFile());
        FileInputStream fos = null;
        CertificateFactory certFac;
        try {
            certFac = CertificateFactory.getInstance(CERTIFICATE_TYPE);

            fos = new FileInputStream(pathsPage.getCertificateFile());
            cert = (X509Certificate) certFac.generateCertificate(fos);

            byte[] certBytes = cert.getEncoded();

            ByteBuffer licenseBuffer = ByteBuffer.allocate(xmlBytes.length + certBytes.length);
            licenseBuffer.put(xmlBytes);
            licenseBuffer.put(certBytes);

            writeToFile(licenseBuffer);

            super.performFinish();
        } catch (CertificateException e) {
            logger.error(e.getMessage());
        } catch (FileNotFoundException e) {
            logger.error(e.getMessage());
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    logger.error(e.getMessage());
                }
            }
        }
        return true;
    }

    /**
     * 
     *{@inheritDoc}
     */
    public String getPayloadFilePath() {
        return pathsPage.getTVPLicenseFile();

    }

    /**
     * 
     *{@inheritDoc}
     */
    public GenericSoftwareType getPayloadType() {
        return GenericSoftwareType.TVP_LICENSE;
    }

    private byte[] readBytesFromFile(String filePath) {

        InputStream is = null;
        byte[] bytes = null;
        try {

            File file = new File(filePath);
            is = new FileInputStream(file);

            bytes = new byte[(int) file.length()];

            int offset = 0;
            int numRead = 0;
            while (offset < bytes.length) {

                numRead = is.read(bytes, offset, bytes.length - offset);
                if (numRead < 0) {
                    break;
                }
                offset += numRead;
            }

            if (offset < bytes.length) {
                throw new IOException("Could not completely read file " + filePath);
            }

        } catch (IOException e) {
            logger.error(e.getMessage());
        } finally {
            if (is != null) {
                try {
                    is.close();
                } catch (IOException e) {
                    logger.error(e.getMessage());
                }
            }
        }
        return bytes;

    }

    private void writeToFile(ByteBuffer buffer) {
        FileChannel channel = null;
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(pathsPage.getTVPLicenseFile(), false);
            channel = fos.getChannel();
            buffer.position(0);
            channel.write(buffer);

        } catch (IOException e) {
            logger.error(e.getMessage());
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    logger.error(e.getMessage());
                }
            }
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    logger.error(e.getMessage());
                }
            }
        }
    }

}
