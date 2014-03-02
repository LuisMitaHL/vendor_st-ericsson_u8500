/**
 * 
 */
package com.stericsson.sdk.common.ui.wizards.pages;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard.EmptyTextValidator;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard.RegexpTextValidator;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard.ValidatedControl;

/**
 * @author xhelciz
 * @author Pawel Kapala
 * @author Pavel Kutac
 */
public class NewProfileWizardPropertiesSettingsPage extends WizardPage {
    private static final int COLUMN_NUMBER = 3;

    Logger logger = Logger.getLogger(NewProfileWizardPropertiesSettingsPage.class.getName());

    // private static final String CHECKBOX_ROW_DATA = "ROW";

    private static final String VALIDATED_CONTROL_ID_DATA = "ID";

    private int securityPropertiesRowNum = 0;

    // Used to store resulting data format
    private static final String CONTROL_DATA = "CONTROL_DATA";

    private static final String PROPERTIES_SECTION_NAME = "Properties";

    private static final String SECURITY_PROPERTIES_SECTION_NAME = "Security Properties";

    private static final String SECURITY_PROPERTY_CONTROL = "SECURITY_PROPERTY_CONTROL";

    private static final String SECURITY_PROPERTY_CHECK_BOX = "SECURITY_PROPERTY_CHECK_BOX";

    private static final String GDFS_AREA_PATH_REGEXP = "(/[-a-zA-Z0-9_]+)+/{0,1}";

    private static final String TRIM_AREA_PATH_REGEXP = "(/[-a-zA-Z0-9_]+)+/{0,1}";

    private Hashtable<String, String> types = null;

    private Combo bootIndication = null;

    private Button debugTraceOnUART = null;

    private Button xLoaderDebugTraceOnUART = null;

    private Button withDMAOnUSB = null;

    private Button usesLoaderCommunication = null;

    private Text gdfsAreaPath = null;

    private Text trimAreaPath = null;

    private Button addButton = null;

    private Button removeButton = null;

    private List<Control> securityPropertiesRows = new ArrayList<Control>();

    // List of all validated controls, with internal ids (may not be continous)
    // Integer id is needed to remove from the list, when control is disposed.
    // TODO: create proper class for validated controls
    private final Map<Integer, NewProfileWizard.ValidatedControl> validatedControls =
        new HashMap<Integer, NewProfileWizard.ValidatedControl>();

    // Last id added to the map
    private int nextValidatedId = 0;

