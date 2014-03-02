/**
 * 
 */
package com.stericsson.sdk.common.ui.controls;

import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;

/**
 * UI control specific class for UIButton. Keeps Button control and its layout data.
 * 
 * @author xhelciz
 * 
 */
public class UIButton extends UIControl {

    private Button uiButton;

    /**
     * @param label
     *            Label which describes logical name of the button.
     * @param combo
     *            Actual button UI control.
     * @param pPlaceholder
     *            Placeholder control if it is needed for correct layout of Button control in the
     *            design, null otherwise.
     * 
     */
    UIButton(Button pButton, Control pPlaceholder) {
        this.uiButton = pButton;
        this.uiPlaceholder = pPlaceholder;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Button getUIControl() {
        return uiButton;
    }

    /**
     * returns null for Button
     * 
     * @return null
     */
    @Override
    public Label getControlLabel() {
        return null;
    }

    /**
     * @param pLabel
     *            null does nothing for Button
     */
    @Override
    public void setControlLabel(Label pLabel) {
        // does nothing
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setLayoutData(Object pLayoutData) {
        uiButton.setLayoutData(pLayoutData);
    }

}
