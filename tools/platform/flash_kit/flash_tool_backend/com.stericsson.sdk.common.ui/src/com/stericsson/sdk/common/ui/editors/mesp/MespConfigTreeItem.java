package com.stericsson.sdk.common.ui.editors.mesp;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;


/**
 * Instances of this class will be shown as nodes in a tree of MespEditor editor overview section. 
 * 
 * @author xmicroh
 *
 */
public class MespConfigTreeItem implements IMespTreeItem {

    private MespConfigurationFile parent;

    private IConfigurationRecord record;

    private MespDescriptionTreeItem descriptionTreeItem;

    /**
     * @param configFile Root node stored as parent.
     * @param pRecord mesp record which is wrapped by this node. 
     */
    public MespConfigTreeItem(MespConfigurationFile configFile, IConfigurationRecord pRecord) {
        parent = configFile;
        record = pRecord;
        descriptionTreeItem = new MespDescriptionTreeItem(this);
    }

    /**
     * @return Mesp record wrapped by this node.
     */
    public IConfigurationRecord getRecord() {
        return record;
    }

    /**
     * @return The only child node which provides more descriptive representation of wrapped mesp record.
     */
    public MespDescriptionTreeItem getDescriptionTreeItem() {
        return descriptionTreeItem;
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getChildren() {
        return new Object[] {descriptionTreeItem};
    }

    /**
     * {@inheritDoc}
     */
    public Object getParent() {
        return parent;
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasChildren() {
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return record.getName();
    }

}
