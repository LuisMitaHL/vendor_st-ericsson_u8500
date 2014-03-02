package com.stericsson.sdk.equipment.internal;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.equipment.EquipmentException;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

/**
 * @author xtomlju
 */
public class EquipmentProfileService implements IEquipmentProfileManagerService, IConfigurationServiceListener,
    Runnable {

    /** Profile map */
    private HashMap<String, IEquipmentProfile> profiles;

    /** Queue of profiles to load */
    private LinkedBlockingQueue<IConfigurationRecord> profilesToLoad;

    /** Running flag */
    private boolean running;

    /** Counter of profiles to load */
    private int profileCounter = 0;

    private boolean isLoading = true;

    /** Logger */
    private static Logger logger = Logger.getLogger(EquipmentProfileService.class);

    /**
     * Constructor.
     */
    public EquipmentProfileService() {
        profiles = new HashMap<String, IEquipmentProfile>();
        profilesToLoad = new LinkedBlockingQueue<IConfigurationRecord>();
        running = true;
        new Thread(this).start();
    }

    /**
     * {@inheritDoc}
     */
    public IEquipmentProfile getProfile(String alias, boolean wait) {
        if (!wait) {
            return profiles.get(alias);
        }

        try {
            while (profiles.get(alias) == null && ((hasProfileToLoad(alias)) || isLoading)) {
                Thread.sleep(500);
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return profiles.get(alias);
    }

    /**
     * Check if profile is in list of profiles to load.
     * 
     * @param alias
     *            Profile alias
     * @return True if profile alias is in list of profiles to load
     */
    private boolean hasProfileToLoad(String alias) {
        // Assume that profile to load is not in queue
        boolean found = false;
        for (IConfigurationRecord profileRecord : profilesToLoad) {
            if (profileRecord.getValue(ConfigurationOption.PROFILE_ALIAS).equalsIgnoreCase(alias)) {
                found = true;
                break;
            }
        }
        return found;
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {

        if (ConfigurationOption.PROFILE.equalsIgnoreCase(newRecord.getName())) {
            try {
                profilesToLoad.put(newRecord);
                profileCounter++;
            } catch (InterruptedException e) {
                logger.error("Failed to add '" + newRecord.getValue(ConfigurationOption.PROFILE_PATH)
                    + "' to queue of profiles to load", e);
            }
            logger.debug("Added '" + newRecord.getValue(ConfigurationOption.PROFILE_PATH)
                + "' to queue of profiles to load");
        }
    }

    /**
     * {@inheritDoc}
     */
    public void run() {
        while (running) {
            try {
                IConfigurationRecord record = profilesToLoad.poll(500, TimeUnit.MILLISECONDS);
                if (record != null) {
                    isLoading = true;
                    String profilesRoot = SystemProperties.getProperty(SystemProperties.BACKEND_PROFILES_ROOT);
                    if (profilesRoot == null) {
                        profilesRoot = "";
                    }
                    String path = record.getValue(ConfigurationOption.PROFILE_PATH);
                    if (path.startsWith("@profiles")) {
                        path = path.replace("@profiles", profilesRoot);
                    }

                    EquipmentProfile profile = new EquipmentProfile();

                    if (!new File(path).exists()) {
                        logger.info("Cannot load profile from '" + path + "'");
                        continue;
                    }
                    profile.read(new File(path));

                    logger.info("Loaded profile: '" + profile.getAlias() + "' from '" + path + "'");
                    profiles.put(profile.getAlias(), profile);
                } else {
                    isLoading = false;
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public int getPercentageOfLoadedProfiles() {
        int loadedProfiles = profileCounter - profilesToLoad.size();
        float perLoadedProfiles = (float) loadedProfiles / (float) profileCounter;
        float toReturn = perLoadedProfiles * 100;
        return (int) toReturn;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isLoading() {
        return (!profilesToLoad.isEmpty() || isLoading);
    }

    /**
     * {@inheritDoc}
     * 
     * @throws EquipmentException
     */
    public String[] getAllAvailableProfilesNames() throws EquipmentException {

        if (!profilesToLoad.isEmpty()) {
            throw new EquipmentException("Still loading profiles. Remaining profiles to load: " + profilesToLoad.size());
        }
        Vector<String> availableVector = new Vector<String>();

        if (profiles == null || profiles.size() == 0) {
            return null;
        }

        for (Iterator<String> it = profiles.keySet().iterator(); it.hasNext();) {
            String current = (String) it.next();
            availableVector.add(current);
        }
        return availableVector.toArray(new String[availableVector.size()]);
    }
}
