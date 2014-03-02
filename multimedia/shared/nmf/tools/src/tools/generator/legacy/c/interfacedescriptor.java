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
package tools.generator.legacy.c;

import java.io.File;
import java.io.PrintStream;
import java.util.HashSet;

import tools.idl.ast.*;

public class interfacedescriptor extends tools.generator.raw.itf 
{
    public interfacedescriptor(boolean _inFinalDirectory) {
        super(_inFinalDirectory, "host");
    }

    public void generate(Interface itf) 
    {
        // Check that interface is not compile twice during same compilation
        File out = getInterfaceExt(inFinalDirectory, subDirectory, itf, "h");

        // Generate interface header file ...
        PrintStream outh = needRegeneration(itf.lastModifiedFile, out);
        if(outh != null) {
            outh.println("/* '" + itf.name + "' interface */");
            outh.println("#if !defined(" + itf.name.replace('.', '_') + "_IDL)");
            outh.println("#define " + itf.name.replace('.', '_') + "_IDL");
            outh.println();

            outh.println("#include <cm/inc/cm_type.h>");        

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

            outh.print("typedef ");
            outh.println("struct sI" + itf.name_ + " {");
            outh.println("  void* THIS;");
            for(final Method md : itf.methods) 
            {
                outh.print("  ");
                outh.print(md.printReference(md.name, true, "t_cm_error"));
                outh.println(";");
            }
            outh.println("} I" + itf.name_ + ";");
            outh.println();

            // Generate interface descriptor
            outh.print("typedef ");
            outh.println("struct {");
            outh.println("  void* THIS;");
            for(final Method md : itf.methods) 
            {
                outh.print("  ");
                outh.print(md.printReference(md.name, true, null));
                outh.println(";");
            }
            outh.println("} CB" + itf.name_ + ";");
            outh.println();

            outh.println("#endif");      

            outh.close();
        }
    }
}
