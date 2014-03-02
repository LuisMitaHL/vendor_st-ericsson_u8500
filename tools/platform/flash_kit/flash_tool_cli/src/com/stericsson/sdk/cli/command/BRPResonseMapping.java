package com.stericsson.sdk.cli.command;

import java.util.HashMap;
import java.util.Locale;

import com.stericsson.sdk.brp.CommandName;

/**
 * response map class for BRPCommands
 * 
 * @author xdancho
 * 
 */
public final class BRPResonseMapping {

    static HashMap<String, String> responseMap = new HashMap<String, String>();

    static {
        responseMap.put(CommandName.BACKEND_ENABLE_AUTO_SENSE.name(), "Auto sense ON");
        responseMap.put(CommandName.BACKEND_DISABLE_AUTO_SENSE.name(), "Auto sense OFF");
        responseMap.put(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name(), "No connected equipments");
        responseMap.put(CommandName.FLASH_PROCESS_FILE.name(), "Process file completed");
        responseMap.put(CommandName.BACKEND_SET_ACTIVE_PROFILE.name(), "Active profile set successfully");
    }

    private BRPResonseMapping() {

    }

    /**
     * get the string to print when a command has succeeded with no payload
     * 
     * @param command
     *            the BRP command
     * @return the default string to print
     */
    public static String getDefaultResponse(String command) {
        return responseMap.get(command.toUpperCase(Locale.getDefault()));
    }

}
