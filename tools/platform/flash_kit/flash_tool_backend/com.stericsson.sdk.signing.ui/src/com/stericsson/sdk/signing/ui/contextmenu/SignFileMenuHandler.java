package com.stericsson.sdk.signing.ui.contextmenu;

import java.util.List;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.expressions.EvaluationContext;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.ui.PlatformUI;
import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.ui.Activator;
import com.stericsson.sdk.signing.ui.utils.SignedSoftwareUtils;
import com.stericsson.sdk.signing.ui.wizards.signedsoftware.SignJobsUtils;
import com.stericsson.sdk.signing.ui.wizards.signedsoftware.SigningWizard;

/**
 * 
 * @author MielcLuk
 * 
 */
public class SignFileMenuHandler extends AbstractHandler {

    /**
     * 
     * {@inheritDoc}
     */
    public Object execute(ExecutionEvent event) throws ExecutionException {
        IFile file = null;
        EvaluationContext context = (EvaluationContext) event.getApplicationContext();
        if (context == null || !(context.getDefaultVariable() instanceof List<?>)) {
            return null;
        }
        List<?> defaultVariable = (List<?>) context.getDefaultVariable();
        if (defaultVariable.isEmpty()) {
            return null;
        }
        Object variable = defaultVariable.get(0);
        if (variable != null && variable instanceof IFile) {
            file = (IFile) variable;
        }
        if (file != null) {
            boolean useDefaultSignPackage =
                Activator.getDefault().getPreferenceStore().getBoolean(
                    FlashKitPreferenceConstants.SIGNING_USE_DEFAULT_SIGN_PACKAGE);
            final String filePath = getFilePath(event);
            final String payloadTypeName =
                event.getParameter("com.stericsson.sdk.signing.ui.contextmenu.commandPayloadTypeParameter");
            if (useDefaultSignPackage) {

                final String defaultSignPackage =
                    Activator.getDefault().getPreferenceStore().getString(
                        FlashKitPreferenceConstants.SIGNING_DEFAULT_SIGN_PACKAGE);
                final GenericSoftwareType payloadType = SignedSoftwareUtils.getSofwareType(payloadTypeName);
                SignJobsUtils.runSignJob(filePath, payloadType, defaultSignPackage, SignedSoftwareUtils
                    .getDefaultOutputPath(filePath), file.getProject());
            } else {
                SigningWizard wizard =
                    new SigningWizard(filePath, payloadTypeName, file.getProject());
                WizardDialog dialog =
                    new WizardDialog(PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(), wizard);
                dialog.create();
                dialog.open();
            }

        }
        return null;
    }

    private String getFilePath(ExecutionEvent event) {
        IFile file = null;
        EvaluationContext context = (EvaluationContext) event.getApplicationContext();
        if (context == null || !(context.getDefaultVariable() instanceof List<?>)) {
            return null;
        }
        List<?> defaultVariable = (List<?>) context.getDefaultVariable();
        if (defaultVariable.isEmpty()) {
            return null;
        }
        Object variable = defaultVariable.get(0);

        if (variable != null && variable instanceof IFile) {
            file = (IFile) variable;
        }
        if (file != null) {
            IPath absolutePath = file.getRawLocation();
            if (absolutePath != null) {
                return absolutePath.toOSString();
            }
        }
        return null;
    }
}
