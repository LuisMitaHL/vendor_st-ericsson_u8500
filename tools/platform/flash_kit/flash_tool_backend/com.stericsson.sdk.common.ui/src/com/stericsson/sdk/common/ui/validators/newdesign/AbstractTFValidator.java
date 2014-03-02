package com.stericsson.sdk.common.ui.validators.newdesign;

import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.Text;

/**
 * Convenient class to extend when new IValidator for the text field should be implemented.
 * 
 * @author xmicroh
 * 
 */
public abstract class AbstractTFValidator extends AbstractValidator implements ModifyListener {

    private boolean registered;

    private final Text tf;

    private String valueName;

    // /**
    // * @param pValueName Name of the text field. Colon at the end of name will be stripped.
    // * @param pValidatedObject Text field itself.
    // */
    // public AbstractTFValidator(String pValueName, Text pValidatedObject) {
    // super(pValidatedObject);
    // if (pValueName.trim().endsWith(":")) {
    // valueName = pValueName.trim();
    // valueName = valueName.substring(0, valueName.length() - 1);
    // } else {
    // valueName = pValueName.trim();
    // }
    // tf = pValidatedObject;
    // registered = false;
    // }

    /**
     * @param severity
     *            Validation severity. See {@link IValidator#setSeverity(int)}
     * @param pValueName
     *            Name of the text field. Colon at the end of name will be stripped.
     * @param pValidatedObject
     *            Text field itself.
     */
    public AbstractTFValidator(int severity, String pValueName, Text pValidatedObject) {
        super(severity, pValidatedObject);
        if (pValueName.trim().endsWith(":")) {
            valueName = pValueName.trim();
            valueName = valueName.substring(0, valueName.length() - 1);
        } else {
            valueName = pValueName.trim();
        }
        tf = pValidatedObject;
        registered = false;
    }

    /**
     * @return Name of the text field.
     */
    public String getValueName() {
        return valueName;
    }

    /**
     * {@inheritDoc}
     */
    public final void switchOn() {
        if (!registered) {
            tf.addModifyListener(this);
            registered = true;
        }
    }

    /**
     * {@inheritDoc}
     */
    public final void switchOff() {
        if (registered) {
            tf.removeModifyListener(this);
            registered = false;
        }
    }

    /**
     * {@inheritDoc}
     */
    public final void modifyText(ModifyEvent e) {
        checkValue(tf.getText());
    }

    /**
     * {@inheritDoc}
     */
    public final void checkCurrentValue() {
        checkValue(tf.getText());
    }

    /**
     * @param str
     *            String for validation.
     */
    public abstract void checkValue(String str);
}
