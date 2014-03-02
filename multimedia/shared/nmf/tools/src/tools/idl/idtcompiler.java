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
package tools.idl;


import java.io.*;
import java.util.Hashtable;

import tools.common.*;
import tools.common.mail.Attachements;
import tools.idl.parser.ParseException;
import tools.idl.ast.*;
import tools.idl.parser.TokenMgrError;
import tools.idl.parser.IDLParser;

public class idtcompiler extends configuration {

    protected static void resolvType(definedtype itf, Type type) throws compilerexception {
        if(type.typedefinition != null)
            return;
        
        if (type instanceof typestructure) {
            typestructure ts = (typestructure)type;
            structure s = (structure)itf.getDefinedDefinition(ts.typeName, structure.class);
            if(s != null) {
                ts.typedefinition = s;
                for(structentry se : s.members) {
                    resolvType(itf, se.type);
                }
            } else {
                throw new compilerexception(ts, error.STRUCT_MUST_BE_DEFINED,
                        ts.typeName);
            }
        } else  if (type instanceof structurepredeclared) {
        } else  if (type instanceof structure) {
            structure s = (structure)type;
            for(structentry se : s.members) {
                resolvType(itf, se.type);
            }
        } else if (type instanceof TypeTypedef) {
            TypeTypedef td = (TypeTypedef) type;
            // Look if this is a defined type
            typedef t = (typedef)itf.getDefinedDefinition(td.typeName, typedef.class);
            if(t != null) {
                resolvType(itf, t.type);
                td.typedefinition = t;
            } else {
                // Look if this is a interface type
                try {
                    Interface ritf = idlcompiler.Compile(td.typeName, td);
                    td.typedefinition = new typeinterface(td.parsedFile, td.line, td.column, ritf);
                } catch (compilerexception e) {
                    if(td.typeName.contains("."))
                        throw new compilerexception(td, error.ITF_MUST_BE_DEFINED,
                                td.typeName);
                    else 
                    {
                        String knowtype = "";

                        // Display local and first level includes type !!!
                        for(final definition def : itf.definitions) 
                        {
                            knowtype += "\n    " + def.typeName;
                        }
                        for(final include inc : itf.includes) 
                        {
                            definedtype dt = inc.definition;
                            for(final definition def : dt.definitions) 
                            {
                                knowtype += "\n    " + def.typeName;
                            }
                        }
                        
                        throw new compilerexception(td, error.TYPEDEF_MUST_BE_DEFINED,
                                td.typeName, knowtype);
                    }
                }
            }
        } else  if (type instanceof typedef) {
            typedef td = (typedef)type;
            resolvType(itf, td.type);
        } else if (type instanceof enumeration) {
        } else if (type instanceof typeenum) {
            typeenum te = (typeenum) type;
            enumeration e = (enumeration)itf.getDefinedDefinition(te.typeName, enumeration.class);
            if(e != null) {
                te.typedefinition = e;
            } else {
                throw new compilerexception(te, error.ENUM_MUST_BE_DEFINED,
                        te.typeName);
            }
        } else  if (type instanceof TypePointer) {
            TypePointer tp = (TypePointer)type;
            // LOOKAHEAD of 1 in order to allow "structurepredeclared*"
            if(tp.type instanceof typestructure)
            {
                if(itf.getDefinedDefinition(tp.type.typeName, structurepredeclared.class) != null)
                    return;
            }
            resolvType(itf, tp.type);
        } else if (type instanceof typereference) {
            typereference tr = (typereference) type;
            resolvType(itf, tr.type);
        } else if (type instanceof typearray)  {
            typearray array = (typearray) type;
            resolvType(itf, array.type);
            if(Character.isLetter(array.sizeString.charAt(0))) {
                diesedefine dd = (diesedefine)itf.getDefinedDefinition(array.sizeString, diesedefine.class); 
                if(dd != null) {
                    try {
                        array.sizeInteger = (int)util.StringToDecimal(dd.value);
                    } catch (NumberFormatException e) {
                        throw new compilerexception(array, error.BAD_INTEGER, dd.value);
                    }
                } else 
                    throw new compilerexception(array, error.CONSTANT_MUST_BE_DEFINED,
                            array.sizeString);
            } else {
                array.sizeInteger = (int)util.StringToDecimal(array.sizeString);
            }
        } else if (type instanceof typeconst)  {
            typeconst cst = (typeconst)type;
            resolvType(itf, cst.type);
        } else if (type instanceof TypePrimitive) {
        } else if (type instanceof typeprimitiveunmanaged) {
            throw new compilerexception(type, error.UNMANAGED_IN_MANAGED,
                    type.typeName);
        } else if (type instanceof TypeVoid) {
        } else if (type instanceof diesedefine) {
        } else {
            throw new compilerexception(type, "Unknow type " + type);
        }
    }

    public static void populateInclude(definedtype dt, boolean unmanaged) throws compilerexception {
        /*
         * Parse idt included file
         */
        for(final include incl : dt.includes) 
        {
            if(incl.filename.toLowerCase().endsWith(".idt"))
            {
                incl.definition = Compile(incl.filename, incl);

                // Compute last modified file
                if(incl.definition.lastModifiedFile.lastModified() > dt.lastModifiedFile.lastModified())
                    dt.lastModifiedFile = incl.definition.lastModifiedFile;
            } 
            else if(! unmanaged)
            {
                throw new compilerexception(incl, error.ONLY_IDT_CAN_BE_INCLUDED);
            }
        }
    }
    
    public static void resolveInterfaceType(definedtype dt) throws compilerexception 
    {
        populateInclude(dt, false);

        /*
         * Replace typestructure by structure if relevant !
         */ 
        for(final Type t :dt.definitions) 
        {
            resolvType(dt, t);
        }			

    }

    private static Hashtable<String, definedtype> defines = new Hashtable<String, definedtype>();
    public static definedtype Compile(String filename, astbasefile callercontext) throws compilerexception {
        File file = getSharedFile(filename);
        definedtype dt = defines.get(file.getAbsolutePath());
        if(dt == null) {
            try {
                FileInputStream inputfile = new FileInputStream(file);
                IDLParser parser = new IDLParser(inputfile);
                parser.file = file;
                debug(Debug.step, "Compile IDT " + filename);
                
                Attachements.addFile(filename, file);

                dt = parser.idtDeclaration();
                dt.nameUsedAsInclude = filename;
                defines.put(file.getAbsolutePath(), dt);

                resolveInterfaceType(dt);

            } catch (FileNotFoundException e) {
                throw new compilerexception(callercontext, error.IDT_SOURCE_NOT_FOUND, 
                        file.getPath());
            } catch (ParseException e) {
                throw new compilerexception(new astbasefile(file, e.currentToken.next.beginLine, e.currentToken.next.beginColumn), 
                        error.SYNTAXIC_ERROR, e.getMessage());
            } catch (TokenMgrError e) {
                throw new compilerexception(new astbasefile(file, 0, 0), 
                        error.LEXICAL_ERROR, e.getMessage());
            }
        }
        return dt;
    }
}
