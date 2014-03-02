package com.stericsson.sdk.backend.remote.executor.backend;

import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.backend.remote.io.BackendClientHandler;
import com.stericsson.sdk.backend.remote.io.IBackendServerConnection;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.InvalidSyntaxException;
import com.stericsson.sdk.equipment.EquipmentException;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

/**
 * @author ezaptom
 * 
 */
public class GetAvailableProfilesExecutorTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testGetAvailableProfiles() {
        GetAvailableProfilesExecutor executor = new GetAvailableProfilesExecutor();

        try {
            executor.setCommand(createCommand());
            String activeProfiles = executor.execute();

            assertNotNull(activeProfiles);

            executor.setExecutionHandler(new BackendClientHandler(new IBackendServerConnection() {
                public void write(String line) throws IOException {
                }

                public String readLine() throws IOException {
                    return "test";
                }

                public String getAddress() {
                    return "test";
                }

                public void close() {

                }
            }));

            IEquipmentProfileManagerService service = new IEquipmentProfileManagerService() {
                private int count = 0;

                private int percent = 0;

                public boolean isLoading() {
                    boolean isLoading = count < 3;
                    count++;
                    return isLoading;
                }

                public IEquipmentProfile getProfile(String alias, boolean wait) {
                    return null;
                }

                public int getPercentageOfLoadedProfiles() {
                    return percent++;
                }

                public String[] getAllAvailableProfilesNames() throws EquipmentException {
                    return null;
                }
            };

            executor.checkLoading(service);
        } catch (Exception e) {
            fail(e.getMessage());
        }

    }

    private AbstractCommand createCommand() throws InvalidSyntaxException {
        String completeString = CommandName.BACKEND_GET_AVAILABLE_PROFILES.name() + AbstractCommand.DELIMITER + "yes";
        return CommandFactory.createCommand(completeString);
    }
}
