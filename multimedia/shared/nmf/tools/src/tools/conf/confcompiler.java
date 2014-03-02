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
package tools.conf;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.Hashtable;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.configuration.Debug;
import tools.conf.ast.*;
import tools.conf.parser.*;
import tools.common.mail.Attachements;

public class confcompiler 
{ 
    private static Hashtable<String, asttype> Components = new Hashtable<String, asttype>();
    
    public static void registerComponent(asttype type)
    {
        // Add the loaded component to the already parsed database
        Components.put(type.fullyname, type);
    }
    
    public static asttype Compile(String compname, astbasefile callercontext) throws compilerexception 
    {
        asttype type = Components.get(compname);
        if(type == null) 
        {
            // Find the package of the loader component
            String pack, name;
            pack = compname.replace('.', File.separatorChar);
            if(compname.lastIndexOf('.') == -1) 
                name = compname;
            else 
                name = compname.substring(compname.lastIndexOf('.')+1);

            // Compute the file according the naming scheme
            //  compPath.arch
            //  compPath.type
            //  compPath/component.adl
            //  compPath/comp.adl
            //  compPath/comp.conf
            File file = configuration.getFile(pack + ".arch");
            boolean isComposite = false;
            if(file.exists()) 
            {
                isComposite = true;
            } 
            else 
            {
                file = configuration.getFile(pack + ".type");
                if(! file.exists()) {
                    file = configuration.getFile(pack + File.separator + "component.adl");
                    if(! file.exists()) {
                        file = configuration.getFile(pack + File.separator + name + ".adl");
                        if(! file.exists()) {
                            file = configuration.getFile(pack + File.separator + name + ".conf");
                        }
                    }
                }
            }
            
            try {
                // Load the component
                java.io.FileInputStream inputfile = new java.io.FileInputStream(file);
                Parser parser = new Parser(inputfile);
                parser.file = file;
                configuration.debug(Debug.step, "Compile CONF " + compname);

                Attachements.addFile(pack + File.separator + name + ".conf", file);
                
                if(isComposite)
                    type = parser.CompositeDeclaration(compname);
                else
                    type = parser.ComponentDeclaration(compname, file.getParentFile());
                inputfile.close();

                if(parser.foundedName != null && compname.equals(parser.foundedName) == false) {
                    throw new compilerexception(type, error.WRONG_NAME_INFERRED,
                            compname,  parser.foundedName);
                }

                // Adjust the sources information according configuration directory
                if(type instanceof astprimitive) {
                    astprimitive primitive = (astprimitive)type;
                    if(primitive.sources.isEmpty()) {
                        File src = new File(file.getParentFile(), "src");
                        // Add the whole content of the src directory
                        String files[] = src.list( new FilenameFilter() {
                            public boolean accept(File dir, String name) {
                                return (name.endsWith(".cpp") || name.endsWith(".c") || name.endsWith(".s") || name.endsWith(".asm"));
                            }
                        });
                        if (files == null) {
                             configuration.warning("Warning: no source files for " + primitive.fullyname);
                        } else {
                            for(int i = 0; i < files.length; i++) {
                                primitive.sources.add(new astsource(new File(src, files[i])));
                            }
                        }
                    } 
                }
            } catch (FileNotFoundException e) {
                throw new compilerexception(callercontext, error.ADL_SOURCE_NOT_FOUND,
                        configuration.getFile(pack + File.separator + name + ".adl"));
            } catch (ParseException e) {
                throw new compilerexception(new astbasefile(file, e.currentToken.next.beginLine, e.currentToken.next.beginColumn), 
                        error.SYNTAXIC_ERROR, e.getMessage());
            } catch (TokenMgrError e) {
                throw new compilerexception(new astbasefile(file, 0, 0),  
                        error.LEXICAL_ERROR, e.getMessage());
            } catch (IOException e) {
                e.printStackTrace();
            }

            registerComponent(type);
        }
        
        return type;
    }

    public static asttype getPreviouslyCompile(String compname) 
    {
        return Components.get(compname);	
    }
    
    public static void clearComponents()
    {
        Components.clear();
    }
/*
    private static Hashtable<String, asttype> Types = new Hashtable<String, asttype>();
    public static asttype CompileType(String typename, context callercontext) throws compilerexception {
        asttype type = Types.get(typename);
        if(type == null) {
            // Compute the file according the naming scheme
            File file = configuration.getFile(typename.replace('.', File.separatorChar) + ".type");

            try {
                // Load the component
                java.io.FileInputStream inputfile = new java.io.FileInputStream(file);
                Parser parser = new Parser(inputfile);
                parser.file = file;
                configuration.debug(Debug.step, "Compile TYPE " + typename);
                type = parser.TypeDeclaration(typename);
                if(parser.foundedName != null && typename.equals(parser.foundedName) == false) {
                    throw new compilerexception(new context(type), error.WRONG_NAME_INFERRED,
                            typename, parser.foundedName);
                }
            } catch (FileNotFoundException e) {
                throw new compilerexception(callercontext, error.ADL_SOURCE_NOT_FOUND, 
                        file.getPath());
            } catch (ParseException e) {
                throw new compilerexception(new context(file, e.currentToken.next.beginLine, e.currentToken.next.beginColumn), 
                        error.SYNTAXIC_ERROR, e.getMessage());
            } catch (TokenMgrError e) {
                throw new compilerexception(new context(file, 0, 0), 
                        error.LEXICAL_ERROR, e.getMessage());
            }

            // Add the loaded component to the already parsed database
            Types.put(typename, type);
        }
        return type;
    }
*/
}
