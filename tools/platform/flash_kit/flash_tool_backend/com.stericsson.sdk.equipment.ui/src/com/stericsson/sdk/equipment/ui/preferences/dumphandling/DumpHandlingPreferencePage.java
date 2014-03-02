package com.stericsson.sdk.equipment.ui.preferences.dumphandling;

import java.io.File;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.DirectoryFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.PlatformUI;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Dump Handling Preference Page
 * 
 * @author esrimpa
 * 
 */
public class DumpHandlingPreferencePage extends FieldEditorPreferencePage implements IWorkbenchPreferencePage {

    static final Logger log = Logger.getLogger(DumpHandlingPreferencePage.class);

    private static final String DEFAULT_DOWNLOAD_DIRECTORY = "AutoDownloadedDumps";

    private BooleanFieldEditor fieldEnableAutoDownload;

    private BooleanFieldEditor fieldEnableAutoDelete;

    private DirectoryFieldEditor fieldDownloadPath;

    private boolean isDirty;

    /**
     * Constructor
     */
    public DumpHandlingPreferencePage() {
        super(GRID);
        setPreferenceStore(Activator.getDefault().getPreferenceStore());
        setDescription("Core Dump Preferences");
    }

    /**
     * Creates the field editors. Field editors are abstractions of the common GUI blocks needed to
     * manipulate various types of preferences. Each field editor knows how to save and restore
     * itself.
     */
    public void createFieldEditors() {
        fieldEnableAutoDownload =
            new BooleanFieldEditor(SystemProperties.BACKEND_AUTO_DOWNLOAD_ENABLED,
                "Enable &Automatic Download of Dump", getFieldEditorParent());
        addField(fieldEnableAutoDownload);

        fieldDownloadPath =
            new DirectoryFieldEditor(SystemProperties.BACKEND_DOWNLOAD_PATH, "&Download Path:", getFieldEditorParent()) {

                // Overriding these methods to remove check for the directory path when AutoDownload
                // is unchecked.
                @Override
                protected boolean doCheckState() {
                    if (!fieldEnableAutoDownload.getBooleanValue()) {
                        return true;
                    } else {
                        return super.doCheckState();
                    }
                }

                // Overriding these methods to remove check for the directory path when AutoDownload
                // is unchecked.
                @Override
                public boolean isValid() {
                    if (!fieldEnableAutoDownload.getBooleanValue()) {
                        refreshValidState();
                        setValid(true);
                        return true;
                    } else {
                        refreshValidState();
                        boolean valid = doCheckState();
                        setValid(valid);
                        return valid;
                    }
                }
            };

        addField(fieldDownloadPath);

        fieldEnableAutoDelete =
            new BooleanFieldEditor(SystemProperties.BACKEND_AUTO_DELETE_ENABLED,
                "&Enable Automatic Delete of Dump After Download", getFieldEditorParent());
        addField(fieldEnableAutoDelete);

        IPreferenceStore store = getPreferenceStore();
        boolean useAutoDownloadEnabled = store.getBoolean(SystemProperties.BACKEND_AUTO_DOWNLOAD_ENABLED);
        setAutoDownloadEnabled(useAutoDownloadEnabled);

    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench) {
        isDirty = false;

        IPreferenceStore store = getPreferenceStore();
        updateBackendService(store.getBoolean(SystemProperties.BACKEND_AUTO_DOWNLOAD_ENABLED), store
            .getBoolean(SystemProperties.BACKEND_AUTO_DELETE_ENABLED), store
            .getString(SystemProperties.BACKEND_DOWNLOAD_PATH));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performOk() {

        if (isDirty) {
            // Activator.setSystemVariables();
            boolean isAutoDelete = false;
            String downloadPath = null;
            boolean isAutoDownload = fieldEnableAutoDownload.getBooleanValue();

            if (isAutoDownload) {
                isAutoDelete = fieldEnableAutoDelete.getBooleanValue();
                downloadPath = fieldDownloadPath.getStringValue();
            }

            if (isAutoDownload && ((downloadPath == null) || (downloadPath.length() == 0))) {
                String message =
                    "'Download Path' is empty. This will set the download path to default path "
                        + new Path(System.getProperty("user.home") + "/" + DEFAULT_DOWNLOAD_DIRECTORY).toOSString();
                boolean response =
                    MessageDialog.openConfirm(getFieldEditorParent().getShell(), "Confirm default path", message);

                // Return
                if (!response) {
                    return false;
                }
                downloadPath =
                    new Path(System.getProperty("user.home") + "/" + DEFAULT_DOWNLOAD_DIRECTORY).toOSString();
                fieldDownloadPath.setStringValue(downloadPath);

                // Check if download directory exists. If not, create it.
                File directory = new File(downloadPath);
                if (!directory.exists()) {
                    directory.mkdirs();
                }

            }
            updateBackendService(fieldEnableAutoDownload.getBooleanValue(), isAutoDelete, downloadPath);
        }

        // We need to do this check, when some property change, to refresh the valid state
        fieldDownloadPath.isValid();

        boolean perform = super.performOk();
        return perform;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void propertyChange(PropertyChangeEvent event) {
        super.propertyChange(event);
        isDirty = true;
        setAutoDownloadEnabled(fieldEnableAutoDownload.getBooleanValue());

        // We need to do this check, when some property change, to refresh the valid state
        fieldDownloadPath.isValid();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void performDefaults() {
        super.performDefaults();
        isDirty = true;
        setAutoDownloadEnabled(fieldEnableAutoDownload.getBooleanValue());
    }

    private void setAutoDownloadEnabled(boolean enabled) {
        Composite parent = getFieldEditorParent();
        fieldEnableAutoDelete.setEnabled(enabled, parent);
        fieldDownloadPath.setEnabled(enabled, parent);

    }

    private void updateBackendService(boolean useAutoDownload, boolean useAutoDelete, String directoryPath) {
        // get backend service
        IBackendService backendService = null;
        backendService = getBackendService();
        if (backendService == null) {
            return;
        }

        try {

            backendService.setCoreDumpLocation(directoryPath);
            backendService.setAutoDownload(useAutoDownload);
            backendService.setAutoDelete(useAutoDelete);

            IPreferenceStore store = getPreferenceStore();
            store.setValue(SystemProperties.BACKEND_AUTO_DELETE_ENABLED, useAutoDelete);
            store.setValue(SystemProperties.BACKEND_AUTO_DOWNLOAD_ENABLED, useAutoDownload);
            if (directoryPath != null) {
                store.setValue(SystemProperties.BACKEND_DOWNLOAD_PATH, directoryPath);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private IBackendService getBackendService() {
        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            log.error(e.getMessage());
        }

        if ((references == null) || (references.length == 0)) {
            return null;
        }

        return (IBackendService) Activator.getBundleContext().getService(references[0]);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/gettingstarted/overview_preferences.html");
    }
}
