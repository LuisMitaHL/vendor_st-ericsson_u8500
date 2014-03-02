package com.stericsson.sdk.signing.ui.wizards.isswcertificate;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map.Entry;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.decorators.ControlDecorator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.data.KeyData;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.HashTypeExtended;
import com.stericsson.sdk.signing.generic.SignatureType;

/**
 * @author xmicroh
 * 
 */
public class ISSWCertificateKeySelectionPage extends AbstractWizardPage {

    private Combo[] combos;

    /***/
    public static final int KEY_NOT_SELECTED = -1;

    private static final float HEADER_HEIGHT_MULTIPLIER = 1.5f;

    private static final IStatus MISSING_KEY_ERROR =
        ControlDecorator.getStatusMessage(IStatus.ERROR, "Key doesn't point to valid key file!");

    private static final IStatus MISSING_SW_KEY_ASSIGNMENT =
        ControlDecorator.getStatusMessage(IStatus.WARNING, "There's no assigned key to this software type!");

    /***/
    protected LinkedHashMap<GenericSoftwareType, Integer> values;

    /***/
    protected java.util.List<KeyData> keys;

    /**
     * Array holding software types, necessary to holding a right order
     */
    private GenericSoftwareType[] softwareTypes;

    private Button[][] buttons;

    private int nrHashTypes = 8;

    private Button[][] revocationTypes;

    private ScrolledComposite scrolledKeySelector;

    private Composite keySelector;

    private int rowHeight = 0;

    private int lastVisibleKey = 0;

    private List<ControlDecorator> keyDecorators;

    private List<ControlDecorator> labelDecorators;

    private Color evenRowColor;

    private Color oddRowColor;

    private Color highlightingColor;

    private Composite captionColumn;

    /**
     * 
     * @param pageTitle
     *            Page title needed for construction.
     */
    protected ISSWCertificateKeySelectionPage(String pageTitle) {
        super(pageTitle);
        setDefaultKeys();
        keys = new ArrayList<KeyData>();
        keys.add(new KeyData(null, null));
        keyDecorators = new ArrayList<ControlDecorator>();
        labelDecorators = new ArrayList<ControlDecorator>();
        combos = new Combo[values.size()];
    }

    /**
     * Sets default values to the keys
     */
    private void setDefaultKeys() {
        initializeValues();
        softwareTypes = getSoftwareTypes(values.keySet().toArray());
    }

    /**
     * Method initializing input values
     */
    public void initializeValues() {
        values = new LinkedHashMap<GenericSoftwareType, Integer>();
        values.put(GenericSoftwareType.TRUSTED, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.PRCMU, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.MEM_INIT, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.XLOADER, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.OSLOADER, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.APE_NORMAL, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.LDR, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.MODEM, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.FOTA, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.DNT, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.AUTH_CERT, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.IPL, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.FLASH_ARCHIVE, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.ITP, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.TVP_LICENSE, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.FRAC, KEY_NOT_SELECTED);
    }

    private GenericSoftwareType[] getSoftwareTypes(Object[] array) {
        GenericSoftwareType[] type = new GenericSoftwareType[array.length];
        for (int i = 0; i < array.length; i++) {
            type[i] = (GenericSoftwareType) array[i];
        }
        return type;
    }

    private void createColors(Composite parent) {
        highlightingColor = new Color(parent.getDisplay(), new RGB(242, 100, 100));
        oddRowColor = new Color(parent.getDisplay(), new RGB(242, 242, 242));
        evenRowColor = new Color(parent.getDisplay(), new RGB(229, 229, 229));
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite parent) {
        createColors(parent);
        scrolledKeySelector = new ScrolledComposite(parent, SWT.V_SCROLL | SWT.H_SCROLL);
        setControl(scrolledKeySelector);

        // create key selector composite
        createKeySelectorComposite(scrolledKeySelector);
        scrolledKeySelector.setContent(keySelector);

        scrolledKeySelector.setExpandVertical(true);
        scrolledKeySelector.setExpandHorizontal(true);
        scrolledKeySelector.setLayoutData(new GridData(GridData.FILL_BOTH));

        scrolledKeySelector.setMinSize(keySelector.computeSize(SWT.DEFAULT, SWT.DEFAULT));

        setDescription("Select combinations of software types and keys to be used for their signing. "
            + "Use buttons for adding/removing keys and click them to modify their properties.");
    }

    int getNumberOfKeys() {
        return lastVisibleKey;
    }

    /**
     * @return Types of selected keys.
     */
    public Combo[] getKeyTypes() {
        return combos;
    }

