/**
 * 
 */
package com.stericsson.sdk.common.ui.wizards.pages;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Hashtable;

import org.apache.log4j.Logger;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard.EmptyTextValidator;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard.IControlValidator;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard.RegexpTextValidator;

/**
 * General settings page of New Profile Wizard.
 * 
 * @author xhelciz
 * @author Pawel Kapala
 * @author Pavel Kutac
 */
public class NewProfileWizardGeneralSettingsPage extends WizardPage {

    Logger logger = Logger.getLogger(NewProfileWizardGeneralSettingsPage.class.getName());

    private static final String TF_NAME_SAVE_TO = "Save to file:";

    private String profileFileName = null;

    private Text saveToTF = null;

    // TODO: create map the same as in NewProfileWizardPropertiesSettings page, or better create
    // AbstractWizardPage class
    // and put the list there
    // List of all validated controls
    private final List<NewProfileWizard.ValidatedControl> validatedControls =
        new ArrayList<NewProfileWizard.ValidatedControl>();

    // Used to store resulting data format
    private static final String CONTROL_DATA = "CONTROL_DATA";

    private static final String GD_COMP_NAME = "GD_COMP_NAME";

    private static final String GD_COMP_NAME_PREFIX = "NewProfileWizard_GeneralSettings_";

    private static final String BASIC_SETTINGS_SECTION_NAME = "Basic Settings";

    private static final String SOFTWARE_SETTINGS_SECTION_NAME = "Software Settings";

    private static final String[] SOFTWARE_EXTENSION = {
        "*.ldr"};

    private static final String[] PROFILE_EXTENSION = {
        "*.prfl"};

    private Hashtable<String, String> platforms = null;

    private Hashtable<String, String> types = null;

    private Text profileName = null;

    private Combo platformFilter = null;

    private Combo softwareType = null;

    private UITextField softwarePath = null;

    /**
     * @param pPageName
     *            name of general settings wizard page
     * 
     */
    public NewProfileWizardGeneralSettingsPage(String pPageName) {
        super(pPageName);
        platforms = new Hashtable<String, String>();
        platforms.put("U8500", "(platform=U8500)");
        platforms.put("U5500", "(platform=U5500)");
        platforms.put("M7x00", "(platform=m7x00)");
        platforms.put("l9540", "(platform=l9540)");

        types = new Hashtable<String, String>();
        types.put("Loader", "loader");
        types.put("Other", "other");
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        setControl(createPageControl(pParent));
        setTitle(getName());
        setDescription("Fill in profile name and select platform filter, choose software type and provide software path."
            + " Select where to save new profile.");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        // Iterate over validators, to see if everything is ok
        for (NewProfileWizard.ValidatedControl vc : validatedControls) {
            if (!vc.validate()) {
                return false;
            }
        }
        return true;
    }

