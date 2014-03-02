package com.stericsson.sdk.common.ui.controls;

import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.ui.forms.widgets.FormToolkit;

import com.stericsson.sdk.common.ui.formatters.IFormatter;
import com.stericsson.sdk.common.ui.formatters.NullFormatter;

/**
 * This Factory is used when creating ListBoxes and the layouts for it.
 * 
 * @author xdancho
 * 
 */
public final class UIListBoxFactory {

    private final FormToolkit toolkit;

    private static UIListBoxFactory instance;

    private UIListBoxFactory(FormToolkit pToolkit) {
        toolkit = pToolkit;
    }

    /**
     * Will create factory which is capable to generate UI controls. Factory will use given toolkit
     * if it is set.
     * 
     * @param pToolkit
     *            Set toolkit or null depending on how you want to create UI controls (form toolkit
     *            is used if the control should be placed into the form).
     * @return Instance of this factory.
     */
    public static UIListBoxFactory getInstance(FormToolkit pToolkit) {
        if (pToolkit != null) {
            return new UIListBoxFactory(pToolkit);
        } else if (instance == null) {
            instance = new UIListBoxFactory(null);
        }
        return instance;
    }

    /**
     * Create a ListBox
     * 
     * @param pParent
     *            The parent.
     * @param dialogTitle
     *            The title of the input dialog.
     * @param dialogDescription
     *            The description to show for the dialog.
     * @param pStyle
     *            The style to use.
     * @param pName
     *            The label text.
     * @param pSpanColumns
     *            Horizontal span.
     * @param pValidator
     *            The inputValidator to use.
     * @return A ListBox.
     */
    public UIListBox createListBox(Composite pParent, final String dialogTitle, final String dialogDescription,
        int pStyle, String pName, int pSpanColumns, final IInputValidator pValidator) {

        return createListBox(pParent, dialogTitle, dialogDescription, pStyle, pName, pSpanColumns, pValidator,
            new NullFormatter<String>());
    }

    /**
     * Create a ListBox
     * 
     * @param pParent
     *            The parent.
     * @param dialogTitle
     *            The title of the input dialog.
     * @param dialogDescription
     *            The description to show for the dialog.
     * @param pStyle
     *            The style to use.
     * @param pName
     *            The label text.
     * @param pSpanColumns
     *            Horizontal span.
     * @param pValidator
     *            The inputValidator to use.
     * @param pFormatter
     *            The inputFormatter to use.
     * @return A ListBox.
     */
    public UIListBox createListBox(Composite pParent, final String dialogTitle, final String dialogDescription,
        int pStyle, String pName, int pSpanColumns, final IInputValidator pValidator,
        final IFormatter<String> pFormatter) {

        Composite group = null;
        if (toolkit != null) {
            group = toolkit.createComposite(pParent, SWT.NONE);
        } else {
            group = new Composite(pParent, SWT.NONE);
        }

        final GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.horizontalSpan = pSpanColumns;
        group.setLayoutData(gridData);
        group.setLayout(new GridLayout(3, false));

        Label tableDescription = null;
        if (toolkit != null) {
            tableDescription = toolkit.createLabel(group, pName);
        } else {
            tableDescription = new Label(group, SWT.NONE);
            tableDescription.setText(pName);
        }

        final GridData labelData = new GridData(SWT.FILL, SWT.NONE, true, false);
        labelData.horizontalSpan = pSpanColumns;
        tableDescription.setLayoutData(labelData);

        Table table = null;
        if (toolkit != null) {
            table = toolkit.createTable(group, pStyle | SWT.FULL_SELECTION | SWT.SINGLE);
        } else {
            table = new Table(group, pStyle | SWT.FULL_SELECTION | SWT.SINGLE);
        }

        final GridData tableData = new GridData(SWT.FILL, SWT.NONE, true, true);
        tableData.horizontalSpan = 2;
        table.setLayoutData(tableData);

        final UIListBox uiListBox = new UIListBox(tableDescription, table, group);
        final Table tableRef = table;

        // BUTTON GROUP
        Composite groupComponent = null;
        if (toolkit != null) {
            groupComponent = toolkit.createComposite(group, SWT.NULL);
        } else {
            groupComponent = new Composite(group, SWT.NULL);
        }
        groupComponent.setLayout(new GridLayout(1, false));

        final GridData buttonData = new GridData(SWT.FILL, SWT.NONE, false, false);
        buttonData.horizontalSpan = 1;

        Button addButton = null;
        if (toolkit != null) {
            addButton = toolkit.createButton(groupComponent, "Add", SWT.PUSH);
        } else {
            addButton = new Button(groupComponent, SWT.PUSH);
            addButton.setText("Add");
        }
        uiListBox.setAddButton(addButton);
        addButton.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {
                String formatedvalue;
                final UIInputDialog dlg =
                    new UIInputDialog(Display.getCurrent().getActiveShell(), dialogTitle, dialogDescription, "",
                        pValidator);

                if (dlg.open() == Window.OK) {
                    formatedvalue = pFormatter.format(dlg.getValue());
                    if (uiListBox.addListBoxItem(formatedvalue)) {
                        final TableItem ti = new TableItem(tableRef, SWT.NONE);
                        ti.setText(formatedvalue);
                        tableRef.showItem(ti);

                    }
                }

            }
        });
        addButton.setLayoutData(buttonData);

        Button removeButton = null;
        if (toolkit != null) {
            removeButton = toolkit.createButton(groupComponent, "Remove", SWT.PUSH);
        } else {
            removeButton = new Button(groupComponent, SWT.PUSH);
            removeButton.setText("Remove");
        }

        uiListBox.setRemoveButton(removeButton);
        removeButton.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {
                final TableItem[] items = tableRef.getSelection();
                if (items.length > 0) {
                    uiListBox.removeListBoxItem(items[0].getText());
                    items[0].dispose(); // Table is single selection
                }

            }
        });

        removeButton.setLayoutData(buttonData);

        return uiListBox;
    }
}
