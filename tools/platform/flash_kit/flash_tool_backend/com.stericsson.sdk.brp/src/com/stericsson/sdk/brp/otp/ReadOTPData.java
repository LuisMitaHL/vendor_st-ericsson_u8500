package com.stericsson.sdk.brp.otp;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for reading OTP data from specified ME and returning it as Base64 string.
 * 
 * @author pkutac01
 * 
 */
public class ReadOTPData extends AbstractCommand {

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.OTP_READ_OTP_DATA;
    }

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_EQUIPMENT_ID, PARAMETER_OTP_DATA};
    }

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public boolean isCancellable() {
        return false;
    }

}
