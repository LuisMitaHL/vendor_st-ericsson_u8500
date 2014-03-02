package com.stericsson.ftc.command;

import java.util.Collection;
import java.util.Locale;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Class is calling get connected equipment, to list available devices.
 * 
 * @author TSIKOR01
 * 
 */
public class DevCommand extends FTCAbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) {
        if(input.size()!=0){
            return false;
        }
        setCommand(new String[] {
            CommandName.getCLISyntax(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name()).toLowerCase(
                Locale.getDefault())});
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("DEV");
        System.out.println("- Prints devices ID");
    }
}
