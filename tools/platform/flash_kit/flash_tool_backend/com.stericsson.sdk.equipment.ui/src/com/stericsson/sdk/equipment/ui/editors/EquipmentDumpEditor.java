package com.stericsson.sdk.equipment.ui.editors;

import java.util.Locale;

import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialogWithToggle;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.part.MultiPageEditorPart;

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
 * Handle Dump page
 * 
 * @author esrimpa
 * 
 */
public class EquipmentDumpEditor extends MultiPageEditorPart implements IResourceChangeListener, IBackendViewer {

    private FormToolkit toolkit;

    private EquipmentEditorDumpPage dumpPage;

    /**
     * Creates a multi-page editor
     */
    public EquipmentDumpEditor() {
        super();
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void createPages() {
        toolkit = new FormToolkit(getContainer().getDisplay());
        dumpPage =
            new EquipmentEditorDumpPage((IEquipment) getEditorInput().getAdapter(IEquipment.class), getContainer(),
                toolkit);

        // Add Page
        setPageText(addPage(dumpPage.getPageComposite()), "Dump Information");
        setPartName(getEditorInput().getName().toUpperCase(Locale.getDefault()));
    }

    /**
     * {@inheritDoc}
     * 
     */
    public void dispose() {
        if (toolkit != null) {
            toolkit.dispose();
        }
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
        BackendViewerAdapter.getInstance().removeViewer(this);
        super.dispose();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void doSave(IProgressMonitor monitor) {

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void doSaveAs() {

    }

    /**
     * {@inheritDoc}
     */
    public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
        if (!(editorInput instanceof IFileEditorInput)) {
            throw new PartInitException("Invalid Input: Must be IFileEditorInput");
        }
        super.init(site, editorInput);
        BackendViewerAdapter.getInstance().addViewer(this);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }

    /**
     * Closes all project files on project close. {@inheritDoc}
     */
    public void resourceChanged(IResourceChangeEvent event) {

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
        //NA
    }

    /**
     * Equipment connected
     * 
     * @param equipment
     *            The equipment
     * @see com.stericsson.sdk.equipment.ui.listeners.IBackendViewer#equipmentConnected(com.stericsson
     *      .sdk.equipment.IEquipment)
     */
    public void equipmentConnected(IEquipment equipment) {
        // if we chose not to close the editor on disconnect we want to update the equipment upon
        // new connect
        IEquipment selectedEquipment = getSelectedEquipment();
        if (equipment != null && selectedEquipment != null && equipment.toString().equals(selectedEquipment.toString())) {
            setInput(new EquipmentEditorInput(equipment));
            dumpPage.setEquipment(equipment);

        }

    }

    /**
     * {@inheritDoc}
     */
    public void equipmentDisconnected(IEquipment equipment) {
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
                        getSite().getPage().closeEditor(EquipmentDumpEditor.this, false);
                    } else {
                        dumpPage.setEquipment(null);
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
        if (task != null && equipment != null && task.getEquipment() == getSelectedEquipment()) {
            String taskId = task.getId();
            if (taskId != null) {
                taskId = taskId.toUpperCase(Locale.getDefault());
                if (taskId.startsWith("LISTFILE")) {
                    dumpPage.updatePage();
                }
            }

        }
    }
}
