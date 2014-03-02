package com.stericsson.sdk.brp.system;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * @author xtomzap
 *
 */
public class InitArbTable extends AbstractCommand {

    /**
    * {@inheritDoc}
    */
    @Override
    public CommandName getCommandName() {
        return CommandName.SECURITY_INIT_ARB_TABLE;
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_EQUIPMENT_ID, PARAMETER_ARB_DATA};
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public boolean isCancellable() {
        return false;
    }
}
