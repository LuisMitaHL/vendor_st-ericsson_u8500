package com.stericsson.sdk.equipment.ui.dialogs;

import java.io.File;
import java.io.IOException;

import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.ComboFieldEditor;
import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.jface.preference.FileFieldEditor;
import org.eclipse.jface.preference.StringFieldEditor;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;

/**
 * dump area dialog
 * 
 * @author xdancho
 * 
 */
public class DumpAreaDialog extends AreaDialog {

    private static final String SAVE_PATH = "save_path";

    private static final String FILTER = "PartitioneMMC";

    private static final String SKIP_REDUNDANT = "skip_redundnat";

    private static final String INCLUDE_BAD_BLOCKS = "include_bad_blocks";

    /**
     * constructor
     * 
     * @param parent
     *            the shell
     * @param pEquipment
     *            the equipment to retrieve devices from
     */
    public DumpAreaDialog(Shell parent, IEquipment pEquipment) {
        super(parent, "Dump Flash Area", pEquipment);
    }

    /**
     * get the save path for the dump
     * 
     * @return the path
     */
    public String getSavePath() {
        return ps.getString(SAVE_PATH);
    }

    /**
     * 
     * 
     * @return skip redundant
     */
    public Boolean getSkipRedundant() {
        return ps.getBoolean(SKIP_REDUNDANT);
    }

    /**
     * 
     * 
     * @return include bad blocks
     */
    public Boolean getIncludeBadBlocks() {
        return ps.getBoolean(INCLUDE_BAD_BLOCKS);
    }

    /**
     * {@inheritDoc}
     */
    protected void createInputArea(Composite parent) {
        parent.setData("GD_COMP_NAME", "DumpAreaDialog_InputArea_Composite");

        IEquipmentFlashModel model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
        if (model == null) {
            equipment.updateModel(EquipmentModel.FLASH);
            model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
        }
        String[] deviceList = model.getFlashDeviceNames(FILTER);
        String[][] devicesValues = new String[deviceList.length][2];
        for (int i = 0; i < deviceList.length; i++) {
            devicesValues[i][0] = deviceList[i] + ", Size: " + model.getLength(deviceList[i]) + " bytes";
            devicesValues[i][1] = deviceList[i];
        }

        ComboFieldEditor dumpPath = new ComboFieldEditor(DEVICE_PATH, "Device path", devicesValues, parent);
        dumpPath.getLabelControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_DevicePath_Label");
        defaultDevice = devicesValues[0][1];
        editorMap.put(DEVICE_PATH, dumpPath);

        StringFieldEditor start = new StringFieldEditor(START, "Start offset", parent);
        start.getTextControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_StartOffset_Text");
        start.getLabelControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_StartOffset_Label");

        editorMap.put(START, start);

        StringFieldEditor length = new StringFieldEditor(LENGTH, "Length of dump", parent);
        length.getTextControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_LengthOfDump_Text");
        length.getLabelControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_LengthOfDump_Label");
        defaultLength = String.valueOf(model.getLength(deviceList[0]));
        editorMap.put(LENGTH, length);

        BooleanFieldEditor skipRedundant = new BooleanFieldEditor(SKIP_REDUNDANT, "Skip redundant area", 1, parent);
        skipRedundant.getLabelControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_SkipRedundantArea_Button");
        editorMap.put(SKIP_REDUNDANT, skipRedundant);

        BooleanFieldEditor includeBadBlocks =
            new BooleanFieldEditor(INCLUDE_BAD_BLOCKS, "Include bad blocks", 1, parent);
        includeBadBlocks.getLabelControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_IncludeBadBlocks_Button");
        editorMap.put(INCLUDE_BAD_BLOCKS, includeBadBlocks);

        BooleanFieldEditor dumpAll = new BooleanFieldEditor(AREA_ALL, "Dump complete area", 1, parent);
        dumpAll.getLabelControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_DumpCompleteArea_Button");
        editorMap.put(AREA_ALL, dumpAll);

        FileFieldEditor savePath = new FileFieldEditor(SAVE_PATH, "Path to save dump", parent);
        savePath.getTextControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_PathToSaveDump_Text");
        savePath.getLabelControl(parent).setData("GD_COMP_NAME", "DumpAreaDialog_PathToSaveDump_Label");
        savePath.setEmptyStringAllowed(false);
        savePath.getTextControl(inputGroup).addModifyListener(new ModifyListener() {

            public void modifyText(ModifyEvent e) {
                editorMap.get(SAVE_PATH).store();
                if (ps.needsSaving()) {
                    try {
                        ps.save();
                    } catch (IOException e1) {
                        // TODO Auto-generated catch block
                        e1.printStackTrace();
                    }
                }

                if ((ps.getString(SAVE_PATH).equals(""))) {

                    setLabelColor(editorMap.get(SAVE_PATH), SWT.COLOR_RED);
                    addError(SAVE_PATH, "A valid path for the dump must be specified");

                } else {

                    String path = ps.getString(SAVE_PATH);
                    String dir = null;
                    int indexOfFile = path.lastIndexOf(System.getProperty("file.separator"));
                    int indexOfPath = path.lastIndexOf(System.getProperty("path.separator"));

                    if (indexOfFile == -1 && indexOfPath == -1) {
                        setLabelColor(editorMap.get(SAVE_PATH), SWT.COLOR_RED);
                        addError(SAVE_PATH, "A valid path for the dump must be specified");
                    } else {
                        if (indexOfFile < indexOfPath) {
                            dir = path.substring(0, indexOfPath + 1);
                        } else {
                            dir = path.substring(0, indexOfFile + 1);
                        }
                        File dirFile = new File(dir);
                        File pathFile = new File(path);
                        if (dirFile.exists() && !pathFile.isDirectory()) {
                            removeError(SAVE_PATH);
                            setLabelColor(editorMap.get(SAVE_PATH), SWT.COLOR_BLACK);
                        } else {
                            setLabelColor(editorMap.get(SAVE_PATH), SWT.COLOR_RED);
                            addError(SAVE_PATH, "A valid path for the dump must be specified");
                        }
                    }

                }

            }
        });
        editorMap.put(SAVE_PATH, savePath);

        for (FieldEditor fe : editorMap.values()) {

            fe.setPreferenceStore(ps);
            fe.setPropertyChangeListener(this);
            fe.fillIntoGrid(parent, 3);

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
    }

    /**
     * {@inheritDoc}
     */
    protected void setDefaultValues() {
        ps.setDefault(DEVICE_PATH, defaultDevice);
        ps.setDefault(START, 0);
        ps.setDefault(LENGTH, defaultLength);
        ps.setDefault(SAVE_PATH, "");
        for (FieldEditor fe : editorMap.values()) {
            fe.loadDefault();
        }
    }

}
