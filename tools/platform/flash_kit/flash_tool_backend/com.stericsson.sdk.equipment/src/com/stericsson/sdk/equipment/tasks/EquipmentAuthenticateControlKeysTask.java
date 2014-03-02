package com.stericsson.sdk.equipment.tasks;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.IAuthenticate;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Task for authenticate with control keys
 * 
 * @author mbodan01
 */
public class EquipmentAuthenticateControlKeysTask extends AbstractEquipmentTask implements IAuthenticate {

    private static final String PATTERN =
        "Network Lock Key=([0-9]{1,16})|Network Subset Lock Key=([0-9]{1,16})|Service Provider Lock Key=([0-9]{1,16})"
            + "|Corporate Lock Key=([0-9]{1,16})|Flexible ESL Lock Key=([0-9]{1,16})";

    private AbstractLoaderCommunicationEquipment mobileEquipment;

    private static final String ERROR_PREFIX = "Failed to authenticate. Possible reason: ";

    /**
     * keys[0] Network Lock Key keys[1] Network Subset Lock Key keys[2] Service Provider Lock Key
     * keys[3] Corporate Lock Key keys[4] Flexible ESL Lock Key
     */
    private String[] keys;

    private int authenticationType;

    private int resultCode;

    private String resultMessage;

    private int result;

    /**
     * Constructs equipment authentication with control keys task.
     * 
     * @param pEquipment
     *            equipment on which the authentications should take place
     */
    public EquipmentAuthenticateControlKeysTask(AbstractLoaderCommunicationEquipment pEquipment) {
        super(pEquipment);
        mobileEquipment = pEquipment;
        keys = new String[] {};
        resultCode = ERROR_NONE;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
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
        notifyTaskStart();
        if (!(resultCode == ERROR)) {
            if (keys == null) {
                return new EquipmentTaskResult(-1, "Sim lock keys are not set.", null, false);
            }

            for (String key : keys) {
                if (key == null || key.length() == 0) {
                    return new EquipmentTaskResult(-1, "Wrong or incomplete sim lock keys were provided.", null, false);
                }
            }

            result = (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE, new Object[] {
                authenticationType, this}, this);

            if (result != 0) {
                resultMessage =
                    ERROR_PREFIX + mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
            }
        }
        return new EquipmentTaskResult(result, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name();
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
    public void setArguments(String[] arguments) {
        keys = parseKeysArgument(arguments[2]);

        if (arguments[3].equals("null") || arguments[3].equalsIgnoreCase("false")) {
            setAuthenticationType(false);
        } else if (arguments[3].equalsIgnoreCase("true")) {
            setAuthenticationType(true);
        } else {
            resultMessage = "Parameter  \"permanent authentication\"  flag should be set only to true or false";
            resultCode = ERROR;
        }
    }

    private void setAuthenticationType(Boolean permanentAuthenticationFlag) {
        if (permanentAuthenticationFlag) {
            authenticationType = LCDefinitions.AUTHENTICATE_CONTROL_KEYS_PERSISTENT;
        } else {
            authenticationType = LCDefinitions.AUTHENTICATE_CONTROL_KEYS;
        }
    }

    /**
     * Method which used for parsing input keys passed as string argument into array of strings
     * 
     * @param argument
     *            input keys
     * @return array of keys in the string form
     */
    private String[] parseKeysArgument(String argument) {
        String tmp;
        String[] keyArguments = new String[5];
        Pattern p = Pattern.compile(PATTERN);
        Matcher m = p.matcher(argument);
        while (m.find()) {
            for (int i = 1; i <= m.groupCount(); i++) {
                tmp = m.group(i);
                if (tmp != null) {
                    keyArguments[i - 1] = tmp;
                }
            }
        }
        return keyArguments;
    }

    // will be removed before last commit this task
    // private void paddingEnteredKeys(String[] parsedkeys) {
    // for (int i = 0; i < parsedkeys.length; i++) {
    // if (parsedkeys[i] == null) {
    // parsedkeys[i] = new String("0");
    // }
    // paddingKey(parsedkeys[i], 16, (char) 0xFF);
    // }
    // }
    //
    // private void paddingKey(String pKey, final int pSize, final char pChar) {
    // StringBuilder builder;
    // int diff = pSize - pKey.length();
    // if (diff > 0) {
    // builder = new StringBuilder();
    // builder.append(pKey);
    // for (int i = 0; i < diff; i++) {
    // builder.append(pChar);
    // }
    // pKey = builder.toString();
    // }
    // }

    /**
     * {@inheritDoc}
     */
    public byte[] getChallengeResponse(byte[] challenge) {
        // N/A - this is valid only for authentication with certificate
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getControlKeys() {
        return keys;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment;
    }
}
