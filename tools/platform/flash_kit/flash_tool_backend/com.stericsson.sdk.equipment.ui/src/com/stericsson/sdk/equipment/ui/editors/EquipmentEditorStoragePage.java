package com.stericsson.sdk.equipment.ui.editors;

import java.util.Hashtable;
import java.util.Locale;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.eclipse.ui.forms.widgets.TableWrapData;
import org.eclipse.ui.forms.widgets.TableWrapLayout;

import com.stericsson.sdk.equipment.AbstractPlatform;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;
import com.stericsson.sdk.equipment.ui.actions.flash.DumpAreaAction;
import com.stericsson.sdk.equipment.ui.actions.flash.EraseAreaAction;
import com.stericsson.sdk.equipment.ui.jobs.DumpAreaJob;
import com.stericsson.sdk.equipment.ui.jobs.EraseAreaJob;

/**
 * The EquipmentEditorStoragePage is used to describe equipment properties found during service mode
 * boot.
 * 
 * @author xolabju
 */
public class EquipmentEditorStoragePage {

    private IEquipment equipment;

    private Table flashDeviceTable;

    private Composite pageComposite;

    private DumpAreaAction actionDump;

    private EraseAreaAction actionErase;

    private Button buttonErase;

    private Button buttonDump;

    private Text textReport;

    /**
     * 
     * @param e
     *            TBD
     * @param parent
     *            TBD
     * @param toolkit
     *            TBD
     */
    public EquipmentEditorStoragePage(IEquipment e, Composite parent, FormToolkit toolkit) {
        equipment = e;
        pageComposite = createPage(parent, toolkit);
        updatePage();
    }

    /**
     * @return TBD
     */
    public Composite getPageComposite() {
        return pageComposite;
    }

    /**
     */
    public void updatePage() {
        equipment.updateModel(EquipmentModel.FLASH);
        final IEquipmentFlashModel finalModel = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);

        PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable() {
            public void run() {

                flashDeviceTable.removeAll();

                if (finalModel != null) {
                    TableItem item;
                    String[] deviceNames = finalModel.getFlashDeviceNames("PartitioneMMC");
                    for (String name : deviceNames) {
                        item = new TableItem(flashDeviceTable, SWT.NONE);
                        item.setText(0, name);
                        item.setText(1, finalModel.getType(name));
                        item.setText(2, "0x"
                            + Long.toHexString(finalModel.getStart(name)).toUpperCase(Locale.getDefault()));
                        item.setText(3, "0x"
                            + Long.toHexString(finalModel.getLength(name)).toUpperCase(Locale.getDefault()));
                        item.setText(4, "0x"
                            + Long.toHexString(finalModel.getBlockSize(name)).toUpperCase(Locale.getDefault()));
                    }
                }

                for (TableColumn col : flashDeviceTable.getColumns()) {
                    col.pack();
                }
                String flashReport = "No flash report available";
                String report = finalModel.getReport();
                if (report != null && !report.trim().equals("")) {
                    flashReport = report;
                }
                textReport.setText(flashReport);
            }

        });
    }

    /**
     * 
     * @param parent
     *            TBD
     * @param toolkit
     *            TBD
     * @return TBD
     */
    private Composite createPage(Composite parent, FormToolkit toolkit) {
        ScrolledForm scrolledForm = toolkit.createScrolledForm(parent);
        scrolledForm.getForm().setText("Mobile Equipment Flash Storage");
        TableWrapLayout layout = new TableWrapLayout();
        scrolledForm.getBody().setLayout(layout);
        toolkit.decorateFormHeading(scrolledForm.getForm());
        createFlashDeviceTable(scrolledForm, toolkit);
        createFlashReportText(scrolledForm, toolkit);

        parent.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_L1ParentPageComposite_Composite");
        scrolledForm.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_PageComposite_ScrolledForm");

        // GUI Dancer properties
        buttonDump.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_DumpArea_Button");
        buttonErase.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_EraseArea_Button");
        flashDeviceTable.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_FlashDeviceTable_Table");

        return scrolledForm;
    }

    private void createFlashReportText(ScrolledForm scrolledForm, FormToolkit toolkit) {

        TableWrapData data = new TableWrapData();
        GridData gridData = null;

        Section section = toolkit.createSection(scrolledForm.getBody(), Section.TITLE_BAR | Section.DESCRIPTION);
        section.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_FlashReportSection_Section");
        section.setText("Flash report");
        section.setLayoutData(data);

        Composite composite = toolkit.createComposite(section);
        composite.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_FlashReportComposite_Composite");
        composite.setLayout(new GridLayout(1, false));
        gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        composite.setLayoutData(gridData);

        String report = "No flash report available";

        IEquipmentFlashModel model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
        if ((model != null) && (model.getReport() != null)) {
            report = model.getReport();
        }

        textReport = toolkit.createText(composite, report, SWT.MULTI | SWT.READ_ONLY | SWT.H_SCROLL | SWT.V_SCROLL);
        gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.heightHint = 300;
        textReport.setLayoutData(gridData);
        textReport.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_FlashReportText_Text");

        section.setClient(composite);

    }

    private void createFlashDeviceTable(ScrolledForm scrolledForm, FormToolkit toolkit) {

        TableWrapData data = new TableWrapData();

        Section section = toolkit.createSection(scrolledForm.getBody(), Section.TITLE_BAR | Section.DESCRIPTION);
        section.setDescription("This is the current flash device list reported back from the flash loader");
        section.setText("Flash devices");
        section.setLayoutData(data);

        Composite client = toolkit.createComposite(section);
        client.setLayout(new GridLayout(2, false));
        GridData gridData = new GridData(GridData.FILL_BOTH);
        client.setLayoutData(gridData);

        flashDeviceTable = toolkit.createTable(client, SWT.FULL_SELECTION);
        flashDeviceTable.setHeaderVisible(true);
        flashDeviceTable.setData("GD_COMP_NAME", "EquipmentEditorStoragePage_FlashDevceSection_Section");
        gridData = new GridData();
        gridData.heightHint = 200;
        gridData.horizontalAlignment = GridData.FILL;
        gridData.grabExcessHorizontalSpace = true;
        gridData.verticalAlignment = GridData.FILL;
        gridData.grabExcessVerticalSpace = true;
        flashDeviceTable.setLayoutData(gridData);
        TableColumn col;

        col = new TableColumn(flashDeviceTable, SWT.NONE);
        col.setText("Path");
        col = new TableColumn(flashDeviceTable, SWT.NONE);
        col.setText("Type");
        col = new TableColumn(flashDeviceTable, SWT.NONE);
        col.setText("Start");
        col = new TableColumn(flashDeviceTable, SWT.NONE);
        col.setText("Length");
        col = new TableColumn(flashDeviceTable, SWT.NONE);
        col.setText("Block size");

        Composite buttonClient = toolkit.createComposite(client);

        GridLayout gridLayout = new GridLayout(1, false);
        gridLayout.marginHeight = 0;
        buttonClient.setLayout(gridLayout);
        buttonClient.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));

        // create menu
        MenuManager popupMenu = new MenuManager();
        fillContextMenu(popupMenu);
        Menu menu = popupMenu.createContextMenu(flashDeviceTable);
        flashDeviceTable.setMenu(menu);

        buttonErase = toolkit.createButton(buttonClient, "Erase", SWT.PUSH);
        buttonErase.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent e) {
                eraseArea();
            }

            public void widgetDefaultSelected(SelectionEvent e) {
            }
        });

        buttonDump = toolkit.createButton(buttonClient, "Dump", SWT.PUSH);
        buttonDump.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent e) {
                dumpArea();
            }

            public void widgetDefaultSelected(SelectionEvent e) {
            }
        });

        flashDeviceTable.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent e) {
                boolean selected = flashDeviceTable.getSelectionCount() > 0 && equipment != null;
                actionDump.setEnabled(selected);
                actionErase.setEnabled(selected);
                buttonDump.setEnabled(selected);
                buttonErase.setEnabled(selected);
            }

            public void widgetDefaultSelected(SelectionEvent e) {

            }
        });

        buttonDump.setEnabled(false);
        buttonErase.setEnabled(false);

        section.setClient(client);
    }

    private void fillContextMenu(MenuManager popupMenu) {
        // dump
        actionDump = new DumpAreaAction(null) {
            @Override
            public void run() {
                dumpArea();
            }
        };
        actionDump.setEnabled(false);

        // erase
        actionErase = new EraseAreaAction(null) {
            @Override
            public void run() {
                eraseArea();
            }
        };
        actionErase.setEnabled(false);

        popupMenu.add(actionErase);
        popupMenu.add(actionDump);
    }

    void setEquipment(IEquipment e) {
        if (e != equipment) {
            equipment = e;
        }
        if (equipment != null) {
            updatePage();
        }

        PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable() {
            public void run() {
                setEnabled(equipment != null && flashDeviceTable.getSelectionCount() > 0);
            }
        });
    }

    private void setEnabled(final boolean enabled) {
        buttonDump.setEnabled(enabled);
        buttonErase.setEnabled(enabled);
        actionDump.setEnabled(enabled);
        actionErase.setEnabled(enabled);
    }

    private void eraseArea() {
        int index = flashDeviceTable.getSelectionIndex();
        if (index != -1 && equipment != null) {
            TableItem item = flashDeviceTable.getItem(index);
            String path = item.getText(0);
            boolean confirm =
                MessageDialog.openConfirm(getPageComposite().getShell(), "Erase Area", "This will erase " + path
                    + " on " + equipment.toString() + " .\nContinue?");
            if (confirm) {
                EraseAreaJob job = new EraseAreaJob(path, "0", "-1", equipment);
                job.schedule();
            }
        }
    }

    private void dumpArea() {
        int index = flashDeviceTable.getSelectionIndex();
        if (index != -1 && equipment != null) {
            String platform = "";
            TableItem item = flashDeviceTable.getItem(index);
            String path = item.getText(0);
            String start = item.getText(2);
            String length = item.getText(3);
            Hashtable<String, EquipmentProperty> properties = equipment.getProperties();
            if (properties != null) {
                String prop = properties.get(AbstractPlatform.PLATFORM_PROPERTY).getPropertyValue();
                if (prop != null) {
                    platform = prop;
                }
            }

            String defaultFileName =
                platform + path.replace("/", "_").replace("\\", "_") + "_" + System.currentTimeMillis() + ".bin";

            // hardcode for /flash0 for now
            if ("/flash0/".equals(path.toLowerCase(Locale.getDefault()))) {
                length = "-1";
            } else {
                // loader does only react when using /flash0/ as device
                path = "/flash0/";
            }

            FileDialog dialog = new FileDialog(Display.getDefault().getActiveShell(), SWT.SAVE);
            dialog.setText("Select dump file for " + path + " on " + equipment.toString());
            dialog.setFileName(defaultFileName);
            String fileName = dialog.open();
            if (fileName != null) {
                DumpAreaJob job = new DumpAreaJob(path, start, length, fileName, equipment, "true", "true");
                job.schedule();
            }
        }
    }

}
