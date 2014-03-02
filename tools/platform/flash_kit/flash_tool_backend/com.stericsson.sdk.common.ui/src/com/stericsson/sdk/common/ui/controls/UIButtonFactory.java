/**
 * 
 */
package com.stericsson.sdk.common.ui.controls;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.FormToolkit;

/**
 * This factory serves for creating UIButton controls and setting the common L&F to it.
 * 
 * @author xhelciz
 * 
 */
public final class UIButtonFactory {

    private FormToolkit toolkit;

    private static UIButtonFactory instance;

    private UIButtonFactory(FormToolkit pToolkit) {
        toolkit = pToolkit;
    }

    /**
     * Will create factory which is capable to generate UI controls. Factory will use given toolkit
     * if it is set.
     * 
     * @param pToolkit
     *            Set toolkit or null depending on how you want to create UI controls (form toolkit
     *            is used if the control should be placed into the form).
     * @return Instance of this factory.
     */
    public static UIButtonFactory getInstance(FormToolkit pToolkit) {
        if (pToolkit != null) {
            return new UIButtonFactory(pToolkit);
        } else if (instance == null) {
            instance = new UIButtonFactory(null);
        }
        return instance;
    }

    /**
     * @param pParent
     *            parent composite
     * @param pStyle
     *            button style (e.g. SWT.CHECK)
     * @param pName
     *            name or label to be displayed on button
     * @param pSpanColumns
     *            number of columns to be used by button
     * @param pDefaultSelected
     *            true if the button should be selected at start
     * @return newly created UIButton
     */
    public UIButton createButton(Composite pParent, int pStyle, String pName, int pSpanColumns, boolean pDefaultSelected) {
        return createBtn(pParent, pStyle, pName, pSpanColumns, pDefaultSelected);
    }

    /**
     * @param pParent
     *            parent composite
     * @param pStyle
     *            button style (e.g. SWT.CHECK)
     * @param pName
     *            name or label to be displayed on button
     * @param pColumns
     *            count of columns for this control (a composite with specified columns is created
     *            and button is placed inside it)
     * @param pSpan
     *            number of columns to be used by button
     * @param pDefaultSelected
     *            true if the button should be selected at start
     * @return newly created UIButton
     */
    public UIButton createButton(Composite pParent, int pStyle, String pName, int pColumns, int pSpan,
        boolean pDefaultSelected) {
        Composite c = createNewComposite(pParent, pColumns);
        UIButton ret = createBtn(c, pStyle, pName, pSpan, pDefaultSelected);
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

    private UIButton createBtn(Composite pParent, int pStyle, String pName, int pSpanColumns, boolean pDefaultSelected) {

        if ((pStyle & SWT.CHECK) != 0) {
            if (toolkit != null) {
                toolkit.createLabel(pParent, "");
            } else {
                new Label(pParent, SWT.NONE);
            }
        }
        Button button = null;
        if (toolkit != null) {
            button = toolkit.createButton(pParent, "", pStyle);
        } else {
            button = new Button(pParent, pStyle);
        }
        GridData gd = new GridData(SWT.BEGINNING, SWT.CENTER, false, false);
        gd.horizontalIndent = UIControl.HORIZONTAL_INDENT;
        // gd.widthHint = UIControl.COMP_MIN_SIZE;
        button.setLayoutData(gd);

        button.setText(pName);
        button.setSelection(pDefaultSelected);

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
            return new UIButton(button, placeHolder);
        } else {
            return new UIButton(button, null);
        }

    }
}
