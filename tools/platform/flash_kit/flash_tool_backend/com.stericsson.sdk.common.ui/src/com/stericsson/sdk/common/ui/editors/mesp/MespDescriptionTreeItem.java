package com.stericsson.sdk.common.ui.editors.mesp;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;


/**
 * Instances of this class will be shown as nodes in a tree of MespEditor editor overview section.
 * 
 * @author xmicroh
 *
 */
public class MespDescriptionTreeItem implements IMespTreeItem {

    private MespConfigTreeItem mespConfigTreeItem;

    /**
     * Construct instance of this class.
     *
     * @param pMespConfigTreeItem Parent node containing mesp record which is wrapped by this class.
     */
    public MespDescriptionTreeItem(MespConfigTreeItem pMespConfigTreeItem) {
        mespConfigTreeItem = pMespConfigTreeItem;
    }

    /**
     * @return Parent node.
     */
    public MespConfigTreeItem getMespConfigTreeItem() {
        return mespConfigTreeItem;
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getChildren() {
        return new Object[0];
    }

    /**
     * {@inheritDoc}
     */
    public Object getParent() {
        return mespConfigTreeItem;
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasChildren() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        IConfigurationRecord record = mespConfigTreeItem.getRecord();
        return record.toString().substring(record.getName().length()).trim();
    }

}
