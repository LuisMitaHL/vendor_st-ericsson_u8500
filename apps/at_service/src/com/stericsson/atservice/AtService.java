/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.io.IOException;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;

import android.app.Service;
import android.content.Intent;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.os.IBinder;
import android.util.Log;

/**
 * The AtService class used to handle AT commands in android. This classes sets
 * up an thread that listens for incoming socket connections and creates an
 * AtHandler for each connection.
 */
public class AtService extends Service {

    /**
     * The log tag.
     */
    public static final String LOG_TAG = "AtService";

    /**
     * Service start flags.
     */
    public static final String BOOT_COMPLETED_STR = "boot_completed";
    public static final String USER_PRESENT_STR = "user_present";
    public static final String THREAD_START_STR = "thread_start";

    /**
     * The number of parallel connections that can be handled by the service.
     */
    private static final int NBR_OF_CONNECTIONS = 8;

    /**
     * A pool of AtParsers
     */
    private AtParserPool atParserPool;

    /**
     * The thread listening for incoming socket connections.
     */
    private ConnectionThread connectionThread;

    /**
     * You cannot bind to the AT service at this point.
     *
     * @param intent
     *            not handled.
     * @return always null
     */
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    /**
     * Creates a LocalServerSocket and a thread used to listen for incoming
     * socket connections.
     */
    @Override
    public void onCreate() {
        Log.i(LOG_TAG, "onCreate: Entering");

        // create parsers
        atParserPool = new AtParserPool();

        Log.i(LOG_TAG, "onCreate: Leaving");
    }

    /**
     * Starts the connection thread.
     */
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        int result;
        Log.i(LOG_TAG, "onStartCommand: Entering intent=" + intent + ", flags=" + flags + ", startId=" + startId);

        // Start the connection thread. Set sticky return code so
        // the service remains running.
        // 'intent' will be null if this is an explicit restart by the system.
        // Regard this case a 'Thread start request'
        if (intent == null || intent.getBooleanExtra(THREAD_START_STR, false)) {
            // If the connectionThread is already running.  Do nothing.

            if (connectionThread == null || !connectionThread.isAlive()) {
                try {
                    createAndStartConnectionThread();

                    // Stick around.
                    result = START_STICKY;
                } catch (IOException e) {
                    Log.e(LOG_TAG, "onStartCommand: Could not create local server socket");
                    // fatal error, stop the service
                    stopSelf(startId);
                }
            }
            result = START_STICKY;

        // If the service gets started due to a broadcast intent, stop this start request
        // and send a new 'Thread start request' instead. This is done in order to
        // release the broadcast intent so there will be no timeout for not releasing it.
        } else if (intent.getBooleanExtra(BOOT_COMPLETED_STR, false) ||
                   intent.getBooleanExtra(USER_PRESENT_STR, false)) {

            startService(new Intent(this, AtService.class)
                     .putExtra(THREAD_START_STR, true));

            // Must stop or the broadcast intent will not be released
            stopSelf(startId);

            // Don't hang around
            result = START_NOT_STICKY;

        // Handle unexpected invocations
        } else {
            Log.e(LOG_TAG, "onStartCommand: Unexpected invocation!");

            result = START_NOT_STICKY;
            stopSelf(startId);
        }

