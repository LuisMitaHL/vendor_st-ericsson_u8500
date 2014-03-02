package com.stericsson.sdk.equipment.ui.editors;

import java.util.Locale;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.eclipse.ui.forms.widgets.TableWrapLayout;

import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipment;

/**
 * 
 * @author xolabju
 * 
 */
public class EquipmentEditorOverviewPage {

    private IEquipment equipment;

    private Composite pageComposite;

    private Text textPlatform;

    private Text textPort;

    private Text textProfile;

    private Text textNomadikID;

    private Text textASICVersion;

    private Text textSecureMode;

    private Text textPublicID;

    private Text textRootKeyHash;

    private Text textPublicROMCRC;

    private Text textSecureRomCRC;

    private Text textISSW;

    private Text textXLOADER;

    private Text textPWRMGT;

    private Section sectionGeneral;

    private Section sectionASICID;

    private Section sectionBootImageVersions;

    private Text textMEMINIT;

    /**
     * Constructor
     * 
     * @param e
     *            Equipment object
     * @param parent
     *            Parent composite object
     * @param toolkit
     *            Form toolkit object
     */
    public EquipmentEditorOverviewPage(IEquipment e, Composite parent, FormToolkit toolkit) {
        equipment = e;
        pageComposite = createPage(parent, toolkit);
    }

    /**
     * @return The page
     */
    public Composite getPageComposite() {
        return pageComposite;
    }

