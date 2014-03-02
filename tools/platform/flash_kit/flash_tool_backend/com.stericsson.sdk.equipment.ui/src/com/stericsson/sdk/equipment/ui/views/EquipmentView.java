package com.stericsson.sdk.equipment.ui.views;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.EquipmentException;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.BackendViewerAdapter;
import com.stericsson.sdk.equipment.ui.actions.OpenEditorAction;
import com.stericsson.sdk.equipment.ui.actions.OpenEditorHandleDumpAction;
import com.stericsson.sdk.equipment.ui.actions.backend.AcceptConnectionsAction;
import com.stericsson.sdk.equipment.ui.actions.backend.AcceptRemoteClientsAction;
import com.stericsson.sdk.equipment.ui.actions.backend.ConnectionProfileAction;
import com.stericsson.sdk.equipment.ui.actions.backend.ReloadConfigurationAction;
import com.stericsson.sdk.equipment.ui.actions.flash.DumpAreaAction;
import com.stericsson.sdk.equipment.ui.actions.flash.EraseAreaAction;
import com.stericsson.sdk.equipment.ui.actions.flash.ProcessFileAction;
import com.stericsson.sdk.equipment.ui.actions.flash.ReadGDFSAreaAction;
import com.stericsson.sdk.equipment.ui.actions.flash.ReadTrimAreaAction;
import com.stericsson.sdk.equipment.ui.actions.flash.WriteGDFSAreaAction;
import com.stericsson.sdk.equipment.ui.actions.flash.WriteTrimAreaAction;
import com.stericsson.sdk.equipment.ui.actions.system.AutenticateControlKeysAction;
import com.stericsson.sdk.equipment.ui.actions.system.AuthenticateCertificateAction;
import com.stericsson.sdk.equipment.ui.actions.system.RebootAction;
import com.stericsson.sdk.equipment.ui.actions.system.RemoveAction;
import com.stericsson.sdk.equipment.ui.actions.system.ShutdownAction;
import com.stericsson.sdk.equipment.ui.editors.EquipmentEditorInput;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;
import com.stericsson.sdk.equipment.ui.views.adapters.EquipmentViewerDropAdapter;
import com.stericsson.sdk.equipment.ui.views.providers.EquipmentViewContentProvider;
import com.stericsson.sdk.equipment.ui.views.providers.EquipmentViewLabelProvider;

/**
 * 
 * @author xtomlju, xolabju
 * 
 */

public class EquipmentView extends ViewPart implements IBackendViewer {

    /**
     * The ID of the view as specified by the extension.
     */
    public static final String ID = "com.stericsson.sdk.equipment.ui.views.EquipmentView";

    private TableViewer viewer;

    private Label labelActiveProfile;

    private Action actionProcessFileToFlash;

    private Action actionEraseFlashArea;

    private Action actionDumpFlashArea;

    private Action actionReadGDFSArea;

    private Action actionWriteGDFSArea;

    private Action actionReadTrimArea;

    private Action actionWriteTrimArea;

    private RebootAction[] rebootActions;

    private Action actionShutdown;

    private Action actionOpenEditorHandleDump;

    private Action actionAcceptRemoteClients;

    private Action actionAcceptConnections;

    private Action actionReloadConfiguration;

    private Action actionRemove;

    private Action actionOpenEditor;

    /*
     * The content provider class is responsible for providing objects to the view. It can wrap
     * existing objects in adapters or simply return objects as-is. These objects may be sensitive
     * to the current input of the view, or ignore it and always show the same content (like Task
     * List, for example).
     */

    private Action actionCOM1;

    private Action actionAuthenticateCertificate;

    private Action actionAuthenticateControlKeys;

    class NameSorter extends ViewerSorter {

    }

    /**
     * The constructor.
     */
    public EquipmentView() {
    }

