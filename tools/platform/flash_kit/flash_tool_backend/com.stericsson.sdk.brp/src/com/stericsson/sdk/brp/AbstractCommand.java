package com.stericsson.sdk.brp;

import java.util.LinkedHashMap;

/**
 * Abstract class for a backend remote command
 * 
 * @author xolabju
 * 
 */
public abstract class AbstractCommand {

    /** */
    public static final String PARAMETER_EQUIPMENT_ID = "equipment_id";

    /** */
    public static final String PARAMETER_PROFILE_NAME = "profile_name";

    /** */
    public static final String PARAMETER_REBOOT_MODE = "mode";

    /** */
    public static final String PARAMETER_PATH = "path";

    /** */
    public static final String PARAMETER_SIGN_PACKAGE = "sign_package_name";

    /** */
    public static final String PARAMETER_CONTROL_KEYS = "key_set";

    /** */
    public static final String PARAMETER_STORAGE_ID = "storage_id";

    /** */
    public static final String PARAMETER_UNIT_ID = "unit_id";

    /** */
    public static final String PARAMETER_UNIT_DATA = "unit_data";

    /** */
    public static final String PARAMETER_DOMAIN = "domain";

    /** */
    public static final String PARAMETER_EQUIPMENT_PROPERTIES = "equipment_properties";

    /** */
    public static final String PARAMETER_EQUIPMENT_PROPERTY = "equipment_property";

    /** */
    public static final String PARAMETER_EQUIPMENT_PROPERTY_SET = "equipment_property";

    /** */
    public static final String PARAMETER_AREA_PATH = "area_path";

    /** */
    public static final String PARAMETER_LENGTH = "length";

    /** */
    public static final String PARAMETER_OFFSET = "offset";

    /** */
    public static final String PARAMETER_SKIP_REDUNDANT_AREA = "skip_redundant_area";

    /** */
    public static final String PARAMETER_INCLUDE_BAD_BLOCKS = "include_bad_blocks";

    /** */
    public static final String PARAMETER_WAIT = "wait";

    /** */
    public static final String PARAMETER_RAW_DATA = "data";

    /** */
    public static final String PARAMETER_TYPE = "type";

    /** */
    public static final String PARAMETER_MESSAGE = "message";

    /** */
    public static final String PARAMETER_COREDUMP_FILE_NAME = "dump_file_name";

    /** */
    public static final String PARAMETER_COREDUMP_SUBSCRIBER_IP = "subscriber_ip";

    /** */
    public static final String PARAMETER_COREDUMP_SUBSCRIBER_PORT = "subscriber_port";

    /** */
    public static final String PARAMETER_COREDUMP_SUBSCRIPTION_TYPE = "subscription_type";

    /** */
    public static final String PARAMETER_COREDUMP_SET_STATUS = "set_status";

    /** */
    public static final String PARAMETER_DEVICE_PATH = "device_path";

    /** */
    public static final String PARAMETER_TARGET_PATH = "target_path";

    /** */
    public static final String PARAMETER_SOURCE_PATH = "source_path";

    /** */
    public static final String PARAMETER_DESTINATION_PATH = "destination_path";

    /** */
    public static final String PARAMETER_ACCESS = "access";

    /** */
    public static final String PARAMETER_LOCAL_SIGNING = "local_signing";

    /** */
    public static final String PARAMETER_PORT_NAME = "port_name";

    /** */
    public static final String PARAMETER_OTP_DATA = "otp_data";

    /** */
    public static final String PARAMETER_ARB_DATA = "arb_data";

    /**  */
    public static final String PARAMETER_PERMANENT = "permanent";

    /**  */
    public static final String PARAMETER_UI_PROPERTY = "ui_property";

    /**  */
    public static final String PARAMETER_UI_VALUE = "ui_value";

    /**  */
    public static final String PARAMETER_DEVICE_ID = "device_id";

    /**  */
    public static final String PARAMETER_COMMERCIAL = "commercial";

    /** */
    protected String commandString = "";

    /** */
    public static final String DELIMITER = ";";

    /** */
    public static final String CANCEL = "cancel";

    /** */
    public static final String LINE_SEPARATOR = "\r\n";

    /** */
    public static final String EQUIPMENT_ID_NEXT = "next";

    /** */
    public static final String EQUIPMENT_ID_CURRENT = "current";

    private LinkedHashMap<String, String> values;

    /**
     * Constructor
     */
    public AbstractCommand() {
        values = new LinkedHashMap<String, String>();
    }

    /**
     * Constructor
     * 
     * @param cmdString
     *            the complete command String including parameters on the form COMMAND
     *            {@value #DELIMITER} PARAM1{@value #DELIMITER}PARAM2 ...
     * @throws InvalidSyntaxException
     *             if the command syntax does not match the command specification
     */
    public AbstractCommand(String cmdString) throws InvalidSyntaxException {
        this();
        setCommand(splitAndTrim(cmdString));
    }

    /**
     * 
     * @param commandName
     *            the CommandName enum
     * @param arguments
     *            the command arguments, without delimiters
     * @throws InvalidSyntaxException
     *             if the command syntax does not match the command specification
     */
    public AbstractCommand(CommandName commandName, String[] arguments) throws InvalidSyntaxException {
        this();
        StringBuffer buffer = new StringBuffer();
        buffer.append(getCommandName().name());
        if (arguments != null && arguments.length > 0) {
            for (String arg : arguments) {
                buffer.append(AbstractCommand.DELIMITER + arg);
            }
        }
        setCommand(buffer.toString());
    }

