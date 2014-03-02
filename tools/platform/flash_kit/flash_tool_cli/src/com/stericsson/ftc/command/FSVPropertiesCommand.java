package com.stericsson.ftc.command;

import java.util.Collection;
import java.util.Locale;

import com.stericsson.ftc.FTCAbstractCommand;
import com.stericsson.ftc.FTCPath;
import com.stericsson.ftc.FTCPathParser;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Command providing FileSystem Volume properties.
 * 
 * @author TSIKOR01
 * 
 */
public class FSVPropertiesCommand extends FTCAbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean setParameters(Collection<String> input) {
        if (input.size() == 0) {//TODO input size != 1
            System.out.println("Error! Target volume path must be specyfied!");
            return false;
        }

        String[] args = (String[]) input.toArray(new String[0]);
        FTCPath path = FTCPathParser.parse(context,args[0]);

        setCommand(new String[] {
            CommandName.getCLISyntax(CommandName.FILE_SYSTEM_VOLUME_PROPERTIES.name()).toLowerCase(Locale.getDefault()),
            "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, path.getPrefix(),
            "-" + AbstractCommand.PARAMETER_DEVICE_PATH, path.toString()});
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void printHelp() {
        System.out.println("FSV <TARGET_VOLUME_PATH>");
        System.out.println("- Prints volume properties");
    }
}
