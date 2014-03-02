package com.stericsson.sdk.common.ui.editors.mesp;

import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;

/**
 * @author xmicroh
 *
 */
public class MespTreeContentProvider implements ITreeContentProvider {

    /**
     * {@inheritDoc}
     */
    public Object[] getElements(Object pInputElement) {
        return getChildren(pInputElement);
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getChildren(Object element) {
        if (element instanceof IMespTreeItem) {
            return ((IMespTreeItem) element).getChildren();
        } else {
            return new Object[0];
        }
    }

    /**
     * {@inheritDoc}
     */
    public Object getParent(Object element) {
        if (element instanceof IMespTreeItem) {
            return ((IMespTreeItem) element).getParent();
        } else {
            return null;
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasChildren(Object element) {
        if (element instanceof IMespTreeItem) {
            return ((IMespTreeItem) element).hasChildren();
        } else {
            return false;
        }
    }

    /**
     * {@inheritDoc}
     */
    public void dispose() {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public void inputChanged(Viewer pViewer, Object pOldInput, Object pNewInput) {
        // TODO Auto-generated method stub
    }

}
