package com.stericsson.sdk.equipment.ui.dialogs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;

/**
 * superclass for input dialogs that involves areas
 * 
 * @author xdancho
 * 
 */
public abstract class AreaDialog extends InputDialog implements IPropertyChangeListener {

    /** */
    protected static final String AREA_ALL = "area_all";

    /** */
    protected static final String LENGTH = "length";

    /** */
    protected static final String ERROR_LENGTH = "error_length";

    /** */
    protected static final String ERROR_START = "error_start";

    /** */
    protected static final String ERROR_BOUNDARY = "error_boundary";

    /** */
    protected static final String START = "start";

    /** */
    protected static final String DEVICE_PATH = "device_path";

    List<String> devices = new ArrayList<String>();

    Map<String, FieldEditor> editorMap = new HashMap<String, FieldEditor>();

    Set<FieldEditor> errorList = new HashSet<FieldEditor>();

    /** */
    protected String defaultDevice;

    /** */
    protected String defaultLength;

    /** */
    protected IEquipment equipment;

    /**
     * get start value
     * 
     * @return start
     */
    public String getStart() {
        return ps.getString(START);
    }

    /**
     * get length value
     * 
     * @return length
     */
    public String getLength() {
        return ps.getString(LENGTH);
    }

    /**
     * get device path value
     * 
     * @return device path
     */
    public String getDevicePath() {
        return ps.getString(DEVICE_PATH);
    }

    /**
     * constructor
     * 
     * @param parent
     *            the parent shell
     * @param info
     *            the title and the info of the dialog
     * @param pEquipment
     *            the equipment to retrieve the devices to operate on
     */
    public AreaDialog(Shell parent, String info, IEquipment pEquipment) {
        super(parent, info);
        equipment = pEquipment;
        titleText = info;
    }

