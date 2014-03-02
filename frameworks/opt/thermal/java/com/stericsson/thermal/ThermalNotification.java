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
package com.stericsson.thermal;

import android.util.Log;
import java.util.ArrayList;
import java.util.Vector;

/**
 * ThermalNotification
 *
 * Passed to ThermalNotificationHandlers to inform them of a
 * thermal event
 */
public class ThermalNotification {
	private boolean shutdownNotification;
	private String sensorID;
	private ArrayList<String> mActions;
	private int mShutdownIn;
	private int mCurrentLevel;
	private int mPreviousLevel;

	private ThermalNotification(String sensor,
	                            boolean shutdown,
	                            int shutdownTimeout,
	                            int currentNotificationLevel,
	                            int previousNotifcationLevel) {
		mActions = null;
		sensorID = sensor;
		shutdown = shutdown;
		mCurrentLevel = currentNotificationLevel;
		mPreviousLevel = previousNotifcationLevel;
	}

	/**
	 * Determines if a shutdown is going to occur
	 * @return true If the platform is going to shutdown.
	 */
	public boolean isShutdownNotification() {
		return shutdownNotification;
	}

	/**
	 * Determines Sensor name
	 * @return Sensor ID
	 */
	public String getSensorName() {
		return sensorID;
	}

	/**
	 * Determines shutdown time out
	 * @return shutdown timeout in seconds
	 */
	public int getShutdownTimeout() {
		return mShutdownIn;
	}

	/**
	 * Determines current notification level
	 * @return Current level
	 */
	public int getCurrentNotificationLevel() {
		return mCurrentLevel;
	}

	/**
	 * Determines previous notification level
	 * @return Previous level
	 */
	public int getPreviousNotificationLevel() {
		return mPreviousLevel;
	}

	/**
	 * Determines mitigation actions
	 * @return Actions
	 * @return null if no action
	 */
	public ArrayList<String> getMitigationActions() {
		return mActions;
	}

	/*
	 * Helper function used by JNI code to add a mitigation action
	 */
	private void addAction(String action) {
		if (mActions == null) {
			mActions = new ArrayList<String>();
		}
		mActions.add(action);
	}
}