    // TODO: abstract class for wizard pages
    /**
     * Create all the page controls
     * 
     * @param pParent
     *            parent composite
     * @return page control (ready to be later set with setControl)
     */
    private Control createPageControl(Composite pParent) {
        // Create main, root composite - page layout holder
        final int layoutColumns = 3;
        final Composite parent = new Composite(pParent, SWT.NONE);
        parent.setLayout(new GridLayout(layoutColumns, false));

        // Get instance of text field factory
        final UITextFieldsFactory uiTFFactory = UITextFieldsFactory.getInstance(null);

        // TODO: integrate the list with text factory (?)

        // Create validator for empty strings
        final EmptyTextValidator emptyTextValidator = new EmptyTextValidator();

        // Create basic settings section
        createBasicSettingsSection(parent, layoutColumns, uiTFFactory, emptyTextValidator);
        // Create software settings section
        createSoftwareSettingsSection(parent, layoutColumns, uiTFFactory, emptyTextValidator);

        // placeholder
        new Label(parent, SWT.NONE).setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, layoutColumns, 1));

        saveToTF =
            NewProfileWizard.createFilePathTextField(uiTFFactory, parent, TF_NAME_SAVE_TO, PROFILE_EXTENSION)
                .getUIControl();
        NewProfileWizard.ValidatedControl vName = new NewProfileWizard.ValidatedControl(saveToTF);
        vName.setControlValidator(emptyTextValidator, getWizard().getContainer());
        validatedControls.add(vName);

        // Validate inital input
        for (NewProfileWizard.ValidatedControl vc : validatedControls) {
            vc.validate();
            getWizard().getContainer().updateButtons();
        }

        return parent;
    }

    // Create basic settings group and its content
    private void createBasicSettingsSection(Composite parent, int layoutColumns, UITextFieldsFactory uiTFFactory,
        IControlValidator emptyTextValidator) {

        Composite group = NewProfileWizard.createGroup(parent, layoutColumns, BASIC_SETTINGS_SECTION_NAME);
        Composite comp = new Composite(group, SWT.NONE);
        comp.setLayout(new GridLayout(layoutColumns, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = layoutColumns;
        comp.setLayoutData(gd);
        comp.setData(CONTROL_DATA, "Basic");

        profileName = NewProfileWizard.createTextField(uiTFFactory, comp, "Name:").getUIControl();
        String name = "Name";
        String dataFormat = "Profile " + name + "=\".*\"";
        profileName.setData(CONTROL_DATA, dataFormat);
        profileName.setData(GD_COMP_NAME, GD_COMP_NAME_PREFIX + name);
        NewProfileWizard.ValidatedControl vText = new NewProfileWizard.ValidatedControl(profileName);
        String regexp = "[a-zA-Z0-9_]+";
        RegexpTextValidator rtv = new RegexpTextValidator(regexp);
        vText.setControlValidator(rtv, getWizard().getContainer());
        validatedControls.add(vText);

        String[] items = new String[platforms.size()];
        platforms.keySet().toArray(items);
        Arrays.sort(items);

        platformFilter = NewProfileWizard.createComboField(comp, "Platform:", items);
        name = "Filter";
        dataFormat = "Platform " + name + "=\".*\"";
        platformFilter.setData(CONTROL_DATA, dataFormat);
        platformFilter.setData("GD_COMP_NAME", GD_COMP_NAME_PREFIX + name);
    }

    // Create software settings group and its content
    private void createSoftwareSettingsSection(Composite parent, int layoutColumns, UITextFieldsFactory uiTFFactory,
        IControlValidator emptyTextValidator) {

        Composite group = NewProfileWizard.createGroup(parent, layoutColumns, SOFTWARE_SETTINGS_SECTION_NAME);
        Composite comp = new Composite(group, SWT.NONE);
        comp.setLayout(new GridLayout(layoutColumns, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = layoutColumns;
        comp.setLayoutData(gd);
        comp.setData(CONTROL_DATA, "Software");

        String[] items = new String[types.size()];
        types.keySet().toArray(items);
        Arrays.sort(items);

        softwareType = NewProfileWizard.createComboField(comp, "Type:", items);
        String name = "Type";
        String dataFormat = name + "=\".*\"";
        softwareType.setData(CONTROL_DATA, dataFormat);
        softwareType.setData("GD_COMP_NAME", GD_COMP_NAME_PREFIX + name);

        softwarePath = NewProfileWizard.createFilePathTextField(uiTFFactory, comp, "Boot image:", SOFTWARE_EXTENSION);
        name = "Path";
        dataFormat = name + "=\".*\"";
        softwarePath.getTextFieldButton().setData("GD_COMP_NAME", GD_COMP_NAME_PREFIX + name + "_PathBtn");
        Text text = softwarePath.getUIControl();
        text.setData(CONTROL_DATA, dataFormat);
        text.setData("GD_COMP_NAME", GD_COMP_NAME_PREFIX + name);
        NewProfileWizard.ValidatedControl vText = new NewProfileWizard.ValidatedControl(text);
        vText.setControlValidator(emptyTextValidator, getWizard().getContainer());
        validatedControls.add(vText);
    }

    /**
     * @return path to new profile file
     */
    public String getProfileFile() {
        profileFileName = saveToTF.getText();
        return profileFileName;
    }

    /**
     * Generate section of the resulting file based on user input
     * 
     * @param ps
     *            output file
     */
    public void generateFileSection(final PrintStream ps) {
        String text = getOutputText(profileName);
        printLine(ps, text);

        text = getOutputText(platformFilter);
        printLine(ps, text);

        text = "Software " + getOutputText(softwareType) + " " + getOutputText(softwarePath.getUIControl());
        printLine(ps, text);
    }

    private void printLine(final PrintStream ps, String text) {
        if (text != null) {
            ps.println(text);
        }
    }

    private String getOutputText(Control control) {
        String line = null;
        if (control instanceof Text) {
            Text text = (Text) control;
            String format = (String) control.getData(CONTROL_DATA);
            line = format.replace(".*", text.getText());
        } else if (control instanceof Combo) {
            Combo combo = (Combo) control;
            String format = (String) control.getData(CONTROL_DATA);
            String label = combo.getItem(combo.getSelectionIndex());
            String replace = "";
            if (combo == platformFilter) {
                replace = platforms.get(label);
            }
            if (combo == softwareType) {
                replace = types.get(label);
            }
            line = format.replace(".*", replace);
        } else if (control instanceof Button) {
            Button button = (Button) control;
            String format = (String) control.getData(CONTROL_DATA);
            if (format == null) {
                return null;
            }
            line = format.replace(".*", String.valueOf(button.getSelection()));
        }
        return line;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/new_profile_wizard/new_profile_wizard.html");
    }
}
