/**
 * 
 */
package com.stericsson.sdk.common.ui.controls;

import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;

/**
 * UI Control specific class for UIComboBox. Keeps Combo, its items and information about which item
 * is selected. Also contains information about layout data set to this composite of controls (SWT
 * Combo and Label)
 * 
 * @author xhelciz
 * 
 */
public class UIComboBox extends UIControl {

    private Combo uiComboBox;

    private String[] uiComboItems;

    /** label or name of this UI control. */
    protected Label uiLabel;

    /**
     * @param pLabel
     *            Label which describes logical name of the combo box.
     * @param pCombo
     *            Actual combo UI control.
     * @param pComboItems
     *            items to be displayed in combo box
     * @param pPlaceholder
     *            Place-holder if was used, null otherwise.
     * 
     */
    UIComboBox(Label pLabel, Combo pCombo, String[] pComboItems, Control pPlaceholder) {
        this.uiLabel = pLabel;
        this.uiComboBox = pCombo;
        this.uiComboItems = pComboItems;
        this.uiPlaceholder = pPlaceholder;
    }

    /**
     * @return Actual Combo box object.
     */
    @Override
    public Combo getUIControl() {
        return uiComboBox;
    }

    /**
     * @return combo items
     */
    public String[] getComboItems() {
        return uiComboItems;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setControlLabel(Label pLabel) {
        uiLabel = pLabel;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Label getControlLabel() {
        return uiLabel;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setLayoutData(Object pLayoutData) {
        uiComboBox.setLayoutData(pLayoutData);
    }
}
