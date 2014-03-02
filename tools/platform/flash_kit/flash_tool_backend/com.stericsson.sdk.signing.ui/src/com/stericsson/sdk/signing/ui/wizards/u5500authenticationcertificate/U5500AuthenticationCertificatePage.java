package com.stericsson.sdk.signing.ui.wizards.u5500authenticationcertificate;

import java.io.File;
import java.io.IOException;
import java.security.interfaces.RSAPublicKey;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.TimeZone;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DateTime;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.generic.HashTypeExtended;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;
import com.stericsson.sdk.signing.generic.cert.GenericAuthenticationCertificate;
import com.stericsson.sdk.signing.util.PEMParser;

/**
 * Main page for creating an Authentication Certificate for the U5500 platform
 * 
 * @author xolabju
 * 
 */
public class U5500AuthenticationCertificatePage extends AbstractWizardPage {

    private Composite container;

    private Text textSerialNumber;

    private DateTime dateTimeFrom;

    private DateTime dateTimeTo;

    private Combo comboAuthLevel;

    private Text textAuthKey;

    private Button buttonAuthKey;

    private Text textOutputFile;

    private Button buttonOutputFile;

    private Button buttonEnableValidity;

    private String outputFile;

    private Button[] revocationTypes;

    private int nrHashTypes = 8;

