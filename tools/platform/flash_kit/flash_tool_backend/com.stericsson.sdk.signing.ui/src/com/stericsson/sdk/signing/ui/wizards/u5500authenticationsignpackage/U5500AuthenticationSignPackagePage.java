package com.stericsson.sdk.signing.ui.wizards.u5500authenticationsignpackage;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.generic.cert.GenericAuthenticationCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.ui.wizards.u5500authenticationcertificate.U5500AuthenticationCertificateWizard;
import com.stericsson.sdk.signing.util.SignPackageWriter;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500AuthenticationSignPackagePage extends AbstractWizardPage {

    private Composite container;

    private Text textAlias;

    private Text textRevision;

    private Text textDescription;

    private Text textAuthCert;

    private Button buttonBrowseCert;

    private Button buttonCreateCert;

    private Text textOutputFile;

    private Button buttonOutputFile;

    /**
     * Constructor
     */
    protected U5500AuthenticationSignPackagePage() {
        super("Authentication Sign Package");
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite parent) {
        container = new Composite(parent, SWT.NULL);
        GridLayout layout = new GridLayout(4, false);
        container.setLayout(layout);

        GridData gd = null;

        ModifyListener modifyListener = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                validate();
            }
        };

        // alias
        Label labelAlias = new Label(container, SWT.NULL);
        labelAlias.setText("Alias:");
        textAlias = new Text(container, SWT.BORDER | SWT.SINGLE);
        textAlias.setData("GD_COMP_NAME", "U5500AuthenticationSignPackagePage_Alias_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 3;
        textAlias.setLayoutData(gd);
        textAlias.setText("");
        textAlias.addModifyListener(modifyListener);

        // revision
        Label labelRevision = new Label(container, SWT.NULL);
        labelRevision.setText("Revision:");
        textRevision = new Text(container, SWT.BORDER | SWT.SINGLE);
        textRevision.setData("GD_COMP_NAME", "U5500AuthenticationSignPackagePage_Revision_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 3;
        textRevision.setLayoutData(gd);
        textRevision.setText("");
        textRevision.addModifyListener(modifyListener);

        // description
        Label labelDescription = new Label(container, SWT.NULL);
        labelDescription.setText("Description:");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 4;
        textDescription = new Text(container, SWT.BORDER | SWT.MULTI | SWT.WRAP | SWT.V_SCROLL);
        textDescription.setData("GD_COMP_NAME", "U5500AuthenticationSignPackagePage_Description_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 4;
        gd.heightHint = 100;
        textDescription.setLayoutData(gd);
        textDescription.setText("");
        textDescription.addModifyListener(modifyListener);

        // auth cert
        Label labelAuthCert = new Label(container, SWT.NULL);
        labelAuthCert.setText("Authentication Certificate:");
        textAuthCert = new Text(container, SWT.BORDER | SWT.SINGLE);
        textAuthCert.setData("GD_COMP_NAME", "U5500AuthenticationSignPackagePage_AuthCert_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        // gd.horizontalSpan = 2;
        textAuthCert.setLayoutData(gd);
        textAuthCert.setText("");
        textAuthCert.addModifyListener(modifyListener);

        buttonCreateCert = new Button(container, SWT.PUSH);
        buttonCreateCert.setText("Create...");
        buttonCreateCert.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent pE) {
                String cert = U5500AuthenticationCertificateWizard.openWizard();
                if (cert != null && !cert.trim().equals("")) {
                    textAuthCert.setText(cert);
                }
            }
        });

        buttonBrowseCert = new Button(container, SWT.PUSH);
        buttonBrowseCert.setData("GD_COMP_NAME", "U5500AuthenticationSignPackagePage_AuthCert_Button");
        buttonBrowseCert.setText("Browse...");
        buttonBrowseCert.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                String cert = selectAuthCert();
                if (cert != null && !cert.trim().equals("")) {
                    textAuthCert.setText(cert);
                }
            }
        });

        // output file
        Label labelOutputFile = new Label(container, SWT.NULL);
        labelOutputFile.setText("Output File:");
        textOutputFile = new Text(container, SWT.BORDER | SWT.SINGLE);
        textOutputFile.setData("GD_COMP_NAME", "U5500AuthenticationSignPackagePage_OutputFile_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 2;
        textOutputFile.setLayoutData(gd);
        textOutputFile.setText("");
        textOutputFile.addModifyListener(modifyListener);
        buttonOutputFile = new Button(container, SWT.PUSH);
        buttonOutputFile.setData("GD_COMP_NAME", "U5500AuthenticationSignPackagePage_OutputFile_Button");
        buttonOutputFile.setText("Browse...");
        buttonOutputFile.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                String outputFile = selectOutputFile();
                if (outputFile != null && !outputFile.trim().equals("")) {
                    textOutputFile.setText(outputFile);
                }
            }
        });

        setControl(container);
        validate();
    }

    private String selectAuthCert() {
        FileDialog dialog = new FileDialog(getShell(), SWT.OPEN);
        dialog.setText("Select Authentication Certificate");
        return dialog.open();
    }

    private String selectOutputFile() {
        FileDialog dialog = new FileDialog(getShell(), SWT.SAVE);
        dialog.setText("Select Output File");
        return dialog.open();
    }

    private void validate() {
        removeAllMessages();
        boolean isValid = true;

        // alias
        isValid = validateAlias() && isValid;

        // revision
        isValid = validateRevision() && isValid;

        // description
        isValid = validateDescription() && isValid;

        // auth cert
        isValid = validateAuthCert() && isValid;

        // output file
        isValid = validateOutputFile() && isValid;

        setPageComplete(isValid);

    }

    private boolean validateOutputFile() {
        boolean isValid = true;
        String test = textOutputFile.getText();
        if (test != null && !test.trim().equals("")) {
            File file = new File(test);
            if (file.isDirectory()) {
                isValid = false;
                addMessage(textOutputFile, "Invalid output file: " + test, MESSAGE_TYPE.ERROR);
            }
        } else {
            isValid = false;
            addMessage(textOutputFile, "Output file must be specified", MESSAGE_TYPE.INFO);
        }
        return isValid;
    }

    private boolean validateAuthCert() {
        boolean isValid = true;
        String text = textAuthCert.getText();
        if (text == null || text.trim().equals("")) {
            isValid = false;
            addMessage(textAuthCert, "Authentication certificate must be specified", MESSAGE_TYPE.INFO);
        } else {
            File certFile = new File(text);
            if (!certFile.exists() || !certFile.isFile()) {
                isValid = false;
                addMessage(textAuthCert, "Cannot find certificate: " + text, MESSAGE_TYPE.ERROR);
            } else {
                try {
                    byte[] bytes = readBytes(certFile);
                    if (bytes == null) {
                        throw new IOException();
                    }
                    GenericAuthenticationCertificate cert = new GenericAuthenticationCertificate();
                    cert.setData(bytes);
                    if (cert.getMagic() != GenericCertificate.MAGIC_AUTHENTICATION_CERTIFICATE) {
                        throw new Exception();
                    }
                    if (cert.getSignatureSize() == 0) {
                        addMessage(textAuthCert, "The specified certificate is unsigned", MESSAGE_TYPE.ERROR);
                        isValid = false;
                    }
                } catch (Exception e) {
                    isValid = false;
                    addMessage(textAuthCert, "Invalid certificate file", MESSAGE_TYPE.ERROR);
                }
            }
        }
        return isValid;
    }

    private byte[] readBytes(File file) throws IOException {
        byte[] bytes = null;
        if (file == null) {
            throw new IOException("File is null");
        }
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(file);
            bytes = new byte[(int) file.length()];
            int read = fis.read(bytes);
            if (read < 0) {
                throw new IOException("Failed to read from " + file.getAbsolutePath());
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

    private boolean validateAlias() {
        boolean isValid = true;
        String text = textAlias.getText();
        if (text == null || text.trim().equals("")) {
            addMessage(textAlias, "Alias must be specified", MESSAGE_TYPE.INFO);
            isValid = false;
        }
        return isValid;
    }

    private boolean validateRevision() {
        boolean isValid = true;
        String text = textRevision.getText();
        if (text == null || text.trim().equals("")) {
            addMessage(textRevision, "Revision must be specified", MESSAGE_TYPE.INFO);
            isValid = false;
        }
        return isValid;
    }

    private boolean validateDescription() {
        boolean isValid = true;
        String text = textDescription.getText();
        if (text == null || text.trim().equals("")) {
            addMessage(textDescription, "It is recommended to add a description", MESSAGE_TYPE.WARNING);
        }
        return isValid;
    }

    void savePackage() throws IOException {
        SignPackageWriter.writeSignPackage(textOutputFile.getText(), textAlias.getText(),
            ISignPackage.PACKAGE_TYPE_U5500_AUTH, null, textRevision.getText(), textDescription.getText(), textAuthCert
                .getText());
    }

    String getOutputFile() {
        return textOutputFile.getText();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_auth_pkg.html");
    }
}
