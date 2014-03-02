package com.stericsson.sdk.equipment.ui.editors;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.management.modelmbean.XMLParseException;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.ui.forms.IMessage;
import org.eclipse.ui.forms.IMessageManager;
import org.eclipse.ui.forms.widgets.Form;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.Section;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.EditorXMLParser;
import com.stericsson.sdk.common.ui.IEditorUIChangeListener;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler;
import com.stericsson.sdk.equipment.ui.editors.security.SecurityFactory;
import com.stericsson.sdk.equipment.ui.jobs.BindPropertiesJob;
import com.stericsson.sdk.equipment.ui.jobs.ReadEquipmentPropertyJob;
import com.stericsson.sdk.equipment.ui.jobs.WriteEquipmentPropertyJob;

/**
 * @author xdancho
 * 
 */
public class EquipmentEditorSecurityPage implements IEditorUIChangeListener, IValidatorMessageListener,
    IJobChangeListener {

    private IEquipment equipment;

    private Composite pageComposite;

    private Composite sectionComposite;

    private FormToolkit toolkit;

    private List<Composite> uiControls;

    private Form form;

    private AtomicInteger jobCounter = new AtomicInteger(0);

    private static final String GD = "GD_COMP_NAME";

    private static final String PROPERTY_ID = "ID";

    private static final String IMEI_PROPERTY_REGEX = "[0-9]{14}";

    private Vector<IValidator> validatorList = new Vector<IValidator>();

    private String platform;

    Hashtable<String, EquipmentProperty> securityProperties;

    private Button bindPropertiesButton;

    private ISecurityPropertiesHandler securityPropertiesHandler;

    private Button refreshButton;

    private int waitingJobs;

    /**
     * @param e
     *            tbd
     * @param parent
     *            tbd
     * @param formToolkit
     *            tbd
     */
    public EquipmentEditorSecurityPage(IEquipment e, Composite parent, FormToolkit formToolkit) {
        equipment = e;
        toolkit = formToolkit;
        uiControls = new ArrayList<Composite>();
        pageComposite = createPage(parent, toolkit);

    }

    /**
     * @return TBD
     */
    public Composite getPageComposite() {
        return pageComposite;
    }

    void setEquipment(IEquipment e) {
        equipment = e;
        if (equipment == null) {
            setButtonEnabled(false);
        } else {
            setButtonEnabled(true);
        }
    }

    /**
     */
    public void updatePage() {

        if (decrementJobCounter() == 0) {
            setButtonEnabled(true);
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    securityPropertiesHandler.updateUI(equipment);

                }
            });
        }
    }

    private int decrementJobCounter() {
        return jobCounter.decrementAndGet();
    }

    private int incrementJobCounter() {
        return jobCounter.incrementAndGet();
    }

    /**
     * 
     * @param parent
     *            TBD
     * @param toolkit
     *            TBD
     * @return TBD
     */
    private Composite createPage(Composite parent, FormToolkit formToolkit) {
        form = toolkit.createForm(parent);
        form.setText("Mobile Equipment Security");
        form.getBody().setLayout(new GridLayout(1, true));
        toolkit.decorateFormHeading(form);
        form.setData("GD_COMP_NAME", "EquipmentEditorSecurityPage_PageComposite_ScrolledForm");

        try {
            platform = equipment.getProfile().getPlatformFilter(); // "(platform=U8500)"
            platform = platform.substring(platform.indexOf("=") + 1, platform.indexOf(")", platform.indexOf("=")));
            InputStream xmlIn = getClass().getClassLoader().getResourceAsStream("ui_security_" + platform + ".xml");
            EditorXML structure = EditorXMLParser.parse(xmlIn);

            // look up the current profile's security properties
            securityProperties = equipment.getSupportedSecurityProperties();

            // set the security properties handler
            try {

                securityPropertiesHandler = SecurityFactory.getSecurityPropertiesHandler(platform);

            } catch (Exception e) {
                Activator.getDefault().getLog().log(new Status(IStatus.ERROR, Activator.PLUGIN_ID, e.getMessage()));
                return form;
            }
            // save as configuration records
            saveAsConfigurationRecords(securityProperties);

            // create sections
            for (EditorXML.Section sectionStructure : structure.getSections()) {
                Section section =
                    toolkit.createSection(form.getBody(), Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
                section.setText(sectionStructure.getName());
                // make the section fit the window
                section.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
                // create section content
                createSectionContent(sectionStructure, section);

            }

            // create buttons
            Composite buttonComposite = toolkit.createComposite(form.getBody());
            buttonComposite.setLayout(new GridLayout(2, false));
            refreshButton = new Button(buttonComposite, SWT.PUSH);
            refreshButton.setText("Refresh Properties");
            refreshButton.setData(GD, "EquipmentEditorSecurityPage_Refresh_Button");

            refreshButton.addListener(SWT.Selection, new Listener() {

                public void handleEvent(Event event) {
                    refreshProperties();

                }
            });

            bindPropertiesButton = new Button(buttonComposite, SWT.PUSH);
            bindPropertiesButton.setText("Bind properties");
            bindPropertiesButton.setData(GD, "EquipmentEditorSecurityPage_Refresh_Button");
            bindPropertiesButton.addListener(SWT.Selection, new Listener() {

                public void handleEvent(Event event) {
                    bindProperties();
                }
            });
            bindPropertiesButton.setEnabled(false);
        } catch (XMLParseException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return form;
    }

    private void refreshProperties() {
        setButtonEnabled(false);
        // securityPropertiesHandler.readSecurityProperties(equipment);

        for (String s : securityPropertiesHandler.getReadIdValueMap().keySet()) {
            // command, device_id, name of property=value
            ReadEquipmentPropertyJob job = new ReadEquipmentPropertyJob(equipment, s);
            job.schedule();
            incrementJobCounter();
        }

        // read domain and update domain

    }

    private void bindProperties() {
        setButtonEnabled(false);

        // add the bind properties job
        incrementJobCounter();

        for (String s : securityPropertiesHandler.getWriteIdValueMap().keySet()) {

            // TODO: HACK OTP IS NOT AVAILABLE
            if (securityPropertiesHandler.getWriteIdValueMap().get(s) == null) {
                continue;
            }

            if (s.equals(ISecurityPropertiesHandler.IMEI_PROPERTY_ID)) {
                Pattern regex = Pattern.compile(IMEI_PROPERTY_REGEX);
                Matcher matcher = regex.matcher(securityPropertiesHandler.getWriteIdValueMap().get(s));
                if (!matcher.matches()) {
                    continue;
                }
            }
            // TODO:HACK END

            WriteEquipmentPropertyJob job =
                new WriteEquipmentPropertyJob(equipment, s + "="
                    + securityPropertiesHandler.getWriteIdValueMap().get(s));
            job.addJobChangeListener(this);
            incrementJobCounter();
            waitingJobs++;
            job.schedule();

        }

        while (waitingJobs != 0) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        BindPropertiesJob job = new BindPropertiesJob(equipment);
        job.schedule();

    }

    private void saveAsConfigurationRecords(Hashtable<String, EquipmentProperty> properties) {

        securityPropertiesHandler.setConfigurationRecords(properties);
    }

    private void createSectionContent(EditorXML.Section structure, Section section) {

        sectionComposite = toolkit.createComposite(section);
        GridLayout layout = new GridLayout(structure.getColumns(), true);
        layout.marginHeight = 0;
        layout.verticalSpacing = 0;
        sectionComposite.setLayout(layout);
        section.setClient(sectionComposite);
        HashMap<String, Integer> gdIds = new HashMap<String, Integer>();

        for (int i = 0; i < securityPropertiesHandler.getConfigurationRecords().size(); i++) {
            EditorXML.Section.Record recordStructure =
                structure.findMatchingRecord(securityPropertiesHandler.getConfigurationRecords().get(i).getValue(
                    PROPERTY_ID));
            if (recordStructure != null) {
                Integer gdId = gdIds.get(recordStructure.getMatch());
                if (gdId == null) {
                    gdId = 1;
                } else {
                    gdId = gdId + 1;
                }
                gdIds.put(recordStructure.getMatch(), gdId);
                IConfigurationRecord configRecord = securityPropertiesHandler.getConfigurationRecords().get(i);

                Composite controlComposite = new Composite(sectionComposite, SWT.NONE);
                GridData gridData = new GridData(SWT.FILL, SWT.FILL, false, false);
                gridData.verticalSpan = 2;
                controlComposite.setLayoutData(gridData);
                GridLayout gl = new GridLayout(2, false);
                gl.verticalSpacing = 0;
                gl.marginHeight = 0;
                controlComposite.setLayout(gl);

                EditorUIControl editorUIControl =
                    new EditorUIControl(toolkit, controlComposite, recordStructure, configRecord, gdId);
                securityPropertiesHandler.addReferenceToComponents(editorUIControl, recordStructure);
                uiControls.add(controlComposite);
            }
        }

        sectionComposite.layout();

        if (uiControls.size() != 0) {
            for (Composite control : uiControls) {
                if (control != null) {
                    for (Control child : control.getChildren()) {
                        if (child instanceof EditorUIControl) {

                            for (IValidator validator : ((EditorUIControl) child).getValidators()) {
                                validator.addValidatorListener(this);
                                validator.checkCurrentValue();
                                validatorList.add(validator);
                            }
                            ((EditorUIControl) child).addChangeListener(this);
                        }
                    }
                }
            }
        }

        section.layout();

    }

    private void setButtonEnabled(boolean enabled) {

        final boolean enableState = enabled;

        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                bindPropertiesButton.setEnabled(enableState);
                refreshButton.setEnabled(enableState);

            }
        });

    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.common.ui.IEditorUIChangeListener#uiControlChanged(com.stericsson.sdk.common.ui.EditorUIControl)
     *      {@inheritDoc}
     */
    public void uiControlChanged(EditorUIControl source) {

        securityPropertiesHandler.updateUIFromConfigurationRecord(source);

        for (IValidator validator : validatorList) {
            if (!validator.isValueOk()) {

                // TODO: HACK OTP IS NOT AVAILABLE
                // TODO: END HACK
                bindPropertiesButton.setEnabled(false);
                return;
            } else {
                bindPropertiesButton.setEnabled(true);
            }
        }

    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.common.ui.validators.IValidatorMessageListener#
     *      validatorMessage(com.stericsson.sdk.common.ui.validators.IValidator, java.lang.String)
     *      {@inheritDoc}
     */
    public void validatorMessage(IValidator src, String message) {
        IMessageManager localMM = form.getMessageManager();

        if (!src.isValueOk()) {
            localMM.addMessage(src, message, null, IMessage.INFORMATION, src.getValidatedObject());

        } else {
            localMM.removeMessage(src, src.getValidatedObject());

        }

    }

    /**
     * (non-Javadoc)
     * 
     * @see org.eclipse.core.runtime.jobs.IJobChangeListener#aboutToRun(org.eclipse.core.runtime.jobs.IJobChangeEvent)
     *      {@inheritDoc}
     */
    public void aboutToRun(IJobChangeEvent event) {
        // TODO Auto-generated method stub

    }

    /**
     * (non-Javadoc)
     * 
     * @see org.eclipse.core.runtime.jobs.IJobChangeListener#awake(org.eclipse.core.runtime.jobs.IJobChangeEvent)
     *      {@inheritDoc}
     */
    public void awake(IJobChangeEvent event) {
        // TODO Auto-generated method stub

    }

    /**
     * (non-Javadoc)
     * 
     * @see org.eclipse.core.runtime.jobs.IJobChangeListener#done(org.eclipse.core.runtime.jobs.IJobChangeEvent)
     *      {@inheritDoc}
     */
    public void done(IJobChangeEvent event) {
        waitingJobs--;

    }

    /**
     * (non-Javadoc)
     * 
     * @see org.eclipse.core.runtime.jobs.IJobChangeListener#running(org.eclipse.core.runtime.jobs.IJobChangeEvent)
     *      {@inheritDoc}
     */
    public void running(IJobChangeEvent event) {
        // TODO Auto-generated method stub

    }

    /**
     * (non-Javadoc)
     * 
     * @see org.eclipse.core.runtime.jobs.IJobChangeListener#scheduled(org.eclipse.core.runtime.jobs.IJobChangeEvent)
     *      {@inheritDoc}
     */
    public void scheduled(IJobChangeEvent event) {
        // TODO Auto-generated method stub

    }

    /**
     * (non-Javadoc)
     * 
     * @see org.eclipse.core.runtime.jobs.IJobChangeListener#sleeping(org.eclipse.core.runtime.jobs.IJobChangeEvent)
     *      {@inheritDoc}
     */
    public void sleeping(IJobChangeEvent event) {
        // TODO Auto-generated method stub

    }

}
