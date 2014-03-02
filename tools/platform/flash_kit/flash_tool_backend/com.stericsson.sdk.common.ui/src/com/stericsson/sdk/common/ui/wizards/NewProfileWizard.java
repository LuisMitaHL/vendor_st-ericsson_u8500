/**
 * 
 */
package com.stericsson.sdk.common.ui.wizards;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardContainer;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.decorators.ControlDecorator;
import com.stericsson.sdk.common.ui.wizards.pages.NewProfileWizardGeneralSettingsPage;
import com.stericsson.sdk.common.ui.wizards.pages.NewProfileWizardPropertiesSettingsPage;
import com.stericsson.sdk.common.ui.wizards.pages.NewProfileWizardSummaryPage;

/**
 * Wizard creating a New Profile.
 * 
 * @author xhelciz
 * @author Pawel Kapala
 * @author Pavel Kutac
 */
public class NewProfileWizard extends Wizard implements INewWizard {

    Logger logger = Logger.getLogger(NewProfileWizard.class.getName());

    private static final String WIZARD_NAME = "New Profile Wizard";

    private static final String CONTROL_DECORATOR_DATA = "CONTROL_DECORATOR";

    private static final String VALIDATOR_DATA = "Validator";

    private static final String GENERAL_SETTINGS_PAGE = "General Settings";

    private static final String PROPERTIES_SETTINGS_PAGE = "Properties Settings";

    private static final String SUMMARY_PAGE = "Summary";

    NewProfileWizardGeneralSettingsPage generalSettingsPage;

    NewProfileWizardPropertiesSettingsPage propertiesSettingsPage;

    NewProfileWizardSummaryPage summaryPage;

