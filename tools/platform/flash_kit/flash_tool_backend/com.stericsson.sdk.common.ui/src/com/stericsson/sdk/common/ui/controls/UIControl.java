/**
 * 
 */
package com.stericsson.sdk.common.ui.controls;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;

/**
 * Generic class for UI controls. Specific UI Control (e.g. Button, TextField) classes should derive
 * from this class and implement functionality of provided methods.
 * 
 * Generally, UIControls should ease the development process, especially works with layout (provide
 * common L&F). Also the code should become more readable with no need to code the same control and
 * its layout twice. This is visible especially when it is needed to create file text field with
 * browse button.
 * 
 * When a specific UIControl is picked up, it is possible the change the common L&F if necessary
 * (layoutdata, borders, etc).
 * 
 * @author xhelciz
 * 
 */
public abstract class UIControl {
    /** minimum width of a component */
    protected static final int COMP_MIN_SIZE = 130;

    /** horizontal indent of a component */
    protected static final int HORIZONTAL_INDENT = 5;

    /** parent of this UI control */
    protected Composite parentComposite;

    /** placeholder for managing L&F */
    protected Control uiPlaceholder;

    /**
     * @return Actual UI control object.
     */
    public abstract Control getUIControl();

    /**
     * Set layout data of this UI control object
     * 
     * @param pLayoutData
     *            layout data to be set
     */
    public abstract void setLayoutData(Object pLayoutData);

    /**
     * Set label or name of this UI control. This label will be displayed in GUI
     * 
     * @param pLabel
     *            label or name of this UI control
     */
    public abstract void setControlLabel(Label pLabel);

    /**
     * @return Label or name of this UI control. This label will be displayed in GUI.
     */
    public abstract Label getControlLabel();

    /**
     * Set parent composite
     * 
     * @param pParent
     *            composite
     */
    void setParent(Composite pParent) {
        this.parentComposite = pParent;
    }

    /**
     * @return Parent composite or null.
     */
    public Composite getParent() {
        return parentComposite;
    }

    void setPlaceholder(Control pPlaceholder) {
        this.uiPlaceholder = pPlaceholder;
    }

    /**
     * @return Place-holder or null.
     */
    public Control getPlaceholder() {
        return uiPlaceholder;
    }

}
