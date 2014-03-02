/**
 * 
 */
package com.stericsson.sdk.backend.remote.executor.filesystem;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * @author Vit Sykala
 */
public class FileSystemExecutorTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testListDirectory() {
        FileSystemCommandExecutor executor = new FileSystemCommandExecutor();
        AbstractCommand ac = null;
        try {
            ac =
                CommandFactory.createCommand(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name() + AbstractCommand.DELIMITER
                    + "PortA" + AbstractCommand.DELIMITER + "/flash/");

        } catch (InvalidSyntaxException e) {
            fail(e.getMessage());
        }
        executor.setCommand(ac);
        try {
            assertNotNull(executor.execute());
            executor.taskStart(null);
            executor.taskProgress(null, 0, 0);
        } catch (ExecutionException e) {
            fail(e.getMessage());
        }
    }
}
