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
package tools.generator.c;

import java.io.File;
import java.io.PrintStream;
import java.util.HashSet;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.targetfactory;
import tools.conf.graph.component;
import tools.conf.graph.primitive;
import tools.conf.graph.visitor;
import tools.conf.graph.computer.lifecycle;
import tools.generator.api.state.constructor;
import tools.generator.api.state.instantiator;

public class CAssembleGenerator implements tools.generator.api.AssembleGeneratorInterface 
{
    enum Action {setShared, declare, construct, destroy, start, stop};
    
    public HashSet<String> sharedVariables = new HashSet<String>();
    public HashSet<String> sharedIncluded = new HashSet<String>();

    class initializerOfLink extends visitor 
    {
        PrintStream out;
        Action action;
        
        public initializerOfLink(PrintStream _out, Action _action)
        {
            out = _out;
            action = _action;
            if(action == Action.destroy || action == Action.stop)
                reserveOrder = true;
        }
        
        protected void travelPrimitive(primitive primitive, Object data) throws compilerexception 
        {     
            // Add specific initialization if need !
            if(action == Action.setShared)
            {
                if (primitive instanceof instantiator)
                {
                    ((instantiator)primitive).declareSharedVariable(sharedVariables);  
                }
                if(primitive instanceof constructor)
                {
                    ((constructor)primitive).includeFile(sharedIncluded);
                }
            }
            else if(action == Action.declare) 
            {
                if (primitive instanceof instantiator) 
                {
                    ((instantiator)primitive).declareInstance(out);  
                }
                if (primitive.getProvide("constructor") != null) 
                    out.println("extern t_nmf_error " + primitive.type.fullyname_ + "_construct(void);");
                if (primitive.getProvide("starter") != null) 
                    out.println("extern void " + primitive.type.fullyname_ + "_start(void);");
                if (primitive.getProvide("stopper") != null) 
                    out.println("extern void " + primitive.type.fullyname_ + "_stop(void);");
                if (primitive.getProvide("destructor") != null) 
                    out.println("extern void " + primitive.type.fullyname_ + "_destroy(void);");

            }
            else if(action == Action.construct)
            {
                if (primitive instanceof constructor) 
                    ((constructor)primitive).constructInstance(out);
                if (primitive.getProvide("constructor") != null) 
                {
                    out.println("  if((error = " + primitive.type.fullyname_ + "_construct()) != NMF_OK)");
                    out.println("    return error;");
                }
            } 
            else if(action == Action.start)
            {
                if (primitive.getProvide("starter") != null) 
                    out.println("  " + primitive.type.fullyname_ + "_start();");
            } 
            else if(action == Action.stop) 
            {
                if (primitive.getProvide("stopper") != null) 
                    out.println("  " + primitive.type.fullyname_ + "_stop();");
            } 
            else if(action == Action.destroy)
            {
                if (primitive instanceof constructor) 
                    ((constructor)primitive).destroyInstance(out);
                if (primitive.getProvide("destructor") != null) 
                    out.println("  " + primitive.type.fullyname_ + "_destroy();");
            }
        }
    }

    public void generate(component instance, File dstfile, lifecycle lccstate) throws compilerexception
    {
        PrintStream out = configuration.forceRegeneration(dstfile);

        out.println("/* Static composition of " + instance.type.fullyname + " */");
        out.println("#include <inc/type.h>");
        out.println("#include <c.h>");
        (new initializerOfLink(out,  Action.setShared)).browseComponent(instance);
        for(final String include : sharedIncluded)
            out.println("#include <" + include + ">");
        out.println();
        
        for(final String variable: sharedVariables)
            out.println(variable);
        out.println();

        /*
         * Declare primitive component instance
         */
        out.println("/*");
        out.println(" *Declare primitive component instance and methods");
        out.println(" */");
        (new initializerOfLink(out,  Action.declare)).browseComponent(instance);
        out.println();

        if(lccstate.needConstuctor && !(instance instanceof primitive))
        {
            if(targetfactory.linktype.uniqueLCCMethodName)
                out.println("t_nmf_error construct(void) {");
            else
                out.println("t_nmf_error " + instance.type.fullyname_ + "_construct(void) {");
            out.println("  t_nmf_error error = NMF_OK;");
            (new initializerOfLink(out, Action.construct)).browseComponent(instance);
            out.println("  return error;");
            out.println("}");
            out.println();
        }

        if(lccstate.needStarter && !(instance instanceof primitive)) 
        {
            if(targetfactory.linktype.uniqueLCCMethodName)
                out.println("void start(void) {");
            else
                out.println("void " + instance.type.fullyname_ + "_start(void) {");
            (new initializerOfLink(out, Action.start)).browseComponent(instance);
            out.println("}");
            out.println();
        }

        if(lccstate.needStopper && !(instance instanceof primitive))
        {
            if(targetfactory.linktype.uniqueLCCMethodName)
                out.println("void stop(void) {");
            else
                out.println("void " + instance.type.fullyname_ + "_stop(void) {");
            (new initializerOfLink(out, Action.stop)).browseComponent(instance);
            out.println("}");
            out.println();
        }

        if(lccstate.needDestructor && !(instance instanceof primitive))
        {
            if(targetfactory.linktype.uniqueLCCMethodName)
                out.println("void destroy(void) {");
            else
                out.println("void " + instance.type.fullyname_ + "_destroy(void) {");
            (new initializerOfLink(out, Action.destroy)).browseComponent(instance);
            out.println("}");
            out.println();
        }

        out.close();
    }
}
