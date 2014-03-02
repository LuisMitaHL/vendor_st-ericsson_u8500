package com.stericsson.sdk.equipment.m7x00.tasks;

import java.io.File;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.m7x00.bsu.BSUUtils;
import com.stericsson.sdk.equipment.m7x00.bsu.BSUUtils.BSUResult;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDirectoryType;

/**
 * This task copy files between device and computer or from phone to phone.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class M7X00EquipmentCopyFileTask extends AbstractLCEquipmentTask {

    AbstractLoaderCommunicationEquipment equipment;

    private static final String PC = "PC:";

    private BSUResult bsuResult;

    /**
     * @param pEquipment
     *            Equipment for which this task is created.
     */
    public M7X00EquipmentCopyFileTask(AbstractLoaderCommunicationEquipment pEquipment) {
        super(pEquipment, LCDefinitions.METHOD_NAME_FS_COPY_FILE, CommandName.FILE_SYSTEM_COPY_FILE);
        this.equipment = pEquipment;
    }

    /**
     * From 2 arguments makes 4 arguments which are necessary for method fileSystemCopyFile.
     * 
     * @param pArguments
     *            Arguments, which are set for copy file task
     * @see LoaderCommunicationInterface
     */
    @Override
    public void setArguments(String[] pArguments) {
        String src = pArguments[2];
        String dest = pArguments[3];
        boolean srcIsPC = src.startsWith(PC);
        if (srcIsPC) {
            src = src.substring(PC.length());
        }
        boolean destIsPC = dest.startsWith(PC);

        if (destIsPC) {
            dest = dest.substring(PC.length());
            if (isDirectory(dest)) {
                dest = addSourceFileNametoDestDir(dest, src);
            }
        } else {
            if (isMeDirectory(dest)) {
                dest = addSourceFileNametoDestDir(dest, src);
            }

        }
        bsuResult = null;
        // have to split "if" in two parts due to checkstyle constraints
        if (EnvironmentProperties.isWindows()) {
            if (srcIsPC && !destIsPC && src.endsWith(".elf") && isLoadModuleDir(dest)) {
                try {
                    bsuResult = BSUUtils.checkCompatibility((M7x00Equipment) getEquipment(), new File(src));
                } catch (Exception e) {
                    bsuResult = new BSUResult(e.getMessage(), -1);
                }
            }
        }
        super.arguments = new Object[] {
            src, dest, srcIsPC, destIsPC};
    }

    private boolean isMeDirectory(String dest) {
        Object listDirectoryResult = equipment.sendCommand(LCDefinitions.METHOD_NAME_FS_LIST_DIRECTORY, new Object[] {
            dest}, this);
        ListDirectoryType directoryList = (ListDirectoryType) listDirectoryResult;
        return directoryList.getStatus() == ERROR_NONE;
    }

    /**
     * Method checking if destination folder is directory
     * 
     * @param dest
     * @return
     */
    private boolean isDirectory(String dest) {
        return new File(dest).isDirectory();
    }

    /**
     * Method takes source file name and appends it to the destination directory
     * 
     * @param destination
     *            Destination directory name
     * @param source
     *            Source file name
     * @return
     */
    private String addSourceFileNametoDestDir(String destination, String source) {
        String sourceSeparator = getFileSeparator(source);
        String destinationSeparator = getFileSeparator(destination);

        if (!destination.endsWith(destinationSeparator)) {
            destination = destination + destinationSeparator;
        }

        return destination + source.substring(source.lastIndexOf(sourceSeparator) + 1, source.length());
    }

    /**
     * Method returning file separator, we must check even of windows machines if there is backslash
     * used because of ME paths.
     * 
     * @param path
     *            Tested path
     * @return File separator
     */
    private String getFileSeparator(String path) {
        if (path.contains("/")) {
            return "/";
        }
        return "\\";
    }

    /**
     * Checks if given directory on ME is defined as a load modules holding directory.
     * 
     * @param pMEPath
     *            path on ME
     * @return true if the path is meant to hold load modules
     */
    private boolean isLoadModuleDir(String pMEPath) {
        // make sure that pMEPath ends with a slash so that we can match
        // it against loadModulesPath (loadModulesPath always ends with "/")
        String mePath = new String(pMEPath);
        if (!mePath.endsWith("/")) {
            mePath = mePath + "/";
        }

        for (BSUUtils.BSUPath loadModulesPath : BSUUtils.BSUPath.values()) {
            if (loadModulesPath.getPath().endsWith(mePath)) {
                return true;
            }
        }
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void updateResult(Object resultObj) {
        super.updateResult(resultObj);

        if (bsuResult != null) {
            // bsu result code overrides LCD result code only if error occurred
            if (bsuResult.getExitCode() != 0) {
                resultCode = bsuResult.getExitCode();
            }

            // append bsu message to possible LCD message
            if (bsuResult.getText().trim().length() > 0) {
                resultMessage += "\n" + bsuResult.getText();
            }
        }
    }

}
