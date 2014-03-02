package com.stericsson.sdk.signing.ui.wizards.u5500dt;

import java.nio.ByteBuffer;
import java.util.SortedSet;
import java.util.TreeSet;

import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.window.Window;
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
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;

/**
 * @author xdancho
 * 
 */
public class U5500DTTypePage extends AbstractWizardPage implements Listener {

    private static final String PAGE_NAME = "D&&T Type Settings";

    private static final int BYTE_SIZE_CHIP_ID = 20;

   /*
    * Enable easy manipulation flags
    * (have to add/edit FLAG_TOOLTIPS and add the usage to the help document)
    * 
    */
    private static final int[] FLAGS = {4,3,2,1};
    private static final String[] FLAG_TOOLTIPS = {"Enable Development RPMB Authentication Key",
                                                      "Add keys instead of replacement",
                                                    "Permanent Authentication if Magic Word Hash is correct",
                                                   "Permanent Authentication"};

    private Table referenceTable;

    private Button addReference;

    private Button removeReference;

    private static final String DEVELOPMENT = "Development";

    private static final String PROD_CHIPSET = "Production chipset id match";

    private static final String PROD_BATCH = "Production batch id match";

    private static final String OPEN_MARKET = "Open market production device";

    private static final String CUSTOMER_VARIANT = "Customer variant";

    private static final String FRAC = "Field Return Authentication Certificate";

    private static final String ENABLED = "Enabled";

    private static final String DISABLED = "Disabled";

    private static final String LEVEL_NO_HIDDEN = "No hidden sections";

    private static final String LEVEL_HIDDEN = "Default OTP hidden sections";

    private Text serialText;

    private Text flagsText;

    private Combo typeCombo;

    private Combo levelCombo;

    private Combo secureCombo;

    private Combo apeCombo;

    private Combo modemCombo;

    private Combo prcmuCombo;

    private Combo stmCombo;

    private Group capacityGroup;

    private final SortedSet<String> references = new TreeSet<String>();

    private Label flagsLabel;

    private Label authenticateFlagsLabel;

    private Text reservedText;

    private Label reservedLabel;

    private Button[] authenticateFlagsButton;

    private Button newDntArchitectureTypeButton;

