package com.stericsson.sdk.common.ui.editors.mesp;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPathEditorInput;
import org.eclipse.ui.forms.IManagedForm;
import org.eclipse.ui.forms.IMessage;
import org.eclipse.ui.forms.IMessageManager;
import org.eclipse.ui.forms.editor.FormPage;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.IEditorUIChangeListener;
import com.stericsson.sdk.common.ui.editors.MespEditor;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;

/**
 * @author xmicroh
 * 
 */

public class MespEditorPage extends FormPage implements IEditorUIChangeListener, IValidatorMessageListener {

    private static final String OVERVIEW_SECTION_NAME = "Configuration Overview";

    private static final String SETTINGS_SECTION_NAME = "Configuration Settings";

    private MespEditor parentEditor;

    private MespConfigurationFile configFile;

    private ScrolledForm form;

    private Section overviewSection;

    private MespSettingsSection settingsSection;

    private TreeViewer treeViewer;

    private ToolBar overviewToolbar;

    private boolean dirty;

    private EditorXML structure;

    /**
     * @param pEditor
     *            Owning editor.
     * @param pId
     *            Id of this page.
     * @param pTitle
     *            Title of this page
     * @param pStructure
     *            Structure of the editor (description of UI components read from xml config file)
     * 
     */
    public MespEditorPage(MespEditor pEditor, String pId, String pTitle, EditorXML pStructure) {
        super(pEditor, pId, pTitle);
        parentEditor = pEditor;
        structure = pStructure;

        IEditorInput input = pEditor.getEditorInput();
        if ((input != null) && (input instanceof IPathEditorInput)) {
            configFile = new MespConfigurationFile(((IPathEditorInput) input).getPath().toOSString());
        }
    }

    void createToolbar() {
        overviewToolbar = new ToolBar(overviewSection, SWT.FLAT | SWT.HORIZONTAL);
        overviewToolbar.setLayoutData(new GridData(SWT.RIGHT, SWT.NONE, true, false));

        ToolItem add = new ToolItem(overviewToolbar, SWT.PUSH);
        add.setData("GD_COMP_NAME", "MespEditorSection_Add_tbi");
        add.setText("Add");
        overviewToolbar.setVisible(true);

        add.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                openAddEntryDialog(overviewToolbar.getShell());
            }
        });

        overviewSection.setTextClient(overviewToolbar);
    }

    private void openAddEntryDialog(Shell shell) {
        InputDialog dialog = new InputDialog(shell, "Add new entry", "Enter the exact content of new row.", null, null);
        dialog.setBlockOnOpen(true);
        if (dialog.open() == InputDialog.OK) {
            // gets here after the dialog is closed
            String old = configFile.getInputAsString();
            configFile.doUpdateFromString(old + dialog.getValue());
            refreshUIControls();
            setDirty(true);
        }

    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void createFormContent(IManagedForm managedForm) {
        FormToolkit toolkit = getEditor().getToolkit();
        form = managedForm.getForm();
        form.getBody().setLayout(new GridLayout(3, true));

        // overview Section
        overviewSection = toolkit.createSection(form.getBody(), Section.TITLE_BAR);
        overviewSection.setText(OVERVIEW_SECTION_NAME);
        overviewSection.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createToolbar();

        /* Overview section content */
        treeViewer = new TreeViewer(overviewSection, SWT.SINGLE);
        treeViewer.getControl().setData("GD_COMP_NAME", "MespEditorPage_TreeViewer_tre");
        overviewSection.setClient(treeViewer.getTree());
        treeViewer.setContentProvider(new MespTreeContentProvider());
        treeViewer.setLabelProvider(new MespTreeLabelProvider());
        treeViewer.setInput(configFile);
        treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                treeSelectionChanged(event);
            }
        });

        // settings section
        if ((structure.getSections().size() != 1)
            || (!structure.getSections().get(0).getName().equals(SETTINGS_SECTION_NAME))) {
            throw new IllegalStateException("MESP editor configuration file should contain one and only one section, "
                + "the section should be named \"" + SETTINGS_SECTION_NAME + "\"");
        }
        EditorXML.Section section = structure.getSections().get(0);

        settingsSection = new MespSettingsSection(toolkit, form, this, section);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = 2;
        settingsSection.setLayoutData(gd);

        refreshUIControls();
    }

    private void refreshUIControls() {
        parentEditor.getHeaderForm().getMessageManager().removeAllMessages();

        treeViewer.refresh();
        settingsSection.refreshSection(configFile.getConfigRecords());

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isDirty() {
        return dirty;
    }

    /**
     * @param pDirty
     *            True indicates that editor input has been changed, false otherwise.
     */
    public void setDirty(boolean pDirty) {
        treeViewer.refresh();
        dirty = pDirty;
        getEditor().editorDirtyStateChanged();
    }

    /**
     * {@inheritDoc}
     */
    public void uiControlChanged(EditorUIControl source) {
        TreeSelection selection = ((TreeSelection) treeViewer.getSelection());
        MespDescriptionTreeItem treeItemToUpdate = null;
        if (selection.getFirstElement() instanceof MespConfigTreeItem) {
            treeItemToUpdate = ((MespConfigTreeItem) selection.getFirstElement()).getDescriptionTreeItem();
        } else if (selection.getFirstElement() instanceof MespDescriptionTreeItem) {
            treeItemToUpdate = (MespDescriptionTreeItem) selection.getFirstElement();
        }

        treeViewer.update(treeItemToUpdate, null);
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
     * Will reload input for this editor and refresh appropriate UI controls.
     */
    public void doRevertToSaved() {
        configFile.doRevertToSaved();
        refreshUIControls();
    }

    /**
     * Will reload input for this editor from given string and refresh appropriate UI controls.
     * 
     * @param text
     *            String representation of mesp configuration records.
     */
    public void updateFromTextEditor(String text) {
        configFile.doUpdateFromString(text);
        refreshUIControls();
    }

    /**
     * @return Return input of this editor in string representation.
     */
    public String getInputAsString() {
        return configFile.getInputAsString();
    }

    private void treeSelectionChanged(final SelectionChangedEvent event) {
        IConfigurationRecord record = getRecordFromSelectionChangeEvent(event.getSelection());
        settingsSection.selectionChanged(record);
    }

    private IConfigurationRecord getRecordFromSelectionChangeEvent(ISelection selection) {
        IConfigurationRecord record = null;
        if (selection instanceof TreeSelection) {
            Object treeItem = ((TreeSelection) selection).getFirstElement();
            if (treeItem instanceof MespConfigTreeItem) {
                record = ((MespConfigTreeItem) treeItem).getRecord();
            } else if (treeItem instanceof MespDescriptionTreeItem) {
                record = ((MespDescriptionTreeItem) treeItem).getMespConfigTreeItem().getRecord();
            }
        }
        return record;
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
     * @return configuration file
     */
    public MespConfigurationFile getConfigFile() {
        return configFile;
    }

}
