package com.stericsson.sdk.equipment.ui.actions.flash;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.widgets.Display;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.dialogs.EraseAreaDialog;
import com.stericsson.sdk.equipment.ui.jobs.EraseAreaJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Erase Area Action
 * 
 * @author xolabju
 * 
 */
public class EraseAreaAction extends BackendAction {

    /**
     * Constructor
     * 
     * @param pViewer
     *            the equipment viewer
     */
    public EraseAreaAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Erase Flash Area...");
        setToolTipText("Erase specified flash device area on selected equipment");
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.EQUIPMENT_ICON_ERASE));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        IEquipment equipment = viewer.getSelectedEquipment();
        if (equipment != null) {
            EraseAreaDialog dialog = new EraseAreaDialog(Display.getDefault().getActiveShell(), equipment);

            if (dialog.open() == IDialogConstants.OK_ID) {

                String startArg = dialog.getStart();
                String lengthArg = dialog.getLength();
                String deviceArg = dialog.getDevicePath();

                IEquipmentFlashModel model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
                startArg = String.valueOf(Long.parseLong(startArg) + model.getStart(deviceArg));

                // hardcode for /flash0 for now
                if (deviceArg.equals("/flash0/")) {
                    lengthArg = "-1";
                }
                startArg = "0";

                EraseAreaJob job = new EraseAreaJob(deviceArg, startArg, lengthArg, equipment);
                job.schedule();

            }
        }
    }
}
