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
 * ThermalManager
 *
 * Provides interface to the platform thermal management service.
 *
 * Callers must retrieve access to ther ThermalManager via the
 * .getInstance() method.
 *
 * Once .release() has been called, the object reference should
 * be destroyed as undefined behaviour may occur.
 */
public class ThermalManager {
	private static final String sTAG = "ThermalManager";
	private static Thread mEventThread = null;
	private Vector<ThermalNotificationHandler> mNotificationHandlers = null;

	private ThermalManager() {
		Log.d(sTAG, "ThermalManager()");

		mNotificationHandlers = new Vector<ThermalNotificationHandler>();

		native_init(this);

		mEventThread = new ThermalManagerEventThread();
		mEventThread.start();
	}

	/**
	 * Retrieve ThermalManager instance
	 */
	public static ThermalManager getInstance() {
		Log.d(sTAG, "getInstance()");
		return new ThermalManager();
	}

	/**
	 * Releases the ThermalManager. Once called all object references
	 * should be destroyed and a new object should be obtained via
	 * the .getInstance() method.
	 */
	public void release() {
		mEventThread.interrupt();
		try {
			mEventThread.join();
		} catch (InterruptedException ie) {
			Log.e(sTAG, "unable to wait for event thread to be properly destroyed");
		}
		native_release();
	}

	/**
	 * Registers a new client that wishes to receive thermal notifications
	 * @param handler Thermal client
	 */
	public void registerNotificationHandler(ThermalNotificationHandler handler) {
		Log.d(sTAG, "registerNotificationHandler()");

		synchronized (mNotificationHandlers) {
			mNotificationHandlers.add(handler);
		}
	}

	/**
	 * Unregisters notification client
	 * @param handler Client to unregister
	 */
	public void unregisterNotificationHandler(ThermalNotificationHandler handler) {
		Log.d(sTAG, "unregisterNotificationHandler()");

		synchronized (mNotificationHandlers) {
			mNotificationHandlers.remove(handler);
		}
	}

	/**
	 * Retrieves a list of sensors currently available in the platform
	 * @return Sensors list
	 */
	public ArrayList<String> getAvailableSensors() {
		Log.d(sTAG, "getAvailableSensorIDs()");

		ArrayList<String> availableSensors = new ArrayList<String>();
		native_getsensors(availableSensors);

		return availableSensors;
	}

	/**
	 * Retrieves a list of actions currently available in the platform
	 * @return Actions list
	 */
	public ArrayList<String> getAvailableActions() {
		Log.d(sTAG, "getAvailableActions()");

		ArrayList<String> availableActions = new ArrayList<String>();
		native_getactions(availableActions);

		return availableActions;
	}

	/**
	 * Retrieves configuration of a sensor
	 * @param sensorID Sensor identifier
	 * @exception UnknownSensorException
	 * @return Configuration data
	 */
	public ThermalSensorConfiguration getSensorConfiguration(String sensorID) throws UnknownSensorException {
		Log.d(sTAG, "getSensorConfiguration()");

		ArrayList<String> availableSensors = new ArrayList<String>();
		native_getsensors(availableSensors);

		if (! availableSensors.contains(sensorID)) {
			Log.e(sTAG, "sensor " + sensorID + " can not be found");
			throw new UnknownSensorException(sensorID);
		}

		return native_getsensorconfig(sensorID);
	}

	/*
	 * EventThread
	 */
	private final class ThermalManagerEventThread extends Thread {
		private static final String EVENT_TAG = "ThermalManagerEventThread";

		public ThermalManagerEventThread() {
			super("ThermalManagerEventThread");
		}

		public void run() {
			boolean keepRunning = true;

			Log.d(EVENT_TAG, "starting...");
			while (keepRunning) {
				native_wait_for_event();

				if (interrupted()) {
					keepRunning = false;
				}
			}

			Log.d(EVENT_TAG, "shutting down event thread...");
		}
	}

	/*
	 * Native JNI interface functions
	 */
	{
		System.load("/system/lib/libthermal_jni.so");
		native_class_init();
	}

	private static native void native_class_init();
	private static native void native_init(ThermalManager instance);
	private static native void native_release();
	private static native void native_getsensors(ArrayList<String> list);
	private static native void native_getactions(ArrayList<String> list);
	private static native ThermalSensorConfiguration native_getsensorconfig(String sensorID);
	private static native void native_wait_for_event();

	private void reportEvent(ThermalNotification notification) {
		synchronized (mNotificationHandlers) {
			for (ThermalNotificationHandler h: mNotificationHandlers) {
				h.handleThermalNotification(notification);
			}
		}
	}
}
