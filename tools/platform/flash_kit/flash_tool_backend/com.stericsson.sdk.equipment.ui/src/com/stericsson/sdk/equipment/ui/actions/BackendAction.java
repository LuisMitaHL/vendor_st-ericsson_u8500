package com.stericsson.sdk.equipment.ui.actions;

import org.eclipse.jface.action.Action;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * 
 * @author xolabju
 * 
 */
public abstract class BackendAction extends Action {

    /** */
    protected IBackendViewer viewer;

    /**
     * Constructor
     * 
     * @param pViewer
     *            the associated equipment viewer
     */
    public BackendAction(IBackendViewer pViewer) {
        super();
        viewer = pViewer;
    }

    /**
     * 
     * @return the associated viewer
     */
    public IBackendViewer getViewer() {
        return viewer;
    }

    /**
     * @return Backend service
     */
    protected IBackendService getBackendService() {
        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        if ((references == null) || (references.length == 0)) {
            return null;
        }

        return (IBackendService) Activator.getBundleContext().getService(references[0]);
    }

}
