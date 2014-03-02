package com.stericsson.sdk.common.ui.controls;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.common.ui.IUIConstants;

/**
 * This class extends InputDialog to be able to set data parameters to the individual components of
 * the Dialog. The UIInputDialog is used with the UIListBoxFactory to allow the list to be populated
 * via a input dialog.
 * 
 * @author xdancho
 * 
 */
public class UIInputDialog extends InputDialog {

    /**
     * Extends InputDialog with Strings assigned for the components.
     * 
     * @param parentShell
     *            Parent shell.
     * @param dialogTitle
     *            Dialog title.
     * @param dialogMessage
     *            Dialog message.
     * @param initialValue
     *            initial value of the Text.
     * @param validator
     *            Validator to use.
     */
    public UIInputDialog(Shell parentShell, String dialogTitle, String dialogMessage, String initialValue,
        IInputValidator validator) {
        super(parentShell, dialogTitle, dialogMessage, initialValue, validator);
        super.create();
        getButton(IDialogConstants.OK_ID).setData(IUIConstants.GD_COMP_NAME,
            dialogTitle.replace(" ", "_") + "_OK_Button");
        getButton(IDialogConstants.CANCEL_ID).setData(IUIConstants.GD_COMP_NAME,
            dialogTitle.replace(" ", "_") + "_Cancel_Button");
        getText().setData(IUIConstants.GD_COMP_NAME, dialogTitle.replace(" ", "_") + "_Text");
    }

}