    /**
     * This is a callback that will allow us to create the viewer and initialize it. {@inheritDoc}
     */
    public void createPartControl(Composite parent) {
        makeActions();

        GridLayout layout = new GridLayout(3, false);
        layout.marginHeight = 0;
        layout.marginWidth = 0;
        layout.verticalSpacing = 0;
        parent.setLayout(layout);

        viewer = new TableViewer(parent, SWT.H_SCROLL | SWT.V_SCROLL | SWT.FULL_SELECTION);
        Table table = viewer.getTable();

        // to help GUIdancer recognize components correctly
        table.setData("GD_COMP_NAME", "EquipmentView_Equipment_Table");
        TableViewerColumn column;

        column = new TableViewerColumn(viewer, SWT.NONE);
        column.getColumn().setText("Platform");
        column = new TableViewerColumn(viewer, SWT.NONE);
        column.getColumn().setText("Port");
        column = new TableViewerColumn(viewer, SWT.NONE);
        column.getColumn().setText("State");
        column = new TableViewerColumn(viewer, SWT.NONE);
        column.getColumn().setText("Profile in use");

        EquipmentViewContentProvider evcp = new EquipmentViewContentProvider();
        BackendViewerAdapter.getInstance().addViewer(this);
        viewer.setContentProvider(evcp);
        viewer.setLabelProvider(new EquipmentViewLabelProvider());
        viewer.setSorter(new NameSorter());
        viewer.setInput(getViewSite());

        table.setHeaderVisible(true);
        for (TableColumn col : table.getColumns()) {
            col.pack();
        }

        GridData data = new GridData(GridData.FILL_BOTH);
        data.horizontalSpan = 3;
        table.setLayoutData(data);

        data = new GridData();
        data.heightHint = 18;
        data.verticalIndent = 4;
        data.horizontalIndent = 4;
        data.grabExcessHorizontalSpace = true;
        labelActiveProfile = new Label(parent, SWT.NONE);
        labelActiveProfile.setText("<none>");
        labelActiveProfile.setLayoutData(data);

        // DnD
        int dropOptions = DND.DROP_COPY | DND.DROP_DEFAULT;
        Transfer[] transfers = new Transfer[] {
            FileTransfer.getInstance()};
        viewer.addDropSupport(dropOptions, transfers, new EquipmentViewerDropAdapter(viewer));
        viewer.addSelectionChangedListener(new ISelectionChangedListener() {

            public void selectionChanged(SelectionChangedEvent event) {
                updateActionStates();

            }
        });
        hookContextMenu();
        hookDoubleClickAction();
        contributeToActionBars();
        refreshView();
    }

