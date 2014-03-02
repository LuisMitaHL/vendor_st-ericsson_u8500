/**
 * 
 */
package com.stericsson.sdk.equipment.ui.dialogs;

import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.common.ui.controls.newdesign.UITextField;
import com.stericsson.sdk.common.ui.controls.newdesign.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.validators.newdesign.IValidator;
import com.stericsson.sdk.common.ui.validators.newdesign.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.newdesign.RegexTFValidator;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.structures.ControlKeyCrate;

/**
 * GUI dialogue class for Authentication with Control keys
 * 
 * @author mbodan01
 */
public class AutenticateControlKeysDialog extends Dialog implements IValidatorMessageListener {
    private static final String VALIDATE_MESSAGE = "Value must be number of [8 to 16] digits";

    private static final String REGEXP = "[0-9]{8,16}";

    private static final int BORDER = SWT.BORDER;

    private static final String NETWORK_LOCK = "Network Lock:";

    private static final String NETWORK_SUBSET_LOCK = "Network Subset Lock:";

    private static final String SERVICE_PROVIDER_LOCK = "Service Provider Lock:";

    private static final String CORPORATE_LOCK = "Corporate Lock:";

    private static final String FLEXIBLE_ESL_LOCK = "Flexible ESL Lock:";

    private final String title;

    private final List<IValidator> validators = new ArrayList<IValidator>();

    private String infoText;

    private ControlKeyCrate keys;

    private UITextField nlock;

    private UITextField nslock;

    private UITextField splock;

    private UITextField clock;

    private UITextField elslock;

    /**
     * General constructor
     * 
     * @param pParentShell
     *            parent shell
     * @param pTitle
     *            title of dialogue window
     */
    public AutenticateControlKeysDialog(Shell pParentShell, String pTitle) {
        super(pParentShell);
        title = pTitle;
        setInfoText(pTitle);
        keys = new ControlKeyCrate();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        shell.setText(title);
        URL url = null;
        try {
            url = FileLocator.toFileURL(getClass().getResource("/icons/systrayicon.png"));
            shell.setImage(new Image(Display.getDefault(), url.getFile()));
        } catch (Exception e) {
            Activator.getDefault().getLog().log(
                new Status(IStatus.WARNING, Activator.PLUGIN_ID, "failed to load icon", e));
        }
    }

    private void initValidators() {
        for (IValidator validator : validators) {
            validator.switchOn();
            validator.addValidatorListener(this);
            validator.checkCurrentValue();
        }
    }

    /**
     * @param parent
     *            control parent
     * @return created control
     */
    @Override
    protected Control createContents(Composite parent) {
        Control createContents = super.createContents(parent);
        initValidators();
        return createContents;
    }

    /**
     * @param pParent
     *            control parent
     * @return created control
     */
    @Override
    protected Control createDialogArea(Composite pParent) {
        Composite composite = new Composite(pParent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(2, false));

        createInfoDialogArea(composite);

        UITextFieldsFactory tfFactory = UITextFieldsFactory.getInstance(null);
        nlock = tfFactory.createTextField(composite, BORDER, NETWORK_LOCK, "", 0);
        nslock = tfFactory.createTextField(composite, BORDER, NETWORK_SUBSET_LOCK, "", 0);
        splock = tfFactory.createTextField(composite, BORDER, SERVICE_PROVIDER_LOCK, "", 0);
        clock = tfFactory.createTextField(composite, BORDER, CORPORATE_LOCK, "", 0);
        elslock = tfFactory.createTextField(composite, BORDER, FLEXIBLE_ESL_LOCK, "", 0);

        validators
            .add(new RegexTFValidator(IStatus.ERROR, NETWORK_LOCK, nlock.getUIControl(), REGEXP, VALIDATE_MESSAGE));
        validators.add(new RegexTFValidator(IStatus.ERROR, NETWORK_SUBSET_LOCK, nslock.getUIControl(), REGEXP,
            VALIDATE_MESSAGE));
        validators.add(new RegexTFValidator(IStatus.ERROR, SERVICE_PROVIDER_LOCK, splock.getUIControl(), REGEXP,
            VALIDATE_MESSAGE));
        validators.add(new RegexTFValidator(IStatus.ERROR, CORPORATE_LOCK, clock.getUIControl(), REGEXP,
            VALIDATE_MESSAGE));
        validators.add(new RegexTFValidator(IStatus.ERROR, FLEXIBLE_ESL_LOCK, elslock.getUIControl(), REGEXP,
            VALIDATE_MESSAGE));

        return composite;
    }

    /**
     * @param pParent
     *            control parent
     * @return created control
     */
    protected Control createInfoDialogArea(Composite pParent) {
        Composite composite = new Composite(pParent, SWT.NO_REDRAW_RESIZE);
        GridLayout infoPanelLayout = new GridLayout(3, true);

        composite.setLayout(infoPanelLayout);
        GridData infoPanelData = new GridData(GridData.FILL_HORIZONTAL);
        infoPanelData.heightHint = 60;

        infoPanelData.horizontalSpan = 3;
        composite.setLayoutData(infoPanelData);
        composite.setBackground(pParent.getShell().getDisplay().getSystemColor(SWT.COLOR_WHITE));

        Label infoLabel = new Label(composite, SWT.WRAP);
        infoLabel.setData("GD_COMP_NAME", "InputDialog_Info_Label");

        GridData infoLabelData = new GridData(GridData.BEGINNING);
        infoLabelData.widthHint = 200;
        infoLabelData.horizontalSpan = 2;
        infoLabel.setText(infoText);
        infoLabel.setBackground(composite.getBackground());
        infoLabel.setLayoutData(infoLabelData);
        FontData[] fd = infoLabel.getFont().getFontData();
        fd[0].setHeight(fd[0].getHeight() + 1);
        infoLabel.setFont(new Font(getShell().getDisplay(), fd));

        return composite;
    }

    /**
     * Method is called on press OK button event
     */
    @Override
    protected void okPressed() {
        keys.setNetworkLockKey(nlock.getUIControl().getText());
        keys.setNetworkSubsetLockKey(nslock.getUIControl().getText());
        keys.setServiceProviderLockKey(splock.getUIControl().getText());
        keys.setCorporateLockKey(clock.getUIControl().getText());
        keys.setFlexibleESLLockKey(elslock.getUIControl().getText());
        super.okPressed();
    }

    /**
     * @return the infoText
     */
    public String getInfoText() {
        return infoText;
    }

    /**
     * @param pInfoText
     *            the infoText to set
     */
    private void setInfoText(String pInfoText) {
        infoText = pInfoText;
    }

    /**
     * @return the keys
     */
    public final ControlKeyCrate getKeys() {
        return keys;
    }

    /**
     * @param src
     *            instance of source validator
     * @param message
     *            message from validator
     */
    public void validatorMessage(IValidator src, IStatus message) {
        getButton(IDialogConstants.OK_ID).setEnabled(checkAllValidators());
    }

    /**
     * Method check validity all registered valitator
     * 
     * @return boolean result of valitators state
     */
    private boolean checkAllValidators() {
        for (IValidator validator : validators) {
            if (!validator.isValueOk()) {
                return false;
            }
        }
        return true;
    }

}
