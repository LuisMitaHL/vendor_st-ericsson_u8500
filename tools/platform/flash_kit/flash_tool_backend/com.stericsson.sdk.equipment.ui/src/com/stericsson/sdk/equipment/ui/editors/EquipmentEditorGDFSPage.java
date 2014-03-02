package com.stericsson.sdk.equipment.ui.editors;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.XMLMemento;
import org.eclipse.ui.forms.IMessage;
import org.eclipse.ui.forms.IMessageManager;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.OutputPathTFValidator;
import com.stericsson.sdk.common.ui.validators.PathTFValidator;
import com.stericsson.sdk.common.ui.validators.RegexTFValidator;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.Utils;
import com.stericsson.sdk.equipment.ui.jobs.ReadGlobalDataSetJob;
import com.stericsson.sdk.equipment.ui.jobs.ReadGlobalDataUnitJob;
import com.stericsson.sdk.equipment.ui.jobs.WriteGlobalDataSetJob;
import com.stericsson.sdk.equipment.ui.jobs.WriteGlobalDataUnitJob;

/**
 * Draws and handles events on GDFS parameters editing page. Design of this class exists in EA.
 * Implementation of WP256073, B-01109.
 * 
 * @author xadazim
 */
public class EquipmentEditorGDFSPage implements IValidatorMessageListener {

    private static final String DATA_SET_MANIPULATION_TITLE = "Data set manipulation";

    private static final String DATA_UNIT_MANIPULATION_TITLE = "Data unit manipulation";

    private static final String AREA_LABEL_TEXT = "Area:";

    private static final String CUSTOM_AREA_LABEL_TEXT = "Custom area:";

    private static final String GDFS_AREA_TEXT = "GDFS Area";

    private static final String TRIM_AREA_TEXT = "Trim Area";

    private static final String CUSTOM_AREA_TEXT = "Custom Area";

    private static final String UNIT_LABEL_TEXT = "Unit (hex):";

    private static final String DATA_LABEL_TEXT = "Data (hex):";

    private static final String READ_BUTTON_TEXT = "Read from ME";

    private static final String WRITE_BUTTON_TEXT = "Write to ME";

    private static final String INPUT_FILE_LABEL_TEXT = "Input file:";

    private static final String BROWSE_BUTTON_TEXT = "Browse...";

    private static final String OUTPUT_FILE_LABEL_TEXT = "Output file:";

    private static final int INPUT_OUTPUT_TEXT_WIDTH_HINT = 300;

    private static final String EQUIPMENT_EDITOR_SETTINGS_FILE_NAME = "EquipmentEditorGDFSPage.xml";

    private static final int GDFS_AREA_INDEX = 0;

    private static final int TRIM_AREA_INDEX = 1;

    private static final int CUSTOM_AREA_INDEX = 2;

    private static final String GDFS_MAGICAL_KEYWORD = "gdfs";

    private static final String TRIM_MAGICAL_KEYOWRD = "ta";

    // tags for memento (settings storage)

    private static final String TAG_EQUIPMENT_EDITOR_GDFS_PAGE = EquipmentEditorGDFSPage.class.getName();

    private static final String TAG_UNIT_AREA_COMBO = "UnitAreaCombo";

    private static final String TAG_UNIT = "Unit";

    private static final String TAG_UNIT_CUSTOM_AREA = "UnitCustomArea";

    private static final String TAG_UNIT_ID = "UnitId";

    private static final String TAG_UNIT_DATA = "UnitData";

    private static final String TAG_SET = "Set";

    private static final String TAG_SET_AREA_COMBO = "SetAreaCombo";

    private static final String TAG_SET_CUSTOM_AREA = "SetCustomArea";

    private static final String TAG_SET_INPUT_FILE = "SetInputFile";

    private static final String TAG_SET_OUTPUT_FILE = "SetOutputFile";

    private final IEquipment equipment;

    private final Composite pageComposite;

    private final Logger logger = Logger.getLogger(EquipmentEditorGDFSPage.class.getName());

    // unit controls

    private Text unitCustomArea;

    private Text unitText;

    private StyledText unitDataText;

    private CCombo unitAreaCombo;

    private Label unitCustomAreaLabel;

    private Button unitReadDataButton;

    private Button unitWriteDataButton;

    // set controls

    private Text setOutputFileText;

    private Text setInputFileText;

    private Button setWriteDataButton;

