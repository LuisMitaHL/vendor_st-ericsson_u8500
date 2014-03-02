package com.stericsson.sdk.signing.ui.wizards.u5500dt;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.util.Map;

import org.eclipse.jface.dialogs.MessageDialog;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.cert.GenericDTCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.generic.cert.u8500.U8500DTCertificate;
import com.stericsson.sdk.signing.ui.wizards.AbstractSignWizard;

/**
 * @author xdancho
 * 
 */
public class U5500DTWizard extends AbstractSignWizard {

    enum WizardPage {
        TYPE_SELECTION("D \\& T certificate type"), REPLACE_KEY_SELECTION("Key replacement");

        private String pageName;

        private WizardPage(String pPageName) {
            pageName = pPageName;
        }

        public String getPageName() {
            return pageName;
        }
    }

    private static final String WIZARD_NAME = "New Debug & Test Certificate";

    private GenericDTCertificate cert;

    private final U5500DTTypePage typePage;

    private final U5500DTReplaceKeysPage replaceKeysPage;

    /**
     * Constructor.
     */
    public U5500DTWizard() {
        super("Sign Certificate");
        setWindowTitle(WIZARD_NAME);
        typePage = new U5500DTTypePage();
        replaceKeysPage = new U5500DTReplaceKeysPage();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        addPage(typePage);
        addPage(replaceKeysPage);
        super.addPages();

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {

        try {
            if (typePage.isU8500Certificate()) {
                U8500DTCertificate cert8500 = new U8500DTCertificate();
                if (typePage.isType3()) {

                    cert8500.setReserved(typePage.getReserved());
                }
                cert8500.setFlags(typePage.getFlags());
                cert = cert8500;
            } else {
                cert = new GenericDTCertificate();
            }

            cert.setSerialNumber(typePage.getSerialNumber());
            cert.setType(typePage.getType());
            cert.setLevel(typePage.getLevel());
            cert.setSecureDebug(typePage.getSecureDebug());
            cert.setApeDebug(typePage.getApeDebug());
            cert.setModemDebug(typePage.getModemDebug());
            cert.setPRCMUDebug(typePage.getPrcmuDebug());
            cert.setSTM(typePage.getSTM());
            cert.setReferenceValue(typePage.getReference());

            final Map<Integer, GenericISSWCustomerPartKey> keys = replaceKeysPage.getReplacementKeys();
            for (int i = 1; i < 12; i++) {
                if (keys.get(i) != null) {
                    cert.addKeyToReplace(keys.get(i), i);
                }
            }
            DataOutputStream dos = null;
            final File file = new File(replaceKeysPage.getOutputPath());
            try {
                dos = new DataOutputStream(new FileOutputStream(file));
                dos.write(cert.getData());
            } finally {
                if (dos != null) {
                    dos.close();
                }
            }

            super.performFinish();

        } catch (final Exception e) {
            MessageDialog.openError(getShell(), "Create D&T Certificate", "Failed to create D&T certificate. \n"
                + e.getMessage());
            return false;
        }

        return true;
    }

    /**
     * 
     *{@inheritDoc}
     */
    @Override
    public String getPayloadFilePath() {
        return replaceKeysPage.getOutputPath();
    }

    /**
     * 
     *{@inheritDoc}
     */
    @Override
    public GenericSoftwareType getPayloadType() {
        if (typePage.isType5()) {
            return GenericSoftwareType.FRAC;
        }
        return GenericSoftwareType.DNT;
    }

}
