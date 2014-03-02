package com.stericsson.sdk.signing.ui.wizards.u5500dt;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.security.interfaces.RSAPublicKey;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.SortedSet;
import java.util.TreeSet;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.TableEditor;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.PathTFValidator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import com.stericsson.sdk.signing.ui.Activator;
import com.stericsson.sdk.signing.util.PEMParser;

/**
 * @author xdancho
 * 
 */
public class U5500DTReplaceKeysPage extends AbstractWizardPage {

    private Logger logger = Logger.getLogger(U5500DTReplaceKeysPage.class.getName());

    private static final String PAGE_NAME = "D&&T Key Replacement Settings";

    private static final String SOURCE_FILE = "file";

    private static final String SOURCE_SERVER = "server";

    private static final String GD = "GD_COMP_NAME";

    private static final String SOURCE = "source";

    private final String[] constraintsNames =
        new String[] {
            "Trusted Application", "PRCMU Firmware", "MemInit", "X-Loader", "OS Loader", "APE Normal World Code",
            "Flash & Customization Loader", "Modem Code", "FOTA", "Debug And Test Certificates",
            "Authentication Certificates", "IPL", "Flash Archive", "ITP", "TVP Licence",
            "Field Return Authentication Certificate","User space trusted application","IMAD"};

    private final Map<Control, String> sourceMap = new HashMap<Control, String>();

    private final SortedSet<String> keySet = new TreeSet<String>();

    private final Map<Integer, GenericISSWCustomerPartKey> replacementKeysMap =
        new HashMap<Integer, GenericISSWCustomerPartKey>();

    private Group sourceGroup;

    private Text fileSourceText;

    private Text outputPathText;

    private Button fromFileRadio;

    private Button fileSourceBrowse;

    private Button fromSignServerRadio;

    private Combo signServerSource;

    private Button outputBrowse;

    private static final String INDEX = "index";

    private final TableEditor[] replaceEditor = new TableEditor[constraintsNames.length];

    private final TableEditor[] constraintsEditor = new TableEditor[constraintsNames.length];

    private final TableEditor[] keyEditor = new TableEditor[constraintsNames.length];

    private final TableEditor[] pathEditor = new TableEditor[constraintsNames.length];

    private final TableEditor[] browseEditor = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHash1 = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHash256 = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHash384 = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHash512 = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHmac1 = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHmac256 = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHmac384 = new TableEditor[constraintsNames.length];

    private final TableEditor[] revocationEditorHmac512 = new TableEditor[constraintsNames.length];

    private final Vector<TableEditor[]> editors = new Vector<TableEditor[]>();

    private boolean enableOutputValidation = false;

    private boolean enableKeyValidation = false;

    private Table keyTable;

    /**
     * Constructor
     */
    protected U5500DTReplaceKeysPage() {
        super(PAGE_NAME);
        editors.add(replaceEditor);
        editors.add(constraintsEditor);
        editors.add(keyEditor);
        editors.add(pathEditor);
        editors.add(revocationEditorHash1);
        editors.add(revocationEditorHash256);
        editors.add(revocationEditorHash384);
        editors.add(revocationEditorHash512);
        editors.add(revocationEditorHmac1);
        editors.add(revocationEditorHmac256);
        editors.add(revocationEditorHmac384);
        editors.add(revocationEditorHmac512);
    }