    private Button setReadDataButton;

    private Button setInputBrowse;

    private Button setOutputBrowse;

    private Label setReadStatusLabel;

    private Label setWriteStatusLabel;

    private CCombo setAreaCombo;

    private Label setCustomAreaLabel;

    private Text setCustomArea;

    private final Composite parent;

    private String unitMeData = new String();

    private ScrolledForm scrolledForm;

    private PathTFValidator inputFileValidator;

    private OutputPathTFValidator outputFileValidator;

    private RegexTFValidator unitTextValidator;

    private final Color unitDataColor;

    /**
     * Constructs equipement GDFS editing page.
     * 
     * @param pEquipment
     *            equipment of which GDFS parameters will be edited
     * @param pParent
     *            parent composite
     * @param pToolkit
     *            toolkit to use
     */
    EquipmentEditorGDFSPage(IEquipment pEquipment, Composite pParent, FormToolkit pToolkit) {
        equipment = pEquipment;
        pageComposite = createPage(pParent, pToolkit);
        parent = pParent;
        unitDataColor = new Color(null, 255, 0, 0);
    }

    /**
     * Standard dispose method
     */
    public void dispose() {
        unitDataColor.dispose();
    }

    private Composite createPage(Composite pParent, FormToolkit pToolkit) {
        scrolledForm = pToolkit.createScrolledForm(pParent);
        scrolledForm.getForm().setText("GDFS and Trim Area");
        FillLayout layout = new FillLayout(SWT.VERTICAL);
        scrolledForm.getBody().setLayout(layout);
        pToolkit.decorateFormHeading(scrolledForm.getForm());

        unitCreateSectionComponents(scrolledForm.getBody(), pToolkit);
        unitCreateSectionBehavior();

        setCreateSectionComponents(scrolledForm.getBody(), pToolkit);
        setCreateSectionBehavior();

        scrolledForm.pack();

        restoreState();

        unitUpdateSectionEnablement();
        setUpdateSectionEnablement();

        return scrolledForm;
    }

    private File getSettingsFile() {
        return Activator.getDefault().getStateLocation().append(EQUIPMENT_EDITOR_SETTINGS_FILE_NAME).toFile();
    }

