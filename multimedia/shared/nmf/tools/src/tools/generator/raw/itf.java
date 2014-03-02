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
package tools.generator.raw;

import java.io.File;
import java.io.PrintStream;
import java.util.HashSet;

import tools.common.configuration;
import tools.idl.ast.*;
import tools.idl.generator.predefinemethod;

public abstract class itf extends configuration
{
    protected boolean inFinalDirectory;
    protected String subDirectory;

    public itf(boolean _inFinalDirectory, String _subDirectory) 
    {
        inFinalDirectory = _inFinalDirectory;
        if(_subDirectory != null && ! _subDirectory.equals(""))
            subDirectory = _subDirectory + File.separator;
        else
            subDirectory = "";
    }
    
   protected void includeItf(HashSet<Type> inResolving, Type type, HashSet<Interface> incls, PrintStream out) 
   {
       if(inResolving.contains(type))
           return;
       inResolving.add(type);
        if(type.typedefinition instanceof typeinterface) {
            typeinterface ti = (typeinterface)type.typedefinition;
            if(predefinemethod.GenerateInclude(ti.itf, out, incls)) {
                generate(ti.itf);
            }
        } else if (type instanceof structure) {
            structure s = (structure)type;
            for(int k = 0; k < s.members.size(); k++) {
                structentry se = s.members.get(k);
                includeItf(inResolving, se.type, incls, out);
            }
        } else  if (type instanceof typedef) {
            typedef td = (typedef)type;
            includeItf(inResolving, td.type, incls, out);
        } else  if (type instanceof TypePointer) {
            TypePointer tp = (TypePointer)type;
            includeItf(inResolving, tp.type, incls, out);
        } else if (type instanceof typereference) {
            typereference tr = (typereference) type;
            includeItf(inResolving, tr.type, incls, out);
        } else if (type instanceof typearray)  {
            typearray array = (typearray) type;
            includeItf(inResolving, array.type, incls, out);
        }
    }

    protected void genDefinition(definedtype idt, HashSet<Interface> incls, PrintStream out) 
    {
        for(include incl : idt.includes) {
            if(incl.definition != null) {

                // Include idt
                out.println("#include <" + subDirectory + incl.filename + ".h>");

                // ... and regenerate it if necessary
                GenerateIdt(incl.definition);
            } else {
                out.println("#include <" + incl.filename + ">");

            }
        }
        out.println();

        for(final definition def : idt.definitions) 
        {
            // Include interface used
            includeItf(new HashSet<Type>(), def, incls, out);
            
            // .. And define structure
            if(def instanceof diesedefine)
                out.println(def.printDefinition());
            else       
                out.println(def.printDefinition() + ";");
            out.println();
        }
    }

    /**
     * 
     * @param idt
     * @param thisAsParameter
     */
    protected void GenerateIdt(definedtype idt) 
    {
        // Check that interface is not compile twice during same compilation
        File out = getIdtExt(inFinalDirectory, subDirectory, idt, "h");

        // Generate interface header file ...
        PrintStream outidth = needRegeneration(idt.lastModifiedFile, out);
        if(outidth != null) {
            outidth.println("/* Generated " + idt.nameUsedAsInclude + " defined type */");
            outidth.println("#if !defined(" + idt.GuardedIndentifier + ")");
            outidth.println("#define " + idt.GuardedIndentifier);
            outidth.println();

            // Generate defined type
            HashSet<Interface> incls = new HashSet<Interface>();
            genDefinition(idt, incls, outidth);

            outidth.println("#endif");    
            
            outidth.close();
        }
     }
    
    public abstract void generate(Interface itf); 

}
