/**
 * 
 */
package com.stericsson.sdk.brp.security;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * @author xhelciz
 *
 */
public class GetEquipmentProperty extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.SECURITY_GET_EQUIPMENT_PROPERTY;
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
