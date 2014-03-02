package com.stericsson.sdk.common.ui.editors.mesp;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

/**
 * @author xmicroh
 *
 */
public class MespTreeLabelProvider extends LabelProvider {

    /**
     * {@inheritDoc}
     */
    @Override
    public Image getImage(Object pElement) {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getText(Object element) {
        if (element instanceof IMespTreeItem) {
            return element.toString();
        } else {
            return super.getText(element);
        }
    }

}
