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
package tools.generator.cpp.ee;

import java.io.IOException;
import java.io.PrintStream;
import java.util.Set;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.confcompiler;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astattribute;
import tools.conf.ast.astrequire;
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

public class CppEEStubGenerator extends stubclient implements StubGeneratorInterface
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
    
    public astprimitive registerAst() throws compilerexception 
    {
        String compName = "_stl." + itf.name;
        astprimitive type;

        type = (astprimitive)confcompiler.getPreviouslyCompile(compName);
        if(type == null)
        {
            type = new astprimitive(itf.lastModifiedFile, compName);

            String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);

            PrintStream outh = configuration.needRegeneration(itf.lastModifiedFile, 
                    configuration.getComponentTemporaryFile(type, "inc/" + smallname + ".hpp"));
            if(outh != null) {
                outh.println("/* Generated stub bc for '" + itf.name + "' */");
                outh.println("#include <ee.hpp>");

                //define class
                outh.println("class " + type.fullyname_ + ": public " + type.fullyname_ + "Template {");
                outh.println("  public:");

                //menbers
                outh.println("    t_nmf_fifo_id FIFO;");
                outh.println("    Iee_api_communication Icoms;");
                outh.println();

                //constructor
                outh.println("    " + type.fullyname_ + "() {");
                if (maxnb16 == 0)
                    outh.println("      FIFO = (t_nmf_fifo_id) 4;");
                else
                    outh.println("      FIFO = (t_nmf_fifo_id)" + maxnb16 * 2 + ";");
                outh.println("      eeNetwork.getInterface(\"coms\", &Icoms);"); //TODO : add assert on null
                outh.println("    }");
                outh.println();
                outh.println("  protected:");
                for(Method md : itf.methods) {
                    outh.println("    virtual " + md.printMethodCpp("", false) + ";");
                }
                outh.println("};");
                outh.println();

                outh.close();
            }

            astsource source = new astsource(
                    configuration.getComponentTemporaryFile(type, "st-" + smallname + ".cpp"));
            PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
            if(outc != null) 
            {
                outc.println("/* Generated stub for '" + itf.name + "' */");
                outc.println("#include <_stl/" + itf.nameslash + ".nmf>");
                outc.println();

                for(int j = 0; j < itf.methods.size(); j++) {
                    Method md = (Method)itf.methods.get(j);

                    /*
                     * Generate all method stubs for mpc side
                     */ 
                    outc.println(md.printMethodMETH() + " {");
                    outc.println("    t_uint16 *_xyuv_data = (t_uint16 *) Icoms.allocEvent(ATTR(FIFO));");
                    //TODO : check if empty ?
                    //declare variables
                    for(int n = 0; n < needi[j]; n++)
                        outc.println("    int ___i" + n + ", ___j" + n +";");
                    // Marshall parameter
                    int nb16 = 0;
                    for(int k = 0; k < md.parameters.size(); k++) 
                    {
                        Parameter fp = md.parameters.get(k);
                        outc.println("    /* " + fp.name + " <" + fp.type.printType() + "> marshalling */");
                        nb16 += writeData(fp.type, fp.name, outc, Integer.toString(nb16), "    ", 0);
                    }
                    outc.println("    /* Event sending */");
                    outc.println("    Icoms.pushEvent(ATTR(FIFO), (void *)_xyuv_data, " + j + ");");

                    outc.println("}");
                }
                outc.println();

                outc.close();
            }

            // Fill the type
            tools.idl.idlcompiler.Compile("ee.api.communication", null);

            // provide itf
            type.addProvide(new astprovide(null, 0, 0, itf.name, "source"));
            type.sources.add(source);

            //require itf
            astrequire reqsched = new astrequire(null, 0, 0, "ee.api.communication", "coms");
            reqsched.optional = true;
            type.addRequire(reqsched);

            //add attributes
            type.addAttribute(new astattribute(null, 0,0,"FIFO"));

            confcompiler.registerComponent(type);
        }

        return type;
    }
}
