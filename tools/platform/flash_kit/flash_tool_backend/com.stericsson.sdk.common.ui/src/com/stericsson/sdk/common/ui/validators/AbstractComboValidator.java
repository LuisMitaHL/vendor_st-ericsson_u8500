package com.stericsson.sdk.common.ui.validators;

import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.Combo;

/**
 * Convenient class to extend when new IValidator for the combo field should be implemented. 
 * 
 * @author xmicroh
 *
 */
public abstract class AbstractComboValidator extends AbstractValidator implements ModifyListener {

    private boolean registered;

    private Combo combo;

    private String valueName;

    /**
     * @param pValueName Name of the text field. Colon at the end of name will be stripped. 
     * @param pValidatedObject Combo field itself.
     */
    public AbstractComboValidator(String pValueName, Combo pValidatedObject) {
        super(pValidatedObject);
        if (pValueName.trim().endsWith(":")) {
            valueName = pValueName.trim();
            valueName = valueName.substring(0, valueName.length() - 1);
        } else {
            valueName = pValueName.trim();
        }
        combo = pValidatedObject;
        registered = false;
    }

    /**
     * @return Name of the combo field.
     */
    public String getValueName() {
        return valueName;
    }

    /**
     * {@inheritDoc}
     */
    public final void switchOn() {
        if (!registered) {
            combo.addModifyListener(this);
            registered = true;
        }
    }

    /**
     * {@inheritDoc}
     */
    public final void switchOff() {
        if (registered) {
            combo.removeModifyListener(this);
            registered = false;
        }
    }

    /**
     * {@inheritDoc}
     */
    public final void modifyText(ModifyEvent e) {
        checkValue(combo.getText());
    }

    /**
     * {@inheritDoc}
     */
    public final void checkCurrentValue() {
        checkValue(combo.getText());
    }

    /**
     * @param str String for validation.
     */
    public abstract void checkValue(String str);

}