    private Label newDntArchitectureTypeLabel;
    /**
     * Constructor
     */
    protected U5500DTTypePage() {
        super(PAGE_NAME);
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite parent) {

        final Composite top = new Composite(parent, SWT.NONE);
        top.setLayout(new GridLayout(3, false));
        top.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        setControl(top);
        // SERIAL
        final Label serialLabel = new Label(top, SWT.NONE);
        final GridData serialLabelData = new GridData(SWT.NONE, SWT.NONE, false, false);
        serialLabel.setText("Serial number (Hex):");
        serialLabelData.grabExcessHorizontalSpace = false;
        serialLabel.setLayoutData(serialLabelData);

        ModifyListener listener = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                setPageComplete(isPageComplete());
            }
        };

        ModifyListener listenerFlags = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                checkFlagsToSet();
                setPageComplete(isPageComplete());
            }
        };

        final GridData serialTextData = new GridData(SWT.FILL, SWT.NONE, true, false);
        serialTextData.horizontalSpan = 2;
        serialText = new Text(top, SWT.BORDER);
        serialText.setLayoutData(serialTextData);
        serialText.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Serial_Text");
        serialText.addModifyListener(listener);

        // TYPE
        final Label typeLabel = new Label(top, SWT.NONE);
        final GridData typeLabelData = new GridData(SWT.NONE, SWT.NONE, false, false);
        typeLabel.setText("Type:");
        typeLabelData.horizontalSpan = 1;

        typeLabel.setLayoutData(typeLabelData);

        typeCombo = new Combo(top, SWT.READ_ONLY);
        typeCombo.setItems(new String[] {
            DEVELOPMENT, PROD_CHIPSET, PROD_BATCH, OPEN_MARKET, CUSTOMER_VARIANT, FRAC});

        final GridData typeComboData = new GridData(SWT.FILL, SWT.NONE, true, false);
        typeComboData.horizontalSpan = 2;
        typeCombo.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Type_Combo");
        typeCombo.setLayoutData(typeComboData);
        typeCombo.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent e) {
                if (typeCombo.getText().equals(OPEN_MARKET)) {
                    type3Selected();
                } else if (typeCombo.getText().equals(CUSTOMER_VARIANT)) {
                    type4Selected();
                } else {
                    type0or1or2Selected();
                }
                setPageComplete(isPageComplete());
            }

            public void widgetDefaultSelected(SelectionEvent e) {
            }
        });
     // FORCE NEW DnT Architecture
        createNewDntArchitectureSelection(top);
      // CAPACITY GROUP
        createCapacityGroup(top);

        authenticateFlagsLabel = new Label(top, SWT.NONE);
        authenticateFlagsLabel.setText("Enable Flags:");

        Composite comp = new Composite(top, SWT.NONE);
        GridData gd = new GridData(SWT.FILL, SWT.NONE, true, false);
        gd.horizontalSpan = 2;
        gd.heightHint = 24;
        comp.setLayoutData(gd);

        comp.setLayout(new GridLayout(8, true));
        authenticateFlagsButton = new Button[FLAGS.length];
        for (int i = 0; i < FLAGS.length; i++) {
            Button bt = new Button(comp, SWT.CHECK);
            bt.setLayoutData(new GridData(SWT.RIGHT, SWT.RIGHT, true, true));
            bt.setText("flag " + FLAGS[i]+ " ("+HexUtilities.toHexString((int)Math.pow(2,FLAGS[i]-1))+")");
            bt.setToolTipText(FLAG_TOOLTIPS[i]);
            authenticateFlagsButton[i] = bt;
            authenticateFlagsButton[i].addListener(SWT.Selection,
                new SetFlagsButtonSelectionListener(i));
        }

        flagsLabel = new Label(top, SWT.NONE);
        flagsLabel.setText("Flags (Hex):");

        flagsText = new Text(top, SWT.BORDER);
        flagsText.setLayoutData(serialTextData);
        flagsText.addModifyListener(listenerFlags);
        flagsEnabled(true);

        reservedLabel = new Label(top, SWT.NONE);
        reservedLabel.setText("Reserved (Hex):");
        reservedText = new Text(top, SWT.BORDER);
        reservedText.setLayoutData(serialTextData);
        reservedText.addModifyListener(listener);
        reservedEnabled(false);

        // REFERENCE TABLE

        Group referenceGroup = new Group(top, SWT.SHADOW_IN);
        referenceGroup.setText("Reference:");
        referenceGroup.setLayout(new GridLayout(2, false));
        final GridData groupData = new GridData(SWT.FILL, SWT.FILL, true, true);
        groupData.horizontalSpan = 3;
        referenceGroup.setLayoutData(groupData);

        referenceTable = new Table(referenceGroup, SWT.SINGLE | SWT.BORDER | SWT.FULL_SELECTION);
        referenceTable.addListener(SWT.Selection, this);

        final GridData tableData = new GridData(SWT.FILL, SWT.NONE, true, true);
        referenceTable.setLayoutData(tableData);

        // BUTTON GROUP

        final Composite groupComponent = new Composite(referenceGroup, SWT.NULL);
        groupComponent.setLayout(new GridLayout(1, false));

        final GridData buttonData = new GridData(SWT.FILL, SWT.NONE, false, false);
        buttonData.horizontalSpan = 1;

        addReference = new Button(groupComponent, SWT.PUSH);
        addReference.setText("Add");
        addReference.addListener(SWT.Selection, this);
        addReference.setLayoutData(buttonData);

        removeReference = new Button(groupComponent, SWT.PUSH);
        removeReference.setText("Remove");
        removeReference.addListener(SWT.Selection, this);
        removeReference.setLayoutData(buttonData);

        typeCombo.select(0);
        levelCombo.select(0);
        secureCombo.select(0);
        apeCombo.select(0);
        modemCombo.select(0);
        prcmuCombo.select(0);
        stmCombo.select(0);

        setPageComplete(isPageComplete());

    }

    private void type4Selected() {
        reservedEnabled(false);
        flagsEnabled(true);
        enableNewDntArchitectureTypeSelection(false);
    }

    private void type3Selected() {
        flagsEnabled(true);
        enableNewDntArchitectureTypeSelection(false);
        reservedEnabled(true);
    }

    private void type0or1or2Selected() {

        flagsEnabled(true);
        enableNewDntArchitectureTypeSelection(true);
        reservedEnabled(false);
    }

    private void reservedEnabled(boolean enabled) {
        reservedLabel.setEnabled(enabled);
        reservedText.setEnabled(enabled);
        if (!enabled) {
            reservedText.setText("");
        }
    }

    private void flagsEnabled(boolean enabled) {
        authenticateFlagsLabel.setEnabled(enabled);
        flagsLabel.setEnabled(enabled);
        flagsText.setEnabled(enabled);
        for (int i=0; i < FLAGS.length; i++) {
            authenticateFlagsButton[i].setEnabled(enabled);
            if (!enabled) {
                flagsText.setText("");
                authenticateFlagsButton[i].setSelection(false);
            }
        }
    }

    private void enableNewDntArchitectureTypeSelection(boolean enabled) {
        newDntArchitectureTypeButton.setSelection(true);
        newDntArchitectureTypeButton.setEnabled(enabled);
        newDntArchitectureTypeLabel.setEnabled(enabled);
    }

    private void createNewDntArchitectureSelection(Composite top) {

        newDntArchitectureTypeLabel = new Label(top, SWT.NONE);
        final GridData newArchitectureLabelData = new GridData(SWT.NONE, SWT.NONE, false, false);
        newDntArchitectureTypeLabel.setText("New format D&&T:");
        newArchitectureLabelData.horizontalSpan = 1;

        newDntArchitectureTypeLabel.setLayoutData(newArchitectureLabelData);
        newDntArchitectureTypeButton = new Button(top, SWT.CHECK);
        newDntArchitectureTypeButton.setSelection(true);
        final GridData newDntArchitectureButtonData = new GridData(SWT.LEFT, SWT.NONE, false, false);
        newDntArchitectureButtonData.horizontalSpan = 2;
        newDntArchitectureTypeButton.setLayoutData(newDntArchitectureButtonData);
        newDntArchitectureTypeButton.addListener(SWT.Selection,
                new NewDntArchitectureTypeButtonSelectionListener());
    }

    private void createCapacityGroup(Composite top) {
        capacityGroup = new Group(top, SWT.SHADOW_IN);
        capacityGroup.setText("Capacity");
        capacityGroup.setLayout(new GridLayout(3, false));
        final GridData groupData = new GridData(SWT.FILL, SWT.FILL, true, true);
        groupData.horizontalSpan = 3;
        capacityGroup.setLayoutData(groupData);
        capacityGroup.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Capacity_Group");

        // GRIDDATA
        final GridData labelData = new GridData(SWT.NONE, SWT.NONE, false, false);
        labelData.horizontalSpan = 1;
        labelData.widthHint = 100;

        final GridData comboData = new GridData(SWT.FILL, SWT.NONE, true, false);
        comboData.horizontalSpan = 2;

        // LEVEL COMBO
        final Label levelLabel = new Label(capacityGroup, SWT.NONE);
        levelLabel.setText("Level:");
        levelLabel.setLayoutData(labelData);

        levelCombo = new Combo(capacityGroup, SWT.READ_ONLY);
        levelCombo.setItems(new String[] {
            LEVEL_NO_HIDDEN, LEVEL_HIDDEN});
        levelCombo.setLayoutData(comboData);
        levelCombo.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Level_Combo");

        // SECURE WORLD COMBO

        final Label secureLabel = new Label(capacityGroup, SWT.NONE);
        secureLabel.setText("Secure world debug:");
        secureLabel.setLayoutData(labelData);

        secureCombo = new Combo(capacityGroup, SWT.READ_ONLY);
        secureCombo.setItems(new String[] {
            DISABLED, ENABLED});
        secureCombo.setLayoutData(comboData);
        secureCombo.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Secure_Combo");

        // APE

        final Label apeLabel = new Label(capacityGroup, SWT.NONE);
        apeLabel.setText("APE debug:");
        apeLabel.setLayoutData(labelData);

        apeCombo = new Combo(capacityGroup, SWT.READ_ONLY);
        apeCombo.setItems(new String[] {
            DISABLED, ENABLED});
        apeCombo.setLayoutData(comboData);
        apeCombo.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Ape_Combo");

        // MODEM

        final Label modemLabel = new Label(capacityGroup, SWT.NONE);
        modemLabel.setText("Modem debug:");
        modemLabel.setLayoutData(labelData);

        modemCombo = new Combo(capacityGroup, SWT.READ_ONLY);
        modemCombo.setItems(new String[] {
            DISABLED, ENABLED});
        modemCombo.setLayoutData(comboData);
        modemCombo.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Modem_Combo");

        // PRCMU

        final Label prcmuLabel = new Label(capacityGroup, SWT.NONE);
        prcmuLabel.setText("PRCMU debug:");
        prcmuLabel.setLayoutData(labelData);

        prcmuCombo = new Combo(capacityGroup, SWT.READ_ONLY);
        prcmuCombo.setItems(new String[] {
            DISABLED, ENABLED});
        prcmuCombo.setLayoutData(comboData);
        modemCombo.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Prcmu_Combo");

        // STM

        final Label stmLabel = new Label(capacityGroup, SWT.NONE);
        stmLabel.setText("STM:");
        stmLabel.setLayoutData(labelData);

        stmCombo = new Combo(capacityGroup, SWT.READ_ONLY);
        stmCombo.setItems(new String[] {
            "Trace for APE and modem disabled", "Trace for APE disabled, modem enabled",
            "Trace for APE enabled, modem disabled", "Trace for APE enabled, modem enabled"});
        stmCombo.setLayoutData(comboData);
        stmCombo.setData(IUIConstants.GD_COMP_NAME, "U5500DTTypePage_Stm_Combo");
    }

    /**
     * Get APE debug value.
     * 
     * @return APE debug value.
     */
    protected byte getApeDebug() {
        return apeCombo.getItem(apeCombo.getSelectionIndex()).equals(DISABLED) ? (byte) 0 : (byte) 0xFF;
    }

    /**
     * Get level value.
     * 
     * @return Level value.
     */
    protected byte getLevel() {
        return levelCombo.getItem(levelCombo.getSelectionIndex()).equals(LEVEL_HIDDEN) ? (byte) 1 : (byte) 0;
    }

    /**
     * Get modem debug value.
     * 
     * @return Modem debug value.
     */
    protected byte getModemDebug() {
        return modemCombo.getItem(modemCombo.getSelectionIndex()).equals(DISABLED) ? (byte) 0 : (byte) 0xFF;
    }

    /**
     * Get PRCMU debug value.
     * 
     * @return PRCMU debug value.
     */
    protected byte getPrcmuDebug() {
        return prcmuCombo.getItem(prcmuCombo.getSelectionIndex()).equals(DISABLED) ? (byte) 0 : (byte) 0xFF;
    }

    /**
     * Get STM value.
     * 
     * @return PRCMU debug value.
     */
    protected byte getSTM() {
        String tmp = stmCombo.getItem(stmCombo.getSelectionIndex());
        if (tmp.equalsIgnoreCase("Trace for APE and modem disabled")) {
            return (byte) 0x00;
        } else if (tmp.equalsIgnoreCase("Trace for APE disabled, modem enabled")) {
            return (byte) 0x01;
        } else if (tmp.equalsIgnoreCase("Trace for APE enabled, modem disabled")) {
            return (byte) 0x10;
        } else {
            return (byte) 0x11;
        }
    }

    /**
     * Get the references.
     * 
     * @return The references as a byte array.
     * @throws Exception
     *             If references cannot be parsed.
     */
    protected byte[] getReference() throws Exception {
        final ByteBuffer bb = ByteBuffer.allocate(references.size() * BYTE_SIZE_CHIP_ID);

        try {
            for (final String ref : references) {
                bb.put(HexUtilities.toByteArray(ref, BYTE_SIZE_CHIP_ID));
            }
        } catch (final Exception e) {
            throw new Exception("The references could not be parsed");
        }
        return bb.array();
    }

    /**
     * Get secure debug value.
     * 
     * @return Secure debug value.
     */
    protected byte getSecureDebug() {
        return secureCombo.getItem(secureCombo.getSelectionIndex()).equals(DISABLED) ? (byte) 0 : (byte) 0xFF;
    }

    /**
     * Get serial number.
     * 
     * @return The serial number as int.
     */
    protected int getSerialNumber() {

        if (!HexUtilities.hasHexPrefix(serialText.getText())) {
            final long test = Long.parseLong(serialText.getText(), 16);
            return (int) test & 0xFFFFFFFF;
        } else {
            return (int) Long.parseLong(serialText.getText().substring(2, serialText.getText().length()), 16) & 0xFFFFFFFF;
        }
    }

    /**
     * Get flags.
     * 
     * @return The flags as int.
     */
    protected int getFlags() {

        if (!HexUtilities.hasHexPrefix(flagsText.getText().trim())) {
            final long test = Long.parseLong(flagsText.getText(), 16);
            return (int) test & 0xFFFFFFFF;
        } else {
            return (int) Long.parseLong(flagsText.getText().substring(2), 16) & 0xFFFFFFFF;
        }
    }

    /**
     * Get reserved.
     * 
     * @return The reserved as byte array.
     */
    protected byte[] getReserved() {
        String text;
        if (!HexUtilities.hasHexPrefix(reservedText.getText().trim())) {
            text = reservedText.getText();
        } else {
            text = reservedText.getText().substring(2);
        }
        return HexUtilities.toByteArray(text);
    }

    /**
     * Get the type.
     * 
     * @return The type as an int.
     * @throws Exception
     *             If type cannot be found.
     */
    protected int getType() throws Exception {

        if (typeCombo.getText().equals(DEVELOPMENT)) {
            return 0;
        } else if (typeCombo.getText().equals(PROD_CHIPSET)) {
            return 1;
        } else if (typeCombo.getText().equals(PROD_BATCH)) {
            return 2;
        } else if (typeCombo.getText().equals(OPEN_MARKET)) {
            return 3;
        } else if (typeCombo.getText().equals(CUSTOMER_VARIANT)) {
            return 4;
        } else if (typeCombo.getText().equals(FRAC)) {
            return 5;
        } else {
            throw new Exception("The Type of the D&T certificate is not valid");
        }
    }

    /**
     *{@inheritDoc}
     * 
     * @see org.eclipse.swt.widgets.Listener#handleEvent(org.eclipse.swt.widgets.Event)
     */
    public void handleEvent(Event event) {
        if (event.widget.equals(removeReference)) {
            final TableItem[] items = referenceTable.getSelection();
            if (items.length > 0) {
                references.remove(items[0].getText());
                items[0].dispose(); // Table is single selection
            }
        } else if (event.widget.equals(addReference)) {
            final InputDialog dlg =
                new InputDialog(Display.getCurrent().getActiveShell(), "", "Enter chipset or batch id in Hex ("
                    + BYTE_SIZE_CHIP_ID + " Bytes)", "", new ReferenceInputValidator());
            if (dlg.open() == Window.OK) {
                if (references.add(dlg.getValue())) {
                    final TableItem ti = new TableItem(referenceTable, SWT.NONE);
                    ti.setText(dlg.getValue());
                    referenceTable.showItem(ti);
                }
            }
        }
    }

