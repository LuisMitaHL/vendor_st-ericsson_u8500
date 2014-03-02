/**
 * Copyright (C) ST-Ericsson SA 2011
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
 * limitations under the License
 */

package com.stericsson.thermal.test;

import junit.framework.Assert;
import android.test.AndroidTestCase;
import android.util.Log;
import java.util.*;
import com.stericsson.thermal.*;
import java.util.concurrent.*;

/*
 * Thermal Java API Tests
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
public class ThermalTest extends AndroidTestCase
			implements ThermalNotificationHandler {
	private static final String TAG = "ThermalAPITest";
	private SynchronousQueue <ThermalNotification> notificationEvent;

	public void testAPI() {
		ThermalManager tm;

		ArrayList<String> actions;
		ArrayList<String> sensors;

		notificationEvent = new SynchronousQueue<ThermalNotification>();

		// Register ourself as a Thermal Handler
		tm = ThermalManager.getInstance();
		tm.registerNotificationHandler(this);

		// List all actions
		Log.d(TAG, "Actions in the system are:");
		actions = tm.getAvailableActions();
		for (String s:actions) {
			Log.d(TAG, "* " + s);
		}

		// List all sensors
		Log.d(TAG, "Sensors in the system are:");
		sensors = tm.getAvailableSensors();
		for (String s:sensors) {
			Log.d(TAG, "* " + s);
		}

		// Get config for sensors
		for (String sensor:sensors) {
			ThermalSensorConfiguration config;

			Log.d(TAG, "Configuration for: " + sensor);

			try {
				config = tm.getSensorConfiguration(sensor);

				Log.d(TAG, "+ Current notification level: " +
				      config.getCurrentNotificationLevel());

				for (ThermalNotificationLevel level: config.getNotificationLevels()) {
					Log.d(TAG, "+ Minimum alarm level: " + level.getMinAlarmLevel());
					Log.d(TAG, "+ Maximum alarm level: " + level.getMaxAlarmLevel());

					for (String action:level.getMitigationActions()) {
						Log.d(TAG, "+++ Mitigation Action: " + action);
					}
				}
			} catch (UnknownSensorException use) {
				Log.e(TAG, "Unknown sensor " + sensor);
			}
		}

		ThermalNotfication n = notificationEvent.poll();
		tm.release();
	}

	/**
	 * This method is called from ther ThermalManager when a new platform
	 * thermal notification is received.
	 */
	public void handleThermalNotification(ThermalNotification e) {
		Log.d(TAG, "Notification on: " + e.getSensorName());

		if (e.isShutdownNotification()) {
			Log.w(TAG, "WARNING: platform will be shutdown in: " + e.getShutdownTimeout () + " secs");
		}

		Log.d(TAG, "Current notification level: " + e.getCurrentNotificationLevel());
		Log.d(TAG, "Previous notification level: " + e.getPreviousNotificationLevel());
		Log.d(TAG, "the following thermal mitagation actions will be taken: ");

		for (String s:e.getMitigationActions()) {
			Log.d(TAG, "+++ " + s);
		}

		// post this back to the main thread
		try {
			notificationEvent.put(e);
		} catch (Exception ex) {
			Log.e(TAG, "Exception error");
		}
	}
}
