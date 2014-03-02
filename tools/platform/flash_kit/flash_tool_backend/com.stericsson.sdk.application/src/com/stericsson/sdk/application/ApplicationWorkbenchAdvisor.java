package com.stericsson.sdk.application;

import java.net.URL;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.TrayDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPreferenceConstants;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.application.IWorkbenchConfigurer;
import org.eclipse.ui.application.IWorkbenchWindowConfigurer;
import org.eclipse.ui.application.WorkbenchAdvisor;
import org.eclipse.ui.application.WorkbenchWindowAdvisor;
import org.eclipse.ui.ide.IDE;

import org.osgi.framework.Bundle;

/**
 * This workbench advisor creates the window advisor, and specifies the perspective id for the
 * initial window.
 * 
 * @author pfurma01
 * 
 */

public class ApplicationWorkbenchAdvisor extends WorkbenchAdvisor {

    private static final String PERSPECTIVE_ID = "com.stericsson.sdk.equipment.ui.perspective";

    private static final String ICONS_PATH = "icons/full/";

    private static final String PATH_OBJECT = ICONS_PATH + "obj16/";

    private static final String IDE_WORKBENCH = "org.eclipse.ui.ide";

    /**
     * Will create workbench window advisor
     * 
     * @param configurer
     *            workbench configuration
     * @return new workbench window advisor
     */
    public WorkbenchWindowAdvisor createWorkbenchWindowAdvisor(IWorkbenchWindowConfigurer configurer) {
        return new ApplicationWorkbenchWindowAdvisor(configurer);
    }

    /**
     * Will return prespetive id
     * 
     * @return prespetive id
     */
    public String getInitialWindowPerspectiveId() {
        return PERSPECTIVE_ID;
    }

    /**
     * Used for the navigator.
     * 
     * @param configurer
     *            workbench configuration
     */
    public void initialize(IWorkbenchConfigurer configurer) {
        configurer.setSaveAndRestore(true);
        // This will set the tabs as they are in Eclipse and not rectangular.
        PlatformUI.getPreferenceStore().setValue(IWorkbenchPreferenceConstants.SHOW_TRADITIONAL_STYLE_TABS, false);
        // Show a progress bar on startup.
        PlatformUI.getPreferenceStore().setValue(IWorkbenchPreferenceConstants.SHOW_PROGRESS_ON_STARTUP, true);
        TrayDialog.setDialogHelpAvailable(true);
        super.initialize(configurer);
        IDE.registerAdapters();
        declareWorkbenchImages(configurer);
    }

    /**
     * Used for the navigator.
     * 
     * @return workbanch root
     */
    public IAdaptable getDefaultPageInput() {
        return ResourcesPlugin.getWorkspace().getRoot();
    }

    /**
     * Declares all IDE-specific workbench images. This includes both "shared" images (named in
     * {@link IDE.SharedImages}) and internal images (named in
     * 
     * 
     * @param configurer
     *            workbench configuration
     * @see IWorkbenchConfigurer#declareImage
     */
    private void declareWorkbenchImages(IWorkbenchConfigurer configurer) {

        Bundle ideBundle = Platform.getBundle(IDE_WORKBENCH);
        declareWorkbenchImage(configurer, ideBundle, IDE.SharedImages.IMG_OBJ_PROJECT, PATH_OBJECT + "prj_obj.gif",
            true);
        declareWorkbenchImage(configurer, ideBundle, IDE.SharedImages.IMG_OBJ_PROJECT_CLOSED, PATH_OBJECT
            + "cprj_obj.gif", true);
    }

    /**
     * Declares an IDE-specific workbench image.
     * 
     * @param symbolicName
     *            the symbolic name of the image
     * @param path
     *            the path of the image file; this path is relative to the base of the IDE plug-in
     * @param shared
     *            true if this is a shared image, and false if this is not a shared image
     * @param configurer
     *            workbench configuration
     * @see IWorkbenchConfigurer#declareImage
     */
    private void declareWorkbenchImage(IWorkbenchConfigurer configurer, Bundle ideBundle, String symbolicName,
        String path, boolean shared) {
        URL url = ideBundle.getEntry(path);
        ImageDescriptor desc = ImageDescriptor.createFromURL(url);
        configurer.declareImage(symbolicName, desc, shared);
    }

}
