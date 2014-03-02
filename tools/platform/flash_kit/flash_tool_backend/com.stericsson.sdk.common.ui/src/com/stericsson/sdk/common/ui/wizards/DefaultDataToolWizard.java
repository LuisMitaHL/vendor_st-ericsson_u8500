package com.stericsson.sdk.common.ui.wizards;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Collection;
import java.util.EnumSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Vector;

import org.eclipse.core.filesystem.EFS;
import org.eclipse.core.filesystem.IFileStore;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;

import com.stericsson.sdk.common.ui.Activator;
import com.stericsson.sdk.common.ui.wizards.logic.DefaultDataSheetProperties;
import com.stericsson.sdk.common.ui.wizards.logic.PlatformsInfo;
import com.stericsson.sdk.common.ui.wizards.pages.AllPlatformsOutputPage;
import com.stericsson.sdk.common.ui.wizards.pages.OnePlatformOutputPage;
import com.stericsson.sdk.common.ui.wizards.pages.SelectFileWizardPage;

/**
 * Default Data Sheet Tool Wizard is a wizard for reading, writing, consistency checking, and
 * merging 19062, GDF, and GDVAR files used in the production process.
 */
public class DefaultDataToolWizard extends Wizard implements INewWizard {

    private static final String GDVAR_FILE_NAME = "gdvar.h";

    private static final String GDF_FILE_NAME = "19062.gdf";

    private static final String EEVAR_FILE_NAME = "eevar.h";

    private static final String EE_FILE_NAME = "19062.eef";

    private static final String EXCEL_FILE_NAME = "19062_product.xls";

    private static final String CALIBRATION_DIALOG_MESSAGE =
        "Calibration data from this file will overwrite and destroy"
            + "any existing calibration data in targets that this file is sent to."
            + "\nThis may seriously affect the performance of the target ME."
            + "Do you want to include calibration data in the GDFS file?";

    private static final String CALIBRATION_DIALOG_TEXT = "Include calibration data";

    private static final String WIZARD_SETTINGS_SECTION_NAME = "DefaultDataToolWizard";

    private IDialogSettings wizardSettings;

    private SelectFileWizardPage selectFilePage;

    private OnePlatformOutputPage onePlatformOutputPage;

    private AllPlatformsOutputPage allPlatformsOutputPage;

    private DefaultDataSheetProperties ddsp = new DefaultDataSheetProperties();

    private ByteArrayOutputStream report = new ByteArrayOutputStream();

    private boolean calibrationDataDialogShown = false;

    /**
     * The constructor
     */
    public DefaultDataToolWizard() {
        super();
        setHelpAvailable(true);

        IDialogSettings dialogsSettings = Activator.getDefault().getDialogSettings();
        wizardSettings = dialogsSettings.getSection(WIZARD_SETTINGS_SECTION_NAME);
        if (wizardSettings == null) {
            wizardSettings = dialogsSettings.addNewSection(WIZARD_SETTINGS_SECTION_NAME);
        }

        setDefaultDialogSettings();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        if (!createOutputFiles()) {
            return false;
        }

        if (wizardSettings.getBoolean(SettingsKeys.GENERATE_REPORT.toString())) {
            return generateReport(report);
        }

        return true;
    }

