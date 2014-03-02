package com.stericsson.sdk.signing.ui.wizards.crkcsignpackage;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
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
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericRootKeyCertificate;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.util.SignPackageWriter;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500CRKCSignPackagePage extends AbstractWizardPage {

    private Composite container;

    private Text textAlias;

    private Text textRevision;

    private Text textDescription;

    private Text textCustomerRootKey;

    private Button buttonBrowseCustomerRootKey;

    private Text textOutputFile;

    private Button buttonOutputFile;

    /**
     * Constructor
     */
    protected U5500CRKCSignPackagePage() {
        super("Customer Root Key Certificate Sign Package");
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
        textAlias.setData("GD_COMP_NAME", "U5500CRKCSignPackagePage_Alias_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 3;
        textAlias.setLayoutData(gd);
        textAlias.setText("");
        textAlias.addModifyListener(modifyListener);

        // revision
        Label labelRevision = new Label(container, SWT.NULL);
        labelRevision.setText("Revision:");
        textRevision = new Text(container, SWT.BORDER | SWT.SINGLE);
        textRevision.setData("GD_COMP_NAME", "U5500CRKCSignPackagePage_Revision_Text");
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
        textDescription.setData("GD_COMP_NAME", "U5500CRKCSignPackagePage_Description_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 4;
        gd.heightHint = 100;
        textDescription.setLayoutData(gd);
        textDescription.setText("");
        textDescription.addModifyListener(modifyListener);

        // crk
        Label labelCRK = new Label(container, SWT.NULL);
        labelCRK.setText("Root Key Certificate:");
        textCustomerRootKey = new Text(container, SWT.BORDER | SWT.SINGLE);
        textCustomerRootKey.setData("GD_COMP_NAME", "U5500CRKCSignPackagePage_CRK_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 2;
        textCustomerRootKey.setLayoutData(gd);
        textCustomerRootKey.setText("");
        textCustomerRootKey.addModifyListener(modifyListener);

        buttonBrowseCustomerRootKey = new Button(container, SWT.PUSH);
        buttonBrowseCustomerRootKey.setData("GD_COMP_NAME", "U5500CRKCSignPackagePage_CRK_Button");
        buttonBrowseCustomerRootKey.setText("Browse...");
        buttonBrowseCustomerRootKey.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                String key = selectCRK();
                if (key != null && !key.trim().equals("")) {
                    textCustomerRootKey.setText(key);
                }
            }
        });

        // output file
        Label labelOutputFile = new Label(container, SWT.NULL);
        labelOutputFile.setText("Output File:");
        textOutputFile = new Text(container, SWT.BORDER | SWT.SINGLE);
        textOutputFile.setData("GD_COMP_NAME", "U5500CRKCSignPackagePage_OutputFile_Text");
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

    private String selectCRK() {
        FileDialog dialog = new FileDialog(getShell(), SWT.OPEN);
        dialog.setText("Select Root Key Certificate");
        return dialog.open();
    }

    private String selectOutputFile() {
        FileDialog dialog = new FileDialog(getShell(), SWT.SAVE);
        dialog.setText("Select Output File");
        dialog.setFilterExtensions(new String[]{"*.pkg"});
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

        // CRK cert
        isValid = validateCRK() && isValid;

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

    private boolean validateCRK() {
        boolean isValid = true;
        String text = textCustomerRootKey.getText();
        if (text == null || text.trim().equals("")) {
            isValid = false;
            addMessage(textCustomerRootKey, "Root Key Certificate must be specified", MESSAGE_TYPE.INFO);
        } else {
            File keyFile = new File(text);
            if (!keyFile.exists() || !keyFile.isFile()) {
                isValid = false;
                addMessage(textCustomerRootKey, "Cannot find customer root key: " + text, MESSAGE_TYPE.ERROR);
            } else {
                try {
                    ByteBuffer certificateBuffer = readRootKeyCertificateContent(textCustomerRootKey.getText());
                    GenericRootKeyCertificate certificate = new GenericRootKeyCertificate();
                    if (certificate == null) {
                        throw new IOException();
                    }
                    certificate.setData(certificateBuffer.array());
                } catch (Exception e) {
                    isValid = false;
                    addMessage(textCustomerRootKey, "Invalid Root Key Certificate file", MESSAGE_TYPE.ERROR);
                }
            }
        }
        return isValid;
    }

    private ByteBuffer readRootKeyCertificateContent(String path) throws IOException {
        FileChannel channel = null;
        FileInputStream fis = null;
        ByteBuffer certificateBuffer = null;
        try {
            fis = new FileInputStream(path);
            channel = fis.getChannel();
            certificateBuffer = ByteBuffer.allocate((int) channel.size());
            channel.read(certificateBuffer);
            certificateBuffer.rewind();
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return certificateBuffer;
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

    void savePackage() throws Exception {

        ByteBuffer rootKeyCertBuffer = readRootKeyCertificateContent(textCustomerRootKey.getText());
        GenericRootKeyCertificate rootKeyCert = new GenericRootKeyCertificate();
        rootKeyCert.setData(rootKeyCertBuffer.array());

        GenericCustomerRootKeyCertificate custRootKeyCert =
            new GenericCustomerRootKeyCertificate();
        SignatureType signatureType = SignatureType.getByValue(rootKeyCert.getSKSignatureType());
        if (signatureType == null) {
            throw new Exception("Unsupported signature type in Root Key Certificate");
        }
        custRootKeyCert.setCustomerRootKeySignatureType(signatureType);

        HashType hashType = HashType.getByValue(rootKeyCert.getSKSignatureHashType());
        if (hashType == null) {
            throw new Exception("Unsupported signature hash type in Root Key Certificate");
        }
        custRootKeyCert.setCustomerRootKeySignatureHashType(hashType);

        custRootKeyCert.setRootKeyCertificateData(rootKeyCert.getData());

        SignPackageWriter.writeSignPackage(textOutputFile.getText(), textAlias.getText(),
            ISignPackage.PACKAGE_TYPE_U5500_CRK, null, textRevision.getText(), textDescription.getText(),
            custRootKeyCert.getData());
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
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_crkc_pkg.html");
    }
}
