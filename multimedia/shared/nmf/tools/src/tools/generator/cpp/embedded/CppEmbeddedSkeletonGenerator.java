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
package tools.generator.cpp.embedded;

import java.io.IOException;
import java.io.PrintStream;
import java.util.Set;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.confcompiler;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.graph.binding.bc;
import tools.generator.api.SkeletonGeneratorInterface;
import tools.generator.api.state.constructor;
import tools.generator.api.state.instantiator;
import tools.idl.ast.Interface;
import tools.idl.ast.Method;
import tools.idl.ast.Parameter;
import tools.idl.generator.stubserver;

public class CppEmbeddedSkeletonGenerator extends stubserver implements SkeletonGeneratorInterface
{
    public class stubprimitive extends bc implements constructor, instantiator 
    {
        public stubprimitive(astbasefile _declaration,
                String _name,
                Interface _itf, 
                astprimitive _type) throws compilerexception, IOException {
            super(_declaration, _name, SchedPriority.Normal, _type);
        }

        public void includeFile(Set<String> include) 
        {
        }

        public void declareSharedVariable(Set<String> variables) 
        {
        }

        public void declareInstance(PrintStream out) 
        {
            out.println("  // Declare SKELETON");
        }

        public void constructInstance(PrintStream out) 
        {
            out.println("  // Construct SKELETON");
        }

        public void destroyInstance(PrintStream out) 
        {
        }
    }


    public bc getPrimitive(astbasefile _declaration, String _name) throws compilerexception 
    {
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
        String compName = "_skl." + itf.name;
        astprimitive type;

        type = (astprimitive)confcompiler.getPreviouslyCompile(compName);
        if(type == null)
        {
            type = new astprimitive(itf.lastModifiedFile, compName);

            String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);

            PrintStream outh = configuration.needRegeneration(itf.lastModifiedFile, 
                    configuration.getComponentTemporaryFile(type, "inc/" + smallname + ".hpp"));
            if(outh != null) {
                outh.println("/* Generated event bc for '" + itf.name + "' */");

                outh.println("class " + type.fullyname_ + ";");
                outh.println("extern " + type.fullyname_ + " *THIS" + type.fullyname_ + ";");

                outh.println("class " + type.fullyname_ + ": public " + type.fullyname_ + "Template {");
                outh.println("    public:");
                outh.println("    " + type.fullyname_ + "() {");
                outh.println("        THIS" + type.fullyname_ + " = this;");
                outh.println("    }");
                outh.println();

                outh.println("};");
                outh.close();
            }

            astsource source = new astsource(
                    configuration.getComponentTemporaryFile(type, "sk-" + smallname + ".cpp"));
            PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
            if(outc != null) 
            {
                outc.println("/* Generated dsp skeleton for '" + itf.name + "' */");
                outc.println("#include <_skl/" + itf.name.replace('.', '/') + ".nmf>");
                outc.println();

                outc.println(type.fullyname_ + " *THIS" + type.fullyname_ + ";");
                outc.println();

                for(Method md: itf.methods) 
                {
                    outc.println("extern \"C\" " + md.printMethod("SKELC", "", false, false, false) + "{");

                    outc.print("  THIS" + type.fullyname_ + "->target." + md.name+ "(");
                    // Call target
                    for(int k = 0; k < md.parameters.size(); k++) {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        if(k > 0) outc.print(", ");
                        outc.print(fp.name);
                    }
                    outc.println(");");

                    outc.println("}");
                }

                outc.close();
            }

            // Fill the type
            type.addRequire(new astrequire(null, 0, 0, itf.name, "target"));
            type.sources.add(source);

            confcompiler.registerComponent(type);
        }

        return type;
    }
}
