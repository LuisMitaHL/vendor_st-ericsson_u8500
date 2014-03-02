package com.stericsson.sdk.equipment.ui;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.equipment.EquipmentStatus;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentListener;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Adapter class to be used by viewers that are interested in equipment connected to the backend and
 * changes of the backend state
 *
 * @author xolabju
 *
 */
public final class BackendViewerAdapter implements IEquipmentListener,
    IConfigurationServiceListener {

    private CopyOnWriteArrayList<IBackendViewer> viewers;

    private CopyOnWriteArrayList<IEquipment> connectedEquipment;

    private static BackendViewerAdapter instance = null;

    /**
     * @return Instance of backend viewer adapter
     */
    public static BackendViewerAdapter getInstance() {
        if (instance == null) {
            instance = new BackendViewerAdapter();
        }
        return instance;
    }

    /**
     * Constructor
     */
    private BackendViewerAdapter() {
        viewers = new CopyOnWriteArrayList<IBackendViewer>();
        connectedEquipment = new CopyOnWriteArrayList<IEquipment>();
        BundleContext bundleContext = Activator.getBundleContext();
        bundleContext.registerService(IEquipmentListener.class.getName(), this, null);
        bundleContext.registerService(IConfigurationServiceListener.class.getName(), this, null);
        refresh();
    }

    /**
     * Adds a viewer to the adapter
     *
     * @param viewer
     *            the viewer to add
     */
    public void addViewer(IBackendViewer viewer) {
        viewers.add(viewer);
    }

    /**
     * Removes a viewer from the adapter
     *
     * @param viewer
     *            the viewer to remove
     */
    public void removeViewer(IBackendViewer viewer) {
        viewers.remove(viewer);
    }

    /**
     * {@inheritDoc}
     */
    public void equipmentStateChanged(IEquipment equipment, EquipmentStatus newState) {
        addEquipment(equipment);
        for (IBackendViewer viewer : viewers) {
            viewer.equipmentChanged(equipment);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void equipmentDisconnected(IEquipment equipment) {
        boolean removed = false;
        removed = connectedEquipment.remove(equipment);
        if (removed) {
            for (IBackendViewer viewer : viewers) {
                viewer.equipmentDisconnected(equipment);
            }
        }
    }

    /**
     * Refreshes the list of currently connected equipment
     *
     * @return the refreshed list
     */
    public List<IEquipment> refresh() {
        BundleContext bundleContext = Activator.getBundleContext();
        connectedEquipment.clear();
        try {
            ServiceReference[] serviceReferences = bundleContext.getServiceReferences(IEquipment.class.getName(), null);
            if (serviceReferences != null && serviceReferences.length > 0) {
                for (ServiceReference ref : serviceReferences) {
                    connectedEquipment.add((IEquipment) bundleContext.getService(ref));
                }
            }
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }
        return connectedEquipment;
    }

    /**
     * Gets the connected equipment
     *
     * @return the connected equipment
     */
    public List<IEquipment> getConnectedEquipment() {
        return connectedEquipment;
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {
        for (IBackendViewer viewer : viewers) {
            viewer.configurationChanged(service, oldRecord, newRecord);
        }
    }

    /**
     * @param equipment
     *            Equipment to remove
     */
    public void removeEquipment(IEquipment equipment) {
        connectedEquipment.remove(equipment);
        for (IBackendViewer viewer : viewers) {
            viewer.equipmentDisconnected(equipment);
        }
    }

    private void addEquipment(IEquipment equipment) {
        if (!connectedEquipment.contains(equipment)) {
            connectedEquipment.add(equipment);
        }
    }
}