    /**
     * 
     * @param parent
     *            TBD
     * @param toolkit
     *            TBD
     * @return TBD
     */
    private Composite createPage(Composite parent, FormToolkit toolkit) {
        ScrolledForm scrolledForm = toolkit.createScrolledForm(parent);
        scrolledForm.getForm().setText("Mobile Equipment Overview");
        toolkit.decorateFormHeading(scrolledForm.getForm());

        TableWrapLayout layout = new TableWrapLayout();
        scrolledForm.getBody().setLayout(layout);

        sectionGeneral =
            toolkit.createSection(scrolledForm.getBody(), Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
        sectionGeneral.setText("General");

        Composite client = toolkit.createComposite(sectionGeneral);
        client.setLayout(new GridLayout(2, false));
        toolkit.createLabel(client, "Platform:");
        textPlatform = new Text(client, SWT.READ_ONLY);
        textPlatform.setText(getPropertyValue("platform").toUpperCase(Locale.getDefault()));

        toolkit.createLabel(client, "Port:");
        textPort = new Text(client, SWT.READ_ONLY);
        textPort.setText(equipment.getPort().getPortName());

        toolkit.createLabel(client, "Profile in use:");
        textProfile = new Text(client, SWT.READ_ONLY);
        textProfile.setText(equipment.getProfile().getAlias());

        sectionGeneral.setClient(client);

        sectionASICID =
            toolkit.createSection(scrolledForm.getBody(), Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
        sectionASICID.setText("ASIC ID");

        client = toolkit.createComposite(sectionASICID);
        client.setLayout(new GridLayout(2, false));

        toolkit.createLabel(client, "Nomadik ID:");
        textNomadikID = new Text(client, SWT.READ_ONLY);
        textNomadikID.setText(getPropertyValue("Nomadik ID"));

        toolkit.createLabel(client, "ASIC version:");
        textASICVersion = new Text(client, SWT.READ_ONLY);
        textASICVersion.setText(getPropertyValue("ASIC Version"));

        toolkit.createLabel(client, "Secure mode:");
        textSecureMode = new Text(client, SWT.READ_ONLY);
        textSecureMode.setText(getPropertyValue("Secure Mode"));

        toolkit.createLabel(client, "Public ID:");
        textPublicID = new Text(client, SWT.READ_ONLY);
        textPublicID.setText(getPropertyValue("Public ID"));

        toolkit.createLabel(client, "Root Key Hash:");
        textRootKeyHash = new Text(client, SWT.READ_ONLY);
        textRootKeyHash.setText(getPropertyValue("Root Key Hash"));

        toolkit.createLabel(client, "Public ROM CRC:");
        textPublicROMCRC = new Text(client, SWT.READ_ONLY);
        textPublicROMCRC.setText(getPropertyValue("Public ROM CRC"));

        toolkit.createLabel(client, "Secure ROM CRC:");
        textSecureRomCRC = new Text(client, SWT.READ_ONLY);
        textSecureRomCRC.setText(getPropertyValue("Secure ROM CRC"));

        sectionASICID.setClient(client);

        sectionBootImageVersions =
            toolkit.createSection(scrolledForm.getBody(), Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
        sectionBootImageVersions.setText("Boot Image Versions");

        client = toolkit.createComposite(sectionBootImageVersions);
        client.setLayout(new GridLayout(2, false));

        toolkit.createLabel(client, "ISSW:");
        textISSW = new Text(client, SWT.READ_ONLY);//
        textISSW.setText(getPropertyValue("ISSW Version"));

        toolkit.createLabel(client, "X-LOADER:");
        textXLOADER = new Text(client, SWT.READ_ONLY);
        textXLOADER.setText(getPropertyValue("X-LOADER Version"));

        toolkit.createLabel(client, "PWR_MGT:");
        textPWRMGT = new Text(client, SWT.READ_ONLY);
        textPWRMGT.setText(getPropertyValue("PWR_MGT Version"));

        toolkit.createLabel(client, "MEM_INIT:");
        textMEMINIT = new Text(client, SWT.READ_ONLY);
        textMEMINIT.setText(getPropertyValue("MEM_INIT Version"));

        sectionBootImageVersions.setClient(client);

        // GUI-Dancer properties
        parent.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_L1ParentPageComposite_Composite");
        scrolledForm.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_PageComposite_ScrolledForm");

        // general
        sectionGeneral.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_GeneralSection_Section");
        textPlatform.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_PlatformText_Text");
        textPort.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_PortText_Text");
        textProfile.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_ProfileText_Text");

        // ASIC ID
        sectionASICID.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_ASICIDSection_Section");
        textNomadikID.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_NomadikIDText_Text");
        textASICVersion.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_ASICVersionText_Text");
        textSecureMode.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_SecureModeText_Text");
        textPublicID.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_PublicIDText_Text");
        textRootKeyHash.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_RootKeyHashText_Text");
        textPublicROMCRC.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_PublicROMCRCText_Text");
        textSecureRomCRC.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_SecureROMCRCText_Text");

        // Boot Image Versions
        sectionBootImageVersions
            .setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_BootImageVersionsSection_Section");
        textISSW.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_ISSWText_Text");
        textXLOADER.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_XLOADERText_Text");
        textPWRMGT.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_PWRMGTText_Text");
        textMEMINIT.setData("GD_COMP_NAME", "EquipmentEditorOverviewPage_PWRMGTText_Text");

        return scrolledForm;
    }

    private String getPropertyValue(String propertyName) {
        String result = "Not available";

        EquipmentProperty property = equipment.getProperties().get(propertyName);
        if (property != null) {
            result = property.getPropertyValue();
        }

        return result;
    }

    void updateValues() {
        if (equipment != null) {
            Runnable r = new Runnable() {
                public void run() {
                    // general
                    textPlatform.setText(equipment.getProperties().get("platform").getPropertyValue().toUpperCase(
                        Locale.getDefault()));
                    textPort.setText(equipment.getPort().getPortName());
                    textProfile.setText(equipment.getProfile().getAlias());
                    sectionGeneral.pack(true);

                    // ASIC ID
                    textNomadikID.setText(equipment.getProperties().get("Nomadik ID").getPropertyValue());
                    textASICVersion.setText(equipment.getProperties().get("ASIC Version").getPropertyValue());
                    textSecureMode.setText(equipment.getProperties().get("Secure Mode").getPropertyValue());
                    textPublicID.setText(equipment.getProperties().get("Public ID").getPropertyValue());
                    textRootKeyHash.setText(equipment.getProperties().get("Root Key Hash").getPropertyValue());
                    textPublicROMCRC.setText(equipment.getProperties().get("Public ROM CRC").getPropertyValue());
                    textSecureRomCRC.setText(equipment.getProperties().get("Secure ROM CRC").getPropertyValue());
                    sectionASICID.pack(true);

                    // Boot Image Versions
                    textISSW.setText(equipment.getProperties().get("ISSW Version").getPropertyValue());
                    textXLOADER.setText(equipment.getProperties().get("X-LOADER Version").getPropertyValue());
                    textPWRMGT.setText(equipment.getProperties().get("PWR_MGT Version").getPropertyValue());
                    sectionBootImageVersions.pack(true);
                }
            };
            Display.getDefault().asyncExec(r);
        }
    }

    void setEquipment(IEquipment e) {
        if (e != equipment) {
            equipment = e;
        }
        updateValues();
    }
}
