package com.stericsson.ftc.command;

import java.util.Collection;
import java.util.Locale;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * 
 * @author TSIKOR01
 * 
 */
public class CHMODCommand extends FTCAbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) {

        if (input.size()!= 2) {
            System.out.println("Error! Target volume path must be specyfied!");
            return false;
        }

        String[] args = (String[]) input.toArray(new String[0]);
        FTCPath path = FTCPathParser.parse(context,args[0]);
        String access = args[1];

        setCommand(new String[] {
            CommandName.getCLISyntax(CommandName.FILE_SYSTEM_CHANGE_ACCESS.name()).toLowerCase(Locale.getDefault()),
            "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, path.getPrefix(),
            "-" + AbstractCommand.PARAMETER_TARGET_PATH, path.toString(), "-" + AbstractCommand.PARAMETER_ACCESS, access});
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("CHMOD <TARGET_FILE_PATH> <ACCES_LEVEL>");
        System.out.println("- Changes acces rights for the file");
    }
}