    /**
     * @param pPageName
     *            name of properties settings wizard page
     */
    public NewProfileWizardPropertiesSettingsPage(final String pPageName) {

        super(pPageName);
        types = new Hashtable<String, String>();
        types.put("Programming", "programming");
        types.put("Normal", "normal");
        types.put("ADL", "adl");
        types.put("ALT", "alt");
        types.put("Production", "production");
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        setControl(createPageControl(pParent));
        setTitle(getName());
        setDescription("Choose properties.");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        // Iterate over validators, to see if everything is ok
        for (NewProfileWizard.ValidatedControl vc : validatedControls.values()) {
            if (!vc.validate()) {
                return false;
            }
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFlipToNextPage() {
        for (NewProfileWizard.ValidatedControl vc : validatedControls.values()) {
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
    private Control createPageControl(final Composite pParent) {
        // Create main, root composite - page layout holder

        final Composite parent = new Composite(pParent, SWT.NONE);
        parent.setLayout(new GridLayout(1, false));

        // Get instance of text field factory
        final UITextFieldsFactory uiTFFactory = UITextFieldsFactory.getInstance(null);

        // TODO: integrate the list with text factory (?)
        // Create validator for empty strings
        final EmptyTextValidator emptyTextValidator = new EmptyTextValidator();

        // Properties section
        // Create properties group
        createPropertiesGroup(parent, uiTFFactory, emptyTextValidator);
        // Create security properties group
        createSecurityPropertiesGroup(parent, uiTFFactory, emptyTextValidator);

        // Validate inital input
        validateControls();

        return parent;
    }

    // Create security properties group and its content
    private Group createSecurityPropertiesGroup(final Composite pParent, final UITextFieldsFactory pUiTFFactory,
        final EmptyTextValidator pEmptyTextValidator) {

        final Group securityPropertiesGroup =
            NewProfileWizard.createGroup(pParent, 1, SECURITY_PROPERTIES_SECTION_NAME);
        GridData gd = (GridData) securityPropertiesGroup.getLayoutData();
        if (gd == null) {
            gd = new GridData(SWT.NONE, SWT.NONE, true, false);
            securityPropertiesGroup.setLayoutData(gd);
        } else {
            gd.grabExcessVerticalSpace = true;
        }

        final int layoutColumns = 2;

        final Composite comp = new Composite(securityPropertiesGroup, SWT.NONE);
        comp.setLayout(new GridLayout(layoutColumns, false));

        // Add button
        addButton = new Button(comp, SWT.PUSH);
        // Remove button
        removeButton = new Button(comp, SWT.PUSH);

        final ScrolledComposite sc = new ScrolledComposite(securityPropertiesGroup, SWT.V_SCROLL);

        final Composite securityProperties = new Composite(sc, SWT.NONE);
        securityProperties.setLayout(new GridLayout(layoutColumns, false));

        addButton.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_AddButton" + "_Button");
        gd = new GridData(SWT.LEFT, SWT.NONE, false, false);
        addButton.setLayoutData(gd);
        addButton.setText("Add"); // TODO: externalize strings
        addButton.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent pE) {
                createSecurityPropertiesRow(securityProperties, pUiTFFactory, pEmptyTextValidator, layoutColumns);
                updateSecurityProperitesLayout(sc, securityPropertiesGroup, securityProperties);
                validateControls();
                securityPropertiesRowNum++;
                removeButton.setEnabled(true);
            }

            public void widgetDefaultSelected(SelectionEvent pE) {

            }
        });

        removeButton.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_RemoveButton" + "_Button");
        gd = new GridData(SWT.LEFT, SWT.NONE, true, false);
        gd.horizontalSpan = layoutColumns - 1; // The "Add" button uses the first cell
        removeButton.setLayoutData(gd);
        removeButton.setText("Remove selected"); // TODO: externalize strings
        removeButton.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent pE) {
                if (!(pE.getSource() instanceof Button) || securityPropertiesRowNum <= 0) {
                    return;
                }
                securityPropertiesRowNum -= removeSelectedSecurityProperties();
                updateSecurityProperitesLayout(sc, securityPropertiesGroup, securityProperties);
                validateControls();
                removeButton.setEnabled(securityPropertiesRowNum > 0);
            }

            public void widgetDefaultSelected(SelectionEvent pE) {

            }
        });
        removeButton.setEnabled(false);

        sc.setContent(securityProperties);
        sc.setExpandHorizontal(true);
        sc.setExpandVertical(true);
        sc.setLayoutData(new GridData(GridData.FILL_BOTH));
        sc.setMinSize(securityProperties.computeSize(SWT.DEFAULT, SWT.DEFAULT));

        return securityPropertiesGroup;
    }

    private void validateControls() {
        for (NewProfileWizard.ValidatedControl vc : validatedControls.values()) {
            vc.validate();
            getWizard().getContainer().updateButtons();
        }
        getWizard().getContainer().updateButtons();
    }

    private void updateSecurityProperitesLayout(final ScrolledComposite pSc, final Composite pParent,
        final Composite pSecurityProperties) {
        pSecurityProperties.layout();
        pSc.layout();
        pSc.setMinSize(pSecurityProperties.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        pParent.layout();
    }

    // Create properties group and its content
    private Group createPropertiesGroup(final Composite pParent, final UITextFieldsFactory pUiTFFactory,
        final EmptyTextValidator pEmptyTextValidator) {

        final Group propertiesGroup = NewProfileWizard.createGroup(pParent, 2, PROPERTIES_SECTION_NAME);

        // Column for properties
        final int layoutColumns = COLUMN_NUMBER;
        final Composite propertiesColumn = new Composite(propertiesGroup, SWT.NONE);
        propertiesColumn.setLayout(new GridLayout(layoutColumns, false));
        propertiesColumn.setLayoutData(new GridData(SWT.BEGINNING, SWT.NONE, true, false));

        String[] items = new String[types.size()];
        types.keySet().toArray(items);
        Arrays.sort(items);

        bootIndication = NewProfileWizard.createComboField(propertiesColumn, "Boot indication:", items);
        String name = "BootIndication";
        String dataFormat = "Property " + name + "=\".*\"";
        bootIndication.setData(CONTROL_DATA, dataFormat);
        bootIndication.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_" + name + "Combo" + "_Combo");

        name = "DebugTraceOnUART";
        dataFormat = "Property " + name + "=\".*\"";
        debugTraceOnUART = createCheckBox(propertiesColumn, "Debug trace on UART", name, dataFormat, layoutColumns);

        name = "XLoaderDebugTraceOnUART";
        dataFormat = "Property " + name + "=\".*\"";
        xLoaderDebugTraceOnUART =
            createCheckBox(propertiesColumn, "XLoader debug trace on UART", name, dataFormat, layoutColumns);

        name = "WithDMAOnUSB";
        dataFormat = "Property " + name + "=\".*\"";
        withDMAOnUSB = createCheckBox(propertiesColumn, "With DMA on USB", name, dataFormat, layoutColumns);

        name = "UsesLoaderCommunication";
        dataFormat = "Property " + name + "=\".*\"";
        usesLoaderCommunication =
            createCheckBox(propertiesColumn, "Uses loader communication", name, dataFormat, layoutColumns);

        gdfsAreaPath =
            NewProfileWizard.createTextField(pUiTFFactory, propertiesColumn, "GDFS area path:").getUIControl();
        name = "GDFSAreaPath";
        dataFormat = "Property " + name + "=\".*\"";
        gdfsAreaPath.setData(CONTROL_DATA, dataFormat);
        gdfsAreaPath.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_" + name + "Text" + "_Text");
        gdfsAreaPath.setData(VALIDATED_CONTROL_ID_DATA, nextValidatedId);
        NewProfileWizard.ValidatedControl vText = new NewProfileWizard.ValidatedControl(gdfsAreaPath);
        String regexp = GDFS_AREA_PATH_REGEXP;
        RegexpTextValidator rtv = new RegexpTextValidator(regexp);
        vText.setControlValidator(rtv, getWizard().getContainer());
        validatedControls.put(nextValidatedId, vText);
        nextValidatedId++;

        trimAreaPath =
            NewProfileWizard.createTextField(pUiTFFactory, propertiesColumn, "Trim area path:").getUIControl();
        name = "TrimAreaPath";
        dataFormat = "Property " + name + "=\".*\"";
        trimAreaPath.setData(CONTROL_DATA, dataFormat);
        trimAreaPath.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_" + name + "Text" + "_Text");
        trimAreaPath.setData(VALIDATED_CONTROL_ID_DATA, nextValidatedId);
        vText = new NewProfileWizard.ValidatedControl(trimAreaPath);
        regexp = TRIM_AREA_PATH_REGEXP;
        rtv = new RegexpTextValidator(regexp);
        vText.setControlValidator(rtv, getWizard().getContainer());
        validatedControls.put(nextValidatedId, vText);
        nextValidatedId++;

        return propertiesGroup;
    }

    private void createSecurityPropertiesRow(final Composite pParent, final UITextFieldsFactory pUiTFFactory,
        final EmptyTextValidator pEmptyTextValidator, final int layoutColumns) {

        Composite row = new Composite(pParent, SWT.NONE);
        row.setLayout(new GridLayout(14, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, false);
        gd.horizontalSpan = layoutColumns;
        row.setLayoutData(gd);

        Button button = NewProfileWizard.createCheckbox(row, null, layoutColumns);
        button.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_" + "PropertyCheckBox" + "_Button");
        button.setData(SECURITY_PROPERTY_CHECK_BOX, "TRUE");

        // separator label
        Label label = new Label(row, SWT.SEPARATOR | SWT.VERTICAL);

        Text text = NewProfileWizard.createTextField(pUiTFFactory, row, "Name:").getUIControl();
        String name = "Name";
        String dataFormat = name + "=\".*\"";
        text.setData(CONTROL_DATA, dataFormat);
        text.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_" + name + "Text" + "_Text");
        text.setData(VALIDATED_CONTROL_ID_DATA, nextValidatedId);
        text.setData(SECURITY_PROPERTY_CONTROL, "TRUE");
        Object ld = text.getLayoutData();
        if (ld instanceof GridData) {
            ((GridData) ld).widthHint = 100;
            text.setLayoutData(ld);
        }
        NewProfileWizard.ValidatedControl vText = new NewProfileWizard.ValidatedControl(text);
        String regexp = "[a-zA-Z0-9=_]+";
        RegexpTextValidator rtv = new RegexpTextValidator(regexp);
        vText.setControlValidator(rtv, getWizard().getContainer());
        validatedControls.put(nextValidatedId, vText);
        nextValidatedId++;

        // separator label height is based on the text field height
        gd = new GridData();
        gd.heightHint = text.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        label.setLayoutData(gd);

        text = NewProfileWizard.createTextField(pUiTFFactory, row, "ID:").getUIControl();
        name = "ID";
        dataFormat = name + "=\".*\"";
        text.setData(CONTROL_DATA, dataFormat);
        text.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_" + name + "Text" + "_Text");
        text.setData(VALIDATED_CONTROL_ID_DATA, nextValidatedId);
        text.setData(SECURITY_PROPERTY_CONTROL, "TRUE");
        ld = text.getLayoutData();
        if (ld instanceof GridData) {
            ((GridData) ld).widthHint = 80;
            text.setLayoutData(ld);
        }
        vText = new NewProfileWizard.ValidatedControl(text);
        regexp = "0x[0-9a-fA-F]+";
        rtv = new RegexpTextValidator(regexp);
        vText.setControlValidator(rtv, getWizard().getContainer());
        validatedControls.put(nextValidatedId, vText);
        nextValidatedId++;

        name = "Write";
        dataFormat = name + "=\".*\"";
        button = createCheckBox(row, "Write", name, dataFormat, layoutColumns);
        button.setData(SECURITY_PROPERTY_CONTROL, "TRUE");

        name = "Read";
        dataFormat = name + "=\".*\"";
        button = createCheckBox(row, "Read", name, dataFormat, layoutColumns);
        button.setData(SECURITY_PROPERTY_CONTROL, "TRUE");

        // prevent fix
        if (row instanceof Control) {
            securityPropertiesRows.add(row);
        }
    }

    private Button createCheckBox(final Composite parent, final String label, final String name,
        final String dataFormat, final int layoutColumns) {
        Button button = NewProfileWizard.createCheckbox(parent, label, layoutColumns);
        button.setData(CONTROL_DATA, dataFormat);
        button.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "_" + name + "CheckBox" + "_Button");
        return button;
    }

    /**
     * Remove selected security properties from parent composite
     * 
     * @return number of removed security properties
     */
    private int removeSelectedSecurityProperties() {
        int count = 0;
        Control[] selected = getSelectedSecurityProperties();
        for (Control control : selected) {
            if (control instanceof Composite) {
                Control[] children = ((Composite) control).getChildren();
                for (Control child : children) {
                    if (child.getData(VALIDATED_CONTROL_ID_DATA) != null) {
                        int id = (Integer) child.getData(VALIDATED_CONTROL_ID_DATA);
                        ValidatedControl vc = validatedControls.remove(id);
                        vc.dispose();
                    }
                }
                securityPropertiesRows.remove(control);
                control.dispose();
                count++;
            }
        }
        return count;
    }

    /**
     * Return array of composites containing controls of selected security properties
     * 
     * @return array of composites containing controls of selected security properties
     */
    private Control[] getSelectedSecurityProperties() {
        List<Control> selected = new ArrayList<Control>();
        Control[] rows = new Control[securityPropertiesRows.size()];
        securityPropertiesRows.toArray(rows);
        for (Control control : rows) {
            if (control instanceof Composite) {
                Control[] children = ((Composite) control).getChildren();
                for (Control child : children) {
                    String controlData = (String) child.getData(SECURITY_PROPERTY_CHECK_BOX);
                    if (child instanceof Button && controlData != null && controlData.equals("TRUE")) {
                        if (((Button) child).getSelection()) {
                            selected.add(control);
                        }
                    }
                }
            }
        }
        rows = new Control[selected.size()];
        selected.toArray(rows);
        return rows;
    }

    /**
     * Generate section of the resulting file based on user input
     * 
     * @param ps
     *            output file
     */
    public void generateFileSection(final PrintStream ps) {
        String text = getOutputText(bootIndication, false);
        printLine(ps, text);

        text = getOutputText(debugTraceOnUART, false);
        printLine(ps, text);

        text = getOutputText(xLoaderDebugTraceOnUART, false);
        printLine(ps, text);

        text = getOutputText(withDMAOnUSB, false);
        printLine(ps, text);

        text = getOutputText(usesLoaderCommunication, false);
        printLine(ps, text);

        text = getOutputText(gdfsAreaPath, false);
        printLine(ps, text);

        text = getOutputText(trimAreaPath, false);
        printLine(ps, text);

        for (Control control : securityPropertiesRows) {
            if (control instanceof Composite) {
                Control[] children = ((Composite) control).getChildren();
                String rowText = "SecurityProperty";
                for (Control child : children) {
                    String controlData = (String) child.getData(SECURITY_PROPERTY_CONTROL);
                    if (controlData != null && controlData.equals("TRUE")) {
                        text = getOutputText(child, true);
                        if (text != null) {
                            rowText += " " + text;
                        }
                    }
                }
                printLine(ps, rowText);
            }
        }
    }

    private void printLine(final PrintStream ps, String text) {
        if (text != null) {
            ps.println(text);
        }
    }

    private String getOutputText(Control control, boolean isSecurityPropertyControl) {
        String line = null;
        if (control instanceof Text) {
            Text text = (Text) control;
            String format = (String) control.getData(CONTROL_DATA);
            line = format.replace(".*", text.getText()) + " ";
        } else if (control instanceof Combo) {
            Combo combo = (Combo) control;
            String format = (String) combo.getData(CONTROL_DATA);
            String label = combo.getItem(combo.getSelectionIndex());
            String replace = "";
            if (combo == bootIndication) {
                replace = types.get(label);
            }
            line = format.replace(".*", replace) + " ";
        } else if (control instanceof Button) {
            Button button = (Button) control;
            String format = (String) control.getData(CONTROL_DATA);
            if (format != null) {
                boolean selection = button.getSelection();
                // at the moment only properties with flag set to true are exported to file,
                // attributes of security properties are exported even if their flag is set to false
                if (selection || isSecurityPropertyControl) {
                    line = format.replace(".*", String.valueOf(selection)) + " ";
                }
            }
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
