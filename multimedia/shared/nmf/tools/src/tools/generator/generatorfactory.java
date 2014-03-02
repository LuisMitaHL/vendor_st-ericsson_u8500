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
package tools.generator;

import java.io.IOException;
import java.io.PrintStream;
import java.util.HashMap;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.targetfactory;
import tools.common.targetfactory.CodingStyle;
import tools.conf.ast.astcontent;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astsource.SourceType;
import tools.conf.graph.primitive;
import tools.idl.ast.Interface;

public class generatorfactory {
   // static boolean inFinalDirectory = false;
    
    public static void configure() {
       // inFinalDirectory = _inFinalDirectory;
        components = new HashMap<String, primitive>();
    }

    /*
     * Instance creator
     */
    private static HashMap<String, primitive> components;
    private static primitive getInstantiated(String fullyname) {
        return components.get(fullyname);
    }
    private static void addInstantiated(String fullyname, primitive primitive) {
        components.put(fullyname, primitive);
    }

    public static primitive createInstance(astcontent content, astprimitive type) throws IOException, compilerexception {
        String mpc = (content != null) ? content.mpc : "";
        primitive instance = getInstantiated(mpc + ":" + type.fullyname);
        if (instance != null) 
        {
            if(type.singleton) 
            {
                instance = new primitive(content, type, false);
            } 
            else 
            {
                if(targetfactory.style == CodingStyle.C) 
                {
                    configuration.warning("Warning: Duplicate component + '" + type.fullyname + "'");
                    // Create new template and instantiate it
                    // Don't add it in database since we will never reference it !!!!
                    type = new astprimitive(type);
                }
                else if(targetfactory.style == CodingStyle.CPP) 
                {
                    if(type.getSourceType() == SourceType.CSource) 
                        throw new compilerexception(content, error.C_INSTANTIATE_TWICE_IN_CPP);
                }
                instance = new primitive(content, type, true);
            } 
        } else {
            instance = new primitive(content, type, true);

            addInstantiated(mpc + ":" + type.fullyname, (primitive)instance);
        }
        return instance;
    }

    /*
     * Interface Description generator 
     */
    public static void generateInterfaceDescriptor(Interface itf, boolean inFinalDirectory, boolean _legacy) {
        tools.generator.raw.itf itfgen = null;
        if (targetfactory.style == CodingStyle.C) {
            if(_legacy)
                itfgen = new tools.generator.legacy.c.interfacedescriptor(inFinalDirectory);
            else
               itfgen = new tools.generator.c.itf(inFinalDirectory);
        } else if(targetfactory.style == CodingStyle.CPP) {
            itfgen = new tools.generator.cpp.itf(inFinalDirectory, _legacy ? "host" : null);
        } else
            assert false : targetfactory.style + " CodingStyle not valid";
        
        itfgen.generate(itf);
    }
    
    public static void generateSkeletonOnCM(Interface itf, boolean _needheader, PrintStream out) throws compilerexception {
        switch (targetfactory.style) {
        case C:
        {
            tools.generator.legacy.c.user.skel skel = new tools.generator.legacy.c.user.skel(itf, _needheader);
            skel.Generate(out);    
        } break;
        case CPP:
        {
            tools.generator.legacy.cpp.user.skel skel = new tools.generator.legacy.cpp.user.skel(itf, _needheader);
            skel.Generate(out);
        } break;
        }
    }

    public static void generateStubOnCM(Interface itf, boolean _needheader, PrintStream out) throws compilerexception {
        switch (targetfactory.style) {
        case C:
        {
            tools.generator.legacy.c.user.stub stub = new tools.generator.legacy.c.user.stub(itf, _needheader);
            stub.Generate(out);    
        } break;
        case CPP:
        {
            tools.generator.legacy.cpp.user.stub stub = new tools.generator.legacy.cpp.user.stub(itf, _needheader);
            stub.Generate(out);
        } break;
        }
    }

    public static String getInterfaceType(Interface itf) {
        switch (targetfactory.style) {
        case C:
            return "struct sI" + itf.name_;
        case CPP:
            return "I" + itf.name_;
        }
        return null;
    }
}