package com.stericsson.ftc.command;

import java.util.Collection;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Class for copying files from device to drive or from drive to device.
 * 
 * @author TSIKOR01
 * 
 */
public class CPCommand extends FTCAbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) throws Exception {
        FTCPath sourceFile;

        FTCPath targetFile;
        if (input.size() != 2) {
            System.out.println("Error! Source and target path must be specyfied!");
            return false;
        }
        String[] args = (String[]) input.toArray(new String[0]);
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
        String me = "PC";
        if (sourceFile.isOnME()) {
            me = sourceFile.getPrefix();
        } else if (targetFile.isOnME()) {
            me = targetFile.getPrefix();
        }else{
            System.out.println("Cannot copy file from pc to pc by this command.");
            return false;
        }
        setCommand(new String[] {
            CommandName.getCLISyntax(CommandName.FILE_SYSTEM_COPY_FILE.name()),
            "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, me, "-" + AbstractCommand.PARAMETER_SOURCE_PATH,
            sourceFile.getCommandString(), "-" + AbstractCommand.PARAMETER_DESTINATION_PATH,
            targetFile.getCommandString()});
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("CP <SOURCE_PATH> <DESTINATION_PATH>");
        System.out.println("- Copies file from source path to destination path");
    }
}
