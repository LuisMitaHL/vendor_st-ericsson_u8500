package com.stericsson.sdk.signing.ui.preferences;

import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.DirectoryFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.StringFieldEditor;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.signing.ui.Activator;
import com.stericsson.sdk.signing.ui.utils.SignedSoftwareUtils;

/**
 * Signing preference page. TODO set data for GUI dancer if possible.
 * 
 * @author MielcLuk
 * 
 */
public class SigningPreferencePage extends FieldEditorPreferencePage implements IWorkbenchPreferencePage {

    private BooleanFieldEditor fieldUseLocalSigning;

    private BooleanFieldEditor fieldUseDefaultSignPackage;

    private boolean isDirty = false;

    private StringFieldEditor fieldSignServer;

    private StringFieldEditor fieldDefaultSignPackage;

    private DirectoryFieldEditor fieldLocalSignPackageRoot;

    private DirectoryFieldEditor fieldSigningLocalKeyRoot;

    private DirectoryFieldEditor fieldSignPackageRoot;

    private BooleanFieldEditor fieldUseCommonFolder;

    private DirectoryFieldEditor fieldSignLocalKeysPackageRoot;

    /**
     * Constructor
     */
    public SigningPreferencePage() {
        super(GRID);
        setPreferenceStore(Activator.getDefault().getPreferenceStore());
        setDescription("Flash Tool Signing Preferences");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void propertyChange(PropertyChangeEvent event) {
        super.propertyChange(event);
        isDirty = true;
        setLocalSigningEnabled(fieldUseLocalSigning.getBooleanValue(), fieldUseCommonFolder.getBooleanValue());
        setDefaultSignPackageEnabled(fieldUseDefaultSignPackage.getBooleanValue());
        setCommonFolderSigningEnabled(fieldUseCommonFolder.getBooleanValue(), fieldUseLocalSigning.getBooleanValue());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void createFieldEditors() {
        fieldUseLocalSigning =
            new BooleanFieldEditor(FlashKitPreferenceConstants.SIGNING_USE_LOCAL_SIGNING, "&Use local signing",
                getFieldEditorParent());
        addField(fieldUseLocalSigning);

        fieldUseCommonFolder =
            new BooleanFieldEditor(FlashKitPreferenceConstants.SIGNING_USE_COMMONFOLDER, "&Use common folder",
                getFieldEditorParent());
        addField(fieldUseCommonFolder);

        // server
        fieldSignServer =
            new StringFieldEditor(FlashKitPreferenceConstants.SIGNING_SIGN_SERVER, "&Sign server:",
                getFieldEditorParent());

        addField(fieldSignServer);

        fieldSignPackageRoot =
            new DirectoryFieldEditor(FlashKitPreferenceConstants.SIGNING_SIGN_PACKAGE_ROOT,
                "&Sign package root (Linux only):", getFieldEditorParent());
        addField(fieldSignPackageRoot);

        // local
        fieldLocalSignPackageRoot =
            new DirectoryFieldEditor(FlashKitPreferenceConstants.SIGNING_LOCAL_SIGN_PACKAGE_ROOT,
                "&Local sign package root:", getFieldEditorParent());
        addField(fieldLocalSignPackageRoot);

        fieldSigningLocalKeyRoot =
            new DirectoryFieldEditor(FlashKitPreferenceConstants.SIGNING_LOCAL_KEY_ROOT, "&Local key root:",
                getFieldEditorParent());
        addField(fieldSigningLocalKeyRoot);

        fieldSignLocalKeysPackageRoot =
            new DirectoryFieldEditor(FlashKitPreferenceConstants.SIGNING_LOCAL_KEY_PACKAGES_ROOT, "&Local common folder for\n"
                + "keys and packages root:", getFieldEditorParent());
        addField(fieldSignLocalKeysPackageRoot);

        fieldUseDefaultSignPackage =
            new BooleanFieldEditor(FlashKitPreferenceConstants.SIGNING_USE_DEFAULT_SIGN_PACKAGE,
                "&Use default sign package", getFieldEditorParent());
        addField(fieldUseDefaultSignPackage);

        fieldDefaultSignPackage =
            new StringFieldEditor(FlashKitPreferenceConstants.SIGNING_DEFAULT_SIGN_PACKAGE, "&Default sign package:",
                getFieldEditorParent());
        addField(fieldDefaultSignPackage);

        // Set initial fields enabled or disabled
        final IPreferenceStore store = Activator.getDefault().getPreferenceStore();
        setLocalSigningEnabled(store.getBoolean(FlashKitPreferenceConstants.SIGNING_USE_LOCAL_SIGNING), 
                store.getBoolean(FlashKitPreferenceConstants.SIGNING_USE_COMMONFOLDER));
        setDefaultSignPackageEnabled(store.getBoolean(FlashKitPreferenceConstants.SIGNING_USE_DEFAULT_SIGN_PACKAGE));
        setCommonFolderSigningEnabled(store.getBoolean(FlashKitPreferenceConstants.SIGNING_USE_COMMONFOLDER), 
                store.getBoolean(FlashKitPreferenceConstants.SIGNING_USE_LOCAL_SIGNING));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performOk() {
        boolean perform = super.performOk();
        if (isDirty) {
            SignedSoftwareUtils.setSystemVariables();
        }

        return perform;
    }

    private void setLocalSigningEnabled(boolean enabledLocal, boolean enabledCommon) {
        Composite parent = getFieldEditorParent();
        fieldSignServer.setEnabled(!enabledLocal, parent);
        fieldSignPackageRoot.setEnabled(!enabledLocal, parent);
        boolean enableValue = (enabledCommon) ? false : enabledLocal;
        fieldLocalSignPackageRoot.setEnabled(enableValue, parent);
        fieldSigningLocalKeyRoot.setEnabled(enableValue, parent);
    }

    private void setCommonFolderSigningEnabled(boolean enabledCommon, boolean enabledLocal) {
        Composite parent = getFieldEditorParent();
        boolean enableValue = (enabledCommon && enabledLocal) ? true : false;
        fieldSignLocalKeysPackageRoot.setEnabled(enableValue, parent);
    }

    private void setDefaultSignPackageEnabled(boolean enabled) {
        Composite parent = getFieldEditorParent();
        fieldDefaultSignPackage.setEnabled(enabled, parent);
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench pWorkbench) {
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
