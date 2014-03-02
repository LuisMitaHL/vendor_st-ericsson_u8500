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
package tools.generator.cpp;

import java.io.File;
import java.io.PrintStream;
import java.util.HashSet;

import tools.idl.ast.*;

public class itf extends tools.generator.raw.itf 
{
    public itf(boolean _inFinalDirectory, String _subDirectory) {
        super(_inFinalDirectory, _subDirectory);
    }
    
    public void generate(Interface itf) 
    {
        // Check that interface is not compile twice during same compilation
        File out = getInterfaceExt(inFinalDirectory, subDirectory, itf, "hpp");

        // Generate interface header file ...
        PrintStream outh = needRegeneration(itf.lastModifiedFile, out);
        if(outh != null) {
            outh.println("/* '" + itf.name + "' interface */");
            outh.println("#if !defined(" + itf.name.replace('.', '_') + "_IDL)");
            outh.println("#define " + itf.name.replace('.', '_') + "_IDL");
            outh.println();

            outh.println("#include <cpp.hpp>");

            // Generate defined type
            HashSet<Interface> incls = new HashSet<Interface>();
            HashSet<Type> resolvedType = new HashSet<Type>();
            genDefinition(itf, incls, outh);

            // Include interface used as type
            for(final Method md : itf.methods) 
            {
                includeItf(resolvedType, md.type, incls, outh);
                for(final Parameter fp : md.parameters) 
                {
                    includeItf(resolvedType, fp.type, incls, outh);
                }
            }
            outh.println();

            outh.println("class " + itf.name_ + "Descriptor: public NMF::InterfaceDescriptor {");
            outh.println("  public:");
            for(Method md :  itf.methods) {
                outh.println("    virtual " + md.printMethodCpp("", false) + " = 0;");
            }
            outh.println("};");
            outh.println();

            outh.println("class I" + itf.name_ + ": public NMF::InterfaceReference {");
            outh.println("  public:");
            outh.println("    I" + itf.name_ + "(NMF::InterfaceDescriptor* _reference = 0):");
            outh.println("      InterfaceReference(_reference) {}");
            outh.println();
            for(Method md :  itf.methods) 
            {
                outh.println("    " + md.printMethodCpp("", false) + " {");
                outh.print("      ");
                if(! (md.type instanceof TypeVoid))
                    outh.print("return ");
                outh.print("((" + itf.name_ + "Descriptor*)reference)->" + md.name + "(");

                boolean second = false;
                for(Parameter fp : md.parameters) 
                {
                    if(second)
                        outh.print(", ");
                    outh.print(fp.name);
                    second = true;
                }
                outh.println(");");
                outh.println("    }");
            }
            outh.println("};");
            outh.println();

            outh.println("#endif");      

            outh.close();
        }
    }
}
