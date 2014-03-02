/*
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
 * limitations under the License.
 */
package com.stericsson.ril.oem.service.test;


/**
  * Phone is a huge interface and we only need to mock a couple of methods.
  * This class just throws UnsupportedOperationException on all methods.
  */

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.telephony.CellLocation;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;

import com.android.internal.telephony.Call;
import com.android.internal.telephony.CallStateException;
import com.android.internal.telephony.Connection;
import com.android.internal.telephony.DataConnection;
import com.android.internal.telephony.IccCard;
import com.android.internal.telephony.IccPhoneBookInterfaceManager;
import com.android.internal.telephony.IccSmsInterfaceManager;
import com.android.internal.telephony.MmiCode;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneSubInfo;
import com.android.internal.telephony.UUSInfo;
import com.android.internal.telephony.gsm.NetworkInfo;
import com.android.internal.telephony.test.SimulatedRadioControl;

import java.util.List;

public abstract class MockPhoneBase implements Phone {
    /* Not Implemented methods */
    public ServiceState getServiceState() {
        throw new UnsupportedOperationException();
    }

    public CellLocation getCellLocation() {
        throw new UnsupportedOperationException();
    }

    public DataState getDataConnectionState() {
        throw new UnsupportedOperationException();
    }

    public DataActivityState getDataActivityState() {
        throw new UnsupportedOperationException();
    }

    public Context getContext() {
        throw new UnsupportedOperationException();
    }

    public void disableDnsCheck(boolean b) {
        throw new UnsupportedOperationException();
    }

    public boolean isDnsCheckDisabled() {
        throw new UnsupportedOperationException();
    }

    public State getState() {
        throw new UnsupportedOperationException();
    }

    public String getPhoneName() {
        throw new UnsupportedOperationException();
    }

    public int getPhoneType() {
        throw new UnsupportedOperationException();
    }

    public String[] getActiveApnTypes() {
        throw new UnsupportedOperationException();
    }

    public String getActiveApn() {
        throw new UnsupportedOperationException();
    }

    public SignalStrength getSignalStrength() {
        throw new UnsupportedOperationException();
    }