    /**
     * Wizard for creating new profile
     */
    public NewProfileWizard() {
        super();
        setHelpAvailable(true);

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {

        setWindowTitle(WIZARD_NAME);

        generalSettingsPage = new NewProfileWizardGeneralSettingsPage(GENERAL_SETTINGS_PAGE);
        addPage(generalSettingsPage);
        propertiesSettingsPage = new NewProfileWizardPropertiesSettingsPage(PROPERTIES_SETTINGS_PAGE);
        addPage(propertiesSettingsPage);
        summaryPage = new NewProfileWizardSummaryPage(SUMMARY_PAGE);
        addPage(summaryPage);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFinish() {
        for (IWizardPage page : getPages()) {
            if (!page.isPageComplete()) {
                return false;
            }
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        PrintStream stream = null;
        try {
            stream = new PrintStream(generalSettingsPage.getProfileFile(), "UTF-8");
            generalSettingsPage.generateFileSection(stream);
            propertiesSettingsPage.generateFileSection(stream);
        } catch (FileNotFoundException e) {
            logger.error(e.getMessage());
        } catch (UnsupportedEncodingException e) {
            logger.error(e.getMessage());
        } finally {
            if (stream != null) {
                stream.close();
            }
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench, IStructuredSelection selection) {

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public IWizardPage getNextPage(IWizardPage page) {
        IWizardPage p = super.getNextPage(page);
        if (super.getNextPage(page) == summaryPage) {
            summaryPage.updatePage();
        }
        return p;
    }

    /**
     * Create group of controls which belong together
     * 
     * @param pParent
     *            parent
     * @param pLayoutColumns
     *            number of columns in layout
     * @param pText
     *            text of group
     * @return group of controls
     */
    public static Group createGroup(Composite pParent, int pLayoutColumns, String pText) {
        final Group group = new Group(pParent, SWT.SHADOW_ETCHED_OUT);
        group.setLayout(new GridLayout(pLayoutColumns, false));
        group.setText(pText);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, false);
        gd.horizontalSpan = 3;
        group.setLayoutData(gd);
        return group;
    }

    /**
     * Create checkbox button
     * 
     * @param parent
     *            parent
     * @param name
     *            name of button
     * @param layoutColumns
     *            number of columns to be spent in layout for this button
     * @return checkbox
     */
    public static Button createCheckbox(final Composite parent, final String name, final int layoutColumns) {
        int columnsUsed = 0;
        if (name != null) {
            // Checkbox label (created the same way, like in UITextFieldsFactory
            final Label lDescription = new Label(parent, SWT.NONE);
            lDescription.setText(name);
            lDescription.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
            columnsUsed++;
        }
        // The checkbox itself
        final Button checkBox = new Button(parent, SWT.CHECK);
        // Grid data set in the UITextFieldsFactory
        columnsUsed++;
        GridData gd = new GridData(SWT.BEGINNING, SWT.CENTER, false, false);
        checkBox.setLayoutData(gd);
        gd.horizontalIndent = 5 /* HORIZONTAL_INDENT */;
        gd.horizontalAlignment = GridData.FILL;
        gd.grabExcessHorizontalSpace = true;
        final int span = layoutColumns - columnsUsed + 1;
        if (span > 0) {
            gd.horizontalSpan = span;
        }
        return checkBox;
    }

    // TODO: create separate UIComboFieldsFactory?
    /**
     * Create comboBox with the same L&F as these in other wizards
     * 
     * @param parent
     *            parent
     * @param name
     *            name of ComboBox
     * @param items
     *            items to be placed in ComboBox
     * @return comboBox
     */
    public static Combo createComboField(final Composite parent, final String name, final String[] items) {
        // Combo label (created the same way, like in UITextFieldsFactory
        final Label lDescription = new Label(parent, SWT.NONE);
        lDescription.setText(name);
        lDescription.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        // The combo itself
        final Combo combo = new Combo(parent, SWT.BORDER | SWT.READ_ONLY);
        // Grid data set in the UITextFieldsFactory
        GridData gd = new GridData(SWT.BEGINNING, SWT.CENTER, false, false);
        combo.setLayoutData(gd);
        gd.horizontalIndent = 5 /* HORIZONTAL_INDENT */;
        gd.horizontalAlignment = GridData.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 2;
        combo.setItems(items);
        combo.select(0);
        return combo;
    }

    // TODO: merge into UITextFieldsFactory?
    /**
     * Create File Path text field
     * 
     * @param uiTFFactory
     *            factory to be used for creating
     * @param parent
     *            parent
     * @param name
     *            Name of File Path text field
     * @param filterExtensions
     *            extensions to be filtered in opened dialog
     * @return File Path text field
     */
    public static UITextField createFilePathTextField(final UITextFieldsFactory uiTFFactory, final Composite parent,
        final String name, final String[] filterExtensions) {
        final UITextField uitf =
            uiTFFactory.createFilePathTextField(parent, SWT.BORDER, name, "", 0, null, filterExtensions, false);
        uitf.getControlLabel().setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        GridData gd = (GridData) uitf.getUIControl().getLayoutData();
        if (gd == null) {
            gd = new GridData();
            uitf.getUIControl().setLayoutData(gd);
        }
        // TODO: verify at least if file exists
        gd.horizontalAlignment = GridData.FILL;
        gd.grabExcessHorizontalSpace = true;
        return uitf;
    }

    // TODO: merge into UITextFieldsFactory?
    // This method makes text field look like the ones in other tutorials
    /**
     * Create Text field
     * 
     * @param pUiTFFactory
     *            factory to be used for creating
     * @param pParent
     *            parent
     * @param pName
     *            name of Text field
     * @return text field
     */
    public static UITextField createTextField(final UITextFieldsFactory pUiTFFactory, final Composite pParent,
        final String pName) {
        final UITextField uitf = pUiTFFactory.createTextField(pParent, SWT.BORDER, pName, "", 0);
        uitf.getControlLabel().setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        GridData gd = (GridData) uitf.getUIControl().getLayoutData();
        if (gd == null) {
            gd = new GridData();
            uitf.getUIControl().setLayoutData(gd);
        }
        gd.horizontalAlignment = GridData.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 2;
        return uitf;
    }

    // TODO: Integrate with factories (?)
    /**
     * Represents a control that can be validated.
     */
    public static class ValidatedControl {

        private Control control;

        private IControlValidator validator;

        /**
         * Construct ValidatedControl for supplied control.
         * 
         * @param pControl
         *            control which will be validated
         */
        public ValidatedControl(Control pControl) {
            control = pControl;
            control.setData(VALIDATOR_DATA, this);
        }

        /**
         * Assign validator
         * 
         * @param pValidator
         *            validator, which provides validation mechanism.
         * @param pWizardContainer
         *            wizard page container, used to update next, finish buttons
         */
        public void setControlValidator(IControlValidator pValidator, IWizardContainer pWizardContainer) {
            validator = pValidator;
            validator.init(control, pWizardContainer);
        }

        /**
         * Execute validation on the control
         * 
         * @return true if the control is valid, false if it is invalid
         */
        public boolean validate() {
            if (validator == null) { // Assume the content is valid, if no validator is specified
                return true;
            }
            if (!validator.isValid(control)) {
                showErrorMessage(control, validator.getErrorMessage());
                return false;
            }
            hideErrorMessage(control);
            return true;
        }

        /**
         * Dispose validated control (with error messages, if any)
         */
        public void dispose() {
            // if (control.getData(CONTROL_DECORATOR_DATA) != null
            // && control.getData(CONTROL_DECORATOR_DATA) instanceof ControlDecorator) {
            //
            // ControlDecorator cd = (ControlDecorator) control.getData(CONTROL_DECORATOR_DATA);
            // cd.dispose();
            // }
            hideErrorMessage(control);
        }

        /**
         * Remove error message from the control
         * 
         * @param pSource
         *            control for which message will be removed
         */
        private void hideErrorMessage(Control pSource) {
            if (pSource.getData(CONTROL_DECORATOR_DATA) != null
                && pSource.getData(CONTROL_DECORATOR_DATA) instanceof ControlDecorator) {

                ControlDecorator cd = (ControlDecorator) pSource.getData(CONTROL_DECORATOR_DATA);
                cd.removeMessage();
                cd.dispose();
                pSource.setData(CONTROL_DECORATOR_DATA, null);
            }
        }

        /**
         * Add error message to the control
         * 
         * @param pSource
         *            control to which message will be added
         */
        private void showErrorMessage(Control pSource, String pMessage) {
            IStatus message = new Status(IStatus.ERROR, "no_name", pMessage);

            ControlDecorator cd = null;
            if (pSource.getData(CONTROL_DECORATOR_DATA) != null
                && pSource.getData(CONTROL_DECORATOR_DATA) instanceof ControlDecorator) {

                cd = (ControlDecorator) pSource.getData(CONTROL_DECORATOR_DATA);
            } else {
                cd = new ControlDecorator(pSource.getParent(), pSource, SWT.LEFT);
            }
            cd.showMessage(message);
            pSource.setData(CONTROL_DECORATOR_DATA, cd);
        }

    }

    /**
     * Validator for regular expressions
     */
    public static class RegexpTextValidator implements IControlValidator {

        /**
         * Compiled regular expression.
         */
        private Pattern pattern;

        /**
         * Regular expression string.
         */
        private String regexp;

        /**
         * Construct the validator using supplied regular expression.
         * 
         * @param pRegexp
         *            Regular expression string.
         */
        public RegexpTextValidator(final String pRegexp) {
            regexp = pRegexp;
            pattern = Pattern.compile(regexp);
        }

        /**
         * 
         * @param pControl
         *            validated control
         * @param pWizardContainer
         *            Used for Next, Finish buttons update
         */
        public void init(final Control pControl, final IWizardContainer pWizardContainer) {
            // NOTE: Applicable only for Text controls
            if (!(pControl instanceof Text)) {
                return;
            }

            Text text = (Text) pControl;
            text.addModifyListener(new ModifyListener() {

                public void modifyText(ModifyEvent pEvent) {
                    if (!(pEvent.getSource() instanceof Control)) {
                        return;
                    }
                    Control source = (Control) pEvent.getSource();
                    if (source.getData(VALIDATOR_DATA) != null
                        && source.getData(VALIDATOR_DATA) instanceof NewProfileWizard.ValidatedControl) {
                        NewProfileWizard.ValidatedControl vc =
                            (NewProfileWizard.ValidatedControl) source.getData(VALIDATOR_DATA);
                        vc.validate();
                        pWizardContainer.updateButtons();
                    }
                }
            });
        }

        /**
         * 
         *{@inheritDoc}
         */
        public String getErrorMessage() {
            return "This field should match regular expression (" + regexp + ")";
        }

        /**
         * 
         *{@inheritDoc}
         */
        public boolean isValid(Control pControl) {
            // Applicable only for Text controls
            if (!(pControl instanceof Text) || pattern == null) {
                // Assume the control is valid, when there is no pattern set,
                // or when it is set for incompatible controls
                return true;
            }

            final Text text = (Text) pControl;
            // if (text.isDisposed()) {
            // return true;
            // }
            final Matcher matcher = pattern.matcher(text.getText());
            return matcher.matches();
        }
    }

    // Example validator (for empty strings)
    /**
     * Validator for empty strings
     */
    public static class EmptyTextValidator implements IControlValidator {
        /**
         * 
         *{@inheritDoc}
         */
        public void init(final Control pControl, final IWizardContainer pWizardContainer) {
            // NOTE: Applicable only for Text controls
            if (!(pControl instanceof Text)) {
                return;
            }
            Text text = (Text) pControl;
            text.addModifyListener(new ModifyListener() {

                public void modifyText(ModifyEvent pEvent) {
                    if (!(pEvent.getSource() instanceof Control)) {
                        return;
                    }
                    Control source = (Control) pEvent.getSource();
                    if (source.getData(VALIDATOR_DATA) != null
                        && source.getData(VALIDATOR_DATA) instanceof NewProfileWizard.ValidatedControl) {
                        NewProfileWizard.ValidatedControl vc =
                            (NewProfileWizard.ValidatedControl) source.getData(VALIDATOR_DATA);
                        vc.validate();
                        pWizardContainer.updateButtons();
                    }
                }
            });
        }

        /**
         * 
         *{@inheritDoc}
         */
        public String getErrorMessage() {
            return "This field can not be empty";
        }

        /**
         * 
         *{@inheritDoc}
         */
        public boolean isValid(Control pControl) {
            // Applicable only for Text controls
            if (!(pControl instanceof Text)) {
                return true;
            }
            final Text text = (Text) pControl;
            return text.getText() != null && !(text.getText().length() == 0);
        }
    }

    // TODO: Possibly integrate with factories (?)
    /**
     * Validation method provider for {@link NewProfileWizard.ValidatedControl} components.
     */
    public interface IControlValidator {
        /**
         * Validator initializer (executed when validator has been set on the control).
         * 
         * @param pControl
         *            validated control
         * @param pWizardContainer
         *            wizard container, that will have buttons updated
         */
        void init(Control pControl, IWizardContainer pWizardContainer);

        /**
         * The error message that will be displayed if validation fails.
         * 
         * @return error message
         */
        String getErrorMessage();

        /**
         * Validation mechanism.
         * 
         * @param pControl
         *            validated control
         * @return true if the control is valid false if the control is invalid
         */
        boolean isValid(Control pControl);
    }

    /**
     * @return string representing content of profile file to be created
     */
    public String getFileContent() {
        String fileContent = "";
        File file = null;
        PrintStream stream = null;
        BufferedReader reader = null;
        FileInputStream fInput = null;
        try {
            file = new File("temporary.prfl");
            file.deleteOnExit();
            stream = new PrintStream(file, "UTF-8");
            generalSettingsPage.generateFileSection(stream);
            propertiesSettingsPage.generateFileSection(stream);

            fInput = new FileInputStream(file);
            reader = new BufferedReader(new InputStreamReader(fInput, "UTF-8"));
            String read = null;
            do {
                read = reader.readLine();
                if (read != null) {
                    fileContent += read + "<newLine>";
                }
            } while (read != null);

        } catch (FileNotFoundException e) {
            logger.error(e.getMessage());
        } catch (IOException e) {
            logger.error(e.getMessage());
        } finally {
            if (stream != null) {
                stream.close();
            }
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fInput != null) {
                try {
                    fInput.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return fileContent;
    }
}