//    private void reflectAuthButton() {
//        authenticateFlagsButton.setSelection(flagsText.getText().equalsIgnoreCase(AUTH_FLAGS));
//    }


    private void checkFlagsToSet() {
        final String textNoPrefix;
        final long test;
        // If flagsText has no hexPrefix
        if (!HexUtilities.hasHexPrefix(flagsText.getText().trim())) {
            textNoPrefix = flagsText.getText().trim();
        } else {
            textNoPrefix = flagsText.getText().trim().substring(2);
        }
        // If textNoPrefix is not ""
        if (!"".equalsIgnoreCase(textNoPrefix)) {
            test = Long.parseLong(textNoPrefix, 16);

            for (int i = 0; i<FLAGS.length; i++) {
                authenticateFlagsButton[i].setSelection(
                    ((int)test & (int)(Math.pow(2, FLAGS[i]-1)))/Math.pow(2, FLAGS[i]-1) == 1);
            }
        } else {
            for (int i = 0; i<FLAGS.length; i++) {
                authenticateFlagsButton[i].setSelection(false);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        if (validateSerialNumber() && validateFlags() && validateReserved()) {
            return true;
        }
        return false;
    }

    private boolean validateReserved() {
        if (!typeCombo.getText().equals(OPEN_MARKET)) {
            return true;
        }
        final String text = reservedText.getText();
        String input = "";
        int len;

        removeAllMessages();

        if (HexUtilities.isHexDigit(text)) {
            if (HexUtilities.hasHexPrefix(text)) {
                input = HexUtilities.removeHexPrefix(text);
            } else {
                input = text;
            }

        } else {
            addMessage(reservedText, "The input must be a Hex value", MESSAGE_TYPE.ERROR);
            return false;
        }
        len = input.length();

        if (len == 0) {
            addMessage(reservedText, "The \"Reserved\" field must be specified", MESSAGE_TYPE.INFO);
            return false;
        }

        if (len != 64) {
            addMessage(reservedText, "The \"Reserved\" field must be 32 bytes long", MESSAGE_TYPE.ERROR);
            return false;
        }

        return true;
    }

    private boolean validateFlags() {
        if (!newDntArchitectureTypeButton.getSelection()) {
            return true;
        }
        final String text = flagsText.getText();
        String input = "";
        int len;

        removeAllMessages();

        if (HexUtilities.isHexDigit(text)) {
            if (HexUtilities.hasHexPrefix(text)) {
                input = HexUtilities.removeHexPrefix(text);
            } else {
                input = text;
            }

        } else {
            addMessage(flagsText, "The input must be a Hex value", MESSAGE_TYPE.ERROR);
            return false;
        }
        len = input.length();

        if (len == 0) {
            addMessage(flagsText, "The \"Flags\" field must be specified", MESSAGE_TYPE.INFO);
            return false;
        }

        if (len > 8) {
            addMessage(flagsText, "The \"Flags\" field cannot be longer than 4 bytes", MESSAGE_TYPE.ERROR);
            return false;
        }

        return true;
    }

    private boolean validateSerialNumber() {
        final String text = serialText.getText();
        String input = "";
        int len;

        removeAllMessages();

        if (HexUtilities.isHexDigit(text)) {
            if (HexUtilities.hasHexPrefix(text)) {
                input = HexUtilities.removeHexPrefix(text);
            } else {
                input = text;
            }

        } else {
            addMessage(serialText, "The input must be a Hex value", MESSAGE_TYPE.ERROR);
            return false;
        }
        len = input.length();

        if (len == 0) {
            addMessage(serialText, "Serial number must be specified", MESSAGE_TYPE.INFO);
            return false;
        }

        if (len > 8) {
            addMessage(serialText, "The serial number cannot be larger than 4 Bytes", MESSAGE_TYPE.ERROR);
            return false;
        }

        return true;
    }

    /**
     * This class validates a String. It makes sure that the String is between 5 and 8 characters.
     */
    class ReferenceInputValidator implements IInputValidator {
        /**
         * Validates the String.
         * 
         * @param newText
         *            The String to validate.
         * @return String Is null for no error, or an error message.
         */
        public String isValid(String text) {

            String input = "";
            int len;

            if (HexUtilities.isHexDigit(text)) {
                if (HexUtilities.hasHexPrefix(text)) {
                    input = HexUtilities.removeHexPrefix(text);
                } else {
                    input = text;
                }

            } else {
                return "The input must be a Hex value";
            }
            len = input.length();

            if (len > BYTE_SIZE_CHIP_ID * 2) {
                return "The ID cannot be larger than " + (BYTE_SIZE_CHIP_ID * 2) + " characters";
            }
            return null;
        }
    }

    private final class NewDntArchitectureTypeButtonSelectionListener implements Listener {

        private NewDntArchitectureTypeButtonSelectionListener() {
        }

        public void handleEvent(Event event) {

            flagsEnabled(newDntArchitectureTypeButton.getSelection());
            setPageComplete(isPageComplete());
        }

    }

    private final class SetFlagsButtonSelectionListener implements Listener {

        private int index;
        private SetFlagsButtonSelectionListener(int pIndex) {
            index = pIndex;
        }

        public void handleEvent(Event event) {
            final String textNoPrefix;
            long test = 0;

            // If flagsText has no hexPrefix
            if (!HexUtilities.hasHexPrefix(flagsText.getText().trim())) {
                textNoPrefix = flagsText.getText().trim();
            } else {
                textNoPrefix = flagsText.getText().trim().substring(2);
            }
            // If textNoPrefix is not ""
            if (!"".equalsIgnoreCase(textNoPrefix)) {
                test = Long.parseLong(textNoPrefix, 16);
            }

            if (authenticateFlagsButton[index].getSelection()) {
                    flagsText.setText(HexUtilities.toHexString(test | (int)Math.pow(2, FLAGS[index]-1)));
            } else {
                int reverse = (int)Math.pow(2, FLAGS[index]-1) ^ 0xFFFFFFFF;
//              clear text if no button selected
                if ((test & reverse) == 0) {
                    flagsText.setText("");
                } else {
                    flagsText.setText(HexUtilities.toHexString(test & reverse));
              }
            }
        }

    }

    /**
     *{@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_dnt_cert.html");
    }

    /**
     * @return true if type 3 or type 4 is selected or is forced new type
     */
    public boolean isU8500Certificate() {
        return (typeCombo.getText().equals(OPEN_MARKET) 
                || typeCombo.getText().equals(CUSTOMER_VARIANT) 
                || newDntArchitectureTypeButton.getSelection());
    }

    /**
     * @return true if type 3 is selected
     */
    public boolean isType3() {
        return typeCombo.getText().equals(OPEN_MARKET);
    }

    /**
     * @return true if type 4 is selected
     */
    public boolean isType4() {
        return typeCombo.getText().equals(CUSTOMER_VARIANT);
    }

    /**
     * @return true if type 5 is selected
     */
    public boolean isType5() {
        return typeCombo.getText().equals(FRAC);
    }

    /**
     * @return true if override replace with add flag is selected
     */
    public boolean isOverideReplaceWithAddSelected() {
        if (authenticateFlagsButton.length > 2) {
            return authenticateFlagsButton[2].getSelection();
        }
        return false;
    }

}
