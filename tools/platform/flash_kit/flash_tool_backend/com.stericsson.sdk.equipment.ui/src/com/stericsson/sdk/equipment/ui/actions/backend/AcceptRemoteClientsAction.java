package com.stericsson.sdk.equipment.ui.actions.backend;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xtomlju
 */
public class AcceptRemoteClientsAction extends BackendAction {

    /**
     * @param viewer
     *            Backend viewer
     */
    public AcceptRemoteClientsAction(IBackendViewer viewer) {
        super(viewer);
        setText("Accept Remote Clients");
        setChecked(getBackendService().isAcceptingClients());
    }

    /**
     * 
     */
    public void run() {
        IBackendService service = getBackendService();
        if (service != null) {
            service.setAcceptClients(isChecked());
        }
    }
}
