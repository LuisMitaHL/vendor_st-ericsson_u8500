/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.util.ArrayList;

import android.util.Log;

/**
 * A class representing a general AT command, with name, type and arguments. The
 * class does not control the exact syntax of the values it contains.
 */
public class AtCommand {

    /**
     * Type action, example: AT+FOO or ATD010112233 (special case of basic
     * command)
     */
    public static final int TYPE_ACTION = 0;

    /**
     * Type read, example: AT+FOO?
     */
    public static final int TYPE_READ = 1;

    /**
     * Type set, example: AT+FOO=11
     */
    public static final int TYPE_SET = 2;

    /**
     * Type test, example: AT+FOO=?
     */
    public static final int TYPE_TEST = 3;

    /**
     * The name of the command.
     */
    private String name;

    /**
     * The type of the command.
     */
    private int type;

    /**
     * The arguments of the command.
     */
    private Object[] arguments;

    /**
     * A value indicating if the AT command is basic.
     */
    private boolean isBasic;

    /**
     * Constructor for an <code>AtCommand</code>.
     *
     * @param command the string the AT command is created from. The command
     *            string has no end character.
     */
    public AtCommand(String command) throws IllegalArgumentException {
        arguments = new Object[0];
        parse(command);

    }

    /**
     * Method used to set default values for an AtCommand.
     *
     * @param argumentProperties the properties corresponding to this commands
     *            arguments.
     */
    public void setDefaultArguments(AtArgumentProperties[] argumentProperties) {
        // the number of arguments including default ones are as many as the
        // number of argument properties
        Object[] updatedArguments = new Object[argumentProperties.length];

        for (int i = 0; i < argumentProperties.length; i++) {
            if (i < arguments.length) {
                Log.d("DEBUGGING", "argument " + (i + 1));
                Object current = arguments[i];

                if (current.equals("")) { // "" always converted to default
                    // value
                    // use default value
                    Log.d(AtService.LOG_TAG, "use default value "
                          + argumentProperties[i].getDefaultValueAsString());
                    updatedArguments[i] = argumentProperties[i].getDefaultValue();
                } else {
                    // has defined value
                    Log.d(AtService.LOG_TAG, "has defined value " + current.toString());
                    updatedArguments[i] = current;
                }
            } else {
                // optional arguments
                Log.d(AtService.LOG_TAG, "outside defined values, set default "
                      + argumentProperties[i].getDefaultValueAsString());
                updatedArguments[i] = argumentProperties[i].getDefaultValue();
            }
        }

        arguments = updatedArguments;

    }

    /**
     * Parses a string to create an <code>AtCommand</code>. Only checks that is
     * has a name and a type. Arguments are parsed if they are found.
     *
     * @param command the string to parse
     * @throws IllegalArgumentException if the syntax of <code>command</code> is
     *             invalid
     */
    private void parse(String command) throws IllegalArgumentException {
        Log.d(AtService.LOG_TAG, "Command: " + command);

        if (findChar('*', command, 0) == command.length()
                && findChar('+', command, 0) == command.length() || command.startsWith("ATD")
                || command.startsWith("D")) {
            // ATD can include a * in the phone number
            // is a basic command
            isBasic = true;
        }

        // a command could have been part of a chain
        // Both AT+CSCS and +CSCS possible
        int beginIndex = 0;

        if (command.indexOf("AT") != -1) {
            beginIndex = "AT".length();
        }

        int typeIndex = setType(command);

        if (isBasic && type == TYPE_ACTION && beginIndex + 1 < command.length()) {
            // suppose only one letter basic AT commands or a combination of
            // first a sign and then a
            // letter, they have only one argument, and it's a string
            if (Character.isLetter(command.charAt(beginIndex))) {
                name = Character.toString(command.charAt(beginIndex));
            } else {
                // not a letter at beginIndex use next character also
                name = command.substring(beginIndex, beginIndex + 2);
                beginIndex++;
            }

            arguments = new Object[] {
                command.substring(beginIndex + 1),
            };
        } else if (isBasic && type == TYPE_ACTION && beginIndex + 1 == command.length()) {
            // no argument could be interpreted as ""
            // - could mean that the default should be used or no argument
            name = Character.toString(command.charAt(beginIndex));
            arguments = new Object[] {
                "",
            };
        } else {
            name = command.substring(beginIndex, typeIndex);
        }

        // Any characters following ?, =? are ignored
        // parse arguments if TYPE_SET
        if (type == TYPE_SET) {
            if (name.equalsIgnoreCase("+VTS")) {
                // arguments of VTS has a unique syntax
                arguments = new Object[] {
                    command.substring(typeIndex + 1),
                };
            } else {
                arguments = generateArgs(command.substring(typeIndex + 1));
            }

        }
    }

    /**
     * Sets the type of the <code>AtCommand</code> and returns the start index
     * of the type symbol.
     *
     * @param command the AT command which type is checked
     * @return the start index of the type symbol
     */
    private int setType(String command) {
        int typeIndex = command.indexOf("=?");

        if (typeIndex != -1) {
            type = TYPE_TEST;
            return typeIndex;
        }

        typeIndex = command.indexOf('=');

        if (typeIndex != -1) {
            type = TYPE_SET;
            return typeIndex;
        }

        typeIndex = command.indexOf('?');

        if (typeIndex != -1) {
            type = TYPE_READ;
            return typeIndex;
        }

        type = TYPE_ACTION;
        return command.length();

    }

    /**
     * Break an argument string into individual arguments (comma deliminator).
     * Integer arguments are turned into Integer objects. Otherwise a String
     * object is used.
     */
    private Object[] generateArgs(String input) throws IllegalArgumentException {
        int i = 0;
        int j;
        ArrayList<Object> out = new ArrayList<Object>();

        while (i <= input.length()) {
            j = findChar(',', input, i);

            String arg = input.substring(i, j);

            try {
                out.add(new Integer(arg));
            } catch (NumberFormatException e) {
                if (arg.length() == 1) { // to short to be a correct string
                    throw new IllegalArgumentException(
                        "Illegal syntax of arguments, a string must have format \"string1\"");
                } else if (arg.startsWith("\"") && arg.endsWith("\"") || arg.length() == 0) {
                    out.add(arg);
                } else {
                    // incorrect syntax
                    throw new IllegalArgumentException(
                        "Illegal syntax of arguments, a string must have format \"string1\"");
                }
            }

            i = j + 1; // move past comma
        }

        return out.toArray();
    }

    /**
     * Find a character ch, ignoring quoted sections. Return input.length() if
     * not found.
     */
    private int findChar(char ch, String input, int fromIndex) {
        for (int i = fromIndex; i < input.length(); i++) {
            char c = input.charAt(i);

            if (c == '"') {
                i = input.indexOf('"', i + 1);

                if (i == -1) {
                    return input.length();
                }
            } else if (c == ch) {
                return i;
            }
        }

        return input.length();
    }

    /**
     * Returns the name of the <code>AtCommand</code>.
     *
     * @return the name
     */
    public String getName() {
        return name;
    }

    /**
     * Returns the type of the <code>AtCommand</code>.
     *
     * @return the type
     */
    public int getType() {
        return type;
    }

    /**
     * Returns the arguments of the <code>AtCommand</code> or null.
     *
     * @return the arguments or null
     */
    public Object[] getArguments() {
        return arguments;
    }

    /**
     * Returns a value indicating if it is a basic command.
     *
     * @return <code>true</code> or <code>false</code>
     */
    public boolean isBasicCommand() {
        return isBasic;
    }
}
