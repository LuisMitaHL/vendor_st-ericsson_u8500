package com.stericsson.sdk.equipment.ui.editors;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.eclipse.ui.forms.widgets.TableWrapData;
import org.eclipse.ui.forms.widgets.TableWrapLayout;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.actions.coredump.DeleteDumpAction;
import com.stericsson.sdk.equipment.ui.actions.coredump.DownloadDumpAction;
import com.stericsson.sdk.equipment.ui.jobs.DeleteDumpJob;
import com.stericsson.sdk.equipment.ui.jobs.DownloadDumpJob;
import com.stericsson.sdk.equipment.ui.jobs.HandleDumpJob;

/**
 *
 * Editor Dump page
 *
 * @author esrimpa
 *
 */
public class EquipmentEditorDumpPage {

    /** Line split with "|" */
    private static final String LINE_SPLIT = "\\|";

    /** Line break */
    private static final String LINE_BREAK = "\n";

    /** File Downloaded */
    private static final String FTP_DOWNLOADED = "Downloaded";

    /** string to use if downloaded */
    private static final String FTP_DOWNLOADED_YES = "Yes";

    /** string to use if not downloaded */
    private static final String FTP_DOWNLOADED_NO = "No";

    /** File prefix for modem core dump */
    private static final String MODEM_COREDUMP_PREFIX = "mcd";

    /** File prefix for system core dump */
    private static final String SYSTEM_COREDUMP_PREFIX = "scd";

    /** File prefix for report */
    private static final String REPORT_COREDUMP_PREFIX = "rpt";

    /** File prefix for kernel */
    private static final String KERNEL_COREDUMP_PREFIX = "cdump";

    /** Dump type for modem core dump */
    private static final String MODEM_COREDUMP_TYPE = "Modem";

    /** Dump type for system core dump */
    private static final String SYSTEM_COREDUMP_TYPE = "System";

    /** Dump type for report */
    private static final String REPORT_COREDUMP_TYPE = "Report";

    /** Dump type for kernel */
    private static final String KERNEL_COREDUMP_TYPE = "Kernel";

    /** Dump type for others */
    private static final String OTHER_COREDUMP_TYPE = "Other";

    private IEquipment equipment;

    private Composite pageComposite;

    private Table dumpInfoTable;

    private Button buttonDownload;

    private Button buttonDelete;

    private DownloadDumpAction actionDownloadDump;

    private DeleteDumpAction actionDelete;

    /**
     * Constructor
     *
     * @param e
     *            Equipment object
     * @param parent
     *            Parent composite object
     * @param toolkit
     *            Form toolkit object
     */
    public EquipmentEditorDumpPage(IEquipment e, Composite parent, FormToolkit toolkit) {
        equipment = e;
        pageComposite = createPage(parent, toolkit);
        updatePage();
    }

    /**
     * @return composite object
     */
    public Composite getPageComposite() {
        return pageComposite;

    }

