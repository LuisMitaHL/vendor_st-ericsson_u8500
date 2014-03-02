package com.stericsson.sdk.equipment.ui.views.providers;

import java.util.List;

import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.Viewer;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.BackendViewerAdapter;

/**
 * Content provider for the equipment view
 * 
 * @author xolabju
 * 
 */
public class EquipmentViewContentProvider implements IStructuredContentProvider {

    /**
     * {@inheritDoc}
     */
    public Object[] getElements(Object inputElement) {
        List<IEquipment> equipmentList = BackendViewerAdapter.getInstance().getConnectedEquipment();
        return equipmentList.toArray(new IEquipment[equipmentList.size()]);
    }

    /**
     * {@inheritDoc}
     */
    public void dispose() {

    }

    /**
     * {@inheritDoc}
     */
    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        // TODO Auto-generated method stub

    }

}