    public void registerForUnknownConnection(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForUnknownConnection(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForPreciseCallStateChanged(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForPreciseCallStateChanged(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForNewRingingConnection(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForNewRingingConnection(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForIncomingRing(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForIncomingRing(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForDisconnect(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForDisconnect(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForMmiInitiate(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForMmiInitiate(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForMmiComplete(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForMmiComplete(Handler h) {
        throw new UnsupportedOperationException();
    }

    public List<? extends MmiCode> getPendingMmiCodes() {
        throw new UnsupportedOperationException();
    }

    public void sendUssdResponse(String ussdMessge) {
        throw new UnsupportedOperationException();
    }

    public void registerForServiceStateChanged(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForServiceStateChanged(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForSuppServiceNotification(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForSuppServiceNotification(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForSuppServiceFailed(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForSuppServiceFailed(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForInCallVoicePrivacyOn(Handler h, int what, Object obj){
        throw new UnsupportedOperationException();
    }

    public void unregisterForInCallVoicePrivacyOn(Handler h){
        throw new UnsupportedOperationException();
    }

    public void registerForInCallVoicePrivacyOff(Handler h, int what, Object obj){
        throw new UnsupportedOperationException();
    }

    public void unregisterForInCallVoicePrivacyOff(Handler h){
        throw new UnsupportedOperationException();
    }

    public void registerForCdmaOtaStatusChange(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForCdmaOtaStatusChange(Handler h) {
         throw new UnsupportedOperationException();
    }

    public void registerForSubscriptionInfoReady(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForSubscriptionInfoReady(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForEcmTimerReset(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForEcmTimerReset(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForRingbackTone(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForRingbackTone(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForResendIncallMute(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForResendIncallMute(Handler h) {
        throw new UnsupportedOperationException();
    }

    public boolean getIccRecordsLoaded() {
        throw new UnsupportedOperationException();
    }

    public IccCard getIccCard() {
        throw new UnsupportedOperationException();
    }

    public void acceptCall() throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public void rejectCall() throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public void switchHoldingAndActive() throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public boolean canConference() {
        throw new UnsupportedOperationException();
    }

    public void conference() throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public void enableEnhancedVoicePrivacy(boolean enable, Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void getEnhancedVoicePrivacy(Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public boolean canTransfer() {
        throw new UnsupportedOperationException();
    }

    public void explicitCallTransfer() throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public void clearDisconnected() {
        throw new UnsupportedOperationException();
    }

    public Call getForegroundCall() {
        throw new UnsupportedOperationException();
    }

    public Call getBackgroundCall() {
        throw new UnsupportedOperationException();
    }

    public Call getRingingCall() {
        throw new UnsupportedOperationException();
    }

    public Connection dial(String dialString) throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public Connection dial(String dialString, UUSInfo uusInfo) throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public boolean handlePinMmi(String dialString) {
        throw new UnsupportedOperationException();
    }

    public boolean handleInCallMmiCommands(String command) throws CallStateException {
        throw new UnsupportedOperationException();
    }

    public void sendDtmf(char c) {
        throw new UnsupportedOperationException();
    }

    public void startDtmf(char c) {
        throw new UnsupportedOperationException();
    }

    public void stopDtmf() {
        throw new UnsupportedOperationException();
    }

    public void setRadioPower(boolean power) {
        throw new UnsupportedOperationException();
    }

    public boolean getMessageWaitingIndicator() {
        throw new UnsupportedOperationException();
    }

    public boolean getCallForwardingIndicator() {
        throw new UnsupportedOperationException();
    }

    public String getLine1Number() {
        throw new UnsupportedOperationException();
    }

    public String getCdmaMin() {
        throw new UnsupportedOperationException();
    }

    public boolean isMinInfoReady() {
        throw new UnsupportedOperationException();
    }

    public String getCdmaPrlVersion() {
        throw new UnsupportedOperationException();
    }

    public String getLine1AlphaTag() {
        throw new UnsupportedOperationException();
    }

    public void setLine1Number(String alphaTag, String number, Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public String getVoiceMailNumber() {
        throw new UnsupportedOperationException();
    }

    public int getVoiceMessageCount(){
        throw new UnsupportedOperationException();
    }

    public String getVoiceMailAlphaTag() {
        throw new UnsupportedOperationException();
    }

    public void setVoiceMailNumber(String alphaTag,String voiceMailNumber, Message onComplete) {

    }

    public void getCallForwardingOption(int commandInterfaceCFReason, Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void setCallForwardingOption(int commandInterfaceCFReason,
                                        int commandInterfaceCFAction, String dialingNumber,
                                        int timerSeconds, Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void getOutgoingCallerIdDisplay(Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void setOutgoingCallerIdDisplay(int commandInterfaceCLIRMode, Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void getCallWaiting(Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void setCallWaiting(boolean enable, Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void getAvailableNetworks(Message response) {
        throw new UnsupportedOperationException();
    }

    public void setNetworkSelectionModeAutomatic(Message response) {
        throw new UnsupportedOperationException();
    }

    public void selectNetworkManually(NetworkInfo network, Message response) {
        throw new UnsupportedOperationException();
    }

    public void setPreferredNetworkType(int networkType, Message response) {
        throw new UnsupportedOperationException();
    }

    public void getPreferredNetworkType(Message response) {
        throw new UnsupportedOperationException();
    }

    public void getNeighboringCids(Message response) {
        throw new UnsupportedOperationException();
    }

    public void setOnPostDialCharacter(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void setMute(boolean muted) {
        throw new UnsupportedOperationException();
    }

    public boolean getMute() {
        throw new UnsupportedOperationException();
    }

    public void setEchoSuppressionEnabled(boolean enabled) {
        throw new UnsupportedOperationException();
    }

    public void invokeOemRilRequestRaw(byte[] data, Message response) {
        throw new UnsupportedOperationException();
    }

    public void invokeOemRilRequestStrings(String[] strings, Message response) {
        throw new UnsupportedOperationException();
    }

    public void setOnUnsolOemHookRaw(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unSetOnUnsolOemHookRaw(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void getDataCallList(Message response) {
        throw new UnsupportedOperationException();
    }

    public List<DataConnection> getCurrentDataConnectionList() {
        throw new UnsupportedOperationException();
    }

    public void updateServiceLocation() {
        throw new UnsupportedOperationException();
    }

    public void enableLocationUpdates() {
        throw new UnsupportedOperationException();
    }

    public void disableLocationUpdates() {
        throw new UnsupportedOperationException();
    }

    public void setUnitTestMode(boolean f) {
        throw new UnsupportedOperationException();
    }

    public boolean getUnitTestMode() {
        throw new UnsupportedOperationException();
    }

    public void setBandMode(int bandMode, Message response) {
        throw new UnsupportedOperationException();
    }

    public void queryAvailableBandMode(Message response) {
        throw new UnsupportedOperationException();
    }

    public boolean getDataRoamingEnabled() {
        throw new UnsupportedOperationException();
    }

    public void setDataRoamingEnabled(boolean enable) {
        throw new UnsupportedOperationException();
    }

    public void queryCdmaRoamingPreference(Message response) {
        throw new UnsupportedOperationException();
    }

    public void setCdmaRoamingPreference(int cdmaRoamingType, Message response) {
        throw new UnsupportedOperationException();
    }

    public void setCdmaSubscription(int cdmaSubscriptionType, Message response) {
        throw new UnsupportedOperationException();
    }

    public SimulatedRadioControl getSimulatedRadioControl() {
        throw new UnsupportedOperationException();
    }

    public boolean enableDataConnectivity() {
        throw new UnsupportedOperationException();
    }

    public boolean disableDataConnectivity() {
        throw new UnsupportedOperationException();
    }

    public int enableApnType(String type) {
        throw new UnsupportedOperationException();
    }

    public int disableApnType(String type) {
        throw new UnsupportedOperationException();
    }

    public boolean isDataConnectivityEnabled() {
        throw new UnsupportedOperationException();
    }

    public boolean isDataConnectivityPossible() {
        throw new UnsupportedOperationException();
    }

    public String getInterfaceName(String apnType) {
        throw new UnsupportedOperationException();
    }

    public String getIpAddress(String apnType) {
        throw new UnsupportedOperationException();
    }

    public String getGateway(String apnType) {
        throw new UnsupportedOperationException();
    }

    public String[] getDnsServers(String apnType) {
        throw new UnsupportedOperationException();
    }

    public String getDeviceId() {
        throw new UnsupportedOperationException();
    }

    public String getDeviceSvn() {
        throw new UnsupportedOperationException();
    }

    public String getSubscriberId() {
        throw new UnsupportedOperationException();
    }

    public String getIccSerialNumber() {
        throw new UnsupportedOperationException();
    }

    public String getEsn() {
        throw new UnsupportedOperationException();
    }

    public String getMeid() {
        throw new UnsupportedOperationException();
    }

    public PhoneSubInfo getPhoneSubInfo(){
        throw new UnsupportedOperationException();
    }

    public IccSmsInterfaceManager getIccSmsInterfaceManager(){
        throw new UnsupportedOperationException();
    }

    public IccPhoneBookInterfaceManager getIccPhoneBookInterfaceManager(){
        throw new UnsupportedOperationException();
    }

    public void setTTYMode(int ttyMode, Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void queryTTYMode(Message onComplete) {
        throw new UnsupportedOperationException();
    }

    public void activateCellBroadcastSms(int activate, Message response) {
        throw new UnsupportedOperationException();
    }

    public void getCellBroadcastSmsConfig(Message response) {
        throw new UnsupportedOperationException();
    }

    public void setCellBroadcastSmsConfig(int[] configValuesArray, Message response) {
        throw new UnsupportedOperationException();
    }

    public void notifyDataActivity() {
         throw new UnsupportedOperationException();
    }

    public void getSmscAddress(Message result) {
        throw new UnsupportedOperationException();
    }

    public void setSmscAddress(String address, Message result) {
        throw new UnsupportedOperationException();
    }

    public int getCdmaEriIconIndex() {
         throw new UnsupportedOperationException();
    }

     public String getCdmaEriText() {
         throw new UnsupportedOperationException();
     }

    public int getCdmaEriIconMode() {
         throw new UnsupportedOperationException();
    }

    public void sendBurstDtmf(String dtmfString, int on, int off, Message onComplete){
        throw new UnsupportedOperationException();
    }

    public void exitEmergencyCallbackMode(){
        throw new UnsupportedOperationException();
    }

    public boolean isOtaSpNumber(String dialStr){
        throw new UnsupportedOperationException();
    }

    public void registerForCallWaiting(Handler h, int what, Object obj){
        throw new UnsupportedOperationException();
    }

    public void unregisterForCallWaiting(Handler h){
        throw new UnsupportedOperationException();
    }

    public void registerForSignalInfo(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForSignalInfo(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForDisplayInfo(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForDisplayInfo(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForNumberInfo(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForNumberInfo(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForRedirectedNumberInfo(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForRedirectedNumberInfo(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForLineControlInfo(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForLineControlInfo(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerFoT53ClirlInfo(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForT53ClirInfo(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void registerForT53AudioControlInfo(Handler h, int what, Object obj) {
        throw new UnsupportedOperationException();
    }

    public void unregisterForT53AudioControlInfo(Handler h) {
        throw new UnsupportedOperationException();
    }

    public void setOnEcbModeExitResponse(Handler h, int what, Object obj){
        throw new UnsupportedOperationException();
    }

    public void unsetOnEcbModeExitResponse(Handler h){
        throw new UnsupportedOperationException();
    }
}