    private void updateKeySelectorLayout() {
        keySelector.layout();
        scrolledKeySelector.setMinSize(keySelector.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        scrolledKeySelector.layout();
    }

    private void createKeySelectorComposite(Composite parent) {
        buttons = new Button[softwareTypes.length][values.size()];
        revocationTypes = new Button[nrHashTypes][values.size()];

        keySelector = new Composite(parent, SWT.NONE);
        GridLayout selectorLayout = new GridLayout(values.size() + 1, false);
        selectorLayout.horizontalSpacing = 0;
        keySelector.setLayout(selectorLayout);

        createFirstColumn();
        createKeyColumns();

        for (Control colC : keySelector.getChildren()) {
            Composite colCComposite = (Composite) colC;
            for (int i = 1; i < colCComposite.getChildren().length; i++) {
                if ((i % 2) > 0) {
                    colCComposite.getChildren()[i].setBackground(oddRowColor);
                } else {
                    colCComposite.getChildren()[i].setBackground(evenRowColor);
                }
            }
        }
    }

    private void createFirstColumn() {
        captionColumn = new Composite(keySelector, SWT.NONE);
        captionColumn.setLayout(new GridLayout(1, false));

        Composite comp;

        comp = new Composite(captionColumn, SWT.NONE);
        comp.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
        comp.setLayout(new GridLayout(2, true));

        Button removeKeyBT = new Button(comp, SWT.PUSH);
        removeKeyBT.setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateKeySelectionPage_RemoveKey_Button");
        removeKeyBT.setText("Remove Key..");
        removeKeyBT.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        Button addKeyBT = new Button(comp, SWT.PUSH);
        addKeyBT.setText("Add Key..");
        addKeyBT.setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateKeySelectionPage_AddKey_Button");
        addKeyBT.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        addKeyBT.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent pE) {
                keys.add(new KeyData(null, null));
                showLastColumn();
                updateKeySelectorLayout();
            }
        });

        removeKeyBT.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent pE) {
                keys.remove(keys.size() - 1);
                removeSignedSWValues();
                uncheckButtons();
                uncheckRevocationTypes();
                hideLastColumn();
                updateKeySelectorLayout();
            }
        });

        Composite labelcomp = new Composite(captionColumn, SWT.NONE);
        GridData labelgd = new GridData(SWT.FILL, SWT.FILL, false, false);
        labelgd.horizontalIndent = 5;
        labelcomp.setLayoutData(labelgd);
        labelcomp.setLayout(new GridLayout(1, false));

        Label keyType = new Label(labelcomp, SWT.NONE);
        keyType.setText("Key type:");
        keyType.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, true));
        keyType.setBackground(oddRowColor);

        labelcomp = new Composite(captionColumn, SWT.NONE);
        labelgd = new GridData(SWT.FILL, SWT.FILL, false, false);
        labelgd.horizontalIndent = 5;
        labelcomp.setLayoutData(labelgd);
        labelcomp.setLayout(new GridLayout(1, false));

        Label revocationHash = new Label(labelcomp, SWT.NONE);
        revocationHash.setText("Revoke hash types (HASH):");
        revocationHash.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, true));
        revocationHash.setBackground(evenRowColor);

        labelcomp = new Composite(captionColumn, SWT.NONE);
        labelgd = new GridData(SWT.FILL, SWT.FILL, false, false);
        labelgd.horizontalIndent = 5;
        labelcomp.setLayoutData(labelgd);
        labelcomp.setLayout(new GridLayout(1, false));

        Label revocationHmac = new Label(labelcomp, SWT.NONE);
        revocationHmac.setText("Revoke hash types (HMAC):");
        revocationHmac.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, true));
        revocationHmac.setBackground(oddRowColor);

        GridData gd;
        for (int swIndex = 0; swIndex < softwareTypes.length; swIndex++) {
            comp = new Composite(captionColumn, SWT.NONE);
            gd = new GridData(SWT.FILL, SWT.FILL, false, false);
            gd.horizontalIndent = 5;
            comp.setLayoutData(gd);
            comp.setLayout(new GridLayout(1, false));

            Label l = new Label(comp, SWT.CENTER);
            l.setText(softwareTypes[swIndex].getName());
            l.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, true));
            if ((swIndex % 2) > 0) {
                l.setBackground(oddRowColor);
            } else {
                l.setBackground(evenRowColor);
            }

            ControlDecorator ld = new ControlDecorator(comp.getParent(), comp, SWT.LEFT);
            ld.showMessage(MISSING_SW_KEY_ASSIGNMENT);
            labelDecorators.add(ld);

            rowHeight = Math.max(rowHeight, comp.computeSize(SWT.DEFAULT, SWT.DEFAULT).y);
        }

        GridData layoutData = (GridData) captionColumn.getChildren()[0].getLayoutData();
        layoutData.heightHint = getHeaderHeight();
    }

    private void createKeyColumns() {
        for (int keyIndex = 0; keyIndex < values.size(); keyIndex++) {
            Composite columnC = new Composite(keySelector, SWT.NONE);
            columnC.setLayout(new GridLayout(1, false));

            Composite comp = new Composite(columnC, SWT.NONE);
            GridData gd = new GridData(SWT.FILL, SWT.FILL, false, false);
            gd.heightHint = getHeaderHeight();
            gd.horizontalIndent = 5;
            comp.setLayoutData(gd);
            comp.setLayout(new GridLayout(1, false));

            Button keyBT = new Button(comp, SWT.CENTER);
            keyBT.setText("Key" + (keyIndex + 1));
            keyBT.setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateKeySelectionPage_KeyButton" + (keyIndex + 1)
                + "_Button");
            keyBT.setLayoutData(new GridData(SWT.CENTER, GridData.CENTER, true, true));
            KeyButtonListener kbl = new KeyButtonListener(keyIndex);
            keyBT.addSelectionListener(kbl);
            keyBT.addMouseTrackListener(kbl);
            keyBT.setBackground(evenRowColor);
            keyBT.setToolTipText("Click to add/change path for the key..");

            Combo keyTypeCombo = new Combo(columnC, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
            keyTypeCombo.setItems(new String[] {
                SignatureType.RSASSA_PKCS_V1_5.name(), SignatureType.RSASSA_PPS.name()});
            keyTypeCombo.setText(SignatureType.RSASSA_PKCS_V1_5.name());
            combos[keyIndex] = keyTypeCombo;

            ControlDecorator cd = new ControlDecorator(comp.getParent(), comp, SWT.LEFT);
            cd.showMessage(MISSING_KEY_ERROR);
            keyDecorators.add(cd);

            comp = new Composite(columnC, SWT.NONE);
            gd = new GridData(SWT.FILL, SWT.FILL, false, false);
            gd.heightHint = rowHeight;
            comp.setLayoutData(gd);
            comp.setLayout(new GridLayout(4, false));

            for (int hashIndex = 0; hashIndex < nrHashTypes/2; hashIndex++) {
                Button bt = new Button(comp, SWT.CHECK);
                bt.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, true, true));
                bt.setData(HashTypeExtended.getByHashId(hashIndex+1).name());
                bt.setToolTipText(HashTypeExtended.getByHashId(hashIndex+1).name());
                bt.setText(HashTypeExtended.getByHashId(hashIndex+1).getHashDigest());
                revocationTypes[hashIndex][keyIndex] = bt;
                bt.setBackground(evenRowColor);
                bt.setEnabled(false);
            }

            comp = new Composite(columnC, SWT.NONE);
            gd = new GridData(SWT.FILL, SWT.FILL, false, false);
            gd.heightHint = rowHeight;
            comp.setLayoutData(gd);
            comp.setLayout(new GridLayout(4, false));

            for (int hashIndex = nrHashTypes/2; hashIndex < nrHashTypes; hashIndex++) {
                Button bt = new Button(comp, SWT.CHECK);
                bt.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, true, true));
                bt.setData(HashTypeExtended.getByHashId(hashIndex+1).name());
                bt.setToolTipText(HashTypeExtended.getByHashId(hashIndex+1).name());
                bt.setText(HashTypeExtended.getByHashId(hashIndex+1).getHashDigest());
                revocationTypes[hashIndex][keyIndex] = bt;
                bt.setBackground(oddRowColor);
                bt.setEnabled(false);
            }

            for (int swIndex = 0; swIndex < softwareTypes.length; swIndex++) {
                comp = new Composite(columnC, SWT.NONE);
                gd = new GridData(SWT.FILL, SWT.FILL, false, false);
                gd.heightHint = rowHeight;
                comp.setLayoutData(gd);
                comp.setLayout(new GridLayout(1, false));

                Button bt = new Button(comp, SWT.CHECK);
                bt.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, true, true));
                bt.setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateKeySelectionPage_KeyCheckBoxRow" + (swIndex + 1)
                    + "Column" + (keyIndex + 1) + "_Button");
                buttons[swIndex][keyIndex] = bt;
                bt.addListener(SWT.Selection, new ButtonSelectionListener(keyIndex, swIndex));
                bt.setEnabled(false);
            }

        }

        // Ommit the first caption column and leave one column to be initially visible
        lastVisibleKey = keySelector.getChildren().length - 1;
        for (int h = 0; h < values.size() - 1; h++) {
            hideLastColumn();
        }
    }

    private void showLastColumn() {
        // The first index is for the caption column
        if (lastVisibleKey < values.size()) {
            // The first child is the caption column, thus skipping the first element
            Composite column = (Composite) keySelector.getChildren()[lastVisibleKey + 1];
            column.setVisible(true);
            GridData layoutData = (GridData) column.getLayoutData();
            if (layoutData == null) {
                layoutData = new GridData();
                layoutData.exclude = false;
                column.setLayoutData(layoutData);
            } else {
                layoutData.exclude = false;
            }

            if (getWizard().getContainer().getCurrentPage() != null) {
                getWizard().getContainer().updateButtons();
            }
            lastVisibleKey++;
        }
    }

    private void hideLastColumn() {
        if (lastVisibleKey > 0) {
            lastVisibleKey--;

            // The first child is the caption column, thus skipping the first element
            Composite column = (Composite) keySelector.getChildren()[lastVisibleKey + 1];
            column.setVisible(false);

            GridData layoutData = (GridData) column.getLayoutData();
            if (layoutData == null) {
                layoutData = new GridData();
                layoutData.exclude = true;
                column.setLayoutData(layoutData);
            } else {
                layoutData.exclude = true;
            }

            if (getWizard().getContainer().getCurrentPage() != null) {
                getWizard().getContainer().updateButtons();
            }
        }
    }

    private int getHeaderHeight() {
        return (int) (rowHeight * HEADER_HEIGHT_MULTIPLIER);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        if (lastVisibleKey == 0) {
            return false;
        }
        for (int i = 0; i < lastVisibleKey; i++) {
            if (keyDecorators.get(i).isDecorated()) {
                return false;
            }
        }
        return true;
    }

    private final class ButtonSelectionListener implements Listener {

        private int keyIndex;

        private int swIndex;

        private ButtonSelectionListener(int pKeyIndex, int pSwIndex) {
            keyIndex = pKeyIndex;
            swIndex = pSwIndex;
        }

        public void handleEvent(Event pEvent) {
            if (!buttons[swIndex][keyIndex].getSelection()) {
                values.put(softwareTypes[swIndex], KEY_NOT_SELECTED);
                handleLabelDecorator();
                return;
            }

            for (int key = 0; key < values.size(); key++) {
                if (key == keyIndex) {
                    continue;
                }
                buttons[swIndex][key].setSelection(false);
                values.put(softwareTypes[swIndex], KEY_NOT_SELECTED);
            }
            values.put(softwareTypes[swIndex], keyIndex);
            handleLabelDecorator();
        }

        private void handleLabelDecorator() {
            for (int isSet : values.values()) {
                if (isSet != KEY_NOT_SELECTED) {
                    labelDecorators.get(swIndex).removeMessage();
                    return;
                }
            }
            labelDecorators.get(swIndex).showMessage(MISSING_SW_KEY_ASSIGNMENT);
        }
    }

    private final class KeyButtonListener extends MouseTrackAdapter implements SelectionListener {

        int keyIndex;

        boolean columnEnabled;

        private KeyButtonListener(int pIndex) {
            super();
            keyIndex = pIndex;
        }

        @Override
        public void mouseEnter(MouseEvent e) {
            // Ommit first keySelector column (caption column)
            Composite column = (Composite) keySelector.getChildren()[keyIndex + 1];
            for (int i = 4; i < column.getChildren().length; i++) {
                Control row = column.getChildren()[i];
                if (values.get(values.keySet().toArray()[i - 4]) == keyIndex) {
                    row.setBackground(highlightingColor);
                    captionColumn.getChildren()[i].setBackground(highlightingColor);
                } else if ((i % 2) > 0) {
                    row.setBackground(oddRowColor);
                    captionColumn.getChildren()[i].setBackground(oddRowColor);
                } else {
                    row.setBackground(evenRowColor);
                    captionColumn.getChildren()[i].setBackground(evenRowColor);
                }
            }
        }

        @Override
        public void mouseExit(MouseEvent e) {
            Composite column = (Composite) keySelector.getChildren()[keyIndex + 1];
            for (int i = 1; i < column.getChildren().length; i++) {
                Control row = column.getChildren()[i];
                if ((i % 2) > 0) {
                    row.setBackground(oddRowColor);
                    captionColumn.getChildren()[i].setBackground(oddRowColor);
                } else {
                    row.setBackground(evenRowColor);
                    captionColumn.getChildren()[i].setBackground(evenRowColor);
                }
            }
        }

        private void enableColumn() {
            if (columnEnabled) {
                return;
            }
            columnEnabled = true;
            for (int swIndex = 0; swIndex < softwareTypes.length; swIndex++) {
                buttons[swIndex][keyIndex].setEnabled(true);
            }

            for (int hashIndex = 0; hashIndex < nrHashTypes; hashIndex++) {
                revocationTypes[hashIndex][keyIndex].setEnabled(true);
            }

            keyDecorators.get(keyIndex).removeMessage();
            keyDecorators.get(keyIndex).update();
            getWizard().getContainer().updateButtons();
        }

        private void disableColumn() {
            if (!columnEnabled) {
                return;
            }
            columnEnabled = false;
            for (int swIndex = 0; swIndex < softwareTypes.length; swIndex++) {
                buttons[swIndex][keyIndex].setEnabled(false);
            }
            for (int hashIndex = 0; hashIndex < nrHashTypes; hashIndex++) {
                revocationTypes[hashIndex][keyIndex].setEnabled(false);
            }
            keyDecorators.get(keyIndex).showMessage(MISSING_KEY_ERROR);
            getWizard().getContainer().updateButtons();
        }

        public void widgetDefaultSelected(SelectionEvent e) {
            return;
        }

        public void widgetSelected(SelectionEvent e) {
            String keyPath =
                UITextFieldsFactory.openFileDialog(new Shell(), "Select path to the key..", false, null, new String[] {
                    "*.pem"}, true);
            if ((keyPath != null) && (new File(keyPath)).exists()) {
                KeyData replace = keys.get(keyIndex);
                replace.setPath(keyPath);
                keys.set(keyIndex, replace);
                enableColumn();
            } else if (!(new File(keys.get(keyIndex).getPath())).exists()) {
                disableColumn();
            }

            scrolledKeySelector.setFocus();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_issw_cert.html");
    }

    /**
     * @return values
     */
    public HashMap<GenericSoftwareType, Integer> getValues() {
        return values;
    }

    /**
     * @return only selected values by user
     */
    public HashMap<GenericSoftwareType, Integer> getUsedSoftwareTypes() {
        HashMap<GenericSoftwareType, Integer> used = new HashMap<GenericSoftwareType, Integer>();
        for (Entry<GenericSoftwareType, Integer> entry : values.entrySet()) {
            if (entry.getValue() != ISSWCertificateKeySelectionPage.KEY_NOT_SELECTED) {
                used.put(entry.getKey(), entry.getValue());
            }
        }
        return used;
    }

    /**
     * Next page
     * 
     * @return next page
     */
    @Override
    public IWizardPage getNextPage() {
        ((ISSWCertificateWizard) getWizard()).updateKeySelectionResults();
        ISSWCertificateSummaryPage summary = ((ISSWCertificateWizard) getWizard()).summaryPage;
        summary.updatePage();
        return summary;
    }

    /**
     * @return List of used keys, filling software type from combo-boxes and revo
     */
    public List<KeyData> getKeyData() {
        for (int i = 0; i < keys.size(); i++) {
            int hashes = 0;
            keys.get(i).setType(SignatureType.valueOf(combos[i].getText()));

//          collecting revocation hashes
            for (int hashIndex = 0; hashIndex < nrHashTypes; hashIndex++) {
                if (revocationTypes[hashIndex][i].getSelection()) {
                    hashes += (int)Math.pow(2, hashIndex);
                }
            }

//          Shift hashes to the upper byte (bit 8-15)
            keys.get(i).setHashes(hashes<<8);
        }
        return keys;
    }

    private void removeSignedSWValues() {
        for (java.util.Map.Entry<GenericSoftwareType, Integer> entry : values.entrySet()) {
            if (entry.getValue() == lastVisibleKey - 1) {
                values.put(entry.getKey(), KEY_NOT_SELECTED);
            }
        }
    }

    private void uncheckButtons() {
        for (int i = 0; i < buttons.length; i++) {
            buttons[i][keys.size()].setSelection(false);
        }
    }

    private void uncheckRevocationTypes() {
        for (int i = 0; i < nrHashTypes; i++) {
            revocationTypes[i][keys.size()].setSelection(false);
        }
    }

}