    void saveState() {
        XMLMemento memento = XMLMemento.createWriteRoot(TAG_EQUIPMENT_EDITOR_GDFS_PAGE);
        IMemento unitState = memento.createChild(TAG_UNIT);
        unitState.putInteger(TAG_UNIT_AREA_COMBO, unitAreaCombo.getSelectionIndex());
        unitState.putString(TAG_UNIT_CUSTOM_AREA, unitCustomArea.getText());
        unitState.putString(TAG_UNIT_ID, unitText.getText());
        unitState.putString(TAG_UNIT_DATA, unitDataText.getText());

        IMemento setState = memento.createChild(TAG_SET);
        setState.putInteger(TAG_SET_AREA_COMBO, setAreaCombo.getSelectionIndex());
        setState.putString(TAG_SET_CUSTOM_AREA, setCustomArea.getText());
        setState.putString(TAG_SET_INPUT_FILE, setInputFileText.getText());
        setState.putString(TAG_SET_OUTPUT_FILE, setOutputFileText.getText());

        FileWriter writer = null;
        try {
            writer = new FileWriter(getSettingsFile());
            memento.save(writer);
        } catch (IOException e) {
            logger.error("Can not store Equipment Editor GDFS page settings", e);
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e) {
                    logger.error("Can not close stream", e);
                }
            }
        }
    }

    private void restoreState() {
        XMLMemento memento;
        FileReader reader = null;
        try {
            reader = new FileReader(getSettingsFile());
            memento = XMLMemento.createReadRoot(reader);
        } catch (Exception e) {
            logger.error("Can not restore Equipment Editor GDFS page settings", e);
            return;
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    logger.error("Can not close stream", e);
                }
            }
        }

        unitRestoreState(memento);
        setRestoreState(memento);
    }

    private String getAreaName(int pSelectionIndex) {
        switch (pSelectionIndex) {
            case GDFS_AREA_INDEX:
                return GDFS_MAGICAL_KEYWORD;
            case TRIM_AREA_INDEX:
                return TRIM_MAGICAL_KEYOWRD;
            default:
                throw new IllegalArgumentException("Unknown selection index");
        }
    }

    // -----------------------------------------------
    // unit
    // -----------------------------------------------
    private void unitCreateSectionComponents(Composite pParent, FormToolkit pToolkit) {
        Section section = pToolkit.createSection(pParent, Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
        section.setText(DATA_UNIT_MANIPULATION_TITLE);

        Composite composite = pToolkit.createComposite(section);
        composite.setLayout(new GridLayout(3, false));

        // row 1
        pToolkit.createLabel(composite, AREA_LABEL_TEXT);
        unitAreaCombo = new CCombo(composite, SWT.FLAT);
        unitAreaCombo.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_UnitArea_Combo");
        unitAreaCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        unitAreaCombo.add(GDFS_AREA_TEXT);
        unitAreaCombo.add(TRIM_AREA_TEXT);
        unitAreaCombo.add(CUSTOM_AREA_TEXT);
        unitAreaCombo.select(0);
        unitAreaCombo.setEditable(false);
        pToolkit.adapt(unitAreaCombo);
        Composite customAreaComposite = pToolkit.createComposite(composite);
        GridLayout customAreaCompositeLayout = new GridLayout(2, false);
        customAreaComposite.setLayout(customAreaCompositeLayout);
        customAreaComposite.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        unitCustomAreaLabel = pToolkit.createLabel(customAreaComposite, CUSTOM_AREA_LABEL_TEXT);
        unitCustomArea = pToolkit.createText(customAreaComposite, "", SWT.FLAT);
        unitCustomArea.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_UnitCustomArea_Text");
        GridData unitCustomAreaLayoutData = new GridData(SWT.LEFT, SWT.CENTER, true, false);
        unitCustomAreaLayoutData.widthHint = 150;
        unitCustomArea.setLayoutData(unitCustomAreaLayoutData);
        // unitCustomArea.setData(FormToolkit.KEY_DRAW_BORDER, FormToolkit.TEXT_BORDER);
        pToolkit.paintBordersFor(customAreaComposite);

        // row 2
        pToolkit.createLabel(composite, UNIT_LABEL_TEXT);
        String unit = "";
        unitText = pToolkit.createText(composite, unit);
        unitText.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_UnitId_Text");
        unitText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        pToolkit.createLabel(composite, ""); // place holder

        // row 3
        Label dataLabel = pToolkit.createLabel(composite, DATA_LABEL_TEXT);
        GridData dataLabelGridData = new GridData(SWT.LEFT, SWT.TOP, false, false);
        dataLabelGridData.verticalIndent = 3;
        dataLabel.setLayoutData(dataLabelGridData);
        GridData gridData = new GridData();
        gridData.horizontalAlignment = GridData.FILL;
        gridData.horizontalSpan = 2;
        gridData.heightHint = 200;
        gridData.widthHint = 400;
        gridData.verticalIndent = 1; // somehow the distance to the component above is a bit too
        // small
        unitDataText = new StyledText(composite, SWT.NONE);
        unitDataText.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_UnitData_StyledText");
        // magic that makes the border around it
        unitDataText.setData(FormToolkit.KEY_DRAW_BORDER, FormToolkit.TEXT_BORDER);
        unitDataText.setWordWrap(true);
        unitDataText.setLayoutData(gridData);
        unitDataText.setLeftMargin(5); // there is no space between text and border by default
        unitDataText.setFont(new Font(null, "Courier", 9, SWT.NORMAL));
        pToolkit.adapt(unitDataText);

        // row 4
        pToolkit.createLabel(composite, ""); // place holder
        unitReadDataButton = pToolkit.createButton(composite, READ_BUTTON_TEXT, SWT.NONE);
        unitReadDataButton.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_UnitRead_Button");
        GridData readButtonLayoutData = new GridData(SWT.LEFT, SWT.CENTER, false, false);
        unitReadDataButton.setLayoutData(readButtonLayoutData);
        unitWriteDataButton = pToolkit.createButton(composite, WRITE_BUTTON_TEXT, SWT.NONE);
        unitWriteDataButton.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_UnitWrite_Button");
        GridData writeButtonLayoutData = new GridData(SWT.RIGHT, SWT.CENTER, false, false);
        unitWriteDataButton.setLayoutData(writeButtonLayoutData);

        // it looks better if read and write buttons are of the same size
        int readWidth = unitReadDataButton.computeSize(SWT.DEFAULT, SWT.DEFAULT).x;
        int writeWidth = unitWriteDataButton.computeSize(SWT.DEFAULT, SWT.DEFAULT).x;
        int bigger = readWidth > writeWidth ? readWidth : writeWidth;
        readButtonLayoutData.widthHint = bigger;
        writeButtonLayoutData.widthHint = bigger;

        // row 6 (dummy)
        pToolkit.createLabel(composite, ""); // place holder

        pToolkit.paintBordersFor(composite);

        composite.pack(true);

        section.setClient(composite);
    }

    private void unitCreateSectionBehavior() {
        unitTextValidator = new RegexTFValidator("", unitText, "^[\\s]*[0-9a-fA-F]{0,8}[\\s]*$");
        unitTextValidator.switchOn();
        unitTextValidator.addValidatorListener(this);

        unitAreaCombo.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                unitUpdateSectionEnablement();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        unitText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                unitMeData = "";
                unitDataFormat(false);
                unitUpdateSectionEnablement();
            }
        });

        unitReadDataButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                String area =
                    unitAreaCombo.getSelectionIndex() == CUSTOM_AREA_INDEX ? unitCustomArea.getText()
                        : getAreaName(unitAreaCombo.getSelectionIndex());
                Job job = new ReadGlobalDataUnitJob("0x" + unitText.getText(), equipment, area);
                job.addJobChangeListener(new IJobChangeListener() {
                    public void sleeping(IJobChangeEvent pEvent) {
                    }

                    public void scheduled(IJobChangeEvent pEvent) {
                    }

                    public void running(IJobChangeEvent pEvent) {
                    }

                    public void done(IJobChangeEvent pEvent) {
                        if (pEvent.getResult().isOK()) {
                            unitDataRead(pEvent.getResult().getMessage(), true);
                        } else {
                            unitDataRead("", false);
                            Utils.logAndShowError("Reading of unit has failed", pEvent.getResult().getMessage()
                                + "\nError code:" + pEvent.getResult().getCode(), null);
                        }
                    }

                    public void awake(IJobChangeEvent pEvent) {
                    }

                    public void aboutToRun(IJobChangeEvent pEvent) {
                    }
                });
                job.schedule();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        unitWriteDataButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                final String area =
                    unitAreaCombo.getSelectionIndex() == CUSTOM_AREA_INDEX ? unitCustomArea.getText()
                        : getAreaName(unitAreaCombo.getSelectionIndex());
                Job job =
                    new WriteGlobalDataUnitJob("0x" + unitText.getText(), equipment, area,
                        unitRemoveWhitespaces(unitDataText.getText()));
                job.addJobChangeListener(new IJobChangeListener() {
                    public void sleeping(IJobChangeEvent pEvent) {
                    }

                    public void scheduled(IJobChangeEvent pEvent) {
                    }

                    public void running(IJobChangeEvent pEvent) {
                    }

                    public void done(IJobChangeEvent pEvent) {
                        if (pEvent.getResult().isOK()) {
                            Display.getDefault().asyncExec(new Runnable() {
                                public void run() {
                                    unitWriteDataOK(area, unitDataText.getText());
                                }
                            });

                        } else {
                            unitDataRead("", false);
                            Utils.logAndShowError("Writing of unit has failed", pEvent.getResult().getMessage()
                                + "\nError code:" + pEvent.getResult().getCode(), null);
                        }
                    }

                    public void awake(IJobChangeEvent pEvent) {
                    }

                    public void aboutToRun(IJobChangeEvent pEvent) {
                    }
                });
                job.schedule();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        unitDataText.addKeyListener(new KeyListener() {
            public void keyReleased(KeyEvent pE) {
            }

            public void keyPressed(KeyEvent pE) {
                if ((pE.keyCode != SWT.ARROW_LEFT) && (pE.keyCode != SWT.ARROW_RIGHT) && (pE.keyCode != SWT.HOME)
                    && (pE.keyCode != SWT.END)) {
                    unitDataFormat(false);
                }
            }
        });

        unitDataText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                // unitUpdateSectionState();
                unitWriteDataButton.setEnabled(unitDataText.getText().trim().length() > 0);
            }
        });

        unitCustomArea.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                unitUpdateSectionEnablement();
            }
        });
    }

    private void unitWriteDataOK(String pAreaPath, final String pText) {
        Job job = new ReadGlobalDataUnitJob("0x" + unitText.getText(), equipment, pAreaPath);
        job.addJobChangeListener(new IJobChangeListener() {
            public void sleeping(IJobChangeEvent pEvent) {
            }

            public void scheduled(IJobChangeEvent pEvent) {
            }

            public void running(IJobChangeEvent pEvent) {
            }

            public void done(IJobChangeEvent pEvent) {
                if (pEvent.getResult().isOK()) {
                    if (unitRemoveWhitespaces(pText).contentEquals(pEvent.getResult().getMessage().toUpperCase())) {
                        unitDataRead(pText, true);
                    } else {
                        unitDataRead(pText, false);
                        Utils.logAndShowError("Writing of unit has failed", pEvent.getResult().getMessage()
                            .toUpperCase()
                            + "\nError code:" + pEvent.getResult().getCode(), null);
                    }
                } else {
                    unitDataRead("", false);
                    Utils.logAndShowError("Can not verify if data has been written correctly", pEvent.getResult()
                        .getMessage().toUpperCase()
                        + "\nError code:" + pEvent.getResult().getCode(), null);
                }
            }

            public void awake(IJobChangeEvent pEvent) {
            }

            public void aboutToRun(IJobChangeEvent pEvent) {
            }
        });
        job.schedule();
    }

    private String unitRemoveWhitespaces(String text) {
        String result = text.replaceAll("[ \n]", "");
        return result;
    }

    private void unitRestoreState(IMemento memento) {
        IMemento unitState = memento.getChild(TAG_UNIT);
        if (unitState != null) {
            Integer selection = unitState.getInteger(TAG_UNIT_AREA_COMBO);
            if (selection != null && selection >= 0 && selection < unitAreaCombo.getItemCount()) {
                unitAreaCombo.select(selection);
            }
            String customArea = unitState.getString(TAG_UNIT_CUSTOM_AREA);
            if (customArea != null) {
                unitCustomArea.setText(customArea);
            }
            String id = unitState.getString(TAG_UNIT_ID);
            if (id != null) {
                unitText.setText(id);
            }
            String data = unitState.getString(TAG_UNIT_DATA);
            if (data != null) {
                unitDataText.setText(data);
                unitDataFormat(false);
            }
        }
    }

    /**
     * Enables/disables appropriate controls in the unit section based on current editor settings.
     */
    private void unitUpdateSectionEnablement() {
        boolean enabled = unitAreaCombo.getSelectionIndex() == 2;
        unitCustomArea.setEnabled(enabled);
        unitCustomAreaLabel.setEnabled(enabled);

        enabled = unitTextValidator.isValueOk();

        enabled &=
            (unitAreaCombo.getSelectionIndex() != CUSTOM_AREA_INDEX)
                || (unitCustomArea.getText().trim().matches("^/[a-zA-Z0-9_/[-]]+$"));

        unitReadDataButton.setEnabled(enabled);

        enabled &= unitDataText.getText().trim().length() > 0;
        unitWriteDataButton.setEnabled(enabled);
    }

    /**
     * Called when unit data were read, updates unit data text window.
     * 
     * @param pMessage
     *            data that arrived (in hex as string)
     * @param pCorrect
     *            it's true if data read corrected
     */
    private void unitDataRead(final String pMessage, final boolean pCorrect) {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                unitDataText.setText(pMessage);
                unitDataFormat(pCorrect);
                if (pCorrect) {
                    unitMeData = unitDataText.getText();
                }
            }
        });
    }

    private void unitDataFormat(boolean fromME) {
        String current = unitDataText.getText().trim().toUpperCase();
        // how many 0-9 A-F characters are before the cursor
        int charsBeforeCursor = unitCalculateCharsBeforeCursor(current);

        StringBuilder formatted = new StringBuilder();
        int newCursorPos = 0;
        int j = 0;
        int i;
        for (i = 0; i < current.length(); i++) {
            if (((current.charAt(i) > '9') || (current.charAt(i) < '0'))
                && ((current.charAt(i) > 'F') || (current.charAt(i) < 'A'))) {
                continue;
            }
            formatted.append(current.charAt(i));
            if ((j + 1) % 2 == 0) {
                formatted.append(" ");
            }
            if ((j + 1) % 16 == 0) {
                formatted.append("  ");
            }
            if ((j + 1) % 32 == 0) {
                formatted.append("\n");
            }
            j++;

            if (j == charsBeforeCursor) {
                newCursorPos = formatted.length();
            }
        }

        unitDataText.setText(formatted.toString());
        unitDataText.setCaretOffset(newCursorPos);

        if (!fromME) {
            Set<Integer> charsChanged = unitGetCharsChanged(unitMeData, formatted.toString());
            for (Integer idx : charsChanged) {
                unitDataText.setStyleRange(new StyleRange(idx, 1, unitDataColor, unitDataText.getBackground()));
            }
        }
    }

    private Set<Integer> unitGetCharsChanged(String oldData, String newData) {
        Set<Integer> result = new HashSet<Integer>();
        for (int i = 0; i < newData.length(); i++) {
            if (oldData.length() > i) {
                if (newData.charAt(i) != oldData.charAt(i)) {
                    result.add(i);
                }
            } else {
                result.add(i);
            }
        }
        return result;
    }

    private int unitCalculateCharsBeforeCursor(String current) {
        int currentCursorPos = unitDataText.getCaretOffset();
        int charsBeforeCursor = 0;
        int i;
        for (i = 0; i < currentCursorPos; i++) {
            if (((current.charAt(i) <= '9') && (current.charAt(i) >= '0'))
                || ((current.charAt(i) <= 'F') && (current.charAt(i) >= 'A'))) {
                charsBeforeCursor++;
            }
        }

        return charsBeforeCursor;
    }

    // -----------------------------------------------
    // set
    // -----------------------------------------------

    private void setCreateSectionComponents(Composite pParent, FormToolkit pToolkit) {
        Section section = pToolkit.createSection(pParent, Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
        section.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_Set_Section");
        section.setText(DATA_SET_MANIPULATION_TITLE);

        Composite composite = pToolkit.createComposite(section);
        GridLayout gridLayout = new GridLayout(4, false);
        composite.setLayout(gridLayout);

        GridData gridData = new GridData();
        gridData = new GridData(SWT.FILL, SWT.TOP, true, true);
        composite.setLayoutData(gridData);

        // row 1
        pToolkit.createLabel(composite, AREA_LABEL_TEXT);
        setAreaCombo = new CCombo(composite, SWT.FLAT);
        setAreaCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        setAreaCombo.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetArea_Combo");
        setAreaCombo.add(GDFS_AREA_TEXT);
        setAreaCombo.add(TRIM_AREA_TEXT);
        setAreaCombo.add(CUSTOM_AREA_TEXT);
        setAreaCombo.select(0);
        setAreaCombo.setEditable(false);
        pToolkit.adapt(setAreaCombo);

        Composite customAreaComposite = pToolkit.createComposite(composite);
        GridLayout customAreaCompositeLayout = new GridLayout(2, false);
        customAreaComposite.setLayout(customAreaCompositeLayout);
        GridData customAreaCompositeLayoutData = new GridData(SWT.FILL, SWT.CENTER, false, false);
        customAreaCompositeLayoutData.horizontalSpan = 2;
        customAreaComposite.setLayoutData(customAreaCompositeLayoutData);
        setCustomAreaLabel = pToolkit.createLabel(customAreaComposite, CUSTOM_AREA_LABEL_TEXT);
        setCustomArea = pToolkit.createText(customAreaComposite, "", SWT.FLAT);
        setCustomArea.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetCustomArea_Text");
        GridData setCustomAreaLayoutData = new GridData(SWT.LEFT, SWT.CENTER, true, false);
        setCustomAreaLayoutData.widthHint = 150;
        setCustomArea.setLayoutData(setCustomAreaLayoutData);
        pToolkit.paintBordersFor(customAreaComposite);

        // row 2
        pToolkit.createLabel(composite, INPUT_FILE_LABEL_TEXT);
        String inputFile = "";
        setInputFileText = pToolkit.createText(composite, inputFile);
        setInputFileText.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetInputFile_Text");
        GridData inputFileTextGridData = new GridData();// SWT.FILL, SWT.CENTER, false, false);
        inputFileTextGridData.widthHint = INPUT_OUTPUT_TEXT_WIDTH_HINT;
        inputFileTextGridData.horizontalSpan = 2;
        setInputFileText.setLayoutData(inputFileTextGridData);
        setInputBrowse = pToolkit.createButton(composite, BROWSE_BUTTON_TEXT, SWT.NONE);
        setInputBrowse.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetInputBrowse_Button");

        // row 3
        pToolkit.createLabel(composite, ""); // place holder
        setWriteDataButton = pToolkit.createButton(composite, WRITE_BUTTON_TEXT, SWT.NONE);
        setWriteDataButton.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetWrite_Button");
        setWriteDataButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        setWriteStatusLabel = pToolkit.createLabel(composite, "");
        setWriteStatusLabel.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetWriteStatus_Label");
        setWriteStatusLabel.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
        pToolkit.createLabel(composite, ""); // place holder

        // row 4
        pToolkit.createLabel(composite, OUTPUT_FILE_LABEL_TEXT);
        String outputFileName = "";
        setOutputFileText = pToolkit.createText(composite, outputFileName);
        setOutputFileText.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetOutputFile_Text");
        GridData outputFileTextGridData = new GridData();// SWT.FILL, SWT.CENTER, false, false);
        outputFileTextGridData.widthHint = INPUT_OUTPUT_TEXT_WIDTH_HINT;
        outputFileTextGridData.horizontalSpan = 2;
        setOutputFileText.setLayoutData(outputFileTextGridData);
        setOutputBrowse = pToolkit.createButton(composite, BROWSE_BUTTON_TEXT, SWT.NONE);
        setOutputBrowse.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetOutputBrowse_Button");

        // row 5
        pToolkit.createLabel(composite, ""); // place holder
        setReadDataButton = pToolkit.createButton(composite, READ_BUTTON_TEXT, SWT.NONE);
        setReadDataButton.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetRead_Button");
        setReadDataButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        setReadStatusLabel = pToolkit.createLabel(composite, "");
        setReadStatusLabel.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
        setReadStatusLabel.setData(IUIConstants.GD_COMP_NAME, "EquipmentEditorGDFSPage_SetReadStatus_Label");
        pToolkit.createLabel(composite, ""); // place holder

        pToolkit.paintBordersFor(composite);

        section.setClient(composite);
    }

    private void setCreateSectionBehavior() {
        inputFileValidator = new PathTFValidator(setInputFileText.getMessage(), setInputFileText, false);
        inputFileValidator.switchOn();
        inputFileValidator.addValidatorListener(this);
        outputFileValidator = new OutputPathTFValidator(setOutputFileText.getMessage(), setOutputFileText);
        outputFileValidator.switchOn();
        outputFileValidator.addValidatorListener(this);

        setInputFileText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                setUpdateSectionEnablement();
            }
        });

        setOutputFileText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                setUpdateSectionEnablement();
            }
        });

        setInputBrowse.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                setOnInputFileBrowse();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        setOutputBrowse.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                setOnOutputFileBrowse();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        setReadDataButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                String area =
                    setAreaCombo.getSelectionIndex() == CUSTOM_AREA_INDEX ? setCustomArea.getText()
                        : getAreaName(setAreaCombo.getSelectionIndex());
                if (new File(setOutputFileText.getText()).exists()) {
                    MessageBox mb = new MessageBox(parent.getShell(), SWT.YES | SWT.NO);
                    mb.setMessage("File " + setOutputFileText.getText().trim()
                        + " already exists! Do you want to overwrite it?");
                    if (mb.open() != SWT.YES) {
                        return;
                    }
                }
                Job job = new ReadGlobalDataSetJob(setOutputFileText.getText(), equipment, area);
                job.addJobChangeListener(new SetJobStatusChangeListener(setReadStatusLabel));
                job.schedule();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        setWriteDataButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                String area =
                    setAreaCombo.getSelectionIndex() == CUSTOM_AREA_INDEX ? setCustomArea.getText()
                        : getAreaName(setAreaCombo.getSelectionIndex());
                Job job = new WriteGlobalDataSetJob(setInputFileText.getText(), equipment, area);
                job.addJobChangeListener(new SetJobStatusChangeListener(setWriteStatusLabel));
                job.schedule();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        setAreaCombo.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                setUpdateSectionEnablement();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        setCustomArea.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                setUpdateSectionEnablement();
            }
        });
    }

    private void setRestoreState(IMemento memento) {
        IMemento setState = memento.getChild(TAG_SET);
        if (setState != null) {
            Integer selection = setState.getInteger(TAG_SET_AREA_COMBO);
            if (selection != null && selection >= 0 && selection < setAreaCombo.getItemCount()) {
                setAreaCombo.select(selection);
            }
            String customArea = setState.getString(TAG_SET_CUSTOM_AREA);
            if (customArea != null) {
                setCustomArea.setText(customArea);
            }
            String inputFile = setState.getString(TAG_SET_INPUT_FILE);
            if (inputFile != null) {
                setInputFileText.setText(inputFile);
            }
            String outputFile = setState.getString(TAG_SET_OUTPUT_FILE);
            if (outputFile != null) {
                setOutputFileText.setText(outputFile);
            }
        }
    }

    private void setUpdateSectionEnablement() {
        boolean enabled = setAreaCombo.getSelectionIndex() == CUSTOM_AREA_INDEX;
        setCustomArea.setEnabled(enabled);
        setCustomAreaLabel.setEnabled(enabled);

        boolean validArea =
            (setAreaCombo.getSelectionIndex() != CUSTOM_AREA_INDEX)
                || (setCustomArea.getText().trim().matches("^/[a-zA-Z0-9_/[-]]+$"));

        setWriteDataButton.setEnabled(inputFileValidator.isValueOk() && validArea);
        setReadDataButton.setEnabled(outputFileValidator.isValueOk() && validArea);
    }

    private void setOnInputFileBrowse() {
        FileDialog dialog = new FileDialog(Display.getDefault().getActiveShell(), SWT.OPEN);
        dialog.setFilterExtensions(new String[] {
            "*.gdf", "*.bin"});
        dialog.setText("Select input file with GDFS/TA data set");
        String fileName = setInputFileText.getText();
        if (new File(fileName).isFile()) {
            dialog.setFileName(fileName);
        }
        String newFileName = dialog.open();
        if (newFileName != null) {
            setInputFileText.setText(newFileName);
        }
    }

    private void setOnOutputFileBrowse() {
        FileDialog dialog = new FileDialog(Display.getDefault().getActiveShell(), SWT.SAVE);
        dialog.setFilterExtensions(new String[] {
            "*.gdf", "*.bin"});
        dialog.setText("Select a file to store GDFS/TA data set");
        String fileName = setOutputFileText.getText();
        if (setVerifyOutputFile(fileName)) {
            dialog.setFileName(fileName);
        }
        String newFileName = dialog.open();
        if (newFileName != null) {
            setOutputFileText.setText(newFileName);
        }
    }

    private boolean setVerifyOutputFile(String fileName) {
        String parentDir = new File(fileName).getParent();
        return (parentDir != null && !fileName.endsWith(File.separator) && !(new File(fileName).isDirectory()) && new File(
            parentDir).isDirectory());
    }

    Composite getPageComposite() {
        return pageComposite;
    }

    void equipmentDisconnected() {

    }

    void equipmentTaskDone(IEquipmentTask task, EquipmentTaskResult result) {

    }

    private class SetJobStatusChangeListener implements IJobChangeListener {
        private final Label label;

        public SetJobStatusChangeListener(Label pLabel) {
            label = pLabel;
        }

        public void sleeping(IJobChangeEvent pEvent) {
        }

        public void scheduled(IJobChangeEvent pEvent) {
        }

        public void running(IJobChangeEvent pEvent) {
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    label.setText("In progress ...");
                    label.pack(true);
                }
            });
        }

        public void done(IJobChangeEvent pEvent) {
            if (pEvent.getResult().isOK()) {
                Display.getDefault().asyncExec(new Runnable() {
                    public void run() {
                        label.setText("done!");
                        label.pack(true);
                    }
                });
                new Timer().schedule(new TimerTask() {
                    @Override
                    public void run() {
                        Display.getDefault().asyncExec(new Runnable() {
                            public void run() {
                                label.setText("");
                                label.pack(true);
                            }
                        });
                    }
                }, 1000);
            } else {
                Utils.logAndShowError("Reading/writing of set has failed", pEvent.getResult().getMessage()
                    + "\nError code:" + pEvent.getResult().getCode(), null);
            }
        }

        public void aboutToRun(IJobChangeEvent pEvent) {
        }

        public void awake(IJobChangeEvent pEvent) {
        }
    }

    /**
     * {@inheritDoc}
     */
    public void validatorMessage(IValidator src, String message) {
        IMessageManager localMM = scrolledForm.getMessageManager();
        if (!src.isValueOk()) {
            localMM.addMessage(src, message, null, IMessage.ERROR, src.getValidatedObject());
        } else {
            localMM.removeMessage(src, src.getValidatedObject());
        }
    }

}
