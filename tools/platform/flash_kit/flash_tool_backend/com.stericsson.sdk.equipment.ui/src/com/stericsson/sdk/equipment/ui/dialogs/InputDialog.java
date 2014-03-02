package com.stericsson.sdk.equipment.ui.dialogs;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.preference.PreferenceStore;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

import com.stericsson.sdk.equipment.ui.Activator;

/**
 * @author xdancho
 * 
 *         A class for input dialog, the dialog is built up of five different sections: info area,
 *         input area, validation and error area and the button area. all subclasses of InputDialog
 *         is responsible for creating the input area of the Dialog
 * 
 */
public abstract class InputDialog extends Dialog {

    private static final String DETAIL_BTN_EXPAND = "Details >>";

    private static final String DETAIL_BTN_CONTRACT = "<< Details ";

    String infoText;

    Text errorText;

    Label validText;

    Button detailButton;

    Group validGroup;

    Group inputGroup;

    PreferenceStore ps;

    String titleText;

    boolean initialized = false;

    Map<String, String> currentErrors = new HashMap<String, String>();

    /**
     * DumbMessageDialog constructor
     * 
     * @param parent
     *            the parent shell
     * @param info
     *            the text that will be displayed in the title bar and the info area of the dialog
     * 
     * @throws Exception
     */
    public InputDialog(Shell parent, String info) {
        super(parent);

        titleText = info;

        // create temporary ps file
        File psFile = null;
        try {
            psFile = File.createTempFile("input", ".properties");
        } catch (IOException ioe) {
            Activator.getDefault().getLog().log(
                new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Failed to create preference store file", ioe));
        }
        psFile.deleteOnExit();
        ps = new PreferenceStore(psFile.getAbsolutePath());

        try {
            ps.load();
        } catch (IOException ioe) {
            Activator.getDefault().getLog().log(
                new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Failed to load preference store file", ioe));
        }

        // make the dialog resizable
        setShellStyle(getShellStyle() | SWT.RESIZE);

        infoText = info;

    }

    /**
     * {@inheritDoc}
     */
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        shell.setText(titleText);
        URL url = null;
        try {
            url = FileLocator.toFileURL(getClass().getResource("/icons/systrayicon.png"));
            shell.setImage(new Image(Display.getDefault(), url.getFile()));

        } catch (Exception e) {

            Activator.getDefault().getLog().log(
                new Status(IStatus.WARNING, Activator.PLUGIN_ID, "failed to load icon", e));

        }
    }

    /**
     * Closes the dialog
     * 
     * @return boolean
     */
    public boolean close() {

        return super.close();
    }

    /**
     * {@inheritDoc}
     */
    protected Control createDialogArea(Composite parent) {
        // info
        Composite infoPanel = new Composite(parent, SWT.NO_REDRAW_RESIZE);
        GridLayout infoPanelLayout = new GridLayout(3, true);

        infoPanel.setLayout(infoPanelLayout);
        GridData infoPanelData = new GridData(GridData.FILL_HORIZONTAL);
        infoPanelData.heightHint = 60;

        infoPanelData.horizontalSpan = 3;
        infoPanel.setLayoutData(infoPanelData);
        infoPanel.setBackground(parent.getShell().getDisplay().getSystemColor(SWT.COLOR_WHITE));

        Label infoLabel = new Label(infoPanel, SWT.WRAP);
        infoLabel.setData("GD_COMP_NAME", "InputDialog_Info_Label");

        GridData infoLabelData = new GridData(GridData.BEGINNING);
        infoLabelData.widthHint = 200;
        infoLabelData.horizontalSpan = 2;
        infoLabel.setText(infoText);
        infoLabel.setBackground(infoPanel.getBackground());
        infoLabel.setLayoutData(infoLabelData);
        FontData[] fd = infoLabel.getFont().getFontData();
        fd[0].setHeight(fd[0].getHeight() + 1);
        infoLabel.setFont(new Font(getShell().getDisplay(), fd));

        // Input group
        inputGroup = new Group(parent, SWT.SHADOW_ETCHED_IN);
        inputGroup.setText("Input:");
        GridData inputData = new GridData(GridData.FILL_HORIZONTAL);

        inputGroup.setLayoutData(inputData);

        GridLayout inputGroupLayout = new GridLayout(3, false);
        inputGroupLayout.marginHeight = 0;
        inputGroupLayout.marginWidth = 0;
        inputGroup.setLayout(inputGroupLayout);

        // validation area
        Composite invalidInputComposite = new Composite(parent, SWT.NONE);
        GridLayout invalidInputCompositeLayout = new GridLayout(3, true);
        invalidInputComposite.setLayout(invalidInputCompositeLayout);
        GridData invalidInputCompositeData = new GridData(GridData.FILL_HORIZONTAL);
        invalidInputComposite.setLayoutData(invalidInputCompositeData);

        validText = new Label(invalidInputComposite, SWT.LEFT);
        validText.setData("GD_COMP_NAME", "InputDialog_ValidText_Label");
        GridData validTextData = new GridData(GridData.FILL_HORIZONTAL);
        validTextData.horizontalSpan = 2;
        validText.setLayoutData(validTextData);
        validText.setForeground(parent.getShell().getDisplay().getSystemColor(SWT.COLOR_RED));
        validText.setVisible(false);

        detailButton = new Button(invalidInputComposite, SWT.RIGHT);
        detailButton.setData("GD_COMP_NAME", "InputDialog_Detail_Button");
        GridData detailButtonData = new GridData(GridData.HORIZONTAL_ALIGN_END);
        detailButtonData.grabExcessHorizontalSpace = false;
        detailButtonData.horizontalSpan = 1;
        detailButton.setLayoutData(detailButtonData);
        detailButton.setText("Details >>");
        detailButton.addListener(SWT.MouseDown, new Listener() {

            public void handleEvent(Event event) {
                detailButtonClicked(event);

            }
        });

        detailButton.setVisible(false);

        errorText = new Text(parent, SWT.MULTI | SWT.WRAP | SWT.BORDER | SWT.READ_ONLY);
        errorText.setData("GD_COMP_NAME", "InputDialog_Error_Text");
        GridData errorTextData = new GridData(GridData.FILL_BOTH);
        errorTextData.heightHint = 0;
        errorTextData.widthHint = getShell().getMonitor().getClientArea().width / 6;
        errorText.setLayoutData(errorTextData);
        errorText.setVisible(false);

        createInputArea(inputGroup);

        Button okButton = getButton(OK);
        if (okButton != null) {
            okButton.setData("GD_COMP_NAME", "InputDialog_Ok_Button");
        }

        Button cancelButton = getButton(CANCEL);
        if (cancelButton != null) {
            cancelButton.setData("GD_COMP_NAME", "InputDialog_Cancel_Button");
        }

        initialized = true;

        new Thread() {
            public void run() {
                Display.getDefault().asyncExec(new Runnable() {
                    public void run() {
                        setDefaultValues();
                    }
                });

            }
        }.start();

        return inputGroup;

    }

    /**
     * The input area that this dialog should display
     * 
     * @param parent
     *            input area composite
     */
    protected abstract void createInputArea(Composite parent);

    private void detailButtonClicked(Event event) {
        if (((Button) event.widget).getText().equals(DETAIL_BTN_CONTRACT)) {
            ((Button) event.widget).setText(DETAIL_BTN_EXPAND);
            errorText.setVisible(false);
            ((GridData) errorText.getLayoutData()).heightHint = 0;

            int widthOfShell = getShell().getSize().x;
            getShell().pack(true);
            Point p = getShell().getSize();
            p.x = widthOfShell;
            getShell().setSize(p);

        } else {
            ((Button) event.widget).setText(DETAIL_BTN_CONTRACT);
            errorText.setVisible(true);
            ((GridData) errorText.getLayoutData()).heightHint = SWT.DEFAULT;

            int widthOfShell = getShell().getSize().x;
            getShell().pack(true);
            Point p = getShell().getSize();
            p.x = widthOfShell;
            getShell().setSize(p);

        }

    }

    /**
     * Handles a button press
     * 
     * @param buttonId
     *            the ID of the pressed button
     */
    protected void buttonPressed(int buttonId) {
        if (buttonId == IDialogConstants.OK_ID) {
            if (validatedBeforeExit()) {
                setReturnCode(buttonId);
                storeValues();
                close();
            } else {

                return;
            }
        } else {
            setReturnCode(buttonId);
            close();
        }
    }

    /**
     * This is called before exiting the dialog to validate that all fields values are valid
     * 
     * @return true if validation passed, false otherwise
     */
    protected abstract boolean validatedBeforeExit();

    /**
     * Store the input area FieldEditor's values to the preference store
     */
    protected abstract void storeValues();

    /**
     * {@inheritDoc}
     * 
     */
    protected abstract void propertyChange(PropertyChangeEvent event);

    /**
     * @param identifier
     *            tbd
     * @param detailedMessage
     *            the message that should be displayed when the details button is pressed
     */
    protected void addError(String identifier, String detailedMessage) {
        // add error id it does not exists
        if (!currentErrors.containsKey(identifier)) {
            currentErrors.put(identifier, detailedMessage);

            validText.setText("Invalid input");

            StringBuffer sb = new StringBuffer();
            for (String errors : currentErrors.values()) {
                sb.append(errors + System.getProperty("line.separator") + System.getProperty("line.separator"));
            }
            errorText.setText(sb.toString());
            toggleError(true);

        } else { // replace the current error
            if (!currentErrors.get(identifier).equals(detailedMessage)) {
                currentErrors.put(identifier, detailedMessage);

                validText.setText("Invalid input");

                StringBuffer sb = new StringBuffer();
                for (String errors : currentErrors.values()) {
                    sb.append(errors + System.getProperty("line.separator") + System.getProperty("line.separator"));
                }
                errorText.setText(sb.toString());
                toggleError(true);
            }
        }
    }

    /**
     * @param identifier
     *            tbd
     */
    protected void removeError(String identifier) {
        if (currentErrors.containsKey(identifier)) {

            currentErrors.remove(identifier);
            StringBuffer sb = new StringBuffer();
            for (String errors : currentErrors.values()) {
                sb.append(errors + System.getProperty("line.separator") + System.getProperty("line.separator"));
            }
            errorText.setText(sb.toString());
            if (currentErrors.size() == 0) {
                errorText.setText("");
                toggleError(false);
            }
        }

    }

    /**
     * This method is called when all area are created for this dialog, this is used by subclasses
     * to set the default values when all components have been created.
     */
    protected abstract void setDefaultValues();

    /**
     * This method will take care of validation and error area of the dialog depending on the
     * showError input
     * 
     * @param showError
     *            if the error should be shown or not
     */
    protected void toggleError(boolean showError) {

        if (!initialized) {
            return;
        }

        detailButton.setVisible(showError);
        validText.setVisible(showError);
        while (getButton(OK) == null) {
            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        getButton(OK).setEnabled(!showError);

        // if minimized
        if (detailButton.getText().equals(DETAIL_BTN_EXPAND)) {
            //int heightOfErrorText = ((GridData) errorText.getLayoutData()).heightHint;
            ((GridData) errorText.getLayoutData()).heightHint = 0;

            //int heightOfShell = getShell().getSize().y;
            int widthOfShell = getShell().getSize().x;
            getShell().pack(true);
            Point p = getShell().getSize();
            //p.y = heightOfShell - heightOfErrorText;
            p.x = widthOfShell;
            getShell().setSize(p);

        } else { 

            ((GridData) errorText.getLayoutData()).heightHint = SWT.DEFAULT;
            errorText.setVisible(showError);
            //int heightOfErrorText = ((GridData) errorText.getLayoutData()).heightHint;
            //int heightOfShell = getShell().getSize().y;
            int widthOfShell = getShell().getSize().x;
            getShell().pack(true);
            Point p = getShell().getSize();
            //p.y = heightOfShell + heightOfErrorText + 1;
            p.x = widthOfShell;
            getShell().setSize(p);
        }

    }

}
