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
import tools.idl.ast.*;
import tools.idl.parser.IDLParser;
import tools.idl.parser.ParseException;
import tools.idl.parser.TokenMgrError;
import tools.common.mail.Attachements;

public class idlcompiler extends configuration {

    private static boolean isValidIdentifier(String id) {
        return id.matches("[a-zA-Z0-9_.]*") & id.charAt(id.length() - 1) != '.';
    }

    private static Hashtable<String, Interface> Interfaces = new Hashtable<String, Interface>();
    public static Interface Compile(String itfname, astbasefile callercontext) throws compilerexception {
        Interface itf = Interfaces.get(itfname);
        if(itf == null) {
            if (!isValidIdentifier(itfname)) throw new compilerexception(error.INVALID_IDENTIFIER, itfname);
            File file = getFile(itfname.replace('.', File.separatorChar) + ".idl");
            if(! file.exists())
                file = getFile(itfname.replace('.', File.separatorChar) + ".itf");
            try {
                FileInputStream inputfile = new FileInputStream(file);
                IDLParser parser = new IDLParser(inputfile);
                parser.file = file;
                debug(Debug.step, "Compile IDL " + itfname);
                
                Attachements.addFile(itfname.replace('.', File.separatorChar) + ".idl", file);

                itf = parser.InterfaceDeclaration(itfname);
                inputfile.close();

                // Sanity check
                if(itfname.equals(itf.name) == false) {
                    throw new compilerexception(callercontext, error.WRONG_NAME_INFERRED,
                            itfname,  itf.name);
                }

                Interfaces.put(itf.name, itf);

                if(itf.unmanaged) 
                {
                    idtcompiler.populateInclude(itf, true);
                } 
                else
                {
                    /*
                     * Resolve type
                     */ 
                    idtcompiler.resolveInterfaceType(itf);

                    for(final Method md : itf.methods) 
                    {
                        idtcompiler.resolvType(itf, md.type);
                        for(int k = 0; k < md.parameters.size(); k++) {
                            Parameter fp = md.parameters.get(k);
                            idtcompiler.resolvType(itf, fp.type);
                        }
                    }
                }
            } catch (FileNotFoundException e) {
                throw new compilerexception(callercontext, error.IDL_SOURCE_NOT_FOUND,
                        file.getPath());
            } catch (ParseException e) {
                throw new compilerexception(new astbasefile(file, e.currentToken.next.beginLine, e.currentToken.next.beginColumn), 
                        error.SYNTAXIC_ERROR, e.getMessage());
            } catch (TokenMgrError e) {
                throw new compilerexception(new astbasefile(file, 0, 0),  
                        error.LEXICAL_ERROR, e.getMessage());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return itf;
    }

    public static Interface getPreviouslyCompile(String itfname) {
        Interface itf = Interfaces.get(itfname);    
        if(itf == null)
            configuration.error("Unexpected null interface " + itfname);
        return itf; 
    }
    
    public static void addInterface(Interface itf) throws compilerexception {
        Interfaces.put(itf.name, itf);
        idtcompiler.resolveInterfaceType(itf);
    }
}
