package com.stericsson.sdk.cli;

import java.util.Locale;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;

/**
 * test class for CLI
 * 
 * @author xdancho
 * 
 */
public class CLITest {

    static Thread serverThread = null;

    private static LocalServer localServer;

    /**
     * BeforeClass
     */
    @BeforeClass
    public static void setUpOnce() {

        serverThread = new Thread() {
            public void run() {
                localServer = new LocalServer();
            }
        };
        serverThread.start();
    }

    /**
     * 
     */
    @AfterClass
    public static void tearDownOnce() {
        localServer.stopServer();
    }

    /**
     * 
     */
    @Test
    public void testBRPCommands() {

        String inputCLI = "";
        for (CommandName cName : CommandName.values()) {
            FlashtoolCLI cli = new FlashtoolCLI();
            AbstractCommand aCmd = CommandFactory.createCommand(cName);
            String[] pNames = aCmd.getParameterNames();

            if (pNames != null) {
                StringBuffer buffer = new StringBuffer();
                for (String s : pNames) {
                    buffer.append(" -" + s + " value_of_" + s);
                }
                inputCLI =
                    CommandName.getCLISyntax(cName.name()).toLowerCase(Locale.getDefault())
                        + buffer.toString();

            } else {
                inputCLI = CommandName.getCLISyntax(cName.name()).toLowerCase(Locale.getDefault());
            }

            System.out.println("input CLI: " + inputCLI);
            cli.start(inputCLI.split(" "));

        }
    }

    /**
     * test cli help
     */
    @Test
    public void testHelp() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String inputCLI = "-help";
        cli.start(inputCLI.split(" "));

        cli = new FlashtoolCLI();
        inputCLI = "set_active_profile -help";
        cli.start(inputCLI.split(" "));

        cli = new FlashtoolCLI();
        inputCLI = "v -help";
        cli.start(inputCLI.split(" "));

        cli = new FlashtoolCLI();
        inputCLI = "help";
        cli.start(inputCLI.split(" "));

    }

    /**
     * test setting socked
     */
    @Test
    public void testHostAndPortOptions() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "-port 8088 -host localhost reload_configuration";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "-host localhost -port 8088 reload_configuration";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "-host localhost -port 9099 reload_configuration";
        cli.start(input.split(" "));
    }

    /**
     * test unrecognized command
     */
    @Test
    public void testUnrecognizedCommand() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "-port 8088 -host localhost unrecognized";
        cli.start(input.split(" "));
    }

    /**
     * test strange commands
     */
    @Test
    public void testStrangeCommands() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "-8088 -port  -host -unrecognized -localhost ";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "-8088 -port  -host -unrecognized -help -localhost";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "list_devices";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "unrecognized";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = " ";
        cli.start(input.split(" "));
    }

    /**
     * 
     */
    @Test
    public void testHelpNonExistingCommand() {
        CLIOptions cli = new CLIOptions();
        cli.printHelpCommand("nonexiting");

    }

    /**
     * test -gdfs and -ta parameters
     */
    @Test
    public void testGDFSandTACommands() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "write_global_data_set -equipment_id USB0 -gdfs -path c:\nonexisting";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "write_global_data_set -equipment_id USB0 -ta -path c:\nonexisting";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "write_global_data_set -equipment_id USB0 -gdfs -gdfs -path c:\nonexisting";
        cli.start(input.split(" "));

        cli = new FlashtoolCLI();
        input = "write_global_data_set -equipment_id USB0 -ta -ta -path c:\nonexisting";
        cli.start(input.split(" "));

    }

    /**
     * 
     */
    @Test
    public void testRemoteHost() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "get_connected_equipments -host 0.0.0.0 -port 4444 ";
        cli.start(input.split(" "));
    }

    /**
     * test verbose option
     */
    @Test
    public void testVerbose() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "process_file -equipment_id value_of_equipment_id -path value_of_path -v";
        cli.start(input.split(" "));
    }

    /**
     * test bare option
     */
    @Test
    public void testBare() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "get_connected_equipments -b";
        cli.start(input.split(" "));
    }

    /**
     * test NAK reply from server
     */
    @Test
    public void testNAK() {
        LocalServer.forceNAK();
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "process_file -equipment_id value_of_equipment_id -path value_of_path -v";
        cli.start(input.split(" "));
    }

    /**
     * test error reply from server
     */
    @Test
    public void testError() {
        LocalServer.forceError();
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "process_file -equipment_id value_of_equipment_id -path value_of_path -v";
        cli.start(input.split(" "));
    }

    /**
     * test bare option
     */
    @Test
    public void testTimeout() {
        FlashtoolCLI cli = new FlashtoolCLI();
        String input = "disable_auto_sense -t 1";
        cli.start(input.split(" "));
    }

    /**
     * 
     * @throws Exception .
     */
    @Test
    public void testCSocket() throws Exception {
        ClientSocket cs = ClientSocket.getInstance();
        cs.startSocket("localhost", 8088);
        cs.out = null;
        cs.stopSocket();
    }

    /**
     * 
     * @throws Exception .
     */
    @Test
    public void testCSocket1() throws Exception {
        ClientSocket cs = ClientSocket.getInstance();
        cs.startSocket("localhost", 8088);
        cs.in = null;
        cs.stopSocket();
    }

    /**
     * 
     * @throws Exception .
     */
    @Test
    public void testCSocket2() throws Exception {
        ClientSocket cs = ClientSocket.getInstance();
        cs.startSocket("localhost", 8088);
        cs.clientSocket = null;
        cs.stopSocket();
    }
}
