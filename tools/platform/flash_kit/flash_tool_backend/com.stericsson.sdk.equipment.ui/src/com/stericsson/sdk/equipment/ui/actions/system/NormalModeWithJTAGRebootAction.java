package com.stericsson.sdk.equipment.ui.actions.system;

import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xmicroh
 *
 */
public class NormalModeWithJTAGRebootAction extends RebootAction {

    /**
     * @param pViewer TBD
     */
    protected NormalModeWithJTAGRebootAction(IBackendViewer pViewer) {
        super(pViewer);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionText() {
        return "Normal mode with JTAG";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionToolTipText() {
        return "Reboot equipment with JTAG debugging enabled";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getRestartMode() {
        return 2;
    }

}
