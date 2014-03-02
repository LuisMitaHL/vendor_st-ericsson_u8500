package com.stericsson.sdk.backend.config;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

import com.stericsson.sdk.backend.Activator;


/**
 * @author Xxvs0002
 *
 */
public final class BackendConfiguration {

    private static BackendConfiguration backendConfigurationInstance;

    /***/
    public static final String TIME_TO_WAIT_BOOT_TASK_FAIL = "time_to_wait_boot_task_to_fail";
    /***/
    public static final int DEFAULT_TIME_TO_WAIT_BOOT_TASK_FAIL = 100;

    private final String configurationProperties = "configuration.properties";
    private Properties properties = new Properties();

    /**
     * @return instance of the class
     */
    public static BackendConfiguration getInstance() {
        if(backendConfigurationInstance == null) {
            backendConfigurationInstance = new BackendConfiguration();
        }
        return backendConfigurationInstance;
     }

    /**
     * Constructor
     */
    private BackendConfiguration(){
        if(properties.size() == 0){
            File propertiesFile = new File(Activator.getResourcesPath(), configurationProperties);
            try {
                properties.load(new FileInputStream(propertiesFile));
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * @param key key
     * @return the value for the specified key
     */
    public String getProperty(String key){
        return properties.getProperty(key);
    }
}
