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
package tools.generator.c.Embedded;

import java.io.IOException;
import java.io.PrintStream;
import java.util.HashMap;
import java.util.Set;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astsource;
import tools.conf.graph.binding.bc;
import tools.generator.api.StubGeneratorInterface;
import tools.generator.api.state.constructor;
import tools.generator.api.state.instantiator;
import tools.idl.ast.Interface;
import tools.idl.ast.Method;
import tools.idl.ast.Parameter;
import tools.idl.generator.stubclient;

public class EmbeddedStubGenerator extends stubclient implements StubGeneratorInterface
{
    public class stubprimitive extends bc implements constructor, instantiator 
    {
        public stubprimitive(astbasefile _declaration,
                String _name,
                Interface _itf, 
                astprimitive _type) throws compilerexception, IOException {
            super(_declaration, _name, SchedPriority.Normal, _type);
        }

        public void includeFile(Set<String> include) {
        }
        public void declareSharedVariable(Set<String> variables) {
        }

        public void declareInstance(PrintStream out) {
            out.println("  // Declare STUBSTUB");
        }

        public void constructInstance(PrintStream out) {
            out.println("  // Construct STUBSTUB");
        }

        public void destroyInstance(PrintStream out) {
        }
    }
    

    public bc getPrimitive(astbasefile _declaration, String _name) throws compilerexception {
        try
        {
            astprimitive type = registerAst();
            return new stubprimitive(_declaration, _name, itf, type);
        } 
        catch (IOException e)
        {
            throw new compilerexception(_declaration, e.getMessage());
        }
    }
    
    private static HashMap<Interface, astprimitive> registeredBCs = new HashMap<Interface, astprimitive>();

    public astprimitive registerAst() throws compilerexception, IOException {
        astprimitive type = registeredBCs.get(itf);
        if(type == null) {
            type = createAst();
            registeredBCs.put(itf, type);
        } else {
            type = new astprimitive(type);
        }
        
        return type;
    }
    
    protected astprimitive createAst() throws compilerexception 
    {
        astprimitive type = new astprimitive(null, "_st." + itf.name);

        String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
        astsource source = new astsource(
                configuration.getComponentTemporaryFile(type, "st-" + smallname + ".c"));
        PrintStream out = configuration.needRegeneration(itf.lastModifiedFile, source.file);
        if(out != null) 
        {
            out.println("/* Generated host stub for '" + itf.name + "' */");
            out.println("#include <_st/" + itf.nameslash + ".nmf>");
            out.println();
            
            for(Method md: itf.methods) 
            {
                out.println(md.printMethod("SKELC", "", false, false, false) + ";");

                out.println(md.printMethodMETH() + " {");
                out.print("  SKELC_" + md.name + "(");
                for(int k = 0; k < md.parameters.size(); k++)
                {
                    Parameter fp = md.parameters.get(k);
                    if(k > 0) out.print(", ");
                    out.print(fp.name);
                }
                out.println(");");
                out.println("}");
                out.println();
            }
            
            out.close();
        }

        // Fill the type
        type.addProvide(new astprovide(null, 0, 0, itf.name, "target"));
        type.sources.add(source);

        return type;
    }
}
