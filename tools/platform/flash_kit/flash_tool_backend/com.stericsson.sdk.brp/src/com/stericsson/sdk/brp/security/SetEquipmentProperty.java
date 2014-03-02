package com.stericsson.sdk.brp.security;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for setting the properties on an equipment
 * 
 * @author xolabju
 * 
 */
public class SetEquipmentProperty extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.SECURITY_SET_EQUIPMENT_PROPERTY;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isCancellable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_EQUIPMENT_ID, PARAMETER_EQUIPMENT_PROPERTY};
    }

}
