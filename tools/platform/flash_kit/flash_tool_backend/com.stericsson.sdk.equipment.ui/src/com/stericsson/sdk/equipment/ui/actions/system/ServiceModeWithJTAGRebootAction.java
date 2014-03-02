package com.stericsson.sdk.equipment.ui.actions.system;

import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xmicroh
 *
 */
public class ServiceModeWithJTAGRebootAction extends RebootAction {

    /**
     * @param pViewer TBD
     */
    protected ServiceModeWithJTAGRebootAction(IBackendViewer pViewer) {
        super(pViewer);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionText() {
        return "Service mode with JTAG";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionToolTipText() {
        return "Reboot equipment in service mode with JTAG debugging enabled";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getRestartMode() {
        return 3;
    }

}
