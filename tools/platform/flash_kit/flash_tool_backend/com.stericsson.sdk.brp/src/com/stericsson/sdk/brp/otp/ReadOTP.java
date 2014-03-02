package com.stericsson.sdk.brp.otp;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for reading OTP data from specified ME and writing it to specified file.
 * 
 * @author pkutac01
 * 
 */
public class ReadOTP extends AbstractCommand {

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.OTP_READ_OTP;
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