    /**
     * Update the page with the proper data.
     */
    public void updatePage() {

        PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable() {
            public void run() {

                TableItem item;
                dumpInfoTable.removeAll();

                List<String> files = listFile();
                Iterator<String> it = files.iterator();

                // The files list will look as given below.
                // files = {filaname1|Downloaded, filename2|Not Downloaded}
                // So parse and put into the table
                while (it.hasNext()) {
                    String value = (String) it.next();
                    String[] splittedValue = value.split(LINE_SPLIT);

                    int startIndex = splittedValue[0].lastIndexOf("/") + 1;
                    String fileName = splittedValue[0].substring(startIndex);
                    item = new TableItem(dumpInfoTable, SWT.NONE);

                    // Get the file name
                    item.setText(0, splittedValue[0]);

                    // Get the core dump type
                    if (fileName.startsWith(MODEM_COREDUMP_PREFIX)) {
                        item.setText(1, MODEM_COREDUMP_TYPE);
                    } else if (fileName.startsWith(SYSTEM_COREDUMP_PREFIX)) {
                        item.setText(1, SYSTEM_COREDUMP_TYPE);
                    } else if (fileName.startsWith(REPORT_COREDUMP_PREFIX)) {
                        item.setText(1, REPORT_COREDUMP_TYPE);
                    } else if (fileName.startsWith(KERNEL_COREDUMP_PREFIX)) {
                        item.setText(1, KERNEL_COREDUMP_TYPE);
                    } else {
                        item.setText(1, OTHER_COREDUMP_TYPE);
                    }

                    // Check if it is downloaded or not
                    if (splittedValue[1] != null && splittedValue[1].equals(FTP_DOWNLOADED)) {
                        item.setText(2, FTP_DOWNLOADED_YES);
                    } else {
                        item.setText(2, FTP_DOWNLOADED_NO);
                    }
                }

                for (TableColumn col : dumpInfoTable.getColumns()) {
                    col.pack();
                }
            }
        });
    }

    /**
     * Create the page layout
     *
     * @param parent
     *            Composite object
     * @param toolkit
     *            Form toolkit
     * @return composite object for the page
     */
    private Composite createPage(Composite parent, FormToolkit toolkit) {
        ScrolledForm scrolledForm = toolkit.createScrolledForm(parent);
        scrolledForm.getForm().setText("Mobile Equipment Dump Information");
        toolkit.decorateFormHeading(scrolledForm.getForm());
        TableWrapLayout layout = new TableWrapLayout();
        scrolledForm.getBody().setLayout(layout);

        createDumpInfoTable(scrolledForm, toolkit);

        parent.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_L1ParentPageComposite_Composite");
        scrolledForm.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_PageComposite_ScrolledForm");

        // GUI Dancer properties
        buttonDownload.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_Download_Button");
        buttonDelete.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_Delete_Button");
        dumpInfoTable.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_DumpInfoTable_Table");

        return scrolledForm;
    }

    private void createDumpInfoTable(ScrolledForm scrolledForm, FormToolkit toolkit) {

        TableWrapData tableWrapData = new TableWrapData(TableWrapData.FILL);
        Section section = toolkit.createSection(scrolledForm.getBody(), Section.TITLE_BAR | Section.DESCRIPTION);
        section.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_DumpInfoSection_Section");
        section.setDescription("This shows information of core dump files present on the Mobile Equipment.");
        section.setText("Dump Information");
        section.setLayoutData(tableWrapData);

        Composite sectionClient = toolkit.createComposite(section);
        sectionClient.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_DumpInfoComposite_Composite");
        sectionClient.setLayout(new GridLayout(2, false));
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        sectionClient.setLayoutData(gridData);

        dumpInfoTable = toolkit.createTable(sectionClient, SWT.FULL_SELECTION | SWT.MULTI);
        dumpInfoTable.setHeaderVisible(true);
        dumpInfoTable.setData("GD_COMP_NAME", "EquipmentEditorDumpPage_DumpInfoTable_Table");

        gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.heightHint = 300;
        gridData.widthHint = 600;
        dumpInfoTable.setLayoutData(gridData);
        TableColumn col;

        col = new TableColumn(dumpInfoTable, SWT.NONE);
        col.setText("Dump Name");
        col = new TableColumn(dumpInfoTable, SWT.NONE);
        col.setText("Dump Type");
        col = new TableColumn(dumpInfoTable, SWT.NONE);
        col.setText("Downloaded (Y/N?)");
        col.setWidth(100);
        col.setAlignment(SWT.CENTER);

        // Create download button
        Composite buttonDownloadClient = toolkit.createComposite(sectionClient);
        GridLayout gridLayout = new GridLayout(1, false);
        gridLayout.marginHeight = 0;
        buttonDownloadClient.setLayout(gridLayout);
        buttonDownloadClient.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));

        // Create delete button
        Composite buttonDeleteClient = toolkit.createComposite(sectionClient);
        gridLayout = new GridLayout(1, false);
        gridLayout.marginHeight = 0;
        buttonDeleteClient.setLayout(gridLayout);
        buttonDeleteClient.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));

        // create menu
        MenuManager popupMenu = new MenuManager();
        fillContextMenu(popupMenu);
        Menu menu = popupMenu.createContextMenu(dumpInfoTable);
        dumpInfoTable.setMenu(menu);

        buttonDownload = toolkit.createButton(buttonDownloadClient, "Download", SWT.PUSH);
        buttonDownload.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent e) {
                downloadDump();
            }

            public void widgetDefaultSelected(SelectionEvent e) {
            }
        });

        buttonDelete = toolkit.createButton(buttonDownloadClient, "Delete", SWT.PUSH);
        buttonDelete.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent e) {
                deleteDump();
            }

            public void widgetDefaultSelected(SelectionEvent e) {
            }
        });

        dumpInfoTable.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent e) {
                boolean selected = dumpInfoTable.getSelectionCount() > 0 && equipment != null;
                actionDownloadDump.setEnabled(selected);
                actionDelete.setEnabled(selected);
                buttonDownload.setEnabled(selected);
                buttonDelete.setEnabled(selected);
            }

            public void widgetDefaultSelected(SelectionEvent e) {

            }
        });

        buttonDownload.setEnabled(false);
        buttonDelete.setEnabled(false);

        section.setClient(sectionClient);
    }

    private void fillContextMenu(MenuManager popupMenu) {
        // download dump
        actionDownloadDump = new DownloadDumpAction() {
            @Override
            public void run() {
                downloadDump();
            }
        };
        actionDownloadDump.setEnabled(false);

        // delete dump
        actionDelete = new DeleteDumpAction() {
            @Override
            public void run() {
                deleteDump();
            }
        };
        actionDelete.setEnabled(false);

        popupMenu.add(actionDownloadDump);
        popupMenu.add(actionDelete);
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
                setEnabled(equipment != null && dumpInfoTable.getSelectionCount() > 0);
            }
        });
    }

    private void setEnabled(final boolean enabled) {
        buttonDownload.setEnabled(enabled);
        buttonDelete.setEnabled(enabled);
        actionDownloadDump.setEnabled(enabled);
        actionDelete.setEnabled(enabled);
    }

    private void downloadDump() {
        TableItem[] items = dumpInfoTable.getSelection();
        DownloadDumpJob job = null;

        if (items.length != 0 && equipment != null) {

            DirectoryDialog dialog = new DirectoryDialog(Display.getDefault().getActiveShell());
            dialog.setText("Select download directory path " + " for " + equipment.toString());
            String path = dialog.open();
            for (int cnt = 0; cnt < items.length; cnt++) {
                String fileName = items[cnt].getText(0);
                boolean response = true;
                boolean exists = (new File(path + "\\" + fileName)).exists();

                // Check if the file exists, if yes give a message to user asking for overwrite
                if (exists) {
                    response =
                        MessageDialog.openConfirm(getPageComposite().getShell(), "Confirm File Replace",
                            "This folder already contains a file named '" + fileName
                                + "'.\nWould you like to replace the existing file with this one?");
                }

                if ((path != null) && response) {
                    job = new DownloadDumpJob(equipment, fileName, path);
                    job.schedule();
                }
            }
            isJobDone(job);
        }
    }

    private void isJobDone(Job job) {
        if (job != null) {
            job.addJobChangeListener(new JobChangeAdapter() {
                public void done(IJobChangeEvent event) {
                    if (event.getResult().isOK()) {
                        updatePage();
                    }
                }
            });
        }
    }

    /**
     * @return list of undownloaded file
     */
    private List<String> listFile() {

        List<String> fileList = new ArrayList<String>();
        HandleDumpJob job = new HandleDumpJob(equipment);

        try {
            job.schedule();

            while (!job.taskCompletedStatus()) {
                Thread.sleep(100);
            }

            // The result message will be parsed here. If falied result message will be null. Else
            // it contains the file information. For example.
            // resultMassage = "filaname1|Downloaded\nfilename2|Not Downloaded
            String resultString = job.taskMessage();

            if (resultString != null) {
                String[] splittedString = resultString.split(LINE_BREAK);
                for (int i = 0; i < splittedString.length; i++) {
                    if (splittedString[i].trim().length() != 0) {
                        fileList.add(splittedString[i].trim());
                    }
                }
            }
        } catch (Exception e) {
            job.cancel();
        }
        return fileList;
    }

    /**
     * @return delete files
     */
    private void deleteDump() {
        TableItem[] items = dumpInfoTable.getSelection();
        String message;

        if (items.length > 1) {
            message = "Are you sure you want to delete the selected files from ME?";
        } else {
            message = "Are you sure you want to delete the selected file from ME?";
        }

        boolean response = MessageDialog.openConfirm(getPageComposite().getShell(), "Confirm File Deletion", message);

        if (items.length != 0 && equipment != null && response) {

            for (int cnt = 0; cnt < items.length; cnt++) {
                String fileName = items[cnt].getText(0);

                if (fileName != null) {
                    DeleteDumpJob job = new DeleteDumpJob(equipment, fileName);
                    job.schedule();
                    isJobDone(job);
                }
            }
        }
    }
}
