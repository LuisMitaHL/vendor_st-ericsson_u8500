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

import android.content.Intent;
import android.test.AndroidTestCase;

import com.stericsson.modemtracecontrol.internal.ShutdownReceiver;
import com.stericsson.modemtracecontrol.test.CommandServerHelper;

public class ShutdownReceiverTest extends AndroidTestCase {

    private ShutdownReceiver mShutdownReceiver;
    private TestContext mContext;

    /**
     * @throws Exception :
     */
    @Override
    public void setUp() throws Exception {
        mShutdownReceiver = new ShutdownReceiver();
        mContext = new TestContext();
    }

    /**
     * @throws Exception :
     */
    @Override
    public void tearDown() throws Exception {
        mShutdownReceiver = null;
        mContext = null;
    }

    // Assert the broadcast is received but nothing done about it.
    public void testReceivePowerOffTraceNotOngoing() {
        ShutdownReceiver.setTraceOngoing(false);
        Intent intent = new Intent(Intent.ACTION_SHUTDOWN);
        mShutdownReceiver.onReceive(mContext, intent);
        // Assert no connection is set up and no TriggerReport called!
        assertFalse(mShutdownReceiver.getIsTriggerReportCreated());
    }

    // Assert the broadcast is received but nothing done about it.
    public void testReceivePowerOffTriggerDisabled() {
        ShutdownReceiver.setTriggerOnPoweroff(false);
        ShutdownReceiver.setTraceOngoing(true);
        Intent intent = new Intent(Intent.ACTION_SHUTDOWN);
        mShutdownReceiver.onReceive(mContext, intent);
        // Assert no connection is set up and no TriggerReport called!
        assertFalse(mShutdownReceiver.getIsTriggerReportCreated());
    }

    public void testReceivePowerOffEventAndTrigger() {
        CommandServerHelper serv = new CommandServerHelper();
        serv.startServer();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        ShutdownReceiver.setTraceOngoing(true);
        Intent intent = new Intent(Intent.ACTION_SHUTDOWN);
        mShutdownReceiver.onReceive(mContext, intent);
        assertTrue(mShutdownReceiver.getIsTriggerReportCreated());

        serv.setClientConnected(false);
        serv.stopServer();
    }
}