    /**
     * {@inheritDoc}
     */
    public void propertyChange(PropertyChangeEvent event) {

        if (!initialized) {
            return;
        }

        FieldEditor fe = null;
        if (event.getProperty().equals(FieldEditor.VALUE)) {
            if (ps.needsSaving()) {
                try {
                    ps.save();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
            fe = (FieldEditor) event.getSource();
            String devicePath = ps.getString(DEVICE_PATH);

            if (fe.getPreferenceName().equals(DEVICE_PATH)) {
                devicePath = (String) event.getNewValue();
            }

            IEquipmentFlashModel model = (IEquipmentFlashModel) equipment.getModel(EquipmentModel.FLASH);
            String deviceLength = String.valueOf(model.getLength(devicePath));// TODO: should be //
            // total, when loader returns correct paths the device length should be changed to total
            // length
            // String totalLength = String.valueOf(model.getLength(devicePath));

            validateStartValues(event, fe, deviceLength);

            validateLengthValues(event, fe, deviceLength);

            if (fe.getPreferenceName().equals(AREA_ALL)) {
                if ((Boolean) event.getNewValue()) {

                    updateAreaParameters("0", deviceLength);
                    toggleInputFields(false);
                } else {
                    toggleInputFields(true);
                }

            }

            if (fe.getPreferenceName().equals(DEVICE_PATH)) {
                fe.store();
                updateAreaParameters("0", String.valueOf(model.getLength((String) event.getNewValue())));
            }

        }
        customPropertyChange(event);

        if (fe != null) {
            fe.store();
        }
    }

    private void validateLengthValues(PropertyChangeEvent event, FieldEditor fe, String deviceLength) {
        if (fe.getPreferenceName().equals(LENGTH)) {
            if (!validateValues((String) event.getNewValue(), fe)) {
                addError(LENGTH, "The input must be decimal or hexadecimal with the prefix 0x");
                setLabelColor(fe, SWT.COLOR_RED);
            } else {
                removeError(LENGTH);
                setLabelColor(fe, SWT.COLOR_BLACK);

                String errorMessage = validateLength((String) event.getNewValue(), deviceLength);

                if (errorMessage != null) {
                    addError(ERROR_LENGTH, errorMessage);
                    setLabelColor(fe, SWT.COLOR_RED);
                } else {
                    removeError(ERROR_LENGTH);
                    setLabelColor(fe, SWT.COLOR_BLACK);
                }

                errorMessage = validateBounds(ps.getString(START), (String) event.getNewValue(), "0", deviceLength);

                if (errorMessage != null) {
                    addError(ERROR_BOUNDARY, errorMessage);
                    setLabelColor(fe, SWT.COLOR_RED);
                    setLabelColor(editorMap.get(START), SWT.COLOR_RED);
                } else {
                    removeError(ERROR_BOUNDARY);
                    setLabelColor(fe, SWT.COLOR_BLACK);
                    setLabelColor(editorMap.get(START), SWT.COLOR_BLACK);
                }
            }

        }
    }

    private void validateStartValues(PropertyChangeEvent event, FieldEditor fe, String deviceLength) {
        if (fe.getPreferenceName().equals(START)) {
            if (!validateValues((String) event.getNewValue(), fe)) {

                addError(START, "The input must be decimal or hexadecimal with the prefix 0x");
                setLabelColor(fe, SWT.COLOR_RED);
            } else {
                removeError(START);
                setLabelColor(fe, SWT.COLOR_BLACK);

                String errorMessage = validateStart((String) event.getNewValue(), deviceLength);

                if (errorMessage != null) {
                    addError(ERROR_START, errorMessage);
                    setLabelColor(fe, SWT.COLOR_RED);
                } else {
                    removeError(ERROR_START);
                    setLabelColor(fe, SWT.COLOR_BLACK);
                }

                String error = validateBounds((String) event.getNewValue(), ps.getString(LENGTH), "0", deviceLength);
                if (error != null) {
                    addError(ERROR_BOUNDARY, error);
                    setLabelColor(fe, SWT.COLOR_RED);
                    setLabelColor(editorMap.get(LENGTH), SWT.COLOR_RED);
                } else {
                    removeError(ERROR_BOUNDARY);
                    setLabelColor(fe, SWT.COLOR_BLACK);
                    setLabelColor(editorMap.get(LENGTH), SWT.COLOR_BLACK);
                }
            }

        }
    }

    private void toggleInputFields(boolean state) {
        editorMap.get(START).setEnabled(state, inputGroup);
        editorMap.get(LENGTH).setEnabled(state, inputGroup);
    }

    /**
     * should be implemented by subclasses that has added fieldEditors
     * 
     * @param event
     *            {@link PropertyChangeEvent}
     */
    public abstract void customPropertyChange(PropertyChangeEvent event);

    /**
     * {@inheritDoc}
     */
    protected abstract void createInputArea(Composite parent);

    private void updateAreaParameters(String start, String length) {
        ps.setDefault(START, start);
        ps.setDefault(LENGTH, length);
        editorMap.get(START).loadDefault();
        editorMap.get(LENGTH).loadDefault();
    }

    private boolean validateValues(String input, FieldEditor fe) {

        try {

            if (HexUtilities.hasHexPrefix(input)) {
                input = HexUtilities.removeHexPrefix(input);
                Long.parseLong(input, 16);
            } else {
                Long.parseLong(input);
            }

            return true;
        } catch (Exception e) {

            return false;
        }

    }

    /**
     * set the label color of a FieldEditor
     * 
     * @param fe
     *            the FieldEditor
     * @param color
     *            the color
     */
    protected void setLabelColor(FieldEditor fe, int color) {
        fe.getLabelControl(inputGroup).setForeground(getShell().getDisplay().getSystemColor(color));
    }

    private String validateLength(String length, String dLength) {
        long inputLength;
        long deviceLength = Long.parseLong(dLength);
        if (HexUtilities.hasHexPrefix(length)) {
            length = HexUtilities.removeHexPrefix(length);
            inputLength = Long.parseLong(length, 16);
        } else {
            inputLength = Long.parseLong(length);
        }

        if(inputLength<=0) {
            return "The length field must be greater than zero";
        }

        if ((deviceLength) < (inputLength)) {
            return "The length field contains a value that is bigger than the device area";
        } 

        return null;
    }

    private String validateStart(String start, String dLength) {

        long inputStart;
        long deviceLength = Long.parseLong(dLength);
        if (HexUtilities.hasHexPrefix(start)) {
            start = HexUtilities.removeHexPrefix(start);
            inputStart = Long.parseLong(start, 16);
        } else {
            inputStart = Long.parseLong(start);
        }

        if(inputStart<0) {
            return "The start offset field must be greater than or equal to zero";
        }

        if (deviceLength < inputStart) {
            return "The start offset field contains a value that is bigger than the device area";
        }
        return null;
    }

    private String validateBounds(String start, String length, String pDeviceStart, String topDeviceLength) {

        long deviceLength = Long.parseLong(topDeviceLength);
        long inputStart = 0;
        long inputLength = 0;
        if (HexUtilities.hasHexPrefix(start)) {
            start = HexUtilities.removeHexPrefix(start);
            inputStart = Long.parseLong(start, 16);
        } else {
            inputStart = Long.parseLong(start);
        }

        if (HexUtilities.hasHexPrefix(length)) {
            length = HexUtilities.removeHexPrefix(length);
            inputLength = Long.parseLong(length, 16);
        } else {
            inputLength = Long.parseLong(length);
        }

        boolean overflow = (inputStart>=0 && inputLength>=0 && inputStart+inputLength<0);
        if ((deviceLength < inputStart + inputLength) || overflow) {
            return "The start offset value together with the length value is bigger than the device area";
        } else {
            return null;
        }

    }

    /**
     * {@inheritDoc}
     */
    protected void storeValues() {
        for (FieldEditor fe : editorMap.values()) {
            fe.store();

        }

    }
}
