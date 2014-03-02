/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * This code is part of the NMF Toolset.
 *
 * The NMF Toolset is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The NMF Toolset is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the NMF Toolset. If not, see <http://www.gnu.org/licenses/>.
 *
 */
package tools.conf.graph.graphfactory;

import java.io.File;
import java.io.IOException;
import java.util.Stack;

import tools.common.*;
import tools.common.targetfactory.DomainNames;
import tools.conf.ast.*;
import tools.conf.graph.*;
import tools.conf.graph.binding.bc;
import tools.generator.generatorfactory;
import tools.generator.api.SkeletonGeneratorInterface;
import tools.generator.api.StubGeneratorInterface;
import tools.idl.idlcompiler;

public class instantiator extends configuration {
    public File mostRecentFile;

    private String level = "";
    private Stack<composite> callstack = new Stack<composite>();
    
    private final int MAX_PRIORITY_MPCEE = 2;
    private final int MAX_PRIORITY_HOSTEE = 31;

    private void instanciateComposite(composite instance) throws compilerexception {
        callstack.push(instance);
        level = level + "  ";           // Add "  "
        
        // Instantiate all composite contents
        contentloop:
        for (astcontent content :instance.getContents()) {
            debug(Debug.comp, level + content.typedefinition.type + " as " + content.name);

            // Check that component not instantiate twice
            if(instance.getFcSubComponent(content.name) != null) {
                throw new compilerexception(content, error.SUBCOMPONENT_INSTANTIATE_TWICE,
                        content.name, (instance.owner != null) ? instance.getPathName() : instance.type.fullyname);
            }

            // Check that we not go though loop in instantiation
            for(final composite c : callstack) 
            {
                if(c.type.fullyname.equals(content.typedefinition.type))
                    throw new compilerexception(content, error.LOOP_IN_NETWORK,
                            callstack);
            }

            /*
             * Check priority
             * [0..2] or [0 .. realtime]
             */
            int maxpriority;
            if(targetfactory.domain == DomainNames.HostEE) 
                maxpriority = MAX_PRIORITY_HOSTEE;
            else
                maxpriority = MAX_PRIORITY_MPCEE;

            if(content.priority.getOrdinal() > maxpriority) 
                throw new compilerexception(content, error.PRIORITY_NOT_SUPPORTED, content.priority, maxpriority);

            
            if(content.mpc == null) 
            {
                asttype sctype = null;
                if(content.typedefinition.type.startsWith("_st."))
                {
                    StubGeneratorInterface stubgen = targetfactory.getStubGenerator();
                    if(stubgen != null)
                    {
                        stubgen.setItf(idlcompiler.Compile(content.typedefinition.type.substring(4), content));
                        bc scbc = stubgen.getPrimitive(content, content.name);
                        
                        ast2instance a2i = new ast2instance(scbc);
                        a2i.createCanonical(null, null, null);
                        instance.addFcControllerComponent(scbc);
                        
                        continue contentloop;
                    }
                } 
                else
                {
                    if(content.typedefinition.type.startsWith("_sk."))
                    {
                        SkeletonGeneratorInterface skelgen = targetfactory.getSkeletonGenerator();
                        if(skelgen != null)
                        {
                            skelgen.setItf(idlcompiler.Compile(content.typedefinition.type.substring(4), content));
                            bc scbc = skelgen.getPrimitive(content, content.name);
                            
                            ast2instance a2i = new ast2instance(scbc);
                            a2i.createCanonical(null, null, null);
                            instance.addFcControllerComponent(scbc);
                            
                            continue contentloop;
                        }
                    } 
                }

                sctype = tools.conf.confcompiler.Compile(content.typedefinition.type, content);
                
                instanciateComponent(instance, 
                        content, 
                        sctype);
            } else
            { 
                // TODO : check that content don't provide attribute nor template
                if(targetfactory.domain != DomainNames.HostEE) 
                    throw new compilerexception(content, error.MPC_SPECIFIED_IN_C);
                
                if(content.priority.getOrdinal() > 2)
                    throw new compilerexception(content, error.PRIORITY_NOT_SUPPORTED, content.priority, 2);
                
                // Since we are creating a distributed network from the Host, do not go through MPC composition !!!
                asttype sc = tools.conf.confcompiler.Compile(content.typedefinition.type, content);
                if(sc instanceof astcomponent) 
                {
                    debug(Debug.comp, "  Virtualize " + content.name);

                    tools.generator.cpp.hostee.bc.proxycomp proxy = new tools.generator.cpp.hostee.bc.proxycomp(content, (astcomponent)sc);
                    
                    ast2instance a2i = new ast2instance(proxy);
                    a2i.createCanonical(content, null, null);
                    mostRecentFile = configuration.getMostRecent(mostRecentFile, a2i.mostRecentFile);

                    instance.addFcSubComponent(proxy);    
                    debug(Debug.comp, "  ... Done ");
                } else {
                    throw new compilerexception(content, error.INSTANTIATE_TYPE);
                }
            }
        }
        
        level = level.substring(level.length() - 2); // Remove "  "
        callstack.pop();
    }

    protected component instanciateComponent(composite composite, astcontent content, asttype sc) throws compilerexception {
        component instance;
        
        if (sc instanceof astcomposite) 
        {
            instance = new composite(content, (astcomposite)sc);
    
            ast2instance a2i = new ast2instance(instance);
            a2i.createCanonical(content, 
                    (content != null) ? content.typedefinition.templates : null, 
                            (content != null) ? content.typedefinition.attributes : null);
            mostRecentFile = configuration.getMostRecent(mostRecentFile, a2i.mostRecentFile);
            
            instanciateComposite((composite)instance);
        } 
        else if(sc instanceof astprimitive)
        {
            try {
                instance = generatorfactory.createInstance(content, (astprimitive)sc);
            } catch (IOException e) {
                throw new compilerexception(content, e.getMessage());
            }

            ast2instance a2i = new ast2instance(instance);
            a2i.createCanonical(content, 
                    null,
                    (content != null) ? content.typedefinition.attributes : null);
            mostRecentFile = configuration.getMostRecent(mostRecentFile, a2i.mostRecentFile);
        } 
        else 
        {
            throw new compilerexception(content, error.INSTANTIATE_TYPE);
        }
        
        if(composite != null)
            composite.addFcSubComponent(instance);      
        
        return instance;
    }
}
