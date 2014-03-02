package com.stericsson.sdk.backend.remote.test;

import java.io.IOException;
import java.net.Socket;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.backend.remote.io.BackendServerConnection;
import com.stericsson.sdk.backend.remote.io.ServerProperties;
import com.stericsson.sdk.backend.test.Activator;
import com.stericsson.sdk.brp.ServerResponse;

/**
 * 
 * @author xolabju
 * 
 */
public class BackendServerTest extends TestCase {

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testServerCommunication() throws Exception {
        try {
            BackendServerConnection client = null;
            int port = 44455;
            setAcceptClients(false);
            assertFalse(testConnection(port));
            assertFalse(Activator.getBackendService().isAcceptingClients());
            System.setProperty(ServerProperties.PORT_PROPERTY_NAME, port + "");
            setAcceptClients(true);
            Thread.sleep(500);
            assertTrue(Activator.getBackendService().isAcceptingClients());
            assertTrue(testConnection(port));
            Socket s = new Socket("localhost", port);
            client = new BackendServerConnection(s);
            client.write("InvalidCommand");
            String line = client.readLine();
            if (line == null) {
                throw new Exception("Got unexpected null value");
            }
            assertTrue(line.endsWith(ServerResponse.NAK.name()));
            client.close();
            setAcceptClients(false);
            Thread.sleep(1000);
            assertFalse(testConnection(port));
            assertFalse(Activator.getBackendService().isAcceptingClients());
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testServerPortError() throws Exception {
        try {
            setAcceptClients(false);
            Thread.sleep(500);
            BackendServerConnection client = null;
            String port = "error";
            System.setProperty(ServerProperties.PORT_PROPERTY_NAME, port);
            setAcceptClients(true);
            Thread.sleep(500);
            assertTrue(testConnection(ServerProperties.DEFAULT_PORT_NUMBER));
            Socket s = new Socket("localhost", ServerProperties.DEFAULT_PORT_NUMBER);
            client = new BackendServerConnection(s);
            client.write("InvalidCommand");
            String line = client.readLine();
            if (line == null) {
                throw new Exception("Got unexpected null value");
            }
            assertTrue(line.endsWith(ServerResponse.NAK.name()));
            client.close();
            setAcceptClients(false);
            Thread.sleep(500);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testServerConnectionError() {
        try {
            setAcceptClients(false);
            Thread.sleep(500);
            int port = 44455;
            System.setProperty(ServerProperties.PORT_PROPERTY_NAME, port + "");
            setAcceptClients(true);
            Thread.sleep(500);
            assertTrue(testConnection(port));
            setAcceptClients(false);
            Thread.sleep(500);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    private void setAcceptClients(boolean accept) {
        Activator.getBackendService().setAcceptClients(accept);
    }

    private boolean testConnection(int port) {
        boolean ok;
        Socket s = null;
        try {
            s = new Socket("localhost", port);
            ok = true;
        } catch (IOException ioe) {
            ok = false;
        } finally {
            if (s != null) {
                try {
                    s.close();
                } catch (IOException e) {
                    e.getMessage();
                }
            }
        }
        return ok;
    }
}
