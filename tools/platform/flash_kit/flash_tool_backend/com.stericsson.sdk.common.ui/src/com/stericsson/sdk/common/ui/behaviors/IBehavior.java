package com.stericsson.sdk.common.ui.behaviors;

/**
 * Interface for widget behaviors, that is objects that can be registered on a widget and impact 
 * its behavior.
 * For example one could create and register a behavior on a text field containing path which 
 * could assist in path entering with automatic folder/file names completion.
 * 
 * @author xadazim
 *
 */
public interface IBehavior {

    /**
     * Registers the behavior on a widget. 
     */
    void switchOn();

    /**
     * Unregisters the behavior. 
     */
    void switchOff();
}