    /**
     * 
     *{@inheritDoc}
     */
    public void init(IWorkbench pWorkbench, IStructuredSelection pSelection) {
        setWindowTitle("New Default Data");
        selectFilePage = new SelectFileWizardPage(wizardSettings, ddsp);
        onePlatformOutputPage = new OnePlatformOutputPage(wizardSettings);
        allPlatformsOutputPage = new AllPlatformsOutputPage(wizardSettings);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFinish() {
        if (!selectFilePage.isPageComplete()) {
            return false;
        }

        if (wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString())) {
            return allPlatformsOutputPage.isPageComplete();
        } else {
            return onePlatformOutputPage.isPageComplete();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public IWizardPage getNextPage(IWizardPage pPage) {
        if (pPage == selectFilePage) {
            if (wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString())) {
                return allPlatformsOutputPage;
            } else {
                return onePlatformOutputPage;
            }
        } else {
            // IWizardPage p = super.getNextPage(pPage);
            return null;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        super.addPages();
        addPage(selectFilePage);
        addPage(onePlatformOutputPage);
        addPage(allPlatformsOutputPage);
    }

    /**
     * Fills wizardSettings with default values if some are not specified.
     */
    private void setDefaultDialogSettings() {
        if (null == wizardSettings.get(SettingsKeys.INPUT_GDF_FILE.toString())) {
            wizardSettings.put(SettingsKeys.INPUT_GDF_FILE.toString(), "");
        }

        // by default set all "create" check boxes to true
        EnumSet<SettingsKeys> creates =
            EnumSet.of(SettingsKeys.CREATE_GDVAR_FILE, SettingsKeys.CREATE_GDF_FILE, SettingsKeys.CREATE_EEVAR_FILE,
                SettingsKeys.CREATE_EE_FILE, SettingsKeys.CREATE_EXCEL_FILE);
        for (SettingsKeys c : creates) {
            if (null == wizardSettings.get(c.toString())) {
                wizardSettings.put(c.toString(), true);
            }
        }

        if (null == wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString())) {
            wizardSettings.put(SettingsKeys.OUTPUT_DIR.toString(), "");
        }
    }

    /**
     * This method reads the Document Data (with platform selection, if necesary), performs a
     * consistency check of the document and finally creates output files according to user's
     * choices in GUI.
     * 
     * Possible output to create: - GDVAR file - GDF file, possibly merged with a selected GDF file
     * - Customer 19062 document
     * 
     * @return True if this was done without errors
     */
    private boolean createOutputFiles() {
        ddsp.clearIncludeCalibrationDataSelection();
        calibrationDataDialogShown = false;

        if (!confirmFileOverwriting()) {
            return false;
        }

        final boolean allPlatforms = wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString());

        if (allPlatforms) {
            Vector<PlatformsInfo> platforms = ddsp.getAvailablePlatforms();

            boolean resultIsOk = true;

            for (int i = 0; i < platforms.size(); i++) {
                // Platform Selection
                if (resultIsOk) {
                    ddsp.setPlatformToProcess(i, platforms.get(i).getName());
                }

                // Consistency check
                if (resultIsOk) {
                    resultIsOk = ddsp.consistencyCheckForSelectedPlatform();
                }

                // Produce output files
                if (resultIsOk) {
                    resultIsOk = createOutputFilesForSelectedPlatform();
                }

                ddsp.createStatusReport(report);
            }

            return resultIsOk;

        } else {

            boolean resultIsOk = true;

            // Platform Selection
            if (resultIsOk) {
                resultIsOk = (wizardSettings.getBoolean(SettingsKeys.CORRECT_PLATFORM_SELECTED.toString()));
            }

            // Consistency check
            if (resultIsOk) {
                resultIsOk = ddsp.consistencyCheckForSelectedPlatform();
            }

            // Produce output files
            if (resultIsOk) {
                resultIsOk = createOutputFilesForSelectedPlatform();
            }

            ddsp.createStatusReport(report);
            return resultIsOk;
        }
    }

    /**
     * Shows a warning message with a list of files that already exist and might be overwritten.
     * 
     * @return true if user confirms overwriting of files (or there are no files to overwrite) or
     *         false otherwise
     */
    private boolean confirmFileOverwriting() {
        boolean allPlatforms = wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString());
        LinkedList<File> outputFiles = new LinkedList<File>();

        if (allPlatforms) {
            Vector<PlatformsInfo> platforms = ddsp.getAvailablePlatforms();

            for (int i = 0; i < platforms.size(); i++) {
                outputFiles.addAll(getOutputFilesForPlatform(platforms.get(i).getName(), true));
            }
        } else {
            outputFiles.addAll(getOutputFilesForPlatform(ddsp.getPlatformNameToProcess(), false));
        }

