package com.stericsson.sdk.common.ui.editors.mesp;

/**
 * Objects implementing this interface will be ready to use in {@link Tree}. 
 * 
 * @author xmicroh
 *
 */
public interface IMespTreeItem {

    /**
     * Convenient method for the tree node.
     * @return Parent node object or null when this node is root.
     */
    Object getParent();

    /**
     * Convenient method for the tree node indicating if this nodes contains any child.
     * 
     * @return True if this node has children node, false otherwise. 
     */
    boolean hasChildren();

    /**
     * Convenient method for the tree node.
     * @return All children nodes.
     */
    Object[] getChildren();

}
