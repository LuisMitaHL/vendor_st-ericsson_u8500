/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.stericsson.modemtracecontrol.internal;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import android.util.Log;

/**
 * Handles the socket communication between this Android application (client)
 * and the module Modem_log_relay (server).
 */
public class CommandSender extends Socket {

    private static final String LOCALHOST = "localhost";

    private static final int REQUESTPORT = 2001;

    private Socket commandSocket;

    private PrintWriter commandRequest;

    private BufferedReader commandResponse;

    private String mlrResponse;

    /**
     * Constructor
     */
    public CommandSender() {
        mlrResponse = new String("KO");
    }

    /**
     * Wraps host and port while initializing the connection to the server.
     *
     * @return SUCCESS on successful connection FAILURE on error
     */
    public int Connect() {
        return Connect(LOCALHOST, REQUESTPORT);
    }

    /**
     * Initializes the connection to the server.
     *
     * @param host Host name
     * @param port port to connect to
     * @return SUCCESS on successful connection FAILURE on error
     */
    public int Connect(String host, int port) {

        try {
            // Create a client socket to communicate with the MLR
            commandSocket = new Socket(host, port);
            commandRequest = new PrintWriter(commandSocket.getOutputStream(), true);
            commandResponse = new BufferedReader(new InputStreamReader(
                    commandSocket.getInputStream()));
        } catch (UnknownHostException unknownHost) {
            Log.e(Utility.APP_NAME, "Socket connection failed with unknown host");
            return Utility.FAILURE;

        } catch (IOException e) {
            Log.e(Utility.APP_NAME, "Socket connection failed with IOException");
            e.printStackTrace();
            return Utility.FAILURE;
        }

        Log.d(Utility.APP_NAME, "Connected to client socket successfully");
        return Utility.SUCCESS;
    }

    /**
     * Disconnects from the server.
     *
     * @return SUCCESS on successful connection FAILURE on error
     */
    public int Disconnect() {
        try {
            // Close the socket to the MLR
            if (commandRequest != null) {
                commandRequest.close();
            }
            if (commandResponse != null) {
                commandResponse.close();
            }
            if (commandSocket != null) {
                commandSocket.close();
            }
            Log.d(Utility.APP_NAME, "Socket connection to MLR is closed successfully");
        } catch (IOException e) {
            Log.e(Utility.APP_NAME, "Unable to close the connection to MLR");
            e.printStackTrace();
            return Utility.FAILURE;
        }
        return Utility.SUCCESS;
    }

    /**
     * Get the response received from MLR for last command sent
     * 
     * @return Response string from MLR
     */
    public String getCommandResponse() {
        return mlrResponse;
    }

    /**
     * Sends a command to the server.
     *
     * @param command Command to be sent to the server
     * @param Response Response from the server is stored
     * @return SUCCESS on successful connection FAILURE on error
     */
    public int SendCommand(String command) {

        try {
            if (commandRequest == null || commandResponse == null) {
                Log.e(Utility.APP_NAME, "Request channel is not connected");
                return Utility.FAILURE;
            }

            // Send the command over the socket
            commandRequest.println(command);
            commandRequest.flush();

            // Wait for the response
            String response = commandResponse.readLine();

            // Check for Empty response
            if ((response == null) || (response.length() == 0)) {
                Log.e(Utility.APP_NAME, "Received empty Response");
                return Utility.FAILURE;
            }

            // Copy the String
            mlrResponse = response.toString();

            // If we are logging to SDCARD, the response is different,
            // so do a separate check
            if (command.equals("trace -q SDCARD") && response.equals("false")) {
                Log.d(Utility.APP_NAME, "Received Response ( " + command + " ) : " + response);
                return Utility.FAILURE;
            }

            // Check for response type KO
            if (response.equals("KO")) {
                Log.d(Utility.APP_NAME, "Received Response ( " + command + " ) : " + response);
                return Utility.FAILURE;
            }

            // Command succeeded
            Log.d(Utility.APP_NAME, "Received Response on command ( " + command + " ) : "
                    + response);

        } catch (SocketTimeoutException socketTimeoutEx) {
            Log.e(Utility.APP_NAME, "Socket timeout while sending command");
            return Utility.FAILURE;

        } catch (SocketException socketEx) {
            Log.e(Utility.APP_NAME, "Socket exception while sending command");
            return Utility.FAILURE;
        } catch (IOException ioEx) {
            Log.e(Utility.APP_NAME,
                    "Socket connection failed with IOException while sending command");
            return Utility.FAILURE;
        }

        return Utility.SUCCESS;
    }
}
