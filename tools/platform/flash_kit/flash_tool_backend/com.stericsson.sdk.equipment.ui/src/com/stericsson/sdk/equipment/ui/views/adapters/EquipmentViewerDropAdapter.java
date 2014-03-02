package com.stericsson.sdk.equipment.ui.views.adapters;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerDropAdapter;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.TransferData;
import org.eclipse.ui.dialogs.ListDialog;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;
import com.stericsson.sdk.equipment.ui.jobs.ProcessFileJob;
import com.stericsson.sdk.equipment.ui.jobs.WriteGlobalDataSetJob;

/**
 * @author xtomlju
 */
public class EquipmentViewerDropAdapter extends ViewerDropAdapter {

    /**
     * Constructor.
     * 
     * @param viewer
     *            The viewer
     */
    public EquipmentViewerDropAdapter(Viewer viewer) {
        super(viewer);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void dragOver(DropTargetEvent event) {
        Object target = determineTarget(event);
        if (target == null) {
            return;
        }
        IEquipment equipment = (IEquipment) target;
        EquipmentState state = equipment.getStatus().getState();
        if (state == EquipmentState.BOOTING || state == EquipmentState.ERROR
            && state != EquipmentState.LC_NOT_INITIALIZED) {
            event.detail = DND.ERROR_CANNOT_INIT_DROP;
        } else {
            event.detail = DND.DROP_COPY;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void drop(DropTargetEvent event) {
        Object o = event.data;
        if (o instanceof String[] && FileTransfer.getInstance().isSupportedType(event.currentDataType)) {
            String[] words = (String[]) o;
            if (words.length != 1) {
                MessageDialog.openError(getViewer().getControl().getShell(), "Drop failed", "Can only drop one file");
                return;
            }
            IEquipment target = (IEquipment) determineTarget(event);

            String filename = words[0];
            if (filename.endsWith(".zip")) {
                dropFlashArchiveFile(target, filename);
            } else if (filename.endsWith(".gdf")) {
                dropParameterStorageFile(target, filename);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performDrop(Object data) {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean validateDrop(Object target, int operation, TransferData transferType) {
        return true;
    }

    private void dropFlashArchiveFile(IEquipment target, String filename) {
        ProcessFileJob job = new ProcessFileJob(filename, target);
        job.schedule();
    }

    private void dropParameterStorageFile(IEquipment target, String filename) {

        target.updateModel(EquipmentModel.FLASH);
        ListDialog dialog = new ListDialog(getViewer().getControl().getShell());
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select area");
        dialog.setMessage("Select parameter storagae area");
        dialog.setLabelProvider(new LabelProvider() {
            public String getText(Object element) {
                return (String) element;
            };
        });
        dialog.setInput(((IEquipmentFlashModel) target.getModel(EquipmentModel.FLASH)).getFlashDeviceNames(null));
        if (dialog.open() == Dialog.OK) {
            WriteGlobalDataSetJob job = new WriteGlobalDataSetJob(filename, target, (String) dialog.getResult()[0]);
            job.schedule();
        }

    }

}