    /**
     * 
     * @return the command syntax
     */
    private String getCommandSyntax() {
        StringBuffer buffer = new StringBuffer();
        buffer.append(getCommandName().name());
        if (getParameterNames() != null && getParameterNames().length > 0) {
            for (String parameter : getParameterNames()) {
                buffer.append(DELIMITER + "<" + parameter + ">");
            }
        }
        return buffer.toString();
    }

    /**
     * 
     * @param command
     *            the command
     * @return true if the command syntax is correct, else false
     */
    private boolean validateCommandSyntax(String[] command) {
        boolean isValid = false;
        if (command != null && command.length > 0 && getCommandName().name().equals(command[0])) {
            if ((getParameterNames() == null || getParameterNames().length == 0) && command.length == 1) {
                isValid = true;
            } else if (getParameterNames() != null) {
                if ((command.length - 1) == getParameterNames().length) {
                    isValid = true;
                } else if (isCancellable() && isCancelCommand(command)) {
                    isValid = true;
                }
            }
        }
        return isValid;
    }

    /**
     * 
     * @return true if it's possible to cancel the operation, else false
     */
    public abstract boolean isCancellable();

    /**
     * 
     * @return the command as an enum
     */
    public abstract CommandName getCommandName();

    private String[] splitAndTrim(String command) {
        String[] split = null;
        if (command != null) {
            split = command.split(DELIMITER);
            for (int i = 0; i < split.length; i++) {
                if (split[i] != null) {
                    split[i] = split[i].trim();
                }
            }
        }
        return split;
    }

    private String reassembleCommand(String[] command) {
        String result = "";
        StringBuffer buffer = new StringBuffer();
        for (String s : command) {
            buffer.append(s.trim() + DELIMITER);
        }
        result = buffer.toString();
        if (result.length() > 0) {
            result = result.substring(0, result.length() - 1);
        }
        return result;
    }

    /**
     * @param cmdString
     *            the complete command String including parameters on the form COMMAND
     *            {@value #DELIMITER} PARAM1{@value #DELIMITER}PARAM2 ...
     * @throws InvalidSyntaxException
     *             if the command syntax does not match the command specification
     */
    public void setCommand(String cmdString) throws InvalidSyntaxException {
        setCommand(splitAndTrim(cmdString));
    }

    /**
     * @param commandParts
     *            the complete command including parameters on the form [COMMAND][PARAM1][PARAM2]
     *            ...
     * @throws InvalidSyntaxException
     *             if the command syntax does not match the command specification
     */
    public void setCommand(String[] commandParts) throws InvalidSyntaxException {
        if (!validateCommandSyntax(commandParts)) {
            throw new InvalidSyntaxException("Command Usage: " + getCommandSyntax());
        }
        if (getParameterNames() != null) {
            for (int i = 0; i < getParameterNames().length; i++) {
                values.put(getParameterNames()[i], commandParts[i + 1]);
            }
        }
        commandString = reassembleCommand(commandParts);
    }

    /**
     * 
     * @return the complete command as a string
     */
    public String getCommandString() {
        return commandString;
    }

    /**
     * 
     * @return the complete command as a string array (delimiters excluded)
     */
    public String[] getCommandParts() {
        return splitAndTrim(commandString);
    }

    /**
     * Gets the command's parameter name as a String array (e.g {"equipment-id", "param2", ... }.
     * Commands without parameters will return null
     * 
     * @return the command's parameter name as a String array (e.g {"equipment-id", "param2", ... },
     *         or null if the command does not support any parameters
     */
    public abstract String[] getParameterNames();

    /**
     * 
     * @param parameterName
     *            the name of the parameter (see AbstractCommand#PARAMETER_*)
     * @return the value of the given parameter
     */
    public String getValue(String parameterName) {
        return values.get(parameterName);
    }

    /**
     * @param parameterName
     *            TBD
     * @param parameterValue
     *            TBD
     * @throws InvalidSyntaxException
     *             TBD
     */
    public void setValue(String parameterName, String parameterValue) throws InvalidSyntaxException {
        boolean found = false;
        for (String name : getParameterNames()) {
            if (name.equalsIgnoreCase(parameterName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            throw new InvalidSyntaxException("Invalid parameter name: " + parameterName);
        }
        getCommandParts();

        values.put(parameterName, parameterValue);

        String[] array = values.values().toArray(new String[0]);
        String[] valueArray = new String[array.length + 1];
        valueArray[0] = getCommandName().name();
        System.arraycopy(array, 0, valueArray, 1, array.length);

        setCommand(valueArray);
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return commandString;
    }

    private boolean isCancelCommand(String[] command) {
        return command != null && command.length > 1 && command[command.length - 1].equalsIgnoreCase(CANCEL);
    }

    /**
     * 
     * @return true if the command is a cancel command, else false
     */
    public boolean isCancelCommand() {
        String[] commandParts = getCommandParts();
        return commandParts != null && commandParts.length > 1
            && commandParts[commandParts.length - 1].equalsIgnoreCase(CANCEL);
    }

}
