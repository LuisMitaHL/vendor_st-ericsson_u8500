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
 * ThermalNotificationLevel
 */
public class ThermalNotificationLevel {
	private int mNotificationLevel;
	private int mMinAlarmLevel;
	private int mMaxAlarmLevel;
	private ArrayList<String> mActions;

	/*
	 * Prevent this class being created outside of the JNI layer.
	 */
	private ThermalNotificationLevel(int level, int min, int max) {
		mActions = null;
		mNotificationLevel = level;
		mMinAlarmLevel = min;
		mMaxAlarmLevel = max;
	}
	/**
	 * Determines notification level
	 * @return notification level
	 */
	public int getNotificationLevel() {
		return mNotificationLevel;
	}

	/**
	 * Determines minimum alarm level
	 * @return minimum alarm level
	 */
	public int getMinAlarmLevel() {
		return mMinAlarmLevel;
	}

	/**
	 * Determines maximum alarm level
	 * @return maximum alarm level
	 */
	public int getMaxAlarmLevel () {
		return mMaxAlarmLevel;
	}

	/**
	 * Determines mitigation actions
	 * @return list of mitigation actions
	 * @return null if no action
	 */
	public ArrayList<String> getMitigationActions() {
		return mActions;
	}

	private void addAction(String action) {
		if (mActions == null) {
			mActions = new ArrayList<String>();
		}

		mActions.add(action);
	}
}
