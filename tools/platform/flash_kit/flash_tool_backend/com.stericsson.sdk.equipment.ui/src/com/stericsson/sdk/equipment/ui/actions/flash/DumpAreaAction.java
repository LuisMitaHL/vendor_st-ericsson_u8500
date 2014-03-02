package com.stericsson.sdk.equipment.ui.actions.flash;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.widgets.Display;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.dialogs.DumpAreaDialog;
import com.stericsson.sdk.equipment.ui.jobs.DumpAreaJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Dump Area Action
 * 
 * @author xolabju
 * 
 */
public class DumpAreaAction extends BackendAction {

    /**
     * Constructor
     * 
     * @param pViewer
     *            the equipment viewer
     */
    public DumpAreaAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Dump Flash Area...");
        setToolTipText("Dump specified flash device area from selected equipment to file");
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.EQUIPMENT_ICON_DUMP));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        IEquipment equipment = viewer.getSelectedEquipment();
        if (equipment != null) {

            DumpAreaDialog dialog = new DumpAreaDialog(Display.getDefault().getActiveShell(), equipment);

            if (dialog.open() == IDialogConstants.OK_ID) {

                String startArg = dialog.getStart();

                String lengthArg = dialog.getLength();
                String deviceArg = dialog.getDevicePath();

                String savePathArg = dialog.getSavePath();

                String skipRedundant = Boolean.toString(dialog.getSkipRedundant());

                String includeBadBlocks = Boolean.toString(dialog.getIncludeBadBlocks());

                IEquipmentFlashModel model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
                startArg = String.valueOf(Long.parseLong(startArg) + model.getStart(deviceArg));

                // hardcode for /flash0 for now
                if (deviceArg.equals("/flash0/")) {
                    lengthArg = "-1";
                } else {
                    // loader does only react when using /flash0/ as device
                    deviceArg = "/flash0/";
                }

                DumpAreaJob job =
                    new DumpAreaJob(deviceArg, startArg, lengthArg, savePathArg, equipment, skipRedundant,
                        includeBadBlocks);
                job.schedule();
            }
        }
    }
}