        LinkedList<File> existingFiles = new LinkedList<File>();
        for (File f : outputFiles) {
            if (f.exists()) {
                existingFiles.add(f);
            }
        }

        if (!existingFiles.isEmpty()) {
            String message = "Following files already exist:\n\n";
            for (File f : existingFiles) {
                message = message + f + "\n";
            }
            message = message + "\nDo you want to overwrite these files?";

            MessageBox messageBox =
                new MessageBox(PlatformUI.getWorkbench().getDisplay().getActiveShell(), SWT.ICON_QUESTION | SWT.YES
                    | SWT.NO);
            messageBox.setMessage(message);
            messageBox.setText("Warning");

            return (SWT.YES == messageBox.open());
        }

        return true;
    }

    /**
     * Creates collection of output files for given platform.
     * 
     * @param platformName
     *            name of the platform
     * @param addPlatformFolder
     *            platform folder is injected into the path
     * @return collection of output files
     */
    private Collection<? extends File> getOutputFilesForPlatform(String platformName, boolean addPlatformFolder) {
        LinkedList<File> outputFiles = new LinkedList<File>();
        String outputDir = wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString());

        List<SettingsKeys> creates =
            Arrays.asList(SettingsKeys.CREATE_GDVAR_FILE, SettingsKeys.CREATE_GDF_FILE, SettingsKeys.CREATE_EEVAR_FILE,
                SettingsKeys.CREATE_EE_FILE, SettingsKeys.CREATE_EXCEL_FILE);

        // order must be the same as above !
        List<SettingsKeys> files =
            Arrays.asList(SettingsKeys.GDVAR_FILE, SettingsKeys.GDF_FILE, SettingsKeys.EEVAR_FILE,
                SettingsKeys.EE_FILE, SettingsKeys.EXCEL_FILE);

        List<String> defaultFileNames =
            Arrays.asList(GDVAR_FILE_NAME, GDF_FILE_NAME, EEVAR_FILE_NAME, EE_FILE_NAME, EXCEL_FILE_NAME);

        Iterator<SettingsKeys> createsIt = creates.iterator();
        Iterator<SettingsKeys> filesIt = files.iterator();
        Iterator<String> defaultFileNamesIt = defaultFileNames.iterator();
        while (createsIt.hasNext()) {
            SettingsKeys create = createsIt.next();
            SettingsKeys file = filesIt.next();
            String defaultFileName = defaultFileNamesIt.next();
            if (wizardSettings.getBoolean(create.toString())) {
                String fileName = wizardSettings.get(file.toString());
                if (addPlatformFolder) {
                    fileName = addPlatformToPath(outputDir, defaultFileName, platformName);
                }
                outputFiles.add(new File(fileName));
            }
        }

        return outputFiles;
    }

    // Integrated with DefaultDataSheetProperties
    // private boolean consistencyCheckForSelectedPlatform() {
    // boolean resultIsOk = true;
    //
    // if (resultIsOk) {
    // resultIsOk = ddsp.readDefaultData();
    // }
    //
    // if (resultIsOk) {
    // resultIsOk = ddsp.dataConsistensyCheck();
    // }
    //
    // return resultIsOk;
    // }

    /**
     * This method creates output files for the selected platform, according to user's choices in
     * GUI.
     * 
     * Possible output to create: - GDVAR file - GDF file, possibly merged with a selected GDF file
     * - Customer 19062 document
     * 
     * @return True if this was done without errors
     */
    private boolean createOutputFilesForSelectedPlatform() {
        boolean resultIsOk = true;

        resultIsOk &= createEEFile();
        resultIsOk &= createEEVARFile();
        resultIsOk &= createGDVARFile();
        resultIsOk &= createGDFFile();
        resultIsOk &= createExcelFile();

        return resultIsOk;
    }

    private boolean createExcelFile() {
        if ((wizardSettings.getBoolean(SettingsKeys.CREATE_EXCEL_FILE.toString()))) {
            String excelFileName = wizardSettings.get(SettingsKeys.EXCEL_FILE.toString());
            boolean allPlatforms = (wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString()));

            if (allPlatforms) {
                String outputDir = wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString());
                excelFileName = addPlatformToPath(outputDir, EXCEL_FILE_NAME, ddsp.getPlatformNameToProcess());
            }

            return ddsp.createCustomerDocument(excelFileName);
        } else {
            return true;
        }
    }

    private boolean createGDVARFile() {
        if ((wizardSettings.getBoolean(SettingsKeys.CREATE_GDVAR_FILE.toString()))) {
            String gDVARFileName = wizardSettings.get(SettingsKeys.GDVAR_FILE.toString());
            boolean allPlatforms = (wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString()));

            if (allPlatforms) {
                String outputDir = wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString());
                gDVARFileName = addPlatformToPath(outputDir, GDVAR_FILE_NAME, ddsp.getPlatformNameToProcess());
            }

            return ddsp.createGDVARFile(gDVARFileName);
        } else {
            return true;
        }
    }

    private boolean createGDFFile() {
        if (!(wizardSettings.getBoolean(SettingsKeys.CREATE_GDF_FILE.toString()))) {
            return true;
        }

        String gDFFileName = wizardSettings.get(SettingsKeys.GDF_FILE.toString());
        boolean allPlatforms = wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString());

        if (allPlatforms) {
            String outputDir = wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString());
            gDFFileName = addPlatformToPath(outputDir, GDF_FILE_NAME, ddsp.getPlatformNameToProcess());
        }

        if ((wizardSettings.getBoolean(SettingsKeys.MERGE_INPUT_GDF_FILE.toString()))) {

            // Integrated with merge19062andGDFFileCreateNewGDFFile method
            // if
            // (!ddsp.readGDFFileFromDisk(wizardSettings.get(SettingsKeys.INPUT_GDF_FILE.toString())))
            // {
            // return false;
            // }

            if (!calibrationDataDialogShown) {
                final boolean includeCalibrationData = showCalibrateDialog();
                calibrationDataDialogShown = true;
                ddsp.setDocCreatorIncludeCalibrationData(includeCalibrationData);
            }

            final String inputGDFFileName = wizardSettings.get(SettingsKeys.INPUT_GDF_FILE.toString());

            return ddsp.merge19062andGDFFileCreateNewGDFFile(gDFFileName, inputGDFFileName);

        } else {
            if (!calibrationDataDialogShown) {
                final boolean includeCalibrationData = showCalibrateDialog();
                calibrationDataDialogShown = true;
                ddsp.setDocCreatorIncludeCalibrationData(includeCalibrationData);
            }
            return ddsp.createGDFFile(gDFFileName);
        }
    }

    private boolean createEEVARFile() {
        if ((wizardSettings.getBoolean(SettingsKeys.CREATE_EEVAR_FILE.toString()))) {
            String eEVARFileName = wizardSettings.get(SettingsKeys.EEVAR_FILE.toString());
            boolean allPlatforms = (wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString()));

            if (allPlatforms) {
                String outputDir = wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString());
                eEVARFileName = addPlatformToPath(outputDir, EEVAR_FILE_NAME, ddsp.getPlatformNameToProcess());
            }

            return ddsp.createEEVARFile(eEVARFileName);
        } else {
            return true;
        }
    }

    private boolean createEEFile() {
        if (!(wizardSettings.getBoolean(SettingsKeys.CREATE_EE_FILE.toString()))) {
            return true;
        }

        String eefFileName = null;
        final boolean allPlatforms = wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString());

        if (allPlatforms) {
            String outputDir = wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString());
            eefFileName = addPlatformToPath(outputDir, EE_FILE_NAME, ddsp.getPlatformNameToProcess());
        } else {
            eefFileName = wizardSettings.get(SettingsKeys.EE_FILE.toString()).toString();
        }

        File tmpFile;
        try {
            tmpFile = File.createTempFile("gdf_", "gdf");
        } catch (IOException e) {
            Activator.getDefault().getLog().log(new Status(IStatus.ERROR, Activator.PLUGIN_ID, e.getMessage()));
            return false;
        }

        if (!calibrationDataDialogShown) {
            final boolean includeCalibrationData = showCalibrateDialog();
            calibrationDataDialogShown = true;
            ddsp.setDocCreatorIncludeCalibrationData(includeCalibrationData);
        }

        if (!ddsp.createGDFFile(tmpFile.getAbsolutePath())) {
            return false;
        }

        // then create the EEF file: src is path to the tmp file, dst is the path to the EEF
        // file
        if (!ddsp.createEEFFile(tmpFile.getAbsolutePath(), eefFileName, true)) {
            return false;
        }

        return true;
    }

    /**
     * Adds the name of specified platform to the specified file name path, so that the specified
     * file name is placed in a sub directory named after the platform. If platform name doesn't
     * consist of letters, digits and allowed characters (specified in method), the illicit
     * character is replaced by '-'.
     * 
     * @param fileName
     *            The file name (without path)
     * @param topLevelDir
     *            The directory under which platform sub directory will be created
     * @param platformName
     *            The platform name to add as sub directory in path
     * @return the new path with platform name as sub directory
     */
    private static String addPlatformToPath(String topLevelDir, String fileName, String platformName) {
        String platformFileStr = null;

        String allowed = "-_. ";
        StringBuffer validatedPlatformBuffer = new StringBuffer();
        for (int i = 0; i < platformName.length(); i++) {
            char checkChar = platformName.charAt(i);
            if (Character.isLetterOrDigit(checkChar) || (allowed.indexOf(checkChar) != -1)) {
                validatedPlatformBuffer.append(checkChar);
            } else {
                validatedPlatformBuffer.append('-');
            }
        }
        String validatedPlatformName = validatedPlatformBuffer.toString();

        platformFileStr = topLevelDir + File.separator + validatedPlatformName + File.separator + fileName;

        return platformFileStr;
    }

    /**
     * Generates report file and opens workbench page.
     * 
     * @return true if successful, false otherwise
     */
    private static boolean generateReport(final ByteArrayOutputStream reportBytes) {
        File reportFile;
        FileOutputStream os = null;
        try {
            reportFile = File.createTempFile("ddt_report_", ".txt");

            os = new FileOutputStream(reportFile);
            reportBytes.writeTo(os);
            os.close();

            IFileStore fileStore = EFS.getLocalFileSystem().getStore(reportFile.toURI());
            IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();

            IDE.openEditorOnFileStore(page, fileStore);
        } catch (PartInitException e) {
            MessageBox messageBox =
                new MessageBox(PlatformUI.getWorkbench().getDisplay().getActiveShell(), SWT.ICON_ERROR | SWT.OK);
            messageBox.setMessage("Can not open report in editor: " + e);
            messageBox.setText("Error");
            return false;
        } catch (IOException e) {
            MessageBox messageBox =
                new MessageBox(PlatformUI.getWorkbench().getDisplay().getActiveShell(), SWT.ICON_ERROR | SWT.OK);
            messageBox.setMessage("Can not create report file: " + e);
            messageBox.setText("Error");
            return false;
        } finally {
            if (os != null) {
                try {
                    os.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return true;
    }

    /**
     * Shows dialog with calibrate question.
     */
    private static boolean showCalibrateDialog() {
        MessageBox messageBox =
            new MessageBox(PlatformUI.getWorkbench().getDisplay().getActiveShell(), SWT.ICON_QUESTION | SWT.YES
                | SWT.NO);
        messageBox.setMessage(CALIBRATION_DIALOG_MESSAGE);
        messageBox.setText(CALIBRATION_DIALOG_TEXT);

        final boolean result = (SWT.YES == messageBox.open());
        return result;
    }
}
