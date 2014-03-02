package com.stericsson.sdk.cli.command;

import java.util.Locale;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ServerResponse;

/**
 * used for printing different Backend Remote Protocol results
 * 
 * @author xdancho
 * 
 */
public final class BRPPrintFormatter {

    private static final int PROGRESS_PARTS = 20;

    private static BRPPrintFormatter instance = new BRPPrintFormatter();

    private static final String PAD_COLUMN = "      ";

    private static final String BORDER_CHAR = "-";

    private static String progressChar = "|";

    private static final String[] GET_CONNECTED_DEVICE_TITLES = new String[] {
        "PLATFORM", "PORT", "PROFILE", "STATUS", "STATUS MESSAGE"};

    private static final String[] GET_ACTIVE_PROFILE = new String[] {
        "ACTIVE PROFILE"};

    private static final String[] GET_AVAILABLE_PROFILES = new String[] {
        "AVAILABLE PROFILES"};

    private static final String[] GET_DEVICE_PROPERTIES = new String[] {
        "PROPERTY", "VALUE", "SECURITY"};

    private static final String[] GET_DEVICE_SECURITY_PROPERTIES = new String[] {
        "PROPERTY NAME", "PROPERTY ID", "READ", "WRITE"};

    private static final String[] LIST_DEVICES = new String[] {
        "DEVICE PATH", "DEVICE TYPE", "DEVICE START", "DEVICE LENGHT", "DEVICE BLOCK SIZE"};

    private static final String[] TEST = new String[] {
        "TITLE1", "TITLE2", "TITLE3"};

    private static final String[] TEST2 = new String[] {
        "TITLE1", "TITLE2", "TITLE3", "TITLE4", "TITLE5"};

    private static String lastCommand = "";

    private static String lastProgress = "";

    private static void setLastProgress(String progress) {
        BRPPrintFormatter.lastProgress = progress;
    }

    private static void setLastCommand(String cmd) {
        BRPPrintFormatter.lastCommand = cmd;
    }

    private static void setProgressChar(String pChar) {
        BRPPrintFormatter.progressChar = pChar;
    }

    private int indeterminateProgressPosition = 0;

    private int indeterminateProgressAddition = 1;

    /**
     * print progress
     * 
     * @param progress
     *            the progress payload
     * @param command
     *            the command
     */
    public void printProgress(String command, String progress) {
        setLastProgress(progress);
        progress = getResponseData(progress, ServerResponse.PROGRESS);

        if (progress == null || progress.equals("")) {
            return;
        }

        setLastCommand(command);
        String[] progressSplit = progress.split(AbstractCommand.DELIMITER);

        // progress with just percentage for get_available_profiles
        if (!progressSplit[0].trim().equals("") && progressSplit[1].trim().equals("")
            && command.equalsIgnoreCase(CommandName.BACKEND_GET_AVAILABLE_PROFILES.name())) {
            int percent = Integer.parseInt(progressSplit[0]);
            System.out.print("\r" + getProgressChar() + "\t" + percent + "%" + createProgressLine(percent)
                + " of available profiles loaded.\t\t");

            // progress with percentage and kB/s
        } else {
            int percent = Integer.parseInt(progressSplit[0]);
            double speed = Long.parseLong(progressSplit[1]) / 1024.0;
            if (percent < 0) {
                System.out.print("\r" + getProgressChar() + "\t" + createProgressLine(percent) + " "
                    + String.format("%.2f", speed) + " KB/s.\t\t");
            } else {
                System.out.print("\r" + getProgressChar() + "\t" + percent + "%" + createProgressLine(percent) + " "
                    + String.format("%.2f", speed) + " KB/s.\t\t");
            }
        }
    }

    /**
     * Prints progress info
     * @param info
     *            the progress info
     */
    public void printProgressInfo(String info) {
        System.out.print("\r" + info);
    }

