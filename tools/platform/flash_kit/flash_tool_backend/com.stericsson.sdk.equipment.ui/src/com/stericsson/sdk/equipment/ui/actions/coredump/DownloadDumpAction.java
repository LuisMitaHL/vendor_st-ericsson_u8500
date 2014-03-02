package com.stericsson.sdk.equipment.ui.actions.coredump;

import org.eclipse.jface.action.Action;

import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Download dump action
 * 
 * @author esrimpa
 * 
 */
public class DownloadDumpAction extends Action {

    /**
     * Constructor
     * 
     */
    public DownloadDumpAction() {
        super();
        setText("Download");
        setToolTipText("Download selected dump");
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.WARM_EQUIPMENT_ICON_DUMP));
    }

}
