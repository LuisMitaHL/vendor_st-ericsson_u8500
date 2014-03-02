package com.stericsson.sdk.equipment.ui.actions.system;

import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xmicroh
 *
 */
public class ServiceModeRebootAction extends RebootAction {

    /**
     * @param pViewer TBD
     */
    protected ServiceModeRebootAction(IBackendViewer pViewer) {
        super(pViewer);
        // TODO Auto-generated constructor stub
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionText() {
        return "Service mode";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionToolTipText() {
        return "Reboot equipment in service mode";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getRestartMode() {
        return 1;
    }

}