    /**
     * Prints progress info
     * @param command
     *            command
     * @param message
     *            message that should be printed
     */
    public void printProgressMessage(String command, String message) {
        String[] messageSplit = message.split(AbstractCommand.DELIMITER);
        int size = messageSplit.length;
        System.out.print("\r" + messageSplit[size-1] + "\n\r");
    }
    /**
     * Prints indeterminate progress
     */
    public void printIndeterminateProgress() {
        if (lastProgress.equals("")) {
            System.out.print("\r" + getProgressChar());
        } else {
            printProgress(lastCommand, lastProgress);
        }
    }

    private String getProgressChar() {
        String pChar = "";
        synchronized (progressChar) {
            if (progressChar.equals("|")) {
                pChar = "/";
            } else if (progressChar.equals("/")) {
                pChar = "-";
            } else if (progressChar.equals("-")) {
                pChar = "\\";
            } else if (progressChar.equals("\\")) {
                pChar = "|";
            }
            setProgressChar(pChar);
        }
        return pChar;
    }

    /**
     * get BRPPrintFormatter instance
     * 
     * @return the only instance of this class
     */
    public static BRPPrintFormatter getInstance() {
        return instance;
    }

    /**
     * prints NAK
     * 
     * @param command
     *            that generated this NAK
     */
    public void printNak(String command) {
        System.out.println("NAK received while executing command: " + command.toLowerCase(Locale.getDefault()));
    }

    /**
     * prints Error
     * 
     * @param command
     *            command that generated this Error
     * @param error
     *            the error payload
     */
    public void printError(String command, String error) {

        error = getResponseData(error, ServerResponse.ERROR);
        System.out.println();
        System.out.println("Error received while executing command: " + command.toLowerCase(Locale.getDefault()));

        System.out.println("Error description: " + error);
    }

    /**
     * prints result
     * 
     * @param command
     *            command that generated this Result
     * @param result
     *            the result payload
     */
    public void printResult(String command, String result) {

        result = getResponseData(result, ServerResponse.SUCCESS);

        // erase the command active indicator
        System.out.print("\r\t");

        if (result == null) {
            if (BRPResonseMapping.getDefaultResponse(command) == null) {
                System.out.println("\n" + command + " complete.");
            } else {
                System.out.println("\n" + BRPResonseMapping.getDefaultResponse(command));
            }
            return;
        }

        if (command.equalsIgnoreCase("TEST")) {

            printFormattedResult(result, TEST);

        } else if (command.equalsIgnoreCase("TEST2")) {

            printFormattedResult(result, TEST2);

        } else if (command.equalsIgnoreCase(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name())) {
            printFormattedResult(result, GET_CONNECTED_DEVICE_TITLES);
        } else if (command.equalsIgnoreCase(CommandName.BACKEND_GET_AVAILABLE_PROFILES.name())) {
            printFormattedResult(result, GET_AVAILABLE_PROFILES);
        } else if (command.equalsIgnoreCase(CommandName.BACKEND_GET_ACTIVE_PROFILE.name())) {
            printFormattedResult(result, GET_ACTIVE_PROFILE);
        } else if (command.equalsIgnoreCase(CommandName.BACKEND_GET_NEXT_CONNECTED_EQUIPMENT.name())) {
            printFormattedResult(result, GET_CONNECTED_DEVICE_TITLES);
        } else if (command.equalsIgnoreCase(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name())) {
            printFormattedResult(result.replace("<newcolumn>", AbstractCommand.DELIMITER), GET_DEVICE_PROPERTIES);
        } else if (command.equalsIgnoreCase(CommandName.BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES.name())) {
            printFormattedResult(result.replace("<newcolumn>", AbstractCommand.DELIMITER),
                GET_DEVICE_SECURITY_PROPERTIES);
        } else if (command.equalsIgnoreCase(CommandName.FLASH_LIST_DEVICES.name())) {
            printFormattedResult(result, LIST_DEVICES);
        } else {// all commands with one result
            System.out.println("\n" + result);
        }
    }

