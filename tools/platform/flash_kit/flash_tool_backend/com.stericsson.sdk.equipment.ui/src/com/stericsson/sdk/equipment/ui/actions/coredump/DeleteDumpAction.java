package com.stericsson.sdk.equipment.ui.actions.coredump;

import org.eclipse.jface.action.Action;

import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Delete dump action
 *
 * @author esrimpa
 *
 */
public class DeleteDumpAction extends Action {

    /**
     * Constructor
     *
     */
    public DeleteDumpAction() {
        super();
        setText("Delete");
        setToolTipText("Delete selected dump");
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.ICON_DUMP_DELETE));
    }

}
