package com.stericsson.sdk.equipment.ui.actions.backend;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xtomlju
 */
public class AcceptConnectionsAction extends BackendAction {

    /**
     * @param viewer
     *            Backend viewer
     */
    public AcceptConnectionsAction(IBackendViewer viewer) {
        super(viewer);
        setText("Accept Connections");
        setChecked(getBackendService().isAcceptingEquipments());
    }

    /**
     * 
     */
    public void run() {
        IBackendService service = getBackendService();
        if (service != null) {
            service.setAcceptEquipments(isChecked());
        }
    }

}
