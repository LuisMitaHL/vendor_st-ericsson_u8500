package com.stericsson.ftc.command;

import java.util.Collection;
import java.util.Locale;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Command to create directory on ME.
 * 
 * @author TSIKOR01
 * 
 */
public class MKDIRCommand extends FTCAbstractCommand {

    /**
     * {@inheritDoc}
     */
    public boolean setParameters(Collection<String> input){

        if (input.size() < 1) {
            System.out.println("Error! Target path must be specyfied!");
            return false;
        }

        String[] args = (String[]) input.toArray(new String[0]);

        FTCPath path = FTCPathParser.parse(context, args[0]);
            setCommand(new String[] {
                CommandName.getCLISyntax(CommandName.FILE_SYSTEM_CREATE_DIRECTORY.name()).toLowerCase(Locale.getDefault()),
                "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, path.getPrefix(),
                "-" + AbstractCommand.PARAMETER_TARGET_PATH, path.toString()});
            return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("MKDIR <TARGET_DIRECTORY_PATH>");
        System.out.println("- Creates directory on desired device path");
    }

}
