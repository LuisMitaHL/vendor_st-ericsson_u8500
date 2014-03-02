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
package tools.idl.ast;

import java.util.ArrayList;
import java.io.File;

import tools.common.compilerexception;
import tools.common.error;

public class definedtype {
    public ArrayList<definition> definitions;
    public ArrayList<include> includes;

    public File file;		  // The real file where the declaration type reside
    public String nameUsedAsInclude;
    public String GuardedIndentifier;
    
    public File lastModifiedFile;

    public definedtype(File _file) {
        file = lastModifiedFile = _file;
        this.definitions = new ArrayList<definition>();
        this.includes = new ArrayList<include>();
    }

    public void addDefinition(definition newdef) throws compilerexception {
        for(final definition def : definitions) 
        {
            if(def.typeName.equals(newdef.typeName))
                throw new compilerexception(error.IDT_TYPE_ALREADY_DEFINED, def.typeName);
        }
        definitions.add(newdef);
    }
    
    /**
     * 
     * @param name Declaration name
     * @param type Waited type declaration
     * @return Searched definition (null if not found).
     */
    public definition getDefinedDefinition(String name, Class<?> type) {
        // Lookup in local definition
        for(final definition def : definitions) 
        {
            if(type.isInstance(def) && def.typeName.equals(name))
                return def;
        }
        // Lookup in included definition
        for(final include inc : includes) 
        {
            definedtype dt = inc.definition;
            definition def = dt.getDefinedDefinition(name, type);
            if(def != null)
                return def;
        }
        // Not found
        return null;
    }

}
