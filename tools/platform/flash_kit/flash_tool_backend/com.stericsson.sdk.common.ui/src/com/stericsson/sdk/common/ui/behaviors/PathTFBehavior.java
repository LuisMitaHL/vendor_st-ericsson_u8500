package com.stericsson.sdk.common.ui.behaviors;

import java.util.Locale;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;

import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;

/**
 * This behavior should be used on text fields containing paths. Paths will be replaced by relative
 * paths when it's possible. For instance path to some profile will look like @profiles\profile.prfl
 * instead of c:\profile.prfl.
 * 
 * @author xmicroh
 * 
 */
public class PathTFBehavior implements IBehavior, ModifyListener {

    private Text tf;

    /**
     * @param pTf
     *            Text field containing path.
     */
    public PathTFBehavior(Text pTf) {
        tf = pTf;
    }

    /**
     * {@inheritDoc}
     */
    public void switchOff() {
        tf.removeModifyListener(this);
    }

    /**
     * {@inheritDoc}
     */
    public void switchOn() {
        tf.addModifyListener(this);
    }

    /**
     * {@inheritDoc}
     */
    public void modifyText(ModifyEvent pE) {
        Listener[] modifyListeners = tf.getListeners(SWT.Modify);
        for (Listener l : modifyListeners) {
            tf.removeListener(SWT.Modify, l);
        }
        tf.setText(getRelativePath(tf.getText()));
        for (Listener l : modifyListeners) {
            tf.addListener(SWT.Modify, l);
        }
    }

    /**
     * Should return relative path from the given one if such path could be found i.e. c:\profiles
     * -> @profiles Path resolving should be probably based on system properties.
     * 
     * @param path
     *            absolute path
     * @return relative path
     */
    public static String getRelativePath(String path) {
        String profilesRoot = SystemProperties.getProperty(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH);
        String loadersRoot = SystemProperties.getProperty(FlashKitPreferenceConstants.BACKEND_LOADER_PATH);
        String configurationsRoot = SystemProperties.getProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT);

        if (System.getProperty("os.name").matches("^Win.*$")) {
            path = path.toLowerCase(Locale.getDefault());
            profilesRoot = profilesRoot.toLowerCase(Locale.getDefault());
            loadersRoot = loadersRoot.toLowerCase(Locale.getDefault());
            configurationsRoot = configurationsRoot.toLowerCase(Locale.getDefault());
        }

        if (profilesRoot.contentEquals("")) {
            profilesRoot = null;
        }
        if (loadersRoot.contentEquals("")) {
            loadersRoot = null;
        }
        if (configurationsRoot.contentEquals("")) {
            configurationsRoot = null;
        }

        if (profilesRoot != null && path.startsWith(profilesRoot)) {
            path = path.replace(profilesRoot, FlashKitPreferenceConstants.BACKEND_PROFILE_PATH_SHORTCUT);
        } else if (loadersRoot != null && path.startsWith(loadersRoot)) {
            path = path.replace(loadersRoot, FlashKitPreferenceConstants.BACKEND_LOADER_PATH_SHORTCUT);
        } else if (configurationsRoot != null && path.startsWith(configurationsRoot)) {
            path = path.replace(configurationsRoot, FlashKitPreferenceConstants.BACKEND_CONFIGURATION_FILE_SHORTCUT);
        }

        return path;
    }

    /**
     * Should return absolute path from given one i.e. @profiles -> c:\profiles Path resolving
     * should be probably based on system properties.
     * 
     * @param path
     *            relative path
     * @return absolute path
     */
    public static String getAbsolutePath(String path) {
        String profilesRoot = SystemProperties.getProperty(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH);
        String loadersRoot = SystemProperties.getProperty(FlashKitPreferenceConstants.BACKEND_LOADER_PATH);
        String configurationsRoot = SystemProperties.getProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT);
        if (profilesRoot == null) {
            profilesRoot = "";
        }
        if (loadersRoot == null) {
            loadersRoot = "";
        }
        if (configurationsRoot == null) {
            configurationsRoot = "";
        }

        if (path.startsWith(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH_SHORTCUT)) {
            path = path.replace(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH_SHORTCUT, profilesRoot);
        } else if (path.startsWith(FlashKitPreferenceConstants.BACKEND_LOADER_PATH_SHORTCUT)) {
            path = path.replace(FlashKitPreferenceConstants.BACKEND_LOADER_PATH_SHORTCUT, loadersRoot);
        } else if (path.startsWith(FlashKitPreferenceConstants.BACKEND_CONFIGURATION_FILE_SHORTCUT)) {
            path = path.replace(FlashKitPreferenceConstants.BACKEND_CONFIGURATION_FILE_SHORTCUT, configurationsRoot);
        }

        return path;
    }

}