    private void printFormattedResult(String result, String[] columnTitle) {
        System.out.print("\n");

        if (columnTitle.length == 0) {
            return;
        }

        String[] results = result.split(AbstractCommand.DELIMITER);

        if (results.length % columnTitle.length != 0) {
            System.out.println(result);
            return;
        }

        int lengthIndex = 0;
        int[] colLength = new int[columnTitle.length];

        // calc column length
        for (int i = 0; i < columnTitle.length; i++) {
            colLength[i] = columnTitle[i].length();
            for (int j = 0; j < (results.length / columnTitle.length); j++) {
                if (colLength[i] < results[i + j * columnTitle.length].length()) {
                    colLength[i] = results[i + j * columnTitle.length].length();
                }
                lengthIndex++;
            }
        }

        int borderLength = 0;
        // print title
        for (int i = 0; i < columnTitle.length; i++) {

            String title = (columnTitle[i] + pad(columnTitle[i], colLength[i]));
            if (i == columnTitle.length - 1) {
                borderLength += colLength[i];
            } else {
                borderLength += title.length();

            }
            System.out.print(title);
        }
        System.out.print("\n");

        System.out.println(getBorder(borderLength));

        int printIndex = 0;
        // print results
        for (int i = 0; i < results.length / columnTitle.length; i++) { // each row
            for (int j = 0; j < columnTitle.length; j++) { // each column
                String res = (results[printIndex] + pad(results[printIndex], colLength[j]));
                System.out.print(res);
                printIndex++;
            }
            System.out.print("\n");
        }
    }

    private String pad(String s, int columnLength) {

        int pad = columnLength - s.length();
        StringBuffer buffer = new StringBuffer();
        for (int i = 0; i < pad; i++) {
            buffer.append(" ");
        }

        buffer.append(PAD_COLUMN);
        return buffer.toString();
    }

    private String getBorder(int length) {
        StringBuffer buffer = new StringBuffer();
        for (int i = 0; i < length; i++) {
            buffer.append(BORDER_CHAR);
        }

        return buffer.toString();
    }

    private String createProgressLine(int pct) {
        StringBuffer buffer = new StringBuffer();
        buffer.append("\t[");

        if (pct < 0) {
            if (indeterminateProgressPosition <= 1) {
                indeterminateProgressPosition = 1;
                indeterminateProgressAddition = 1;
            }
            if (indeterminateProgressPosition >= PROGRESS_PARTS - 2) {
                indeterminateProgressPosition = PROGRESS_PARTS - 2;
                indeterminateProgressAddition = -1;
            }

            indeterminateProgressPosition += indeterminateProgressAddition;

            for (int i = 0; i < indeterminateProgressPosition - 1; i++) {
                buffer.append(" ");
            }
            buffer.append("<=>");
            for (int i = indeterminateProgressPosition + 2; i < PROGRESS_PARTS; i++) {
                buffer.append(" ");
            }
        } else {
            int partValue = 100 / PROGRESS_PARTS;

            int done = pct / partValue;
            int remaining = PROGRESS_PARTS - done;
            for (int i = 0; i < (done - 1); i++) {
                buffer.append("=");
            }
            if (done > 0) {

                buffer.append(pct == 100 ? "=" : ">");

            }
            for (int i = 0; i < remaining; i++) {
                buffer.append(" ");
            }
        }

        buffer.append("]");

        return buffer.toString();

    }

    /**
     * get the data part of a response
     * 
     * @param completeResult
     *            the complete result
     * @param response
     *            the type of response
     * @return data part of a response
     */
    public String getResponseData(String completeResult, ServerResponse response) {
        String[] splitString = completeResult.split(response.name());
        if (splitString.length == 1) {
            switch (response) {
                case ERROR:
                    return "No description available";
                case NAK:
                    return "No description available";
                case SUCCESS:
                    return null;
                default:
                    return "";

            }
        }
        return splitString[1].substring(1, splitString[1].length());
    }
}
