package com.stericsson.sdk.common.ui.editors.profile;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPathEditorInput;
import org.eclipse.ui.forms.IManagedForm;
import org.eclipse.ui.forms.IMessage;
import org.eclipse.ui.forms.IMessageManager;
import org.eclipse.ui.forms.editor.FormPage;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;

import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.IEditorUIChangeListener;
import com.stericsson.sdk.common.ui.editors.ProfileEditor;
import com.stericsson.sdk.common.ui.editors.mesp.MespConfigurationFile;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;

/**
 * Page of Profile Editor.
 * 
 * @author xhelciz
 */

public class ProfileEditorPage extends FormPage implements IEditorUIChangeListener, IValidatorMessageListener {

    private ProfileEditor parentEditor;

    private MespConfigurationFile configFile;

    private ScrolledForm form;

    private EditorXML structure;

    private List<ProfileEditorSection> sections = new ArrayList<ProfileEditorSection>();

    private boolean dirty;

    /**
     * constructs profile editor page
     * 
     * @param pEditor
     *            profile editor
     * @param pId
     *            id of profile editor
     * @param pTitle
     *            title of profile editor
     * @param pStructure
     *            describes mapping of profile entries to gui representation
     */
    public ProfileEditorPage(ProfileEditor pEditor, String pId, String pTitle, EditorXML pStructure) {
        super(pEditor, pId, pTitle);
        parentEditor = pEditor;
        structure = pStructure;

        IEditorInput input = pEditor.getEditorInput();
        if ((input != null) && (input instanceof IPathEditorInput)) {
            configFile = new MespConfigurationFile(((IPathEditorInput) input).getPath().toOSString());
        }
    }

    /** {@inheritDoc} */
    @Override
    protected void createFormContent(IManagedForm managedForm) {
        FormToolkit toolkit = getEditor().getToolkit();
        form = managedForm.getForm();
        form.getBody().setLayout(new GridLayout(2, false));

        /* Sections */
        for (EditorXML.Section sectionStructure : structure.getSections()) {
            ProfileEditorSection section = new ProfileEditorSection(toolkit, form, this, sectionStructure);
            sections.add(section);
        }

        refreshUIControls();
    }

    private void refreshUIControls() {
        parentEditor.getHeaderForm().getMessageManager().removeAllMessages();

        // cleaning old stuff and fill with new
        for (ProfileEditorSection section : sections) {
            section.refreshSection(configFile.getConfigRecords());
        }
    }

    /**
     * {@inheritDoc}
     */
    public void validatorMessage(IValidator source, String message) {
        IMessageManager localMM = form.getMessageManager();
        IMessageManager parentMM = parentEditor.getHeaderForm().getMessageManager();

        if (!source.isValueOk()) {
            localMM.addMessage(source, message, null, IMessage.ERROR, source.getValidatedObject());
            parentMM.addMessage(source, message, null, IMessage.ERROR, source.getValidatedObject());
        } else {
            localMM.removeMessage(source, source.getValidatedObject());
            parentMM.removeMessage(source, source.getValidatedObject());
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isDirty() {
        return dirty;
    }

    /**
     * sets the page dirty
     * 
     * @param pDirty
     *            true if dirty
     */
    public void setDirty(boolean pDirty) {
        dirty = pDirty;
        getEditor().editorDirtyStateChanged();
    }

    /**
     * {@inheritDoc}
     */
    public void uiControlChanged(EditorUIControl pSource) {
        pSource.update();
        setDirty(true);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void doSave(IProgressMonitor monitor) {
        configFile.doSave(monitor);
    }

    /**
     * Refresh to last saved version of config file
     */
    public void doRevertToSaved() {
        configFile.doRevertToSaved();
        refreshUIControls();

    }

    /**
     * updates content of config file by the text parameter and refreshes UI controls according to
     * it
     * 
     * @param text
     *            to be updated
     */
    public void updateFromTextEditor(String text) {
        configFile.doUpdateFromString(text);
        refreshUIControls();

    }

    /**
     * Returns content of config file as a string
     * 
     * @return input string
     */
    public String getInputAsString() {
        return configFile.getInputAsString();
    }

    /**
     * returns configuration file
     * 
     * @return configuration file
     */
    public MespConfigurationFile getConfigFile() {
        return configFile;
    }
}
