package com.stericsson.sdk.equipment.ui.editors;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IStorage;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPersistableElement;

import com.stericsson.sdk.equipment.IEquipment;

/**
 * @author xtomlju
 */
public class EquipmentEditorInput implements IFileEditorInput {

    private IEquipment mobileEquipment;

    /**
     * Constructor
     * 
     * @param equipment
     *            Equipment object
     */
    public EquipmentEditorInput(IEquipment equipment) {
        mobileEquipment = equipment;
    }

    /**
     * {@inheritDoc}
     */
    public IFile getFile() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public IStorage getStorage() throws CoreException {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public boolean exists() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public ImageDescriptor getImageDescriptor() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public String getName() {
        return mobileEquipment.toString();
    }

    /**
     * {@inheritDoc}
     */
    public IPersistableElement getPersistable() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public String getToolTipText() {
        return mobileEquipment.toString();
    }

    /**
     * {@inheritDoc}
     */
    @SuppressWarnings("unchecked")
    public Object getAdapter(Class adapter) {
        if (adapter == IEquipment.class) {
            return mobileEquipment;
        }
        return null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean equals(Object object) {
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int hashCode() {
        return super.hashCode();
    }

    /**
     * 
     * @return the IEquipment associated with this editor input
     */
    public IEquipment getEquipment() {
        return mobileEquipment;
    }

}
