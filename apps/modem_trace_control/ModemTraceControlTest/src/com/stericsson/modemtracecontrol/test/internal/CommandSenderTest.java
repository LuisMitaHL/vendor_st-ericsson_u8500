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

package com.stericsson.modemtracecontrol.test.internal;

import android.test.AndroidTestCase;

import com.stericsson.modemtracecontrol.internal.CommandSender;
import com.stericsson.modemtracecontrol.test.CommandServerHelper;

public class CommandSenderTest extends AndroidTestCase {
    private static final int SUCCESS = 0;

    private static final int FAILURE = -1;

    CommandSender mCommandSenderTest;

    /**
     * @throws Exception :
     */
    @Override
    public void setUp() throws Exception {
        System.out.println("-------------------------------------------");
        mCommandSenderTest = new CommandSender();

    }

    /**
     * @throws Exception :
     */
    @Override
    public void tearDown() throws Exception {
        mCommandSenderTest = null;
    }

    /**
    *
    */
    public final void testConnectSuccess() {

        CommandServerHelper serv = new CommandServerHelper();
        serv.startServer();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        int retcode = mCommandSenderTest.Connect();
        assert (retcode == SUCCESS);

        mCommandSenderTest.Disconnect();
        serv.setClientConnected(false);
        serv.stopServer();

    }

    /**
    *
    */
    public final void testConnectSuccessUnknownHost() {
        int retcode = mCommandSenderTest.Connect("Unknownhost", 2001);
        assert (retcode == FAILURE);
    }

    /**
    *
    */
    public final void testDisConnectSuccess() {

        CommandServerHelper serv = new CommandServerHelper();
        serv.startServer();

        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        mCommandSenderTest.Connect();
        int retcode = mCommandSenderTest.Disconnect();
        assert (retcode == SUCCESS);
        serv.setClientConnected(false);
        serv.stopServer();

    }

    /**
    *
    */
    public final void testSendCommandSuccess() {

        CommandServerHelper serv = new CommandServerHelper();
        serv.startServer();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        mCommandSenderTest.Connect();
        int retcode = mCommandSenderTest.SendCommand("trace -t buffer");
        mCommandSenderTest.Disconnect();
        assert (retcode == SUCCESS);
        serv.setClientConnected(false);
        serv.stopServer();

    }

    /**
    *
    */
    public final void testSendCommandEmptyResponse() {

        CommandServerHelper serv = new CommandServerHelper();
        serv.setResponse("");
        serv.startServer();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        mCommandSenderTest.Connect();
        int retcode = mCommandSenderTest.SendCommand("trace -t buffer");
        mCommandSenderTest.Disconnect();
        assert (retcode == FAILURE);
        serv.setClientConnected(false);
        serv.stopServer();

    }

    /**
    *
    */
    public final void testSendCommandResponseSDCARDLoggingFalse() {

        CommandServerHelper serv = new CommandServerHelper();
        serv.setResponse("false");
        serv.startServer();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        mCommandSenderTest.Connect();
        int retcode = mCommandSenderTest.SendCommand("trace -q SDCARD");
        String response = mCommandSenderTest.getCommandResponse();
        mCommandSenderTest.Disconnect();
        assert (retcode == FAILURE);
        assertEquals("false", response);
        serv.setClientConnected(false);
        serv.stopServer();

    }

    /**
    *
    */
    public final void testSendCommandResponseResponseKO() {

        CommandServerHelper serv = new CommandServerHelper();
        serv.setResponse("KO");
        serv.startServer();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        mCommandSenderTest.Connect();
        int retcode = mCommandSenderTest.SendCommand("trace -t buffer");
        mCommandSenderTest.Disconnect();
        assert (retcode == FAILURE);
        serv.setClientConnected(false);
        serv.stopServer();

    }

}