    /**
     * Constructor
     */
    public U5500AuthenticationCertificatePage() {
        super("Authentication Certificate");
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite parent) {
        container = new Composite(parent, SWT.NULL);
        GridLayout layout = new GridLayout(3, false);
        container.setLayout(layout);

        GridData gd = null;

        SelectionListener selectionListener = new SelectionListener() {
            public void widgetSelected(SelectionEvent e) {
                validate();
            }

            public void widgetDefaultSelected(SelectionEvent e) {
                validate();
            }
        };

        ModifyListener modifyListener = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                validate();
            }
        };

        // serial number
        Label labelSerialNumber = new Label(container, SWT.NULL);
        labelSerialNumber.setText("Serial Number:");
        textSerialNumber = new Text(container, SWT.BORDER | SWT.SINGLE);
        textSerialNumber.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_SerialNumber_Text");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 2;
        textSerialNumber.setLayoutData(gd);
        textSerialNumber.setText("0");
        textSerialNumber.addModifyListener(modifyListener);

        // validity
        Group groupValidity = new Group(container, SWT.SHADOW_ETCHED_OUT);
        groupValidity.setLayout(new GridLayout(2, false));
        groupValidity.setText("Validity");
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 3;
        groupValidity.setLayoutData(gd);

        // enable/disable validity
        buttonEnableValidity = new Button(groupValidity, SWT.CHECK);
        buttonEnableValidity.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_EnableValidity_Button");
        gd = new GridData();
        gd.horizontalSpan = 2;
        buttonEnableValidity.setLayoutData(gd);
        buttonEnableValidity.setText("Enabled");
        buttonEnableValidity.setSelection(false);
        buttonEnableValidity.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                boolean enabled = buttonEnableValidity.getSelection();
                dateTimeFrom.setEnabled(enabled);
                dateTimeTo.setEnabled(enabled);
                validate();
            }
        });

        // valid from
        Label labelValidFrom = new Label(groupValidity, SWT.NULL);
        labelValidFrom.setText("From:");
        dateTimeFrom = new DateTime(groupValidity, SWT.DATE);
        dateTimeFrom.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_TimeFrom_DateTime");
        dateTimeFrom.addSelectionListener(selectionListener);
        dateTimeFrom.setEnabled(false);
        // valid to
        Label labelValidTo = new Label(groupValidity, SWT.NULL);
        labelValidTo.setText("To:");
        dateTimeTo = new DateTime(groupValidity, SWT.DATE);
        dateTimeTo.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_TimeTo_DateTime");
        dateTimeTo.addSelectionListener(selectionListener);
        dateTimeTo.setEnabled(false);

        // auth level
        Label labelAuthLevel = new Label(container, SWT.NULL);
        labelAuthLevel.setText("Authentication Level:");
        comboAuthLevel = new Combo(container, SWT.READ_ONLY);
        comboAuthLevel.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_AuthLevel_Combo");
        comboAuthLevel.setItems(AuthenticationLevel.valuesToString());
        comboAuthLevel.select(0);
        gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 2;
        comboAuthLevel.setLayoutData(gd);

        // auth key
        Label labelAuthKey = new Label(container, SWT.NULL);
        labelAuthKey.setText("Authentication Key:");
        textAuthKey = new Text(container, SWT.BORDER | SWT.SINGLE);
        textAuthKey.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_AuthKey_Text");
        textAuthKey.addModifyListener(modifyListener);
        gd = new GridData(GridData.FILL_HORIZONTAL);
        textAuthKey.setLayoutData(gd);
        buttonAuthKey = new Button(container, SWT.PUSH);
        buttonAuthKey.setData("GD_COMP_NAME", "U5500AuthenticationCertificatePage_AuthKey_Button");
        buttonAuthKey.setText("Browse...");
        buttonAuthKey.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                String key = selectAuthKey();
                if (key != null && !key.trim().equals("")) {
                    textAuthKey.setText(key);
                }
            }
        });

        createRevocationHashButtons();

        // output file
        Label labelOutputFile = new Label(container, SWT.NULL);
        labelOutputFile.setText("Output File:");
        textOutputFile = new Text(container, SWT.BORDER | SWT.SINGLE);
        textOutputFile.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_OutputFile_Text");
        textOutputFile.addModifyListener(modifyListener);
        gd = new GridData(GridData.FILL_HORIZONTAL);
        textOutputFile.setLayoutData(gd);
        buttonOutputFile = new Button(container, SWT.PUSH);
        buttonOutputFile.setData(IUIConstants.GD_COMP_NAME, "U5500AuthenticationCertificatePage_OutputFile_Button");
        buttonOutputFile.setText("Browse...");
        buttonOutputFile.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                outputFile = selectOutputFile();
                if (outputFile != null && !outputFile.trim().equals("")) {
                    textOutputFile.setText(outputFile);
                }
            }

        });

        setControl(container);
        validate();

    }

    private void createRevocationHashButtons() {
        Label revocationLabel = new Label(container, SWT.NULL);
        revocationLabel.setText("Revocation hash types:");

        revocationTypes = new Button[nrHashTypes];

        Composite comp = new Composite(container, SWT.NONE);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, false, false);
        gd.horizontalSpan = 2;
        gd.heightHint = 24;
        comp.setLayoutData(gd);
        comp.setLayout(new GridLayout(8, false));
        for (int hashIndex = 0; hashIndex < nrHashTypes; hashIndex++) {
            Button bt = new Button(comp, SWT.CHECK);
            bt.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, true, true));
            bt.setData(HashTypeExtended.getByHashId(hashIndex+1).name());
            bt.setToolTipText(HashTypeExtended.getByHashId(hashIndex+1).name());
            bt.setText(HashTypeExtended.getByHashId(hashIndex+1).name().substring(3));
            revocationTypes[hashIndex] = bt;
        }
    }
    /**
     * 
     * @return the selected outputfile
     */
    String getOutputFile() {
        return textOutputFile.getText();
    }

    private void validate() {
        removeAllMessages();
        boolean isValid = true;

        // serial number
        isValid = isValid && validateSerialNumber();

        // validity
        isValid = isValid && validateValidity();

        // auth level
        isValid = isValid && validateAuthLevel();

        // auth key
        isValid = isValid && validateAuthKey();

        // output file
        isValid = isValid && validateOutputFile();

        setPageComplete(isValid);
    }

    private boolean validateOutputFile() {
        boolean isValid = true;
        String test = textOutputFile.getText();
        if (test != null && !test.trim().equals("")) {
            File file = new File(test);
            File parentFile = file.getParentFile();
            if ((parentFile != null && !parentFile.isDirectory()) || (file.isDirectory())) {
                isValid = false;
                addMessage(textOutputFile, "Invalid output file: " + test, MESSAGE_TYPE.ERROR);
            }
        } else {
            isValid = false;
            addMessage(textOutputFile, "Output file must be specified", MESSAGE_TYPE.INFO);
        }
        return isValid;
    }

    private boolean validateAuthKey() {
        boolean isValid = true;
        String test = textAuthKey.getText();
        if (test == null || test.trim().equals("")) {
            isValid = false;
            addMessage(textAuthKey, "Authentication key must be specified", MESSAGE_TYPE.INFO);
        } else {
            File keyFile = new File(test);
            if (!keyFile.exists() || !keyFile.isFile()) {
                isValid = false;
                addMessage(textAuthKey, "Cannot find key: " + test, MESSAGE_TYPE.ERROR);
            } else {
                try {
                    PEMParser.parseRSAPublicKey(keyFile.getAbsolutePath());
                } catch (IOException e) {
                    isValid = false;
                    addMessage(textAuthKey, "Invalid key file", MESSAGE_TYPE.ERROR);
                }
            }
        }
        return isValid;
    }

    private boolean validateAuthLevel() {
        boolean isValid = true;
        // String test = comboAuthLevel.getText();
        // if (test != null && !test.trim().equals("")) {
        // try {
        // int val = Integer.decode(test);
        // if (val < 0) {
        // throw new Exception();
        // }
        // } catch (Exception e) {
        // isValid = false;
        // addMessage(comboAuthLevel, "Invalid authentication level", MESSAGE_TYPE.ERROR);
        // }
        // } else {
        // isValid = false;
        // addMessage(comboAuthLevel, "Authentication level must be specified", MESSAGE_TYPE.INFO);
        // }
        return isValid;
    }

    private boolean validateValidity() {
        boolean isValid = true;
        if (buttonEnableValidity.getSelection()) {
            GregorianCalendar from =
                new GregorianCalendar(dateTimeFrom.getYear(), dateTimeFrom.getMonth(), dateTimeFrom.getDay());
            GregorianCalendar to =
                new GregorianCalendar(dateTimeTo.getYear(), dateTimeTo.getMonth(), dateTimeTo.getDay());
            if (to.getTimeInMillis() <= from.getTimeInMillis()) {
                isValid = false;
                addMessage(buttonEnableValidity, "Invalid validity period", MESSAGE_TYPE.ERROR);
            }
        }
        return isValid;
    }

    private boolean validateSerialNumber() {
        String test = textSerialNumber.getText();
        boolean isValid = true;
        if (test != null && !test.trim().equals("")) {
            try {
                int val = Integer.decode(test);
                if (val < 0) {
                    throw new Exception();
                }
            } catch (Exception e) {
                isValid = false;
                addMessage(textSerialNumber, "Invalid serial number", MESSAGE_TYPE.ERROR);
            }
        } else {
            isValid = false;
            addMessage(textSerialNumber, "Serial number must be specified", MESSAGE_TYPE.INFO);
        }
        return isValid;
    }

    private String selectAuthKey() {
        FileDialog dialog = new FileDialog(getShell(), SWT.OPEN);
        dialog.setText("Select Authentication Key");
        return dialog.open();
    }

    private String selectOutputFile() {
        FileDialog dialog = new FileDialog(getShell(), SWT.SAVE);
        dialog.setText("Select Output File");
        return dialog.open();
    }

    private int getRevocationHashTypes() {

        int hashes = 0;
//      collecting revocation hashes
        for (int hashIndex = 0; hashIndex < nrHashTypes; hashIndex++) {
            if (revocationTypes[hashIndex].getSelection()) {
                hashes += (int)Math.pow(2, hashIndex);
            }
        }

//      Shift hashes to the upper byte (bit 8-15)
        return hashes<<8;
    }

    /**
     * 
     * @return a certificate based on the input values
     * @throws IOException
     *             on errors
     */
    GenericAuthenticationCertificate getCertificate() throws IOException {
        GenericAuthenticationCertificate cert = new GenericAuthenticationCertificate();
        String serialNo = textSerialNumber.getText();
        if (serialNo == null || serialNo.trim().equals("")) {
            serialNo = "0";
        }
        cert.setSerialNumber(Integer.decode(serialNo));

        // validity
        if (buttonEnableValidity.getSelection()) {
            GregorianCalendar cal = new GregorianCalendar(TimeZone.getTimeZone("UTC"));
            cal.set(dateTimeFrom.getYear(), dateTimeFrom.getMonth(), dateTimeFrom.getDay());
            cal.set(Calendar.HOUR_OF_DAY, 0);
            cal.set(Calendar.MINUTE, 0);
            cal.set(Calendar.SECOND, 0);
            cal.set(Calendar.MILLISECOND, 0);
            cert.setNotBefore(cal.getTimeInMillis() / 1000);

            cal = new GregorianCalendar(TimeZone.getTimeZone("UTC"));
            cal.set(dateTimeTo.getYear(), dateTimeTo.getMonth(), dateTimeTo.getDay());
            cal.set(Calendar.HOUR_OF_DAY, 0);
            cal.set(Calendar.MINUTE, 0);
            cal.set(Calendar.SECOND, 0);
            cal.set(Calendar.MILLISECOND, 0);
            cert.setNotAfter(cal.getTimeInMillis() / 1000);
        } else {
            cert.setNotAfter(0);
            cert.setNotBefore(0);
        }

        cert.setAuthenticationLevel(AuthenticationLevel.getLevel(comboAuthLevel.getText()));

        RSAPublicKey key = PEMParser.parseRSAPublicKey(textAuthKey.getText());
        cert.setKeyTypeWithRevocationHashTypes(
                new SignatureTypeWithRevocationHashTypes(SignatureType.RSASSA_PKCS_V1_5, getRevocationHashTypes()));
        if (key == null) {
            throw new IOException("Failed to read authentication key");
        }

        byte[] modulusWithSignBit = key.getModulus().toByteArray();
        byte[] modulus = new byte[modulusWithSignBit.length - 1];
        System.arraycopy(modulusWithSignBit, 1, modulus, 0, modulus.length);
        int sizeOfDummy = 2;
        int sizeOfModSize = 2;
        int sizeOfExponent = 4;

        cert.setModulusSize((short) modulus.length);
        cert.setKeySize(modulus.length + sizeOfDummy + sizeOfModSize + sizeOfExponent);
        cert.setModulus(LittleEndianByteConverter.reverse(modulus));
        cert.setExponent(key.getPublicExponent().intValue());

        return cert;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_auth_cert.html");
    }

    enum AuthenticationLevel {
        SERVICE_MODE(0, "Service mode"),

        FLASH_MODE(1, "Flash mode"),

        ADVANCE_SERVICE_MODE(64, "Advance service mode"),

        LIMITED_SERVICE_MODE(65, "Limited service mode"),

        VERY_LIMITED_SERVICE_MODE(66, "Very limited service mode"),

        PRODUCTION_MODE(127, "Production mode");

        private int level;

        private String description;

        private AuthenticationLevel(int lvl, String descr) {
            level = lvl;
            description = descr;
        }

        @Override
        public String toString() {
            return description;
        }

        private int getLevel() {
            return level;
        }

        static String[] valuesToString() {
            ArrayList<String> strings = new ArrayList<String>();
            for (AuthenticationLevel lvl : AuthenticationLevel.values()) {
                strings.add(lvl.toString());
            }
            return strings.toArray(new String[strings.size()]);
        }

        static int getLevel(String name) {
            for (AuthenticationLevel lvl : AuthenticationLevel.values()) {
                if (lvl.toString().equals(name)) {
                    return lvl.getLevel();
                }
            }
            return -1;
        }

    }
}
