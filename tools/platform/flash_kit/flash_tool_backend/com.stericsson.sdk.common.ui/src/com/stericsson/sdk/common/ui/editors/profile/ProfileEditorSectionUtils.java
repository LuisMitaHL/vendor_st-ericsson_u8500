package com.stericsson.sdk.common.ui.editors.profile;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;

import com.stericsson.sdk.common.ui.EditorXML;

/**
 * Utils used by {@link ProfileEditorSection}
 * 
 * @author kapalpaw
 * 
 */
public final class ProfileEditorSectionUtils {

    private ProfileEditorSectionUtils() {
    }

    /**
     * Creates toolbar assigned with section
     * 
     * @param parent
     *            profile editor section
     * @param section
     *            section widget
     * @param structure
     *            xml section
     */
    public static void createToolbar(ProfileEditorSection parent, Section section, EditorXML.Section structure) {
        ToolBar toolbar = new ToolBar(section, SWT.FLAT | SWT.HORIZONTAL);
        toolbar.setLayoutData(new GridData(SWT.RIGHT, SWT.NONE, true, false));

        if (structure.isExtendable()) {
            AddRecordSelectionAdapter addListener = new AddRecordSelectionAdapter(structure, toolbar, parent);
            ToolItem addItem = new ToolItem(toolbar, SWT.PUSH);
            String joinedName = structure.getName().replaceAll(" ", "");
            addItem.setData("GD_COMP_NAME", "ProfileEditorSection_" + joinedName + "Add_tbi");
            // addItem.setImage(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJ_ADD));
            addItem.setText("Add");
            addItem.addSelectionListener(addListener);
        }
        toolbar.setVisible(true);

        section.setTextClient(toolbar);
    }

    /**
     * Opens remove entry definition dialog
     * 
     * @param shell
     *            shell in which dialog should be opened
     * @return true if Yes has been selected else false
     */
    public static boolean openRemoveEntryDialog(final Shell shell) {
        MessageDialog removeMessage =
            new MessageDialog(shell, "Remove entry", null, "Remove entry definition?", MessageDialog.QUESTION,
                new String[] {
                    "Yes", "No"}, 0);
        return removeMessage.open() == Dialog.OK;
    }

    /**
     * Creates section widget
     * 
     * @param form
     *            scrolled form
     * @param toolkit
     *            form toolkit
     * @param structure
     *            xml structure
     * @return created section
     */
    public static Section createSection(ScrolledForm form, FormToolkit toolkit, EditorXML.Section structure) {
        final Section section =
            toolkit.createSection(form.getBody(), Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, false, false);
        gd.horizontalSpan = structure.getColumnSpan();
        section.setLayoutData(gd);
        section.setText(structure.getName());
        return section;
    }

    /**
     * Creates control composite
     * 
     * @param sectionComposite
     *            section composite
     * @param gdName
     *            guidancer name
     * @param gdId
     *            guidancer id
     * @return creted composite
     */
    public static Composite creteControlComposite(Composite sectionComposite, String gdName, int gdId) {
        final Composite controlComposite = new Composite(sectionComposite, SWT.NONE);
        controlComposite.setData("GD_COMP_NAME", "EditorUIControl_PropertyComposite_" + gdName + gdId);
        controlComposite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        controlComposite.setLayout(new GridLayout(2, false));
        return controlComposite;
    }

    /**
     * Creates remove button
     * 
     * @param controlComposite
     *            control composite
     * @param gdName
     *            guidancer name
     * @param gdId
     *            guidancer id
     * @return created remove button
     */
    public static Button createRemoveButton(Composite controlComposite, String gdName, int gdId) {
        final Button remove = new Button(controlComposite, SWT.PUSH | SWT.FLAT);
        remove.setLayoutData(new GridData(SWT.RIGHT, SWT.BOTTOM, true, true));
        remove.setData("GD_COMP_NAME", "EditorUIControl_Remove_PropertyComposite_" + gdName + gdId + "_Button");

        remove.setText("Remove");
        return remove;
    }
}
