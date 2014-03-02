package com.stericsson.sdk.equipment.ui.actions.backend;

import org.eclipse.jface.action.Action;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * @author xtomlju
 */
public class ConnectionProfileAction extends Action {

    private String profileName;

    /**
     * Constructor
     * 
     * @param name
     *            Profile name
     */
    public ConnectionProfileAction(String name) {
        super(name);
        profileName = name;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        if (references != null && references.length > 0) {
            IBackendService backendService = (IBackendService) Activator.getBundleContext().getService(references[0]);
            backendService.setActiveProfile(profileName);
        }
    }
}
