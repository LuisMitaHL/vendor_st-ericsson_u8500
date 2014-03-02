package com.stericsson.ftc.command;

import java.util.Collection;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;

/**
 * Class for changing working directory of console instance.
 * 
 * @author TSIKOR01
 * 
 */
public class CDCommand extends FTCAbstractCommand {

    private FTCPath path = null;

    /**
     * {@inheritDoc}
     */
    @Override
    public void execute() throws Exception {
        context.setWorkingDirectory(path);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) throws Exception {
        String userInputPath = null;
        if (input.size() == 0) {
            userInputPath = "";
        } else if (input.size() == 1) {
            userInputPath = input.iterator().next();
        } else {
            return false;
        }
        path = FTCPathParser.parse(context, userInputPath);
        if (path == null || !path.isDirectory(context)) {
            System.out.println("Error! Path not found or not directory: " + path);
            return false;
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("CD <PATH_TO_DIRECTORY>");
        System.out.println("- Changes directory to destination path");
    }

}
