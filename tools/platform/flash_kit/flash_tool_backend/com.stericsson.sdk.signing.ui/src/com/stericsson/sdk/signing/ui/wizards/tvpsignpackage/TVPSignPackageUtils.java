/**
 * 
 */
package com.stericsson.sdk.signing.ui.wizards.tvpsignpackage;

import java.io.File;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.wizard.IWizardContainer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.decorators.ControlDecorator;

/**
 * @author kapalpaw
 * 
 */
// TODO: Integrate this class with factories (?)
public final class TVPSignPackageUtils {

    private static final String CONTROL_DECORATOR_DATA = "CONTROL_DECORATOR";

    private static final String VALIDATOR_DATA = "VALIDATOR_DATA";

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

            final Text text = (Text) pControl;
            text.addModifyListener(new ModifyListener() {

                public void modifyText(ModifyEvent pEvent) {
                    if (!(pEvent.getSource() instanceof Control)) {
                        return;
                    }
                    Control source = (Control) pEvent.getSource();
                    if (source.getData(VALIDATOR_DATA) != null
                        && source.getData(VALIDATOR_DATA) instanceof TVPSignPackageUtils.ValidatedControl) {
                        TVPSignPackageUtils.ValidatedControl vc =
                            (TVPSignPackageUtils.ValidatedControl) source.getData(VALIDATOR_DATA);
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

    /**
     * Validator for path text fields
     */
    public static class PathTextValidator implements IControlValidator {

        /**
         * also check if the file exists
         */
        private boolean open;

        /**
         * Construct the validator
         * 
         * @param pOpen
         *            also check if the file exists
         */
        public PathTextValidator(final boolean pOpen) {
            open = pOpen;
        }

        /**
         *{@inheritDoc}
         */
        public void init(final Control pControl, final IWizardContainer pWizardContainer) {
            // NOTE: Applicable only for Text controls
            if (!(pControl instanceof Text)) {
                return;
            }

            final Text text = (Text) pControl;
            text.addModifyListener(new ModifyListener() {

                public void modifyText(ModifyEvent pEvent) {
                    if (!(pEvent.getSource() instanceof Control)) {
                        return;
                    }
                    final Control source = (Control) pEvent.getSource();
                    if (source.getData(VALIDATOR_DATA) != null
                        && source.getData(VALIDATOR_DATA) instanceof TVPSignPackageUtils.ValidatedControl) {
                        TVPSignPackageUtils.ValidatedControl vc =
                            (TVPSignPackageUtils.ValidatedControl) source.getData(VALIDATOR_DATA);
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
            if (open) {
                return "Provide valid, existing file";
            }
            return "Provide existing path and file name.";
        }

        /**
         * 
         *{@inheritDoc}
         */
        public boolean isValid(Control pControl) {
            // Applicable only for Text controls
            if (!(pControl instanceof Text)) {
                // Assume the control is valid, when it is set for incompatible controls
                return true;
            }

            final Text text = (Text) pControl;
            final String providedPath = text.getText();
            // Check if path is empty
            if (providedPath == null || providedPath.trim().equals("")) {
                return false;
            }
            final File file = new File(providedPath);

            if (open) {
                return file.exists();
            }
            return file.getParentFile() != null && file.getParentFile().exists();
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
                        && source.getData(VALIDATOR_DATA) instanceof TVPSignPackageUtils.ValidatedControl) {
                        TVPSignPackageUtils.ValidatedControl vc =
                            (TVPSignPackageUtils.ValidatedControl) source.getData(VALIDATOR_DATA);
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

    private TVPSignPackageUtils() {
    }

    /**
     * Adds GUI Dancer data to the control.
     * 
     * @param pControl
     *            control to which data is to be added
     * @param pClass
     *            class where control is used
     * @param pDescription
     *            additional description
     */
    @SuppressWarnings("unchecked")
    public static void addGDData(final Control pControl, final Class pClass, final String pDescription) {
        String suffix = null;
        if (pControl instanceof Text) {
            suffix = "_Text";
        }
        pControl.setData(IUIConstants.GD_COMP_NAME, pClass.getSimpleName() + pDescription + suffix);
    }

    // This method makes text field look like the ones in other wizards
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
        final UITextField uitf = pUiTFFactory.createTextField(pParent, SWT.BORDER, pName, null, 0);
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

    // This method makes text field look like the ones in other wizards
    /**
     * Create Multiline text field
     * 
     * @param pUiTFFactory
     *            factory to be used for creating
     * @param pParent
     *            parent
     * @param pName
     *            name of Text field
     * @param pHeightHint
     *            height hint for the text field
     * @return text field
     */
    public static UITextField createTextFieldMulti(final UITextFieldsFactory pUiTFFactory, final Composite pParent,
        final String pName, final int pHeightHint) {
        final UITextField uitf = pUiTFFactory.createTextField(pParent, SWT.WRAP | SWT.BORDER | SWT.MULTI, pName, null, 0);
        uitf.getControlLabel().setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        GridData gd = (GridData) uitf.getUIControl().getLayoutData();
        if (gd == null) {
            gd = new GridData();
            uitf.getUIControl().setLayoutData(gd);
        }
        gd.horizontalAlignment = GridData.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 2;
        gd.heightHint = pHeightHint;

        return uitf;
    }

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
     * @param open
     *            True means that path serves as input path, use false for output paths.
     * @return File Path text field
     */
    public static UITextField createFilePathTextField(final UITextFieldsFactory uiTFFactory, final Composite parent,
        final String name, final String[] filterExtensions, boolean open) {
        final UITextField uitf =
            uiTFFactory.createFilePathTextField(parent, SWT.BORDER, name, null, 0, null, filterExtensions, open);
        uitf.getControlLabel().setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        GridData gd = (GridData) uitf.getUIControl().getLayoutData();
        if (gd == null) {
            gd = new GridData();
            uitf.getUIControl().setLayoutData(gd);
        }
        gd.horizontalAlignment = GridData.FILL;
        gd.grabExcessHorizontalSpace = true;
        return uitf;
    }

    /**
     * Creates empty label.
     * 
     * @param pParent
     *            parent composite
     * @param pHorizontalSpan
     *            how many grid cells will this place holder overlap
     * @return reference to created place holder
     */
    public static Label createPlaceHolder(final Composite pParent, final int pHorizontalSpan) {
        final Label label = new Label(pParent, SWT.NONE);
        final GridData gd = new GridData(SWT.FILL, SWT.FILL, false, false);
        gd.horizontalSpan = pHorizontalSpan;
        label.setLayoutData(gd);
        return label;
    }

}
