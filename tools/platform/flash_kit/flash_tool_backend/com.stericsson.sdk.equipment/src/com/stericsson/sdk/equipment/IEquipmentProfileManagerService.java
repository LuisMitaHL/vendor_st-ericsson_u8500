package com.stericsson.sdk.equipment;

/**
 * @author xtomlju
 */
public interface IEquipmentProfileManagerService {

    /**
     * Get equipment profile for specified profile alias. If wait flag is true the method will wait
     * until the specified profile has been loaded. This only applies if the profile alias is in the
     * list of profiles to be loaded.
     * 
     * @param alias
     *            Equipment profile alias
     * @param wait
     *            If method should block until profile gets loaded
     * @return Equipment profile interface or null if no profile with specified alias exists.
     */
    IEquipmentProfile getProfile(String alias, boolean wait);

    /**
     * @return array of all available profile names
     * @throws EquipmentException if profiles are not loaded yet
     */
    String[] getAllAvailableProfilesNames() throws EquipmentException;

    /**
     * @return percentage of loaded profiles
     */
    int getPercentageOfLoadedProfiles();

    /**
     * @return true if service is loading profiles
     */
    boolean isLoading();
}
