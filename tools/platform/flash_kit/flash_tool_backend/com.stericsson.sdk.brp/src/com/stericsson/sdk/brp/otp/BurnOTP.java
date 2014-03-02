package com.stericsson.sdk.brp.otp;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for burning OTP data provided in specified file to specified ME.
 * 
 * @author pkutac01
 * 
 */
public class BurnOTP extends AbstractCommand {

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.OTP_BURN_OTP;
    }

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_EQUIPMENT_ID, PARAMETER_PATH};
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
