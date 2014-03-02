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
 * ThermalSensorConfiguration
 */
public class ThermalSensorConfiguration {
	private String sensorName;
	private int mCurrentNotificationLevel;
	private ArrayList<ThermalNotificationLevel> mNotificationLevels;

	/*
	 * Prevent this class being created outside of the JNI layer
	 */
	private ThermalSensorConfiguration (String name, int curNotLevel) {
		mNotificationLevels = null;
		sensorName = name;
		mCurrentNotificationLevel = curNotLevel;
	}

	private void addNotificationLevel(ThermalNotificationLevel level) {
		//Adds the new notification level into the list
		if (mNotificationLevels == null) {
			mNotificationLevels = new ArrayList<ThermalNotificationLevel>();
		}
		mNotificationLevels.add(level);
	}

	/**
	 * Determines the current notification level
	 * @return Current Notification Level
	 */
	public int getCurrentNotificationLevel() {
		return mCurrentNotificationLevel;
	}

	/**
	 * Determines the thermal notification level
	 * @return list of notification levels
	 */
	public ArrayList<ThermalNotificationLevel> getNotificationLevels() {
		return mNotificationLevels;
	}
}
