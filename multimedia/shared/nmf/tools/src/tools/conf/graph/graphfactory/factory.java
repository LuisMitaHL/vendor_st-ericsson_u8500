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
import java.io.PrintStream;
import java.util.Stack;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.targetfactory;
import tools.common.targetfactory.CodingStyle;
import tools.common.configuration.Debug;
import tools.conf.ast.astdefinition;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.asttype;
import tools.conf.generator.dotter;
import tools.conf.graph.component;
import tools.generator.api.BoundariesBinderInterface;

public class factory {
    /**
     * The mostRecentSource allow to detect if file need to be regenerated. 
     * Nevertheless, this variable must not be used for detecting C compilation and linking 
     * since it's possible that sources depend on other dependencies not managed by NMF.
     */
    public File mostRecentFile;
    
    /**
     * Instantiate a graph according to level component description name.
     * @param componentType
     * @param ctx
     * @return
     * @throws compilerexception
     */
    public component instanciate(String componentType, boolean needdot) throws compilerexception {
        // Load the type
        return instanciate(
                tools.conf.confcompiler.Compile(componentType, null),
                needdot);
    }

    /**
     * Instantiate a graph according to level component description.
     * @param sc
     * @param ctx
     * @return
     * @throws compilerexception
     */
    public component instanciate(asttype sc) throws compilerexception {
        return instanciate(sc, false);
    }
     
    void bindDependencies(
            asttype _type, 
            Stack<asttype> stack,
            PrintStream out) throws compilerexception {

        for(astrequire require : _type.getLocalRequires())
        {
            out.println("   requires " + require.type + " as " + require.getInterfaceName() + (require.optional ? " optional" : ""));
            for(int n = 0; n < require.numberOfElement; n++) {
                out.println("   binds primitive." + require.getInterfaceName(n) + " to this." + require.getInterfaceName(n));
            }
            out.println();
        }
        for(astprovide provide : _type.getLocalProvides())
        {
            if(! provide.type.startsWith("lifecycle.") && ! provide.interrupt)
            {
                out.println("   provides " + provide.type + " as " + provide.getInterfaceName());
                for(int n = 0; n < provide.numberOfElement; n++) {
                    out.println("   binds this." + provide.getInterfaceName(n) + " to primitive." + provide.getInterfaceName(n));
                }
                out.println();
            }
        }

        for(astdefinition extend : _type.getLocalExtends()) 
        {
            asttype implementsType = tools.conf.confcompiler.Compile(extend.type, extend);
            
            // Check that there is no extends in loop
            if(stack.contains(implementsType))
                throw new compilerexception(_type, error.LOOP_IN_EXTENDS,
                        stack);
            stack.push(implementsType);

            bindDependencies(implementsType, stack, out);
            
            stack.pop();
        }
    }

    public component wrapPrimitive(astprimitive sc, String finalCompName) throws compilerexception
    {
        if(targetfactory.style == CodingStyle.CPP)
        {
            configuration.debug(Debug.step, "WRAP primitive + '" + sc.fullyname + "'");

            PrintStream out = configuration.needRegeneration(sc.parsedFile, 
                    configuration.getSourceFile(finalCompName.replace('.', File.separatorChar) + File.separator + "component.adl"));
            if(out != null)
            {
                out.println("composite " + finalCompName);
                if(((astprimitive)sc).singleton)
                    out.println("   singleton");

                out.println("{");
                out.println("   contains " + sc.fullyname + " as primitive priority = priority");
                out.println();

                bindDependencies(sc, new Stack<asttype>(), out);

                out.println("}");
            }
        }
        
        return instanciate(finalCompName, false);
    }
    
    public component instanciate(asttype sc, boolean needdot) throws compilerexception {
        if(targetfactory.style == CodingStyle.CPP && sc instanceof astprimitive) 
        {
            // Re-enter on my self !!
            return wrapPrimitive((astprimitive)sc, sc.fullyname + "Wrapped");
        }
        
        // Instantiate component in graph 
        configuration.debug(Debug.step, "INSTANTIATE composition + '" + sc.fullyname + "'");
        instantiator instantiator = new instantiator();
        component comp = instantiator.instanciateComponent(null, null, sc);
        mostRecentFile = instantiator.mostRecentFile;

        // Check interface validity 
        (new checkvar()).browseComponent(comp);

        // Check interface validity 
        checkinterface checkinterface = new checkinterface();
        checkinterface.browseComponent(comp);
        mostRecentFile = configuration.getMostRecent(mostRecentFile, checkinterface.mostRecentFile);

        // Check binding validity 
        (new checkbinding()).browseComponent(comp);

        // Generate basic graph
        if(needdot)
            (new dotter()).Generate(comp, "");
        
        // Bind component together
        (new binder()).browseComponent(comp);

        BoundariesBinderInterface binder = targetfactory.getBoundariesBinder();
        if(binder != null)
        {
            configuration.debug(Debug.step, "Bind boundaries '" + comp.type.fullyname + "'");
            binder.browseComponent(comp);
        }
        
        // Generate populate graph
        if(needdot)
            (new dotter()).Generate(comp, "full.");
        
        return comp;
    }

}
