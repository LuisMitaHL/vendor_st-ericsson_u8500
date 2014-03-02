/**
 * 
 */
package com.stericsson.sdk.equipment.ui.structures;

/**
 * Pattern class used for transporting the control keys
 * 
 * @author mbodan01
 */
public class ControlKeyCrate {
    private String networkLockKey;

    private String networkSubsetLockKey;

    private String serviceProviderLockKey;

    private String corporateLockKey;

    private String flexibleESLLockKey;

    /**
     * General constructor
     */
    public ControlKeyCrate() {
        networkLockKey = "";
        networkSubsetLockKey = "";
        serviceProviderLockKey = "";
        corporateLockKey = "";
        flexibleESLLockKey = "";
    }

    /**
     * Concrete constructor
     * 
     * @param pNetworkLockKey
     *            ASCII 16 bytes digit
     * @param pNetworkSubsetLockKey
     *            ASCII 16 bytes digit
     * @param pServiceProviderLockKey
     *            ASCII 16 bytes digit
     * @param pCorporateLockKey
     *            ASCII 16 bytes digit
     * @param pFlexibleESLLockKey
     *            ASCII 16 bytes digit
     */
    public ControlKeyCrate(String pNetworkLockKey, String pNetworkSubsetLockKey, String pServiceProviderLockKey,
        String pCorporateLockKey, String pFlexibleESLLockKey) {
        networkLockKey = pNetworkLockKey;
        networkSubsetLockKey = pNetworkSubsetLockKey;
        serviceProviderLockKey = pServiceProviderLockKey;
        corporateLockKey = pCorporateLockKey;
        flexibleESLLockKey = pFlexibleESLLockKey;
    }

    /**
     * @return the networkLockKey
     */
    public String getNetworkLockKey() {
        return networkLockKey;
    }

    /**
     * @param pNetworkLockKey
     *            the networkLockKey to set
     */
    public void setNetworkLockKey(String pNetworkLockKey) {
        this.networkLockKey = pNetworkLockKey;
    }

    /**
     * @return the networkSubsetLockKey
     */
    public String getNetworkSubsetLockKey() {
        return networkSubsetLockKey;
    }

    /**
     * @param pNetworkSubsetLockKey
     *            the networkSubsetLockKey to set
     */
    public void setNetworkSubsetLockKey(String pNetworkSubsetLockKey) {
        this.networkSubsetLockKey = pNetworkSubsetLockKey;
    }

    /**
     * @return the serviceProviderLockKey
     */
    public String getServiceProviderLockKey() {
        return serviceProviderLockKey;
    }

    /**
     * @param pServiceProviderLockKey
     *            the serviceProviderLockKey to set
     */
    public void setServiceProviderLockKey(String pServiceProviderLockKey) {
        this.serviceProviderLockKey = pServiceProviderLockKey;
    }

    /**
     * @return the corporateLockKey
     */
    public String getCorporateLockKey() {
        return corporateLockKey;
    }

    /**
     * @param pCorporateLockKey
     *            the corporateLockKey to set
     */
    public void setCorporateLockKey(String pCorporateLockKey) {
        this.corporateLockKey = pCorporateLockKey;
    }

    /**
     * @return the flexibleESLLockKey
     */
    public String getFlexibleESLLockKey() {
        return flexibleESLLockKey;
    }

    /**
     * @param pFlexibleESLLockKey
     *            the flexibleESLLockKey to set
     */
    public void setFlexibleESLLockKey(String pFlexibleESLLockKey) {
        this.flexibleESLLockKey = pFlexibleESLLockKey;
    }

    /**
     * @return string witch represent this object data
     */
    @Override
    public String toString() {
        return "ControlKeyCrate [Corporate Lock Key=" + corporateLockKey + ", Flexible ESL Lock Key="
            + flexibleESLLockKey + ", Network Lock Key=" + networkLockKey + ", Network Subset Lock Key="
            + networkSubsetLockKey + ", Service Provider Lock Key=" + serviceProviderLockKey + "]";
    }
}
