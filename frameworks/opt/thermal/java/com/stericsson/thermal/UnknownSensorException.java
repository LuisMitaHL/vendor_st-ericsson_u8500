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

/**
 * UnknownSensorException
 *
 * Configuration data was requested/set for a sensor that
 * does not exist
 */
public class UnknownSensorException extends Exception {
	private String sensorID;

	/**
	 * Exception constructor
	 */
	public UnknownSensorException(String sensor) {
		sensorID = sensor;
	}

	/**
	 * Retrives the invalid sensors id
	 * @return SensorID
	 */
	public String getInvalidSensorID() {
		return sensorID;
	}
}
