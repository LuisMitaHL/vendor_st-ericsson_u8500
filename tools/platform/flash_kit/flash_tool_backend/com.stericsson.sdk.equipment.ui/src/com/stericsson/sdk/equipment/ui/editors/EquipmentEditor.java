package com.stericsson.sdk.equipment.ui.editors;

import java.util.Locale;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialogWithToggle;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.part.MultiPageEditorPart;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.BackendViewerAdapter;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 *
 * @author xtomlju
 *
 */
public class EquipmentEditor extends MultiPageEditorPart implements IBackendViewer {

    private FormToolkit toolkit;

    private EquipmentEditorOverviewPage overviewPage;

    private EquipmentEditorStoragePage storagePage;

    private EquipmentEditorSecurityPage securityPage;

    private EquipmentEditorGDFSPage gdfsPage;

    /**
     * Creates a multi-page editor example.
     */
    public EquipmentEditor() {
        super();

        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        window.getPartService().addPartListener(new IPartListener() {
            public void partOpened(IWorkbenchPart pPart) {
            }

            public void partDeactivated(IWorkbenchPart pPart) {
            }

            public void partClosed(IWorkbenchPart pPart) {
                if (pPart == EquipmentEditor.this) {
                    gdfsPage.saveState();
                }
            }

            public void partBroughtToTop(IWorkbenchPart pPart) {
            }

            public void partActivated(IWorkbenchPart pPart) {
            }
        });
    }

    /**
     * Creates the pages of the multi-page editor.
     */
    protected void createPages() {
        toolkit = new FormToolkit(getContainer().getDisplay());

        overviewPage =
            new EquipmentEditorOverviewPage((IEquipment) getEditorInput().getAdapter(IEquipment.class), getContainer(),
                toolkit);
        storagePage =
            new EquipmentEditorStoragePage((IEquipment) getEditorInput().getAdapter(IEquipment.class), getContainer(),
                toolkit);
        securityPage =
            new EquipmentEditorSecurityPage((IEquipment) getEditorInput().getAdapter(IEquipment.class), getContainer(),
                toolkit);
        gdfsPage =
            new EquipmentEditorGDFSPage((IEquipment) getEditorInput().getAdapter(IEquipment.class), getContainer(),
                toolkit);

        // Add pages
        setPageText(addPage(overviewPage.getPageComposite()), "Overview");
        setPageText(addPage(storagePage.getPageComposite()), "Flash storage");
        setPageText(addPage(securityPage.getPageComposite()), "Security");
        setPageText(addPage(gdfsPage.getPageComposite()), "GDFS and TA");
        setPartName(getEditorInput().getName().toUpperCase(Locale.getDefault()));
    }

    /**
     * The <code>MultiPageEditorPart</code> implementation of this <code>IWorkbenchPart</code>
     * method disposes all nested editors. Subclasses may extend.
     */
    public void dispose() {
        // gdfsPage.saveState();

        if (gdfsPage != null) {
            gdfsPage.dispose();
        }
        if (toolkit != null) {
            toolkit.dispose();
        }
        BackendViewerAdapter.getInstance().removeViewer(this);
        super.dispose();
    }

    /**
     * Saves the multi-page editor's document. {@inheritDoc}
     */
    public void doSave(IProgressMonitor monitor) {
        getEditor(0).doSave(monitor);
    }

    /**
     * Saves the multi-page editor's document as another file. Also updates the text for page 0's
     * tab, and updates this multi-page editor's input to correspond to the nested editor's.
     */
    public void doSaveAs() {
    }

    /**
     * The <code>MultiPageEditorExample</code> implementation of this method checks that the input
     * is an instance of <code>IFileEditorInput</code>. {@inheritDoc}
     */
    public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
        if (!(editorInput instanceof IFileEditorInput)) {
            throw new PartInitException("Invalid Input: Must be IFileEditorInput");
        }
        super.init(site, editorInput);
        BackendViewerAdapter.getInstance().addViewer(this);
        // editorInput.getPersistable().saveState(memento)
        // editorInput.getPersistable().
        // editorInput.
    }

    /**
     * {@inheritDoc}
     */
    public boolean isSaveAsAllowed() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public void equipmentChanged(IEquipment equipment) {
        if (equipment != null && equipment == getSelectedEquipment()) {
            overviewPage.updateValues();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void equipmentConnected(IEquipment equipment) {
        // if we chose not to close the editor on disconnect we want to update the equipment upon
        // new connect
        IEquipment selectedEquipment = getSelectedEquipment();
        if (equipment != null && selectedEquipment != null && equipment.toString().equals(selectedEquipment.toString())) {
            setInput(new EquipmentEditorInput(equipment));
            overviewPage.setEquipment(equipment);
            storagePage.setEquipment(equipment);
            securityPage.setEquipment(equipment);
        }

    }

    /**
     * {@inheritDoc}
     */
    public void equipmentDisconnected(final IEquipment equipment) {
        if (equipment != null && equipment == getSelectedEquipment()) {
            getSite().getShell().getDisplay().asyncExec(new Runnable() {
                public void run() {
                    boolean close = false;
                    String eqName = getEditorInput().getName().toUpperCase(Locale.getDefault());
                    String key = FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT;
                    IPreferenceStore pStore = Activator.getDefault().getPreferenceStore();
                    String storeValue = pStore.getString(key);
                    if (FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_ASK.equals(storeValue)) {
                        MessageDialogWithToggle toggle =
                            MessageDialogWithToggle.openYesNoQuestion(getEditorSite().getShell(), eqName
                                + " disconnected", eqName
                                + " was disconnected.\nDo you want to close the associated editor?",
                                "Do not ask me again", false, pStore, key);
                        close = toggle.getReturnCode() == 2;
                        boolean remember = toggle.getToggleState();
                        if (remember) {
                            String newStoreValue =
                                close ? FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_CLOSE
                                    : FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_NO_CLOSE;
                            pStore.setValue(FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT, newStoreValue);
                        }
                    } else {
                        close = FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_CLOSE.equals(storeValue);
                    }

                    if (close) {
                        getSite().getPage().closeEditor(EquipmentEditor.this, false);
                    } else {
                        overviewPage.setEquipment(null);
                        storagePage.setEquipment(null);
                        securityPage.setEquipment(null);
                    }

                }
            });
        }

    }

    /**
     * {@inheritDoc}
     */
    public void equipmentMessage(IEquipment equipment, String message) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public IEquipment getSelectedEquipment() {
        IEditorInput input = getEditorInput();
        if (input != null && input instanceof EquipmentEditorInput) {
            return ((EquipmentEditorInput) input).getEquipment();
        }
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public void equipmentTaskDone(IEquipmentTask task, EquipmentTaskResult result, long duration) {
        IEquipment equipment = getSelectedEquipment();
        gdfsPage.equipmentTaskDone(task, result);
        if (task != null && equipment != null && task.getEquipment() == getSelectedEquipment()) {
            String taskId = task.getId();
            if (taskId != null) {
                taskId = taskId.toUpperCase(Locale.getDefault());
                if (taskId.startsWith("FLASH") && (taskId.contains("PROCESS_FILE") || taskId.contains("ERASE_AREA"))) {
                    storagePage.updatePage();
                } else if (taskId.equals(CommandName.SECURITY_BIND_PROPERTIES.name())
                    || taskId.equals(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name())
                    || taskId.equals(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name())) {
                    securityPage.updatePage();
                }
            }
        }

    }

}
