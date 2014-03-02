package com.stericsson.sdk.equipment.ui.preferences.backend;

import java.io.FileNotFoundException;
import java.io.IOException;

import org.apache.log4j.Logger;
import org.eclipse.jface.preference.DirectoryFieldEditor;
import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.FileFieldEditor;
import org.eclipse.jface.preference.IntegerFieldEditor;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.PlatformUI;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Flash Kit Preference Page
 * 
 * @author xolabju
 * 
 */
public class BackendPreferencePage extends FieldEditorPreferencePage implements IWorkbenchPreferencePage {

    private static final String DEVELOP_WARNING =
        "Preferences are in development mode.\n" + "For more information regarding setting up preferences"
            + " in this mode, please, consult help.";

    private FileFieldEditor fieldConfigFile;

    private boolean isSomethinkDisabled = false;

    private boolean isDirty;

    private Logger logger = Logger.getLogger(BackendPreferencePage.class.getName());

    /**
     * Constructor
     */
    public BackendPreferencePage() {
        super(GRID);
        setPreferenceStore(Activator.getDefault().getPreferenceStore());
        setDescription("Flash Tool Backend Preferences");
    }

    /**
     * Creates the field editors. Field editors are abstractions of the common GUI blocks needed to
     * manipulate various types of preferences. Each field editor knows how to save and restore
     * itself.
     */
    public void createFieldEditors() {
        fieldConfigFile =
            new FileFieldEditor(FlashKitPreferenceConstants.BACKEND_CONFIGURATION_FILE, "&Configuration File:",
                getFieldEditorParent());
        DirectoryFieldEditor fieldProfilePath =
            new DirectoryFieldEditor(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH, "&Profiles Path:",
                getFieldEditorParent());
        DirectoryFieldEditor fieldLoaderPath =
            new DirectoryFieldEditor(FlashKitPreferenceConstants.BACKEND_LOADER_PATH, "&Loader Path:",
                getFieldEditorParent());
        FileFieldEditor fieldLCD =
            new FileFieldEditor(FlashKitPreferenceConstants.BACKEND_LCD_FILE, "LCD Library:", getFieldEditorParent());
        FileFieldEditor fieldLCM =
            new FileFieldEditor(FlashKitPreferenceConstants.BACKEND_LCM_FILE, "LCM Library:", getFieldEditorParent());

        IntegerFieldEditor fieldPort =
            new IntegerFieldEditor(FlashKitPreferenceConstants.BACKEND_BRP_PORT, "Remote Connection Port:",
                getFieldEditorParent());

        checkFieldState(fieldConfigFile, SystemProperties.BACKEND_CONFIGURATIONS_ROOT);
        checkFieldState(fieldProfilePath, SystemProperties.BACKEND_PROFILES_ROOT);
        checkFieldState(fieldLoaderPath, SystemProperties.BACKEND_LOADER_ROOT);
        checkFieldState(fieldLCD, SystemProperties.BACKEND_LCD_FILE);
        checkFieldState(fieldLCM, SystemProperties.BACKEND_LCM_FILE);
        checkFieldState(fieldPort, SystemProperties.BACKEND_BRP_PORT);

        addField(fieldConfigFile);
        addField(fieldProfilePath);
        addField(fieldLoaderPath);
        addField(fieldLCD);
        addField(fieldLCM);
        addField(fieldPort);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected Control createContents(Composite parent) {
        Control result = super.createContents(parent);
        if (isSomethinkDisabled) {
            Label test = new Label(parent, SWT.NONE|SWT.WRAP);
            test.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, true));
            test.setText(DEVELOP_WARNING);
        }
        return result;
    }

    private void checkFieldState(FieldEditor ui, String property) {
        boolean result = false;
        result = SystemProperties.isSystemDefined(property);
        if (result) {
            ui.setEnabled(false, getFieldEditorParent());
        }
        isSomethinkDisabled |= result;
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench) {
        isDirty = false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performOk() {
        boolean perform = super.performOk();
        if (isDirty) {
            try {
                // it should be handled by error dialogue
                getBackendService().loadConfiguration();
            } catch (NullPointerException e) {
                logger.warn(e.getMessage());
            } catch (FileNotFoundException e) {
                logger.error(e.getMessage());
            } catch (IOException e) {
                logger.fatal("Failed to read configuration file", e);
            }

        }
        return perform;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void propertyChange(PropertyChangeEvent event) {
        super.propertyChange(event);
        isDirty = true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void performDefaults() {
        super.performDefaults();
        isDirty = true;
    }

    /**
     * @return Backend service
     */
    protected IBackendService getBackendService() {
        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        if ((references == null) || (references.length == 0)) {
            return null;
        }

        return (IBackendService) Activator.getBundleContext().getService(references[0]);
    }

    /*
     * private void updateBackendService() { // get backend service IBackendService backendService =
     * null; IConfigurationService configService = null; ServiceReference[] references = null;
     * BundleContext context = Activator.getBundleContext(); try { references =
     * context.getAllServiceReferences(IBackendService.class.getName(), null); } catch
     * (InvalidSyntaxException e) { e.printStackTrace(); }
     * 
     * if ((references == null) || (references.length == 0)) { return; } backendService =
     * (IBackendService) context.getService(references[0]);
     * 
     * // get configuration service String filter = ("(&(type=backend)(format=ui.preferencepage))");
     * try { references = context.getAllServiceReferences(IConfigurationService.class.getName(),
     * filter); } catch (InvalidSyntaxException e) {
     * logger.error("Can not fetch configuration service", e); }
     * 
     * if ((references == null) || (references.length == 0)) { return; } configService =
     * (IConfigurationService) context.getService(references[0]); try {
     * backendService.setConfigurationService(configService); } catch (IOException e) {
     * e.printStackTrace(); } }
     */
    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/gettingstarted/overview_preferences.html");
    }
}
