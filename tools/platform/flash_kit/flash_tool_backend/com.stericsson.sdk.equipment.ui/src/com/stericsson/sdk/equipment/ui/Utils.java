package com.stericsson.sdk.equipment.ui;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;

/**
 * 
 * @author xadazim
 *
 */
public final class Utils {
    private Utils() {

    }

    /**
     * Logs and show an error message.
     * @param title title
     * @param message message
     * @param exception exception, can be null
     */
    public static void logAndShowError(String title, String message, Throwable exception) {
        IStatus s;
        if (exception != null) {
            s = new Status(IStatus.ERROR, Activator.PLUGIN_ID, message, exception);
        } else {
            s = new Status(IStatus.ERROR, Activator.PLUGIN_ID, message);
        }

        Activator.getDefault().getLog().log(s);
        ErrorDialog d = new ErrorDialog(null, title, message, s, s.getSeverity());
        d.open();
    }
}
