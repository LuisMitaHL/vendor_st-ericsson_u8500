package com.stericsson.sdk.common.ui.editors;

import java.io.InputStream;

import javax.management.modelmbean.XMLParseException;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.forms.IManagedForm;
import org.eclipse.ui.forms.editor.SharedHeaderFormEditor;
import org.eclipse.ui.forms.widgets.ScrolledForm;

import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.EditorXMLParser;
import com.stericsson.sdk.common.ui.editors.profile.ProfileEditorPage;

/**Profile editor*/
public class ProfileEditor extends SharedHeaderFormEditor {

    private enum EditorPage {
        EDIT, SOURCE
    }

    private static final String XML_DESCRIPTION_FILE = "ui_profile.xml";
    private static final String EDITOR_PAGE_NAME = "Profile Editor";

    private TextEditor textEditor;

    private ScrolledForm form;

    private ProfileEditorPage profileEditorPage;

    /**
     * {@inheritDoc}
     */

    @Override
    protected void createHeaderContents(IManagedForm headerForm) {
        form = headerForm.getForm();
        form.setText(getTitle());
        getToolkit().decorateFormHeading(form.getForm());
        headerForm.getMessageManager().setAutoUpdate(true);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof IFileEditorInput)) {
            throw new PartInitException("Invalid Input: Must be IFileEditorInput");
        }
        super.init(site, input);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void addPages() {
        // Pages order is driven by order in EditorPage enum
        for (EditorPage page : EditorPage.values()) {
            switch (page) {
                case EDIT:
                    createEditPage();
                    break;
                case SOURCE:
                    createSourcePage();
                    break;
                default:
                    break;
            }
        }

        updateTitle();
    }

    private void createSourcePage() {
        try {
            textEditor = new TextEditor();
            int index = addPage(textEditor, getEditorInput());
            setPageText(index, getEditorInput().getName());
        } catch (PartInitException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private void createEditPage() {
        int index;
        try {
            InputStream xmlIn = getClass().getClassLoader().getResourceAsStream(XML_DESCRIPTION_FILE);
            EditorXML structure = EditorXMLParser.parse(xmlIn);
            profileEditorPage = new ProfileEditorPage(this, EditorPage.EDIT.toString(), EDITOR_PAGE_NAME, structure);
            index = addPage(profileEditorPage);
            setPageText(index, EDITOR_PAGE_NAME);
        } catch (PartInitException e) {
            // FIXME - handle error
            e.printStackTrace();
        } catch (XMLParseException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private void updateTitle() {
        IEditorInput input = getEditorInput();
        setPartName(input.getName());
        setTitleToolTip(input.getToolTipText());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void doSave(IProgressMonitor monitor) {
        if (getActivePage() == EditorPage.EDIT.ordinal()) {
            profileEditorPage.doSave(monitor);
            textEditor.doRevertToSaved();
        } else if (getActivePage() == EditorPage.SOURCE.ordinal()) {
            textEditor.doSave(monitor);
            profileEditorPage.doRevertToSaved();
        }
        profileEditorPage.setDirty(false);
    }

    /**
     * {@inheritDoc}
     */

    @Override
    public void doSaveAs() {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */

    @Override
    public boolean isSaveAsAllowed() {
        // TODO Auto-generated method stub
        return false;
    }

    /**
     * {@inheritDoc}
     */

    @Override
    protected void pageChange(int index) {
        super.pageChange(index);
        if (getActivePage() == EditorPage.EDIT.ordinal()) {
            profileEditorPage.updateFromTextEditor(textEditor.getDocumentProvider().getDocument(
                textEditor.getEditorInput()).get());
        } else if (getActivePage() == EditorPage.SOURCE.ordinal()) {
            textEditor.getDocumentProvider().getDocument(textEditor.getEditorInput()).set(
                profileEditorPage.getInputAsString());
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setFocus() {
        if (getActivePage() == EditorPage.EDIT.ordinal()) {
            profileEditorPage.setFocus();
        } else if (getActivePage() == EditorPage.SOURCE.ordinal()) {
            textEditor.setFocus();
        }
    }

}
