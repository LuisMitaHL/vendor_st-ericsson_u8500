package com.stericsson.sdk.brp.otp;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for burning OTP data provided as Base64 string to specified ME.
 * 
 * @author pkutac01
 * 
 */
public class BurnOTPData extends AbstractCommand {

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.OTP_BURN_OTP_DATA;
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
