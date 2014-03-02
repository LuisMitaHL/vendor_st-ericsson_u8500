package com.stericsson.sdk.equipment.dummy.internal.task;

import java.io.File;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * @author xdancho
 * 
 */
public class DummyEquipmentReadGlobalDataSetTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String sourcePath;

    private String devicePath;

    private boolean useBulk;

    static final String SOURCE_PATH =
        System.getProperty("java.io.tmpdir") + System.getProperty("file.separator") + "flashArchive.zip";

    static File file = new File(SOURCE_PATH);

    static long lengthFile = file.length();

    static final String[] ARGUMENTS = new String[] {
        "PARAMETER_STORAGE_READ_GLOBAL_DATA_SET", "/flash0/GDFS", String.valueOf(lengthFile), "PC" + SOURCE_PATH};

    static final String[] ARGUMENTS_BULK = new String[] {
        "PARAMETER_STORAGE_READ_GLOBAL_DATA_SET", "/flash0/GDFS", String.valueOf(lengthFile), "ME" + SOURCE_PATH};

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public DummyEquipmentReadGlobalDataSetTask(DummyEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "Read Global Data Set finished successfully";

    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#cancel() {@inheritDoc}
     */
    public void cancel() {
        // TODO Auto-generated method stub

    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#execute() {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskMessage("Read Global Data Set: devicePath: " + devicePath + "," + "sourcePath:" + sourcePath);
        notifyTaskStart();

        try {
            mobileEquipment.getMock().setResult(0);
        } catch (Exception e) {
            resultCode = 1;
            resultMessage = e.getMessage();
        }

        int result =
            (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_SET, new Object[] {
                devicePath, sourcePath, useBulk, null}, this);

        if (result != 0) {
            resultCode = result;
            try {
                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#getId() {@inheritDoc}
     */
    public String getId() {
        return CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name();
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#isCancelable() {@inheritDoc}
     */
    public boolean isCancelable() {
        // TODO Auto-generated method stub
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#setArguments(java.lang.String[])
     *      {@inheritDoc}
     */
    public void setArguments(String[] arguments) {

        arguments = ARGUMENTS_BULK;
        devicePath = arguments[1];
        // dataLength = Integer.parseInt(arguments[2]);
        sourcePath = arguments[3];
        if (arguments[3].substring(0, 2).equalsIgnoreCase("PC")
            || !(arguments[3].substring(0, 2).equalsIgnoreCase("ME"))) {
            this.useBulk = false;
        } else {
            this.useBulk = true;
        }

    }

}
