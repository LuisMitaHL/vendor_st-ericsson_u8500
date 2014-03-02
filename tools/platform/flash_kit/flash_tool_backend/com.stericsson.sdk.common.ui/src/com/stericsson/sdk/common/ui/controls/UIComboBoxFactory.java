/**
 * 
 */
package com.stericsson.sdk.common.ui.controls;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.FormToolkit;

/**
 * This factory serves for creating UIComboBox controls and setting the common L&F to it.
 * 
 * @author xhelciz
 * 
 */
public final class UIComboBoxFactory {
    private FormToolkit toolkit;

    private static UIComboBoxFactory instance;

    private UIComboBoxFactory(FormToolkit pToolkit) {
        toolkit = pToolkit;
    }

    /**
     * Will create factory which is capable to generate UI controls. Factory will use for UI control
     * creation given toolkit if it's set.
     * 
     * @param pToolkit
     *            Set toolkit or null depending on how you want to create UI controls.
     * @return Instance of this factory.
     */
    public static UIComboBoxFactory getInstance(FormToolkit pToolkit) {
        if (pToolkit != null) {
            return new UIComboBoxFactory(pToolkit);
        } else if (instance == null) {
            instance = new UIComboBoxFactory(null);
        }
        return instance;
    }

    /**
     * @param pParent
     *            parent composite
     * @param pStyle
     *            combo style (e.g. SWT.FLAT)
     * @param pName
     *            name or label to be displayed for this combo
     * @param pComboItems
     *            items to be displayed as items in this combo
     * @param pSpanColumns
     *            number of columns to be used by combo
     * @param pDefaultSelected
     *            index of the default selected item starting at 0
     * @return newly created UICombo
     */
    public UIComboBox createComboBox(Composite pParent, int pStyle, String pName, String[] pComboItems,
        int pSpanColumns, int pDefaultSelected) {
        return createCombo(pParent, pStyle, pName, pComboItems, pSpanColumns, pDefaultSelected);
    }

    /**
     * @param pParent
     *            parent composite
     * @param pStyle
     *            combo style (e.g. SWT.FLAT)
     * @param pName
     *            name or label to be displayed for this combo
     * @param pComboItems
     *            items to be displayed as items in this combo
     * @param pColumns
     *            count of columns for this control (a composite with specified columns is created
     *            and combo is placed inside it)
     * @param pSpan
     *            number of columns to be used by combo
     * @param pDefaultSelected
     *            index of the default selected item starting at 0
     * @return newly created UICombo
     */
    public UIComboBox createComboBox(Composite pParent, int pStyle, String pName, String[] pComboItems, int pColumns,
        int pSpan, int pDefaultSelected) {
        Composite c = createNewComposite(pParent, pColumns);
        UIComboBox ret = createCombo(c, pStyle, pName, pComboItems, pSpan, pDefaultSelected);
        ret.setParent(c);
        return ret;
    }

    private Composite createNewComposite(Composite pParent, int pColumns) {
        Composite c = null;
        if (toolkit != null) {
            c = toolkit.createComposite(pParent);
        } else {
            c = new Composite(pParent, SWT.NONE);
        }
        c.setLayout(new GridLayout(pColumns, false));
        return c;
    }

    private UIComboBox createCombo(Composite pParent, int pStyle, String pName, String[] pComboItems, int pSpanColumns,
        int pDefaultSelected) {
        Label comboLabel = null;
        if (toolkit != null) {
            comboLabel = toolkit.createLabel(pParent, pName + ":");
        } else {
            comboLabel = new Label(pParent, SWT.NONE);
            comboLabel.setText(pName + ":");
        }

        comboLabel.setLayoutData(new GridData(GridData.BEGINNING, GridData.CENTER, false, false));

        Combo combo = new Combo(pParent, pStyle);
        combo.setItems(pComboItems);
        combo.select(pDefaultSelected);

        GridData gd = new GridData(GridData.FILL, GridData.CENTER, false, false);
        gd.horizontalIndent = UIControl.HORIZONTAL_INDENT;
        gd.widthHint = UIControl.COMP_MIN_SIZE;
        combo.setLayoutData(gd);

        if (pSpanColumns > 0) {// will add placeholder
            gd = new GridData(SWT.FILL, SWT.FILL, true, false);
            gd.horizontalSpan = pSpanColumns;
            Label placeHolder = null;
            if (toolkit != null) {
                placeHolder = toolkit.createLabel(pParent, "");
            } else {
                placeHolder = new Label(pParent, SWT.NONE);
            }
            placeHolder.setLayoutData(gd);
            return new UIComboBox(comboLabel, combo, pComboItems, placeHolder);
        } else {
            return new UIComboBox(comboLabel, combo, pComboItems, null);
        }
    }
}
