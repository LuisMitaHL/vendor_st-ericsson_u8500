package com.stericsson.sdk.equipment.ui.contributions.handlers;

import java.util.List;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.expressions.EvaluationContext;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.jobs.ProcessFileJob;

/**
 * Handler for the process file menu contribution. This is called when right clicking a file and
 * selecing Process File
 * 
 * @author xolabju
 * 
 */
public class ProcessFileHandler extends AbstractHandler {

    /**
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
            String port = event.getParameter("com.stericsson.sdk.equipment.ui.commands.processfile.equipmentparam");
            IEquipment equipment = getEquipment(port);
            if (equipment != null) {
                IPath absolutePath = file.getRawLocation();
                if (absolutePath != null) {
                    ProcessFileJob job = new ProcessFileJob(absolutePath.toOSString(), equipment);
                    job.schedule();
                }
            }
        }
        return null;
    }

    private IEquipment getEquipment(String port) {
        IEquipment equipment = null;
        if (port != null) {
            try {
                BundleContext bundleContext = Activator.getBundleContext();
                ServiceReference[] serviceReferences =
                    bundleContext.getServiceReferences(IEquipment.class.getName(), "(port=" + port + ")");
                if (serviceReferences != null && serviceReferences.length > 0) {
                    equipment = (IEquipment) bundleContext.getService(serviceReferences[0]);
                    bundleContext.ungetService(serviceReferences[0]);
                }

            } catch (InvalidSyntaxException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        return equipment;
    }

}
