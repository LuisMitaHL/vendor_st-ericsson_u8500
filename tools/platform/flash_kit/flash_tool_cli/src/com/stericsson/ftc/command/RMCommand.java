package com.stericsson.ftc.command;

import java.util.Collection;
import java.util.Locale;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Command for removing files or folders from ME.
 * 
 * @author TSIKOR01
 * 
 */
public class RMCommand extends FTCAbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) {
        if (input.size() == 0) {
            System.out.println("Error! Target file path must be specyfied!");
            return false;
        }

        String[] args = (String[]) input.toArray(new String[0]);
        FTCPath path =FTCPathParser.parse(context, args[0]);

        setCommand(new String[] {
            CommandName.getCLISyntax(CommandName.FILE_SYSTEM_DELETE_FILE.name()).toLowerCase(Locale.getDefault()),
            "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, path.getPrefix(),
            "-" + AbstractCommand.PARAMETER_TARGET_PATH, path.getCommandString()});
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("RM <TARGET_PATH>");
        System.out.println("- Removes directory from desired path");
    }
}