        Log.i(LOG_TAG, "onStartCommand: Leaving startId=" + startId + ", result=" + result);
        return result;
    }

    /**
     * Stops the connection thread and cleans up.
     */
    @Override
    public void onDestroy() {
        Log.i(LOG_TAG, "onDestroy: Entering");
        // stop connection thread
        if (connectionThread != null && connectionThread.isAlive()) {
            connectionThread.close();
        }
        // empty AtCommandPool
        atParserPool.close();
        Log.i(LOG_TAG, "onDestroy: Leaving");
    }

    /**
     * Removes the at handler with id from the connection thread.
     *
     * @param id
     *            the id
     */
    public void removeHandler(Integer id) {
        connectionThread.removeHandler(id);

    }

    private void createAndStartConnectionThread() throws IOException {
        // create the local server socket
        LocalServerSocket serverSocket = new LocalServerSocket("at");

        // create the thread that will accept incoming connections
        connectionThread = new ConnectionThread(serverSocket);

        // Start the thread that will accept incoming connections
        connectionThread.start();
    }

    /**
     * Class used to handle incoming socket connections.
     */
    private class ConnectionThread extends Thread {

        /**
         * The server socket.
         */
        private LocalServerSocket server;

        /**
         * Value used to indicate if the connection thread should be active.
         */
        private boolean active;

        /**
         * The hash map used to save the at handlers.
         */
        private AtHashMap<Integer, AtHandler> handlers;

        /**
         * Constructor for a ConnectionThread.
         *
         * @param server
         *            the server used to listen for the connections on.
         */
        public ConnectionThread(LocalServerSocket server) {
            this.server = server;
            handlers = new AtHashMap<Integer, AtHandler>();
            active = true;
        }

        /**
         * Waiting for connections and creating at handlers for as long as the
         * connection thread is active.
         */
        public void run() {
            int count = 1;
            while (active) {
                try {
                    Log.d(LOG_TAG, "before accept");
                    // the thread can not be interrupted when it waits in accept
                    // - probably a bug in the android LocalServerSocket code
                    final LocalSocket socket = server.accept();
                    Log.i(LOG_TAG, "accept, have a socket");
                    final Integer id = new Integer(count);

                    // Makes it faster to accept the next connection
                    Thread thread = new Thread() {
                        public void run() {
                            try {
                                AtHandler atHandler = new AtHandler(socket,
                                                                    atParserPool.getParser(id),
                                                                    AtService.this, id);
                                Log.i(LOG_TAG, "adding handler with id " + id
                                      + " to map");

                                handlers.put(id, atHandler);
                            } catch (IOException e) {
                                Log.e(LOG_TAG, e.getMessage());
                            }
                        }
                    };
                    thread.start();
                    count++;

                } catch (IOException e) {
                    Log.i(LOG_TAG, "accept interrupted");
                }

            }
            closeHandlers();
            Log.d(LOG_TAG, "end of run-loop");

        }

        /**
         * Removes the at handler identified by id.
         *
         * @param id
         *            the id
         */
        public void removeHandler(Integer id) {
            Log.i(LOG_TAG, "removing handler " + id + " form map");
            handlers.remove(id);
            // return the parser
            atParserPool.returnParser(id);
        }

        /**
         * Closes the connection thread.
         */
        public void close() {
            active = false;
            try {
                // this does not trigger an exception in server.accept as it is
                // supposed to do in a java server socket. Ought to be a bug in
                // android that should be fixed in time.
                Log.i(LOG_TAG, "closing server");
                server.close();
            } catch (IOException e) {
                Log.e(LOG_TAG, "exception when trying to close server");
            }

        }

        /**
         * Help method for closing all the at handlers.
         */
        private void closeHandlers() {
            Collection<AtHandler> values = handlers.values();
            if (values != null) {
                Iterator<AtHandler> iterator = values.iterator();
                while (iterator.hasNext()) {
                    iterator.next().closeSocket();
                }
            }
            handlers.clear();
        }

    }

    /**
     * Class used to handle the AtParsers. Must be created from the AtService
     * and not a background thread because AtHandlers may access GUI when they
     * are created.
     */
    private class AtParserPool {

        /**
         * The AtParsers not used at the moment
         */
        private LinkedList<AtParser> freeParsers;

        /**
         * The AtParsers connected to an AtHandler with a certain id.
         */
        private AtHashMap<Integer, AtParser> parsersInUse;

        /**
         * Constructor for AtParserPool.
         */
        public AtParserPool() {
            freeParsers = new LinkedList<AtParser>();
            createParsers();
            parsersInUse = new AtHashMap<Integer, AtParser>();
        }

        /**
         * Creates the AtParsers and register all available AtCommandHandlers.
         */
        private void createParsers() {
            for (int i = 0; i < NBR_OF_CONNECTIONS; i++) {
                AtParser parser = new AtParser();
                parser.registerAllCommands(AtService.this);
                freeParsers.add(parser);
            }

        }

        /**
         * Returns a free AtParser as soon as one is available.
         *
         * @param id
         *            the of the AtHandler the parser will be given to
         * @return an AtParser
         */
        public synchronized AtParser getParser(Integer id) {
            while (freeParsers.isEmpty()) {
                try {
                    Log.d(LOG_TAG, "No free parser, already "
                          + NBR_OF_CONNECTIONS
                          + " active connections. Waiting.");
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

            }
            AtParser parser = freeParsers.getLast();
            parsersInUse.put(id, parser);
            return parser;
        }

        /**
         * Returns an AtParser to the pool.
         *
         * @param id
         *            associated with it's handler
         */
        public synchronized void returnParser(Integer id) {
            AtParser parser = parsersInUse.remove(id);
            if (parser != null) {
                freeParsers.add(parser);
                notifyAll();
            } else {
                Log.e(LOG_TAG, "No parser associated with id " + id.intValue());

            }

        }

        /**
         * Closes the AtCommandPool.
         */
        public void close() {
            freeParsers.clear();
            parsersInUse.clear();
        }
    }

}
