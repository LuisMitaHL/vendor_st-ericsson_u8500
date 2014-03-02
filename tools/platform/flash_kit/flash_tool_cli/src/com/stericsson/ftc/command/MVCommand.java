package com.stericsson.ftc.command;

import java.util.Collection;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Command representing MOVE action from PC to device or from device to PC.
 * 
 * @author TSIKOR01
 * 
 */
public class MVCommand extends FTCAbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) throws Exception {
        if (input.size() != 2) {
            System.out.println("Error! Source and target path must be specyfied!");
            return false;
        }
        String[] args = (String[]) input.toArray(new String[0]);
        FTCPath sourceFile;

        FTCPath targetFile;
        sourceFile = FTCPathParser.parse(context, args[0]);
        targetFile = FTCPathParser.parse(context, args[1]);

        if (sourceFile.exist(context)) {
            return prepareCommand(sourceFile, targetFile);
        } else {
            System.out.println("Error while executing CP command! Please check that you are entering correct path, "
                + "and your device is succesfully connected");
            return false;
        }
    }

    private boolean prepareCommand(FTCPath sourceFile, FTCPath targetFile) {
        if (sourceFile.isOnME() && targetFile.isOnME()) {
            setCommand(new String[] {
                CommandName.getCLISyntax(CommandName.FILE_SYSTEM_MOVE_FILE.name()),
                "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, sourceFile.getPrefix(),
                "-" + AbstractCommand.PARAMETER_SOURCE_PATH, sourceFile.getCommandString(),
                "-" + AbstractCommand.PARAMETER_DESTINATION_PATH, targetFile.getCommandString()});
            return true;
        } else {
            return false;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("MW <SOURCE_PATH><TARGET_PATH>");
        System.out.println("- Move file to desired path");
    }
}
