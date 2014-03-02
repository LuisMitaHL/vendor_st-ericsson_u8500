package com.stericsson.sdk.common.ui.controls;

import java.util.Collection;
import java.util.SortedSet;
import java.util.TreeSet;

import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;

/**
 * The responsibilities of UIListBox is to keep a list of items added or deleted to the ListBox, it
 * also holds references to the buttons used with the ListBox.
 * 
 * @author xdancho
 * 
 */
public class UIListBox extends UIControl {

    private Label uiLabel;

    private Table uiTable;

    private Button uiAddButton;

    private Button uiRemoveButton;

    private final SortedSet<String> listBoxItems = new TreeSet<String>();

    /**
     * @param pLabel
     *            Label which describes logical name of the ListBox.
     * @param pTable
     *            Table UI control.
     * @param pPlaceholder
     *            Place-holder if was used, null otherwise.
     * 
     */
    UIListBox(Label pLabel, Table pTable, Control pPlaceholder) {
        uiLabel = pLabel;
        uiPlaceholder = pPlaceholder;
        uiTable = pTable;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Label getControlLabel() {
        return uiLabel;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Control getUIControl() {
        return uiTable;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setControlLabel(Label pLabel) {
        uiLabel = pLabel;

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setLayoutData(Object pLayoutData) {
        uiTable.setLayoutData(pLayoutData);

    }

    /**
     * Get the ListBox items.
     * 
     * @return returns the items as a sorted set.
     */
    public Collection<String> getListBoxItems() {
        return listBoxItems;
    }

    /**
     * Add a item to the listBox.
     * 
     * @param pItem
     *            The item to add.
     * @return true if the item could be added, false otherwise
     */
    public boolean addListBoxItem(String pItem) {
        return listBoxItems.add(pItem);
    }

    /**
     * Remove a item to the listBox.
     * 
     * @param pItem
     *            The item to remove.
     * @return true if the item could be removed, false otherwise
     */
    public boolean removeListBoxItem(String pItem) {
        return listBoxItems.remove(pItem);
    }

    /**
     * Set the Add button.
     * 
     * @param pAddButton
     *            The Button to set.
     */
    public void setAddButton(Button pAddButton) {
        uiAddButton = pAddButton;
    }

    /**
     * @return The Add button, null if no Add button exists
     */
    public Button getAddButton() {
        return uiAddButton;
    }

    /**
     * Set the Remove button
     * 
     * @param pRemoveButton
     *            The Button to set.
     */
    public void setRemoveButton(Button pRemoveButton) {
        uiRemoveButton = pRemoveButton;
    }

    /**
     * @return The Remove button, null if no Remove button exists
     */
    public Button getRemoveButton() {
        return uiRemoveButton;
    }

}
