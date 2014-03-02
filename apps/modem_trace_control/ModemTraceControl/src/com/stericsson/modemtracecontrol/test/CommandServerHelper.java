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

package com.stericsson.modemtracecontrol.test;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import android.util.Log;

import com.stericsson.modemtracecontrol.internal.Utility;

/**
 * Helper class used when the real server (MLR) is not available, such as for
 * when testing on the emulator.
 */
public class CommandServerHelper {

    private static final int REQUESTPORT = 2001;

    private Thread threadServer;

    private ServerSocket serverSocket = null;

    private PrintWriter out;

    private BufferedReader in;

    private Boolean isShutdown = false;

    private Boolean isClientConnected = true;

    private static Boolean isStarted = false;

    private String response = "OK";

    public void setResponse(String response) {
        this.response = response;
    }

    /**
     * Starts the mock server in a new thread.
     */
    public void startServer() {

        if (isStarted == false) {
            Log.d(Utility.APP_NAME, "startServer()");

            isStarted = true;
            isShutdown = false;

            Runnable runnable = new ServerThread("ModemTraceServer");
            threadServer = new Thread(runnable);
            threadServer.start();

            try {
                Thread.sleep(1000);
            } catch (InterruptedException ex) {
                Log.d(Utility.APP_NAME, "Thread.sleep()");
            }
            Log.d(Utility.APP_NAME, "Mock server started");
        }
    }

    /**
     * Stops the mock server.
     */
    public void stopServer() {
        isShutdown = true;

        Log.d(Utility.APP_NAME, "Mock server stopped");
        try {
            Thread.sleep(1000);
            serverSocket.close();
            threadServer.join();
            isStarted = false;
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Sets whether a client is connected to this server or not.
     *
     * @param val
     *            true if a client is connected to this server, false otherwise.
     */
    public void setClientConnected(Boolean val) {
        isClientConnected = val;

    }

    /**
     * The thread in which this mock server is run.
     */
    private class ServerThread implements Runnable {
        public ServerThread(String str) {

        }

        public void run() {
            try {
                serverSocket = new ServerSocket(REQUESTPORT);
                if (serverSocket == null) {
                    Log.e(Utility.APP_NAME,
                            "Not able to create the Server Socket");
                }
                Log.d(Utility.APP_NAME, "Server socket created.");

                Socket client = serverSocket.accept();
                Log.d(Utility.APP_NAME, "Connected to a client");

                isClientConnected = true;
                in = new BufferedReader(new InputStreamReader(
                        client.getInputStream()));
                out = new PrintWriter(client.getOutputStream(), true);

                while (!isShutdown && isClientConnected) {
                    String line = in.readLine();
                    if (line != null) {
                        Log.d(Utility.APP_NAME, "Request from client : "
                                + line);

                        // Send data back to client
                        if (line.equals("trace -r")) {
                            Thread.sleep(2000);
                        }

                        // Send data back to client
                        if (line.equals("trace -q SDCARD")) {
                            out.println(response);
                        } else {

                            out.println(response);
                        }
                    }
                }
                in.close();
                out.close();
                client.close();
                isStarted = false;

            } catch (IOException e) {
                e.printStackTrace();
                return;
            } catch (InterruptedException e) {
                e.printStackTrace();
                return;
            }

        }
    }

}
