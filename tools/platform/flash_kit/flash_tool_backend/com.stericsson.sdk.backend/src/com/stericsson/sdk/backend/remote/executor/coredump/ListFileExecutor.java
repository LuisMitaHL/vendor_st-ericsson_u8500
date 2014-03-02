package com.stericsson.sdk.backend.remote.executor.coredump;

import com.stericsson.sdk.backend.remote.executor.LCCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentTaskResult;

/**
 * Remote command executor for list file
 * 
 * @author esrimpa
 * 
 */
public class ListFileExecutor extends LCCommandExecutor {

    /** File not Downloaded */
    private static final String LINE_BREAK = "\n";

    /** Line split to get download information */
    private static final String LINE_SPLIT = "\\|";

    /** File Downloaded */
    private static final String FTP_DOWNLOADED = "Downloaded";

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        return super.execute();
    }

    /**
     * Format the result message to display downloaded files, e.g.
     * "filaname1|Downloaded\nfilename2|Not Downloaded".
     * 
     * @param result task result.
     * @return Formatted result message.
     */
    @Override
    protected String formatOutputMessage(EquipmentTaskResult result) {
        StringBuffer message = new StringBuffer();

        // Split on line break to get the list of file with download information
        String[] fileNames = result.getResultMessage().split(LINE_BREAK);
        for (int i = 0; i < fileNames.length; i++) {
            String[] downloadInfo = fileNames[i].split(LINE_SPLIT);

            if ((downloadInfo[1].trim().length() != 0) && (!downloadInfo[1].trim().equals(FTP_DOWNLOADED))) {
                message.append(downloadInfo[0] + LINE_BREAK);
            }
        }

        return message.toString();
    }

}
