package com.stericsson.sdk.equipment.tasks;

import java.math.BigInteger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Task for dumping a flash area from a U8500 ME
 * 
 * @author xolabju
 * 
 */
public class EquipmentDumpAreaTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String path;

    private String areaPath;

    private long offset;

    private long length;

    /**
     * Determines if redundant area should be skipped and if bad blocks should be included
     */
    private int extraDumpParameters;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentDumpAreaTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        long start = System.currentTimeMillis();
        if (!(resultCode == ERROR)) {
            notifyTaskMessage("areaPath=" + areaPath + ", offset=" + offset + ", length=" + length + ", path=" + path);
            notifyTaskStart();
            mobileEquipment.setStatus(EquipmentState.DUMPING);
            int result = (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_DUMP_AREA, new Object[] {
                areaPath, offset, length, path, extraDumpParameters, true}, this);

            if (result != 0) {
                resultCode = result;
                resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
            } else {
                notifyTaskProgress(length, length);
            }
            mobileEquipment.setStatus(EquipmentState.IDLE);
        }
        long stop = System.currentTimeMillis();
        return new EquipmentTaskResult(resultCode, resultMessage, null, false, stop - start);
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.FLASH_DUMP_AREA.name();
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] arguments) {

        extraDumpParameters = 1;
        areaPath = arguments[2];

        int radix = 10;
        if (arguments[3].startsWith("0x")) {
            radix = 16;
            arguments[3] = arguments[3].substring(2);
        }

        BigInteger biOffset = new BigInteger(arguments[3], radix);

        radix = 10;
        if (arguments[4].startsWith("0x")) {
            radix = 16;
            arguments[4] = arguments[4].substring(2);
        } else if (arguments[4].equalsIgnoreCase("ALL")) {
            arguments[4] = "-1";
        }

        BigInteger biLength = new BigInteger(arguments[4], radix);

        offset = biOffset.longValue();
        length = biLength.longValue();
        path = arguments[5];

        checkRedundantArea(arguments[6], arguments[7]);
    }

    /**
     * There is possibility to dump redundant area and bad blocks. <br>
     * As default value is 1 - redundant area is skipped and blocks are included.
     * 
     * <pre>
     * Possible values are: 
     * - 0: dump flash without redundant area with bad blocks
     * - 1: dump flash including redundant area with bad blocks
     * - 2: dump flash without redundant area and without bad blocks 
     * - 3: dump flash including redundant area without bad blocks
     *</pre>
     * 
     * @param skipRedundantArea
     *            Determines if redundant area should be skipped
     * @param badBlocks
     *            Determines if bad blocks should be included
     */
    private void checkRedundantArea(String skipRedundantArea, String badBlocks) {
        if (skipRedundantArea(skipRedundantArea) && includeBadBlocks(badBlocks)) {
            extraDumpParameters = 0;
        } else if (!skipRedundantArea(skipRedundantArea) && includeBadBlocks(badBlocks)) {
            extraDumpParameters = 1;
        } else if (skipRedundantArea(skipRedundantArea) && !includeBadBlocks(badBlocks)) {
            extraDumpParameters = 2;
        } else if (!skipRedundantArea(skipRedundantArea) && !includeBadBlocks(badBlocks)) {
            extraDumpParameters = 3;
        }
    }

    /**
     * Method deciding if redundant area should be skiped.As default redundant area is skipped.
     * 
     * @param skipRedundantArea
     *            Parameter specifies if redundant area should be skipped or null if parameter was
     *            not specified, in this case it should be skipped
     * @return True if redundant area should be skipped
     */
    private boolean skipRedundantArea(String skipRedundantArea) {
        return skipRedundantArea.equalsIgnoreCase("true") || skipRedundantArea.equalsIgnoreCase("null");
    }

    /**
     * Method deciding if bad blocks should be included.As default bad blocks are included.
     * 
     * @param badBlocks
     *            Parameter specifies if bad blocks should be included or null if parameter was not
     *            specified, in this case it should be included
     * @return True if bad blocks should be included
     */
    private boolean includeBadBlocks(String badBlocks) {
        return badBlocks.equalsIgnoreCase("true") || badBlocks.equalsIgnoreCase("null");
    }
}
