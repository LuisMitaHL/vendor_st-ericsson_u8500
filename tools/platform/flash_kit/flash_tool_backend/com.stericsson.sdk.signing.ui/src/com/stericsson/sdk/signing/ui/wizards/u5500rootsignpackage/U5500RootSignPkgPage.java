package com.stericsson.sdk.signing.ui.wizards.u5500rootsignpackage;

import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.PathTFValidator;
import com.stericsson.sdk.common.ui.validators.RegexTFValidator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.ui.wizards.isswcertificate.ISSWCertificateWizard;

/**
 * @author xmicroh
 * 
 */
public class U5500RootSignPkgPage extends AbstractWizardPage implements IValidatorMessageListener {

    private static final int TA_HEIGHT_HINT = 100;

    private static final String TF_NAME_ALIAS = "Alias:";

    private static final String TF_NAME_REVISION = "Revision:";

    private static final String TF_NAME_DESC = "Description:";

    private static final String TF_NAME_ISSW = "ISSW certificate:";

    private static final String TF_NAME_CREATED_BY = "Created by:";

    private static final String TF_NAME_CREATED_DATE = "Created date:";

    private List<IValidator> validators;

    private Text alias;

    private Text revision;

    private Text desc;

    private Text isswPath;

    private Text createdBy;

    /**
     * @param pageTitle
     *            Title for this page
     */
    protected U5500RootSignPkgPage(String pageTitle) {
        super(pageTitle);
        validators = new ArrayList<IValidator>();
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite parent) {
        int layoutColumns = 4;
        Composite c = new Composite(parent, SWT.NONE);
        setControl(c);
        c.setLayout(new GridLayout(layoutColumns, false));

        UITextFieldsFactory uiTFFactory = UITextFieldsFactory.getInstance(null);

        GridData gd;

        alias = uiTFFactory.createTextField(c, SWT.BORDER, TF_NAME_ALIAS, "", 0).getUIControl();
        alias.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_Alias_Text");
        gd = (GridData) alias.getLayoutData();
        gd.horizontalSpan = layoutColumns - 1;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        validators.add(new RegexTFValidator(TF_NAME_ALIAS, alias, "[a-zA-Z0-9_.\\\\w]+"));

        revision = uiTFFactory.createTextField(c, SWT.BORDER, TF_NAME_REVISION, "", 0).getUIControl();
        revision.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_Revision_Text");
        gd = (GridData) revision.getLayoutData();
        gd.horizontalSpan = layoutColumns - 1;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        validators.add(new RegexTFValidator(TF_NAME_REVISION, revision, "[a-zA-Z0-9.]+"));

        UITextField descritpion = uiTFFactory.createTextField(c, SWT.BORDER, TF_NAME_DESC, "", 0);
        desc = descritpion.getUIControl();
        desc.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_Description_Text");
        gd = (GridData) desc.getLayoutData();
        gd.horizontalSpan = layoutColumns - 1;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.heightHint = TA_HEIGHT_HINT;

        // gd = (GridData) descritpion.getTextFieldDescriptor().getLayoutData();
        // gd.verticalAlignment = SWT.TOP;

        validators.add(new RegexTFValidator(TF_NAME_DESC, desc, "(.)+"));

        // placeholder
        gd = new GridData(SWT.FILL, SWT.FILL, false, false);
        gd.horizontalSpan = layoutColumns;
        new Label(c, SWT.NONE).setLayoutData(gd);

        UITextField isswCertificate =
            uiTFFactory.createFilePathTextField(c, SWT.BORDER, TF_NAME_ISSW, "", 0, null, new String[] {
                "*.bin"}, true);
        isswCertificate.getTextFieldButton().setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_ISSWPath_Button");
        isswPath = isswCertificate.getUIControl();
        isswPath.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_ISSWPath_Text");
        gd = (GridData) isswPath.getLayoutData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        validators.add(new PathTFValidator(TF_NAME_ISSW, isswPath, false));

        Button createBT = new Button(c, SWT.PUSH);
        createBT.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_CreateBT_Botton");
        createBT.setText("Create...");
        createBT.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        createBT.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent pE) {
                String path = ISSWCertificateWizard.openWizard();
                if (path != null) {
                    isswPath.setText(path);
                }
            }
        });

        // placeholder
        gd = new GridData(SWT.FILL, SWT.FILL, false, false);
        gd.horizontalSpan = layoutColumns;
        new Label(c, SWT.NONE).setLayoutData(gd);

        String value = DateFormat.getDateInstance().format(new Date());
        Text created = uiTFFactory.createTextField(c, SWT.BORDER, TF_NAME_CREATED_DATE, value, 0).getUIControl();
        created.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_Created_Text");
        created.setEditable(false);
        gd = (GridData) created.getLayoutData();
        gd.horizontalSpan = layoutColumns - 1;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;

        createdBy =
            uiTFFactory.createTextField(c, SWT.BORDER, TF_NAME_CREATED_BY, System.getProperty("user.name"), 0)
                .getUIControl();
        createdBy.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPkgPage_CreatedBy_Text");
        createdBy.setEditable(false);
        gd = (GridData) createdBy.getLayoutData();
        gd.horizontalSpan = layoutColumns - 1;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;

        for (IValidator validator : validators) {
            validator.addValidatorListener(this);
            validator.switchOn();
            validator.checkCurrentValue();
        }

        setDescription("Fill in required fields..");

    }

    /**
     * {@inheritDoc}
     */
    public void validatorMessage(IValidator src, String message) {
        if (src.isValueOk()) {
            removeMessage(src.getValidatedObject());
        } else {
            addMessage(src.getValidatedObject(), message, MESSAGE_TYPE.ERROR);
        }

        if (getWizard().getContainer().getCurrentPage() != null) {
            getWizard().getContainer().updateButtons();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        for (IValidator validator : validators) {
            if (!validator.isValueOk()) {
                return false;
            }
        }
        return true;
    }

    String getAlias() {
        return alias.getText();
    }

    String getRevision() {
        return revision.getText();
    }

    String getDesc() {
        return desc.getText();
    }

    String getIsswPath() {
        return isswPath.getText();
    }

    String getCreatedBy() {
        return createdBy.getText();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_root_pkg.html");
    }
}
