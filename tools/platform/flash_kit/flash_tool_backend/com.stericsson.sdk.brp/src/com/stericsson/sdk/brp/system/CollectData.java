package com.stericsson.sdk.brp.system;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * @author ezaptom
 *
 */
public class CollectData extends AbstractCommand {

    /**
    * {@inheritDoc}
    */
    @Override
    public CommandName getCommandName() {
        return CommandName.SYSTEM_COLLECT_DATA;
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public String[] getParameterNames() {
        return new String[] {PARAMETER_EQUIPMENT_ID, PARAMETER_TYPE};
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public boolean isCancellable() {
        return false;
    }
}