    private void hookContextMenu() {
        MenuManager menuMgr = new MenuManager("#PopupMenu");
        menuMgr.setRemoveAllWhenShown(true);
        menuMgr.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                EquipmentView.this.fillContextMenu(manager);
            }
        });
        Menu menu = menuMgr.createContextMenu(viewer.getControl());
        viewer.getControl().setMenu(menu);
        getSite().registerContextMenu(menuMgr, viewer);
    }

    private void contributeToActionBars() {
        IActionBars bars = getViewSite().getActionBars();
        fillContextMenu(bars.getMenuManager());
        fillLocalToolBar(bars.getToolBarManager());
    }

    private void fillConnectionProfileMenu(IMenuManager manager) {

        String activeConnectionProfileName = "<none>";
        ServiceReference[] references = null;

        try {
            references = Activator.getBundleContext().getServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        if (references != null) {
            IBackendService backendService = (IBackendService) Activator.getBundleContext().getService(references[0]);
            IConfigurationService configService = backendService.getConfigurationService();

            IConfigurationRecord[] records = configService.getRecords(new String[] {
                "ActiveProfile"});
            if (records != null && records.length > 0) {
                activeConnectionProfileName = records[0].getValue("alias");
            }

            Activator.getBundleContext().ungetService(references[0]);
        }

        ServiceReference reference = null;

        reference = Activator.getBundleContext().getServiceReference(IEquipmentProfileManagerService.class.getName());

        IEquipmentProfileManagerService service =
            (IEquipmentProfileManagerService) Activator.getBundleContext().getService(reference);
        if (service != null) {
            String[] profileNames;
            try {
                profileNames = service.getAllAvailableProfilesNames();
                if (profileNames != null) {
                    for (String name : profileNames) {
                        ConnectionProfileAction action = new ConnectionProfileAction(name);
                        if (activeConnectionProfileName.compareTo(name) == 0) {
                            action.setChecked(true);
                        }
                        manager.add(action);
                    }
                } else {
                    manager.add(new Action("No connection profiles loaded") {
                        {
                            setEnabled(false);
                        }
                    });
                }
            } catch (EquipmentException e) {
                e.printStackTrace();
            }
            Activator.getBundleContext().ungetService(reference);
        }
    }

    private void fillContextMenu(IMenuManager manager) {

        MenuManager connectionProfileMenu = new MenuManager("Connection Profile");
        connectionProfileMenu.setRemoveAllWhenShown(true);
        IMenuListener listener = new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                fillConnectionProfileMenu(manager);
            }
        };
        connectionProfileMenu.addMenuListener(listener);

        manager.add(connectionProfileMenu);
        MenuManager subMenu = new MenuManager("Flash Tool Backend");

        subMenu.add(actionReloadConfiguration);
        subMenu.add(actionAcceptRemoteClients);
        subMenu.add(actionAcceptConnections);
        manager.add(subMenu);
        manager.add(new Separator());
        manager.add(actionOpenEditor);
        manager.add(new Separator());
        manager.add(actionProcessFileToFlash);
        manager.add(actionDumpFlashArea);
        manager.add(actionEraseFlashArea);
        manager.add(new Separator());
        manager.add(actionReadGDFSArea);
        manager.add(actionWriteGDFSArea);
        manager.add(actionReadTrimArea);
        manager.add(actionWriteTrimArea);
        manager.add(new Separator());
        manager.add(actionAuthenticateCertificate);
        manager.add(actionAuthenticateControlKeys);
        manager.add(new Separator());

        fillRebootMenu(manager);

        manager.add(actionShutdown);
        manager.add(actionOpenEditorHandleDump);
        manager.add(actionRemove);
        // Other plug-ins can contribute there actions here
        // removed due to strange addons in sdk-tools
        // manager.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS));
    }

    private void fillRebootMenu(IMenuManager manager) {

        MenuManager subMenu =
            new MenuManager("Reboot", ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource(
                "icons/reboot.png")), null);
        for (RebootAction action : rebootActions) {
            subMenu.add(action);
        }
        manager.add(subMenu);

    }

    private void fillLocalToolBar(IToolBarManager manager) {
        manager.add(actionProcessFileToFlash);
        manager.add(actionDumpFlashArea);
        manager.add(actionEraseFlashArea);
        manager.add(new Separator());
        // no icons currently
        // manager.add(actionReadGDFSArea);
        // manager.add(actionWriteGDFSArea);
        // manager.add(actionReadTrimArea);
        // manager.add(actionWriteTrimArea);
        manager.add(new Separator());
        manager.add(actionAuthenticateCertificate);
        manager.add(actionAuthenticateControlKeys);
        manager.add(new Separator());
        manager.add(actionShutdown);

        ToolBarManager mg = ((ToolBarManager) manager);
        mg.update(true);
        mg.getControl().getItem(0).setData("GD_COMP_NAME", "EquipmentView_ProcessFile_ToolItem");
        mg.getControl().getItem(1).setData("GD_COMP_NAME", "EquipmentView_DumpFlashArea_ToolItem");
        mg.getControl().getItem(2).setData("GD_COMP_NAME", "EquipmentView_EraseFlashArea_ToolItem");
        mg.getControl().getItem(3).setData("GD_COMP_NAME", "EquipmentView_Separator_ToolItem");
        mg.getControl().getItem(4).setData("GD_COMP_NAME", "EquipmentView_AuthenticateByCert_ToolItem");
        mg.getControl().getItem(5).setData("GD_COMP_NAME", "EquipmentView_AuthenticateByKeys_ToolItem");
        mg.getControl().getItem(6).setData("GD_COMP_NAME", "EquipmentView_Separator_ToolItem");
        mg.getControl().getItem(7).setData("GD_COMP_NAME", "EquipmentView_Shutdown_ToolItem");
    }

    private void makeActions() {
        actionProcessFileToFlash = new ProcessFileAction(this);

        actionDumpFlashArea = new DumpAreaAction(this);

        actionEraseFlashArea = new EraseAreaAction(this);

        actionReadGDFSArea = new ReadGDFSAreaAction(this);

        actionWriteGDFSArea = new WriteGDFSAreaAction(this);

        actionReadTrimArea = new ReadTrimAreaAction(this);

        actionWriteTrimArea = new WriteTrimAreaAction(this);

        actionAuthenticateCertificate = new AuthenticateCertificateAction(this);

        actionAuthenticateControlKeys = new AutenticateControlKeysAction(this);

        actionShutdown = new ShutdownAction(this);

        rebootActions = RebootAction.getRebootActions(this);

        actionRemove = new RemoveAction(this);

        actionReloadConfiguration = new ReloadConfigurationAction(this);

        actionAcceptRemoteClients = new AcceptRemoteClientsAction(this);

        actionAcceptConnections = new AcceptConnectionsAction(this);

        actionCOM1 = new Action() {

        };
        actionCOM1.setText("COM1");

        actionOpenEditor = new OpenEditorAction(this);
        actionOpenEditorHandleDump = new OpenEditorHandleDumpAction(this);
    }

    private void hookDoubleClickAction() {
        viewer.addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {

                try {
                    IEquipment selectedEquipment = getSelectedEquipment();
                    if (selectedEquipment != null) {
                        if (selectedEquipment.isInWarmMode()) {
                            IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                            page.openEditor(new EquipmentEditorInput(selectedEquipment),
                                "com.stericsson.sdk.equipment.ui.editors.EquipmentDumpEditor");
                        } else {
                            IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                            page.openEditor(new EquipmentEditorInput(selectedEquipment),
                                "com.stericsson.sdk.equipment.ui.editors.EquipmentEditor");
                        }
                    }
                } catch (PartInitException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Passing the focus request to the viewer's control.
     */
    public void setFocus() {
        viewer.getControl().setFocus();
    }

    /**
     * {@inheritDoc}
     */
    public void equipmentConnected(IEquipment equipment) {
        refreshView();
    }

    /**
     * {@inheritDoc}
     */
    public void equipmentChanged(IEquipment equipment) {
        refreshView();
    }

    /**
     * {@inheritDoc}
     */
    public void equipmentDisconnected(IEquipment equipment) {
        refreshView();

    }

    /**
     * {@inheritDoc}
     */
    public void equipmentMessage(final IEquipment equipment, String message) {

    }

    private void refreshView() {
        PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable() {
            public void run() {
                viewer.refresh();
                for (TableColumn col : viewer.getTable().getColumns()) {
                    col.pack();
                }
                updateActionStates();
                updateActiveProfile();
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    public IEquipment getSelectedEquipment() {
        IEquipment equipment = null;
        Table table = viewer.getTable();
        int index = table.getSelectionIndex();
        if (index != -1) {
            TableItem item = table.getItem(index);
            if (item != null && item.getData() != null && item.getData() instanceof IEquipment) {
                equipment = (IEquipment) item.getData();
            }
        }
        return equipment;
    }

    private void updateActionStates() {
        IEquipment equipment = getSelectedEquipment();
        boolean enabled;
        boolean enableWarm = false;
        boolean removeEnabled = false;
        boolean shutdownEnabled = false;
        if (equipment == null || equipment.getStatus() == null) {
            enabled = false;
            enableWarm = false;
            shutdownEnabled = false;
        } else {
            EquipmentState state = equipment.getStatus().getState();
            enabled =
                state != EquipmentState.BOOTING && state != EquipmentState.ERROR
                    && state != EquipmentState.LC_NOT_INITIALIZED;
            removeEnabled = !enabled;
            shutdownEnabled = enabled;

            if (equipment.isInWarmMode()) {
                enabled = false;
                enableWarm = true;
                shutdownEnabled = true;
            }
        }

        actionProcessFileToFlash.setEnabled(enabled);
        actionDumpFlashArea.setEnabled(enabled);
        actionEraseFlashArea.setEnabled(enabled);
        actionReadGDFSArea.setEnabled(enabled);
        actionWriteGDFSArea.setEnabled(enabled);
        actionReadTrimArea.setEnabled(enabled);
        actionWriteTrimArea.setEnabled(enabled);
        actionAuthenticateCertificate.setEnabled(enabled);
        actionAuthenticateControlKeys.setEnabled(enabled);
        actionShutdown.setEnabled(enabled);
        actionOpenEditor.setEnabled(enabled);
        actionShutdown.setEnabled(shutdownEnabled);
        actionOpenEditorHandleDump.setEnabled(enableWarm);

        for (RebootAction action : rebootActions) {
            action.setEnabled(enabled);
        }

        actionRemove.setEnabled(removeEnabled);
    }

    private void updateActiveProfile() {
        ServiceReference[] references = null;

        try {
            references = Activator.getBundleContext().getServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        if (references != null) {
            IBackendService backendService = (IBackendService) Activator.getBundleContext().getService(references[0]);
            IConfigurationService configService = backendService.getConfigurationService();

            IConfigurationRecord[] records = configService.getRecords(new String[] {
                "ActiveProfile"});
            if (records != null && records.length > 0) {
                labelActiveProfile.setText("Connection Profile: " + records[0].getValue("alias"));
                labelActiveProfile.pack();
            }

            Activator.getBundleContext().ungetService(references[0]);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {

        if (newRecord.getName().equalsIgnoreCase("Monitor")) {
            actionAcceptRemoteClients.setChecked("true".equalsIgnoreCase(newRecord.getValue("acceptClients")));
            boolean acceptConnections = "true".equalsIgnoreCase(newRecord.getValue("acceptEquipments"));
            actionAcceptConnections.setChecked(acceptConnections);
            // TODO fix setAcceptEquipments(false) in validateAcceptConnections because it's causes
            // strange behavior after setAcceptEquipments(true)
            // if (acceptConnections) {
            // // validateAcceptConnections();
            // }
        }

        refreshView();
    }

    /**
     * Verifies that path to LCD and LCM are set before accepting ME connections. (to avoid some
     * eclipse crashes)
     */
    /*
    private void validateAcceptConnections() {
        String lcdPath = System.getProperty(FlashKitPreferenceConstants.BACKEND_LCD_FILE);
        String lcmPath = System.getProperty(FlashKitPreferenceConstants.BACKEND_LCM_FILE);
        if (lcdPath != null && lcmPath != null) {
            File lcdFile = new File(lcdPath);
            File lcmFile = new File(lcmPath);
            if (lcdFile.exists() && lcdFile.isFile() && lcmFile.exists() && lcmFile.isFile()) {
                return;
            }
        }
        ServiceReference[] references = null;

        try {
            references = Activator.getBundleContext().getServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        if (references != null) {
            IBackendService backendService = (IBackendService) Activator.getBundleContext().getService(references[0]);
            backendService.setAcceptEquipments(false);
            actionAcceptConnections.setToolTipText("Accepting equipment connections will not be "
                + "possible without specifying loader communication libraries");
        }

    }
    */

    /**
     * {@inheritDoc}
     */
    public void equipmentTaskDone(IEquipmentTask task, EquipmentTaskResult result, long duration) {

    }
}
