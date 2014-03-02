package com.stericsson.ftc.command;

import java.io.File;
import java.util.Collection;
import java.util.Locale;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.cli.ClientSocket;
import com.stericsson.sdk.cli.CommandParser;
import com.stericsson.sdk.cli.command.BRPCommandExecutor;

/**
 * Command for listing folders from ME.
 * 
 * @author TSIKOR01
 * 
 */
public class LSCommand extends FTCAbstractCommand {
    /**
     * {@inheritDoc} Method lists folders on disk and ME.
     */

    private FTCPath path;

    private String deviceID;

    /**
     * {@inheritDoc}
     */
    @Override
    public void execute() throws Exception {
        path = FTCPathParser.parse(context, getCommand()[0]);
        if (path!=null) {
            if (path.isOnME()) {
                listDirectoryOnME();
            } else {
                listDirectoryOnPC();
            }
        } else {
            System.out.println("Error! Path not found or not accessible!");
        }
    }

    private void listDirectoryOnPC() {
        File currentWorkingDirectory = new File(path.getSuffix());
        if (currentWorkingDirectory.isDirectory()) {
            printListedFiles(currentWorkingDirectory);
        } else {
            System.out.println("Error, Specified path is not directory!");
        }
    }

    private void printListedFiles(File currentWorkingDirectory) {
        String[] listedFiles = currentWorkingDirectory.list();
        if (listedFiles.length > 0) {
            System.out.println("Listing: " + currentWorkingDirectory.getAbsolutePath());
            for (int i = 0; i < listedFiles.length; i++) {
                System.out.println(listedFiles[i]);
            }
        } else {
            System.out.println(".\n..");
        }
    }

    private void listDirectoryOnME() throws Exception {
        ClientSocket.getInstance().startSocket(context.getHost(), context.getPort());
        deviceID = path.getPrefix();
        CommandParser cmdParser = new CommandParser();
        BRPCommandExecutor cmd =
            cmdParser.parse(new String[] {
                CommandName.getCLISyntax(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name())
                    .toLowerCase(Locale.getDefault()), "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, deviceID,
                "-" + AbstractCommand.PARAMETER_TARGET_PATH, path.toString()}, false);
        if (cmd != null) {
            cmd.execute();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) {
        if (input.size() == 0) {
            setCommand(new String[] {context.getWorkingDirectory().toString()});
        } else if(input.size()==1){
            setCommand(new String[] {
                input.iterator().next()});
        }else{
            return false;
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("LS <TARGET_DIRECTORY_PATH>");
        System.out.println("- List desired directory");
    }

}
