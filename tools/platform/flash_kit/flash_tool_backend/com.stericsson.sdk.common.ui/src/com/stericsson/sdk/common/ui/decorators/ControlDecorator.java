package com.stericsson.sdk.common.ui.decorators;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.FieldDecoration;
import org.eclipse.jface.fieldassist.FieldDecorationRegistry;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * @author xmicroh
 * 
 */
public class ControlDecorator {

    private static FieldDecoration standardError =
        FieldDecorationRegistry.getDefault().getFieldDecoration(FieldDecorationRegistry.DEC_ERROR);

    private static FieldDecoration standardWarning =
        FieldDecorationRegistry.getDefault().getFieldDecoration(FieldDecorationRegistry.DEC_WARNING);

    private static FieldDecoration standardInformation =
        FieldDecorationRegistry.getDefault().getFieldDecoration(FieldDecorationRegistry.DEC_INFORMATION);

    private ControlDecoration decoration;

    private IStatus message;

    /**
     * @param pParent
     *            For more info see
     *            {@link ControlDecoration#ControlDecoration(Control, int, Composite)}.
     * @param control
     *            For more info see
     *            {@link ControlDecoration#ControlDecoration(Control, int, Composite)}.
     * @param decorationPosition
     *            For more info see
     *            {@link ControlDecoration#ControlDecoration(Control, int, Composite)}.
     */
    public ControlDecorator(Composite pParent, Control control, int decorationPosition) {
        decoration = new ControlDecoration(control, decorationPosition, pParent);
    }

    /**
     * 
     * @return tbd
     */
    public boolean isDecorated() {
        return message != null;
    }

    /** */
    public void dispose() {
        decoration.dispose();
    }

    /**
     * Will show message accordingly based on severity.
     * 
     * @param pMessage
     *            Message to show.
     */
    public void showMessage(IStatus pMessage) {
        message = pMessage;
        update();
    }

    /** */
    public void removeMessage() {
        message = null;
        update();
    }

    /**
     * Will update decorator image and text.
     */
    public void update() {
        if (message == null) {
            decoration.setDescriptionText(null);
            decoration.hide();
        } else {
            decoration.setDescriptionText(message.getMessage());
            switch (message.getSeverity()) {
                case IStatus.ERROR:
                    decoration.setImage(standardError.getImage());
                    break;
                case IStatus.WARNING:
                    decoration.setImage(standardWarning.getImage());
                    break;
                case IStatus.INFO:
                    decoration.setImage(standardInformation.getImage());
                    break;
                default:
                    decoration.setDescriptionText(null);
                    decoration.hide();
                    return;
            }
            decoration.show();
        }
    }

    /**
     * Helper for creating status message object.
     * 
     * @param severity
     *            Should be one of following {@link IStatus#OK}, {@link IStatus#INFO},
     *            {@link IStatus#WARNING}, {@link IStatus#ERROR}. In case of different value
     *            {@link IStatus#INFO} will be used by default.
     * @param message
     *            Message to show.
     * @return Message object.
     */
    public static IStatus getStatusMessage(int severity, String message) {
        switch (severity) {
            case IStatus.ERROR:
            case IStatus.WARNING:
            case IStatus.INFO:
            case IStatus.OK:
                return new Status(severity, "no_name", message);
            default:
                return new Status(IStatus.INFO, "no_name", message);
        }
    }
}
