package com.stericsson.sdk.equipment.ui.dialogs;

import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.ComboFieldEditor;
import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.jface.preference.StringFieldEditor;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;

/**
 * Erase area dialog
 * 
 * @author xdancho
 * 
 */
public class EraseAreaDialog extends AreaDialog {

    private static final String FILTER = "PartitioneMMC";

    /**
     * constructor
     * 
     * @param parent
     *            the shell
     * @param pEquipment
     *            the equipment to retrieve the devices to operate on
     */
    public EraseAreaDialog(Shell parent, IEquipment pEquipment) {
        super(parent, "Erase Flash Area", pEquipment);

    }

    /**
     * {@inheritDoc}
     */
    protected void createInputArea(Composite parent) {
        parent.setData("GD_COMP_NAME", "EraseAreaDialog_InputArea_Composite");

        IEquipmentFlashModel model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
        if (model == null || model.getFlashDeviceNames(FILTER) == null || model.getFlashDeviceNames(FILTER).length == 0) {
            equipment.updateModel(EquipmentModel.FLASH);
            model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
        }
        String[] deviceList = model.getFlashDeviceNames(FILTER);
        String[][] devicesValues = new String[deviceList.length][2];
        for (int i = 0; i < deviceList.length; i++) {
            devicesValues[i][0] = deviceList[i] + ", Size: " + model.getLength(deviceList[i]) + " bytes";
            devicesValues[i][1] = deviceList[i];
        }

        ComboFieldEditor erasePath = new ComboFieldEditor(DEVICE_PATH, "Device path", devicesValues, parent);
        erasePath.getLabelControl(parent).setData("GD_COMP_NAME", "EraseAreaDialog_DevicePath_Label");
        defaultDevice = devicesValues[0][1];
        editorMap.put(DEVICE_PATH, erasePath);

        StringFieldEditor start = new StringFieldEditor(START, "Start offset", parent);
        start.getTextControl(parent).setData("GD_COMP_NAME", "EraseAreaDialog_StartOffset_Text");
        start.getLabelControl(parent).setData("GD_COMP_NAME", "EraseAreaDialog_StartOffset_Label");
        editorMap.put(START, start);

        StringFieldEditor length = new StringFieldEditor(LENGTH, "Length to erase", parent);
        length.getTextControl(parent).setData("GD_COMP_NAME", "EraseAreaDialog_LengthToErase_Text");
        length.getLabelControl(parent).setData("GD_COMP_NAME", "EraseAreaDialog_LengthToErase_Label");
        defaultLength = String.valueOf(model.getLength(deviceList[0]));
        editorMap.put(LENGTH, length);

        BooleanFieldEditor eraseAll = new BooleanFieldEditor(AREA_ALL, "Erase complete area", parent);
        eraseAll.getLabelControl(parent).setData("GD_COMP_NAME", "EraseAreaDialog_EraseCompleteArea_Button");
        editorMap.put(AREA_ALL, eraseAll);

        // this will fix the layout configuration changed by the BooleanFieldEditor
        ((GridLayout) parent.getLayout()).numColumns = 2;

        for (FieldEditor fe : editorMap.values()) {
            fe.setPreferenceStore(ps);
            fe.setPropertyChangeListener(this);
            fe.loadDefault();
        }
    }

    /**
     * {@inheritDoc}
     */
    protected boolean validatedBeforeExit() {
        // TODO Auto-generated method stub
        return true;
    }

    /**
     * {@inheritDoc}
     */
    public void customPropertyChange(PropertyChangeEvent event) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    protected void setDefaultValues() {
        ps.setDefault(DEVICE_PATH, defaultDevice);
        ps.setDefault(START, 0);
        ps.setDefault(LENGTH, defaultLength);
        for (FieldEditor fe : editorMap.values()) {
            fe.loadDefault();
        }
    }

}
