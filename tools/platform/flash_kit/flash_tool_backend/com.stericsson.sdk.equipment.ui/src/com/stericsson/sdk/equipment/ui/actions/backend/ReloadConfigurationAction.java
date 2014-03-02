package com.stericsson.sdk.equipment.ui.actions.backend;

import java.io.FileNotFoundException;
import java.io.IOException;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.backend.ReloadConfigurationExecutor;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xtomlju
 */
public class ReloadConfigurationAction extends BackendAction {

    private Logger logger = Logger.getLogger(ReloadConfigurationExecutor.class.getName());

    /**
     * @param viewer
     *            Backend viewer
     */
    public ReloadConfigurationAction(IBackendViewer viewer) {
        super(viewer);
        setText("Reload Configuration");
    }

    /**
     * 
     */
    @Override
    public void run() {
        IBackendService service = getBackendService();
        if (service != null) {
            // TODO it should be handle by error message
            try {
                service.loadConfiguration();
            } catch (NullPointerException e) {
                logger.warn(e.getMessage());
            } catch (FileNotFoundException e) {
                logger.error(e.getMessage());
            } catch (IOException e) {
                logger.fatal("Failed to read configuration file", e);
            }

        }
    }

}