    private void browseButtonEventHandler(Widget item) {

        final FileDialog fd = new FileDialog(Display.getCurrent().getActiveShell(), SWT.OPEN);
        final String path = fd.open();
        if (path == null) {
            return;
        }

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Text) pathEditor[index].getEditor()).setText(path);
            }
            index++;
        }

        setPageComplete(isPageComplete());
    }

    private void clearTable() {

        // clear key, path, replace and browse and revoke state
        for (final TableEditor te : replaceEditor) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : keyEditor) {
            ((Label) te.getEditor()).setText("");
        }

        for (final TableEditor te : pathEditor) {
            ((Text) te.getEditor()).setText("");
        }

        for (final TableEditor te : browseEditor) {
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : constraintsEditor) {
            ((Label) te.getEditor()).setEnabled(false);
        }

        clearRevocation();
    }

    private void clearRevocation() {

        for (final TableEditor te : revocationEditorHash1) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : revocationEditorHash256) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : revocationEditorHash384) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : revocationEditorHash512) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : revocationEditorHmac1) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : revocationEditorHmac256) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : revocationEditorHmac384) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }

        for (final TableEditor te : revocationEditorHmac512) {
            ((Button) te.getEditor()).setSelection(false);
            ((Button) te.getEditor()).setEnabled(false);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite parent) {

        final Composite top = new Composite(parent, SWT.NONE);
        top.setLayout(new GridLayout(3, false));
        top.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        setControl(top);

        // SOURCE GROUP
        sourceGroup = new Group(top, SWT.SHADOW_IN);
        sourceGroup.setText("Source ISSW");
        sourceGroup.setLayout(new GridLayout(3, false));
        final GridData groupData = new GridData(SWT.FILL, SWT.NONE, true, false);
        groupData.horizontalSpan = 3;
        sourceGroup.setLayoutData(groupData);
        sourceGroup.setData(GD, "U5500DTReplaceKeysPage_Source_Group");

        final GridData sourceLabelData = new GridData(SWT.NONE, SWT.NONE, false, false);
        sourceLabelData.horizontalSpan = 1;

        // FROM FILE RADIO
        fromFileRadio = new Button(sourceGroup, SWT.RADIO);
        fromFileRadio.setText("From file:");
        fromFileRadio.setLayoutData(sourceLabelData);
        fromFileRadio.setData(SOURCE, SOURCE_FILE);
        fromFileRadio.setData(GD, "U5500DTReplaceKeysPage_FromFileRadio_Button");
        fromFileRadio.addSelectionListener(new SelectionListener() {

            public void widgetDefaultSelected(SelectionEvent e) {
            }

            public void widgetSelected(SelectionEvent e) {
                sourceSelected(e.getSource());
            }
        });

        // FROM FILE TEXT
        fileSourceText = new Text(sourceGroup, SWT.BORDER);
        final GridData fileInputData = new GridData(SWT.FILL, SWT.NONE, true, false);
        fileInputData.horizontalSpan = 1;
        fileSourceText.setLayoutData(fileInputData);
        fileSourceText.setData(GD, "U5500DTReplaceKeysPage_FileSource_Text");
        final PathTFValidator fromFileValidator = new PathTFValidator("From file", fileSourceText, false);
        fromFileValidator.switchOn();
        fromFileValidator.addValidatorListener(new IValidatorMessageListener() {

            public void validatorMessage(IValidator src, String message) {

                if (fileSourceText.getText().length() == 0) {
                    enableKeyValidation = true;
                    clearTable();
                    removeMessage(fileSourceText);
                    validateKey();
                    setPageComplete(isPageComplete());
                } else if (validateSourcePath(message)) {
                    if (!loadISSW(fileSourceText.getText())) {
                        enableKeyValidation = false;
                        clearTable();
                        removeMessage(fileSourceText);
                        addMessage(fileSourceText, "The ISSW is not valid", MESSAGE_TYPE.ERROR);
                    } else {
                        enableKeyValidation = true;
                        validateKey();
                    }
                    setPageComplete(isPageComplete());
                } else {
                    clearTable();
                }

            }
        });
        sourceMap.put(fileSourceText, SOURCE_FILE);

        // FROM FILE BROWSE
        fileSourceBrowse = new Button(sourceGroup, SWT.PUSH);
        final GridData fileSourceBrowseData = new GridData(SWT.RIGHT, SWT.NONE, false, false);
        fileSourceBrowseData.horizontalSpan = 1;
        fileSourceBrowse.setText("Browse...");
        fileSourceBrowse.setLayoutData(fileSourceBrowseData);
        fileSourceBrowse.setData(GD, "U5500DTReplaceKeysPage_FileSource_Button");
        fileSourceBrowse.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {
                final FileDialog fd = new FileDialog(Display.getCurrent().getActiveShell(), SWT.OPEN);
                final String path = fd.open();
                if (path != null) {
                    fileSourceText.setText(path);
                }

            }
        });
        sourceMap.put(fileSourceBrowse, SOURCE_FILE);

        // FROM SIGNSERVER
        createSignServerRadio();

        // FROM FILE TEXT
        createSignServerCombo();

        // KEY TABLE
        createKeyTable(top);

        // TABLE COLUMNS
        final TableColumn tcReplace = new TableColumn(keyTable, SWT.NONE);
        tcReplace.setText("Replace");

        final TableColumn tcConstraint = new TableColumn(keyTable, SWT.NONE);
        tcConstraint.setText("Constraint");

        final TableColumn tcKey = new TableColumn(keyTable, SWT.NONE);
        tcKey.setText("Key");

        final TableColumn tcPath = new TableColumn(keyTable, SWT.NONE);
        tcPath.setText("Path");

        final TableColumn tcBrowse = new TableColumn(keyTable, SWT.NONE);
        tcBrowse.setText("");

        creteTableColumnsHash();


        // create all rows
        for (int i = 0; i < constraintsNames.length; i++) {
            new TableItem(keyTable, SWT.NONE);
        }

        // fill rows with defaults
        createTableEditors();

        // OUTPUT LABEL
        createOutputControls(top, fileSourceBrowseData);

        // Defaults
        tcReplace.setWidth(50);
        tcConstraint.setWidth(155);
        tcKey.setWidth(50);
        tcPath.setWidth(150);
        tcBrowse.setWidth(75);
        fromFileRadio.setSelection(true);
        sourceSelected(fromFileRadio);
        clearTable();
        fromFileValidator.checkCurrentValue();
        setPageComplete(isPageComplete());
    }

    private void creteTableColumnsHash() {

        final TableColumn tcRevokeHash1 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHash1.setText("HASH 1");

        final TableColumn tcRevokeHash256 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHash256.setText("256");

        final TableColumn tcRevokeHash384 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHash384.setText("384");

        final TableColumn tcRevokeHash512 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHash512.setText("512");

        final TableColumn tcRevokeHmac1 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHmac1.setText("HMAC 1");

        final TableColumn tcRevokeHmac256 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHmac256.setText("256");

        final TableColumn tcRevokeHmac384 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHmac384.setText("384");

        final TableColumn tcRevokeHmac512 = new TableColumn(keyTable, SWT.NONE);
        tcRevokeHmac512.setText("512");

        tcRevokeHash1.setWidth(70);
        tcRevokeHash256.setWidth(50);
        tcRevokeHash384.setWidth(50);
        tcRevokeHash512.setWidth(50);
        tcRevokeHmac1.setWidth(50);
        tcRevokeHmac256.setWidth(50);
        tcRevokeHmac384.setWidth(50);
        tcRevokeHmac512.setWidth(50);
    }

    private Button createSignServerRadio() {
        final GridData sourceLabelData = new GridData(SWT.NONE, SWT.NONE, false, false);
        sourceLabelData.horizontalSpan = 1;
        fromSignServerRadio = new Button(sourceGroup, SWT.RADIO);
        fromSignServerRadio.setText("From sign server:");
        fromSignServerRadio.setLayoutData(sourceLabelData);
        fromSignServerRadio.setData(SOURCE, SOURCE_SERVER);
        fromSignServerRadio.setData(GD, "U5500DTReplaceKeysPage_FromSignServer_Button");
        fromSignServerRadio.addSelectionListener(new SelectionListener() {

            public void widgetDefaultSelected(SelectionEvent e) {

            }

            public void widgetSelected(SelectionEvent e) {
                final Object src = e.getSource();
                if (src instanceof Button) {
                    final Button button = (Button) src;
                    if (!button.getSelection()) {
                        final boolean local =
                            Activator.getDefault().getPreferenceStore().getBoolean(
                                FlashKitPreferenceConstants.SIGNING_USE_LOCAL_SIGNING);
                        final boolean useCommonFolder =
                                Activator.getDefault().getPreferenceStore().getBoolean(
                                    FlashKitPreferenceConstants.SIGNING_USE_COMMONFOLDER);
                        try {
                            Cursor waitCursor = sourceGroup.getDisplay().getSystemCursor(SWT.CURSOR_WAIT);
                            sourceGroup.setCursor(waitCursor);
                            final ISignPackageInformation[] signPackages =
                                new SignerFacade().getSignPackages(null, local, useCommonFolder,(ISignerSettings) null);
                            populateSignPackageCombo(signPackages);
                        } catch (final SignerException e1) {
                            logger.error("Error during connecting to sign server occured. ", e1);
                        } finally {
                            // set default cursor
                            sourceGroup.setCursor(null);
                        }
                    }
                }
                sourceSelected(src);

            }
        });
        return fromSignServerRadio;
    }

    private void populateSignPackageCombo(ISignPackageInformation[] signPackages) {
        signServerSource.removeAll();
        if (signPackages != null) {
            for (final ISignPackageInformation pkg : signPackages) {
                if (ISignPackage.PACKAGE_TYPE_U5500_ROOT.equals(pkg.getType())) {
                    signServerSource.add(pkg.getName());
                }
            }
        }
    }

    private void createSignServerCombo() {
        signServerSource = new Combo(sourceGroup, SWT.READ_ONLY);
        final GridData serverInputData = new GridData(SWT.FILL, SWT.NONE, true, false);
        serverInputData.horizontalSpan = 2;
        signServerSource.setLayoutData(serverInputData);
        signServerSource.setData(GD, "U5500DTReplaceKeysPage_SignServerSource_Combo");
        sourceMap.put(signServerSource, SOURCE_SERVER);
        signServerSource.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {
                final boolean local =
                    Activator.getDefault().getPreferenceStore().getBoolean(
                        FlashKitPreferenceConstants.SIGNING_USE_LOCAL_SIGNING);
                final boolean useCommonFolder =
                        Activator.getDefault().getPreferenceStore().getBoolean(
                            FlashKitPreferenceConstants.SIGNING_USE_COMMONFOLDER);
                final int index = signServerSource.getSelectionIndex();
                if (index != -1) {
                    final String item = signServerSource.getItem(index);
                    if (item == null) {
                        return;
                    }
                    try {
                        final ISignPackage signPackage = new SignerFacade().getSignPackage(item, null, local, 
                                useCommonFolder,(ISignerSettings) null);
                        if (signPackage instanceof GenericSignPackage) {
                            final GenericCertificate certificate = ((GenericSignPackage) signPackage).getCertificate();
                            if (certificate instanceof GenericISSWCertificate) {
                                updateTableFromISSW((GenericISSWCertificate) certificate);
                                enableKeyValidation = true;
                                validateKey();
                            } else {
                                enableKeyValidation = false;
                                clearTable();
                                removeMessage(fileSourceText);
                                addMessage(fileSourceText, "The ISSW is not valid", MESSAGE_TYPE.ERROR);
                            }
                        }
                        sourceSelected(signServerSource);
                        setPageComplete(isPageComplete());
                    } catch (final SignerServiceException e) {
                        logger.error("Error in signer service occured. ", e);

                    }
                }
            }
        });
    }

    private void createKeyTable(final Composite top) {
        keyTable = new Table(top, SWT.SINGLE | SWT.BORDER);
        final GridData tableData = new GridData(SWT.FILL, SWT.FILL, true, true);
        tableData.horizontalSpan = 3;
        keyTable.setLayoutData(tableData);
        keyTable.setHeaderVisible(true);
        keyTable.setLinesVisible(true);
        keyTable.setData(GD, "U5500DTReplaceKeysPage_Key_Table");
        keyTable.addListener(SWT.MeasureItem, new Listener() {
            public void handleEvent(Event event) {
                // height cannot be per row so simply set
                event.height = 20;
            }
        });
    }

    private void createOutputControls(final Composite top, final GridData fileSourceBrowseData) {
        final Label outputLabel = new Label(top, SWT.NONE);
        final GridData outputLabelData = new GridData(SWT.LEFT, SWT.NONE, false, false);
        outputLabel.setText("Output file:");
        outputLabelData.horizontalSpan = 1;
        outputLabel.setLayoutData(outputLabelData);

        // OUTPUT TEXT
        outputPathText = new Text(top, SWT.BORDER);
        final GridData outputTextData = new GridData(SWT.FILL, SWT.NONE, true, false);
        outputTextData.horizontalSpan = 1;
        outputPathText.setData(GD, "U5500DTReplaceKeysPage_OutputPath_Text");
        outputPathText.setLayoutData(outputTextData);

        outputPathText.addListener(SWT.Modify, new Listener() {

            public void handleEvent(Event event) {

                setPageComplete(isPageComplete());

            }
        });

        // OUTPUT BROWSE
        outputBrowse = new Button(top, SWT.PUSH);
        final GridData outputBrowseData = new GridData(SWT.RIGHT, SWT.NONE, false, false);
        outputBrowseData.horizontalSpan = 1;
        outputBrowse.setText("Browse...");
        outputBrowse.setLayoutData(fileSourceBrowseData);
        outputBrowse.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {
                final FileDialog fd = new FileDialog(Display.getCurrent().getActiveShell(), SWT.SAVE);
                fd.setOverwrite(true);
                final String path = fd.open();
                if (path != null) {
                    outputPathText.setText(path);
                }

            }
        });
    }

    private void createTableEditors() {
        final TableItem[] items = keyTable.getItems();
        for (int i = 0; i < constraintsNames.length; i++) {

            // REPLACE
            replaceEditor[i] = new TableEditor(keyTable);
            replaceEditor[i].grabHorizontal = true;
            replaceEditor[i].grabVertical = true;

            final Button replace = new Button(keyTable, SWT.CHECK);
            replace.setData(INDEX, i);
            replace.setData(GD, "U5500DTReplaceKeysPage_ReplaceEditor" + i + "_Button");
            replace.setBackground(keyTable.getBackground());
            replace.addListener(SWT.Selection, new Listener() {

                public void handleEvent(Event event) {

                    replaceButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

                }
            });
            replaceEditor[i].setEditor(replace, items[i], 0);

            // CONSTRAINTS
            constraintsEditor[i] = new TableEditor(keyTable);
            constraintsEditor[i].grabHorizontal = true;
            constraintsEditor[i].grabVertical = true;
            final Label constraintLabel = new Label(keyTable, SWT.NONE);
            constraintLabel.setData(GD, "U5500DTReplaceKeysPage_ConstraintsEditor" + i + "_Label");
            constraintLabel.setText(constraintsNames[i]);
            constraintLabel.setBackground(keyTable.getBackground());
            constraintsEditor[i].setEditor(constraintLabel, items[i], 1);

            // KEY
            keyEditor[i] = new TableEditor(keyTable);
            keyEditor[i].grabHorizontal = true;
            keyEditor[i].grabVertical = true;
            final Label keyLabel = new Label(keyTable, SWT.NONE | SWT.CENTER);
            final GridData keyData = new GridData(SWT.FILL, SWT.NONE, true, true);
            keyLabel.setData(GD, "U5500DTReplaceKeysPage_KeyEditor" + i + "_Label");
            keyLabel.setLayoutData(keyData);
            keyLabel.setBackground(keyTable.getBackground());
            keyEditor[i].setEditor(keyLabel, items[i], 2);

            // PATH
            pathEditor[i] = new TableEditor(keyTable);
            pathEditor[i].grabHorizontal = true;
            pathEditor[i].grabVertical = true;
            final Text pathText = new Text(keyTable, SWT.NONE | SWT.READ_ONLY);

            final GridData textData = new GridData(SWT.FILL, SWT.FILL, true, true);
            pathText.setText("");
            pathText.setData(GD, "U5500DTReplaceKeysPage_PathEditor" + i + "_Text");
            pathText.setLayoutData(textData);
            pathText.setBackground(keyTable.getBackground());
            pathEditor[i].setEditor(pathText, items[i], 3);

            // BROWSE
            browseEditor[i] = new TableEditor(keyTable);
            browseEditor[i].grabHorizontal = true;
            browseEditor[i].grabVertical = true;
            final Button browse = new Button(keyTable, SWT.PUSH);
            browse.setEnabled(false);
            browse.setData(GD, "U5500DTReplaceKeysPage_BrowseEditor" + i + "_Button");
            browse.setData(INDEX, i);
            browse.setText("Browse...");
            browse.addListener(SWT.Selection, new Listener() {

                public void handleEvent(Event event) {
                    browseButtonEventHandler(event.widget);

                }
            });
            browseEditor[i].setEditor(browse, items[i], 4);

            // REVOKE
            createRevocationHash1(items[i], i);
            createRevocationHash256(items[i], i);
            createRevocationHash384(items[i], i);
            createRevocationHash512(items[i], i);
            createRevocationHmac1(items[i], i);
            createRevocationHmac256(items[i], i);
            createRevocationHmac384(items[i], i);
            createRevocationHmac512(items[i], i);

        }

        for (final TableColumn tc : keyTable.getColumns()) {
            tc.pack();

        }
    }

    // REVOKE HASH_1
    private void createRevocationHash1(TableItem item, int i) {

        revocationEditorHash1[i] = new TableEditor(keyTable);
        revocationEditorHash1[i].grabHorizontal = true;
        revocationEditorHash1[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHash1_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHash1ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHash1[i].setEditor(bt, item, 5);
    }

    // REVOKE HASH_256
    private void createRevocationHash256(TableItem item, int i) {

        revocationEditorHash256[i] = new TableEditor(keyTable);
        revocationEditorHash256[i].grabHorizontal = true;
        revocationEditorHash256[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHash256_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHash256ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHash256[i].setEditor(bt, item, 6);
    }

    // REVOKE HASH_384
    private void createRevocationHash384(TableItem item, int i) {

        revocationEditorHash384[i] = new TableEditor(keyTable);
        revocationEditorHash384[i].grabHorizontal = true;
        revocationEditorHash384[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHash384_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHash384ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHash384[i].setEditor(bt, item, 7);
    }

    // REVOKE HASH_512
    private void createRevocationHash512(TableItem item, int i) {

        revocationEditorHash512[i] = new TableEditor(keyTable);
        revocationEditorHash512[i].grabHorizontal = true;
        revocationEditorHash512[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHash512_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHash512ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHash512[i].setEditor(bt, item, 8);
    }

    // REVOKE HMAC_1
    private void createRevocationHmac1(TableItem item, int i) {

        revocationEditorHmac1[i] = new TableEditor(keyTable);
        revocationEditorHmac1[i].grabHorizontal = true;
        revocationEditorHmac1[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHmac1_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHmac1ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHmac1[i].setEditor(bt, item, 9);
    }

    // REVOKE HMAC_256
    private void createRevocationHmac256(TableItem item, int i) {

        revocationEditorHmac256[i] = new TableEditor(keyTable);
        revocationEditorHmac256[i].grabHorizontal = true;
        revocationEditorHmac256[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHmac256_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHmac256ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHmac256[i].setEditor(bt, item, 10);
    }

    // REVOKE HMAC_384
    private void createRevocationHmac384(TableItem item, int i) {

        revocationEditorHmac384[i] = new TableEditor(keyTable);
        revocationEditorHmac384[i].grabHorizontal = true;
        revocationEditorHmac384[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHmac384_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHmac384ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHmac384[i].setEditor(bt, item, 11);
    }

    // REVOKE HMAC_512
    private void createRevocationHmac512(TableItem item, int i) {

        revocationEditorHmac512[i] = new TableEditor(keyTable);
        revocationEditorHmac512[i].grabHorizontal = true;
        revocationEditorHmac512[i].grabVertical = true;

        final Button bt = new Button(keyTable, SWT.CHECK);
        bt.setData(INDEX, i);
        bt.setData(GD, "U5500DTReplaceKeysPage_RevocationEditorHmac512_" + i + "_Button");
        bt.setBackground(keyTable.getBackground());
        bt.addListener(SWT.Selection, new Listener() {

            public void handleEvent(Event event) {

                revokeHmac512ButtonEventHandler(event.widget, ((Button) event.widget).getSelection());

            }
        });
        revocationEditorHmac512[i].setEditor(bt, item, 12);
    }

    void enableRow(int index, boolean enabled) {
        for (final TableEditor[] te : editors) {
            te[index].getEditor().setEnabled(enabled);
        }
    }

    /**
     * Get the path to store the certificate.
     * 
     * @return Path to store the certificate.
     */
    protected String getOutputPath() {
        return outputPathText.getText();
    }

    /**
     * Get the replacement keys.
     * 
     * @return The replacement keys.
     * @throws Exception
     *             If the keys cannot be parsed.
     */
    protected Map<Integer, GenericISSWCustomerPartKey> getReplacementKeys() throws Exception {
        // save keys
        for (final String constraint : keySet) {
            int index = 0;

            for (final TableEditor te : keyEditor) {
                // find the first

                try {
                    if (constraint.equals(((Label) te.getEditor()).getText())
                        && (((Text) pathEditor[index].getEditor()).getText().length() != 0)) {
                        final GenericISSWCustomerPartKey replaceKey = new GenericISSWCustomerPartKey();
                        RSAPublicKey key = null;

                        key = PEMParser.parseRSAPublicKey(((Text) pathEditor[index].getEditor()).getText());
                        replaceKey.setKeyTypeWithRevocationHashTypes(
                                new SignatureTypeWithRevocationHashTypes(SignatureType.RSASSA_PKCS_V1_5,
                                        getRevocationHashTypes(index)));
                        if (key == null) {
                            throw new IOException("Failed to read authentication key");
                        }

                        final byte[] modulusWithSignBit = key.getModulus().toByteArray();
                        final byte[] modulus = new byte[modulusWithSignBit.length - 1];
                        System.arraycopy(modulusWithSignBit, 1, modulus, 0, modulus.length);
                        final int sizeOfDummy = 2;
                        final int sizeOfModSize = 2;
                        final int sizeOfExponent = 4;

                        replaceKey.setModulusSize((short) modulus.length);
                        replaceKey.setKeySize(modulus.length + sizeOfDummy + sizeOfModSize + sizeOfExponent);
                        replaceKey.setModulus(LittleEndianByteConverter.reverse(modulus));
                        replaceKey.setExponent(key.getPublicExponent().intValue());
                        replacementKeysMap.put(Integer.parseInt(constraint), replaceKey);
                        break;
                    }
                } catch (final Exception e) {
                    throw new Exception("Failed to parse key number " + (index + 1) + "\n Reason: " + e.getMessage());
                }
                index++;
            }
        }
        return replacementKeysMap;
    }


    private int getRevocationHashTypes(int index) {
//      collecting revocation hashes

        int hashes = 0;
        if (((Button)revocationEditorHash1[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 0);
        }
        if (((Button)revocationEditorHash256[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 1);
        }
        if (((Button)revocationEditorHash384[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 2);
        }
        if (((Button)revocationEditorHash512[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 3);
        }
        if (((Button)revocationEditorHmac1[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 4);
        }
        if (((Button)revocationEditorHmac256[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 5);
        }
        if (((Button)revocationEditorHmac384[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 6);
        }
        if (((Button)revocationEditorHmac512[index].getEditor()).getSelection()) {
            hashes += (int)Math.pow(2, 7);
        }

//      Shift hashes to the upper byte (bit 8-15)
        return hashes<<8;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {

        if (!validateKey()) {
            return false;
        }

        if (!validateOutputPath()) {
            return false;
        }

        return true;
    }

    private boolean loadISSW(String path) {

        DataInputStream das = null;

        try {
            final GenericISSWCertificate custCert = new GenericISSWCertificate();
            final File file = new File(path);
            try {
                das = new DataInputStream(new FileInputStream(file));
                final byte[] input = new byte[(int) file.length()];

                final int read = das.read(input);
                // if -1, nothing to read; if less then 4 bytes available, it is not ISSW file since
                // it cannot contain the ISSW magic number
                if (read < 4) {
                    return false;
                }
                byte[] temp = new byte[4];
                System.arraycopy(input, 0, temp, 0, 4);
                if (custCert.isISSW(temp)) {
                    custCert.setData(input);
                } else {
                    return false;
                }

            } finally {
                if (das != null) {
                    das.close();
                }
            }

            updateTableFromISSW(custCert);

            return true;

        } catch (final Exception e) {
            logger.error("Error during load of ISSW Certificate. " + e.getMessage());
            return false;
        }

    }

    private void replaceButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) replaceEditor[index].getEditor()).setSelection(checked);
                ((Button) browseEditor[index].getEditor()).setEnabled(checked);
                if (!checked) {
                    ((Text) pathEditor[index].getEditor()).setText("");
                }
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHash1ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHash1[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHash256ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHash256[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHash384ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHash384[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHash512ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHash512[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHmac1ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHmac1[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHmac256ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHmac256[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHmac384ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHmac384[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void revokeHmac512ButtonEventHandler(Widget item, boolean checked) {

        final int keyIndex = (Integer) ((Button) item).getData(INDEX);
        String key = "";
        // get the key
        key = ((Label) keyEditor[keyIndex].getEditor()).getText();

        // look up all rows affected
        int index = 0;
        for (final TableEditor te : keyEditor) {
            if (((Label) te.getEditor()).getText().equals(key)) {
                ((Button) revocationEditorHmac512[index].getEditor()).setSelection(checked);
            }
            index++;
        }

        setPageComplete(isPageComplete());

    }

    private void sourceSelected(Object source) {

        removeAllMessages();

        // buttons
        for (final Control c : sourceGroup.getChildren()) {
            if (c instanceof Button) {
                if (!c.equals(source)) {
                    ((Button) c).setSelection(false);
                } else {
                    ((Button) c).setSelection(true);
                }
            }
        }

        // controls
        for (final Control c : sourceMap.keySet()) {
            if (sourceMap.get(c).equals((((Button) source).getData(SOURCE)))) {

                if (c instanceof Text) {
                    ((Text) c).setText("");
                }

                if (c instanceof Combo) {
                    removeMessage(fileSourceText);
                    if (((Combo) c).getText().length() == 0) {
                        addMessage(c, "A signpackage must be specified", MESSAGE_TYPE.INFO);
                    } else {
                        removeMessage(c);
                    }
                }
                c.setEnabled(true);
            } else {
                c.setEnabled(false);
            }
        }

        // always clear if source has changed
        enableOutputValidation = false;
        enableKeyValidation = false;
        clearTable();

    }

    private void updateTableFromISSW(GenericISSWCertificate issw) {

        clearTable();

        // get keylist
        final GenericISSWCustomerPartKey[] keys = new GenericISSWCustomerPartKey[constraintsNames.length];
        final GenericSoftwareType[] values = getSoftwareTypes(keys.length);
        for (int i = 0; i < constraintsNames.length; i++) {
            keys[i] = issw.getCustomerPart().getKey(values[i]);
        }

        final byte[] signingConstraints = issw.getCustomerPart().getSigningConstraints();

        // go through the keylist
        int index = 0;
        for (final GenericISSWCustomerPartKey key : keys) {

            if (key == null) {
                enableRow(index, false);
            } else {
                enableRow(index, true);

                final String valueOf = String.valueOf(signingConstraints[index]);
                keySet.add(valueOf);
                ((Label) keyEditor[index].getEditor()).setText(valueOf);

                int hashes = ((key.getKeyTypeWithRevocationHashTypes() & 0xFF00) >> 8);

                ((Button) revocationEditorHash1[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 0))!=0);
                ((Button) revocationEditorHash256[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 1))!=0);
                ((Button) revocationEditorHash384[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 2))!=0);
                ((Button) revocationEditorHash512[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 3))!=0);
                ((Button) revocationEditorHmac1[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 4))!=0);
                ((Button) revocationEditorHmac256[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 5))!=0);
                ((Button) revocationEditorHmac384[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 6))!=0);
                ((Button) revocationEditorHmac512[index].getEditor()).setSelection((hashes & (int)Math.pow(2, 7))!=0);
            }

            index++;
        }

    }

    private GenericSoftwareType[] getSoftwareTypes(int numTypes) {
        final ArrayList<GenericSoftwareType> types = new ArrayList<GenericSoftwareType>();
        for (int i = 0; i < numTypes; i++) {
            types.add(getSoftwareType(i));
        }
        return types.toArray(new GenericSoftwareType[types.size()]);
    }

    private GenericSoftwareType getSoftwareType(int signingConstraint) {
        for (final GenericSoftwareType type : GenericSoftwareType.values()) {
            if (type.getPosition() == signingConstraint) {
                return type;
            }
        }
        return null;
    }

    private boolean validateKey() {

        if (!enableKeyValidation) {
            return false;
        }
        int index = 0;
        boolean selected = false;
        boolean keyPathOK = false;

        for (final TableEditor te : replaceEditor) {
            if (((Button) te.getEditor()).getSelection()) {
                selected = true;
                if ((validatePath(((Text) pathEditor[index].getEditor()).getText()))) {
                    keyPathOK = true;
                    final String text = ((Text) pathEditor[index].getEditor()).getText();

                    try {
                        PEMParser.parseRSAPublicKey(text);
                    } catch (final Exception e) {
                        addMessage(keyTable, "Key(s) supplied is not valid", MESSAGE_TYPE.ERROR);
                        return false;
                    }

                } else {
                    keyPathOK = false;
                    break;
                }
            }
            index++;
        }

        // it is only optional to have new keys defined
        // if new keys are defined, the path to them must be correct
        if (!selected || (selected && keyPathOK)) {
            removeMessage(keyTable);
            enableOutputValidation = true;
            validateOutputPath();
            return true;
        } else {
            enableOutputValidation = false;
            removeMessage(outputPathText);
            addMessage(keyTable, "If new key(s) are defined then the path(s) to them must be specified",
                MESSAGE_TYPE.INFO);
            return false;
        }

    }

    private boolean validateOutputPath() {
        if (!enableOutputValidation) {
            return false;
        }
        final String output = outputPathText.getText();
        if (output.length() == 0) {
            addMessage(outputPathText, "An output path must be specified", MESSAGE_TYPE.INFO);
            return false;
        } else {
            if (!validatePath(outputPathText.getText())) {
                addMessage(outputPathText, "The output path is not valid", MESSAGE_TYPE.ERROR);
                return false;
            } else {
                removeMessage(outputPathText);
                return true;
            }
        }
    }

    private boolean validatePath(String path) {

        String dir = null;
        final int indexOfFile = path.lastIndexOf(System.getProperty("file.separator"));
        final int indexOfPath = path.lastIndexOf(System.getProperty("path.separator"));

        if ((indexOfFile == -1) && (indexOfPath == -1)) {
            return false;
        } else {
            if (indexOfFile < indexOfPath) {
                dir = path.substring(0, indexOfPath + 1);
            } else {
                dir = path.substring(0, indexOfFile + 1);
            }
            final File dirFile = new File(dir);
            final File pathFile = new File(path);
            return dirFile.exists() && !pathFile.isDirectory() ? true : false;
        }
    }

    private boolean validateSourcePath(String message) {
        removeMessage(fileSourceText);

        if (fileSourceText.getText().length() == 0) {
            addMessage(fileSourceText, "A path must be specified", MESSAGE_TYPE.INFO);
            return false;
        } else {
            if (message != null) {
                addMessage(fileSourceText, message, MESSAGE_TYPE.ERROR);
                return false;
            } else {
                validateKey();
                return true;
            }
        }
    }

    /**
     *{@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_dnt_cert.html");
    }

}
