/**
 * 
 */
package com.stericsson.sdk.common.ui;

/**
 * @author xhelciz
 * 
 */
public interface IEditorUIChangeListener {

    /**
     * called when UIControl of profile editor has been changed
     * 
     * @param source
     *            UIControl which has been changed
     */
    void uiControlChanged(EditorUIControl source);
}
