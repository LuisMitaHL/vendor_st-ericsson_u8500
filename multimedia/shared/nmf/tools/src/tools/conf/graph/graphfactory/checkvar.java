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
package tools.conf.graph.graphfactory;

import java.util.ArrayList;

import tools.common.compilerexception;
import tools.common.error;
import tools.conf.ast.astoption;
import tools.conf.ast.astproperty;
import tools.conf.ast.astsource;
import tools.conf.ast.astoption.OptionNames;
import tools.conf.graph.component;
import tools.conf.graph.composite;
import tools.conf.graph.primitive;
import tools.conf.graph.visitor;

public class checkvar extends visitor {
    protected void travelCommon(component component, Object data) throws compilerexception {
        // Check that option not declared twice
        for(astoption option : component.getOptions()) {
            if(option.target != null) {
                if(component.getOption(option.target, option.name) != option.value) 
                    throw new compilerexception(option, error.DUPLICATE_DECLARATION,
                            "Option", option.target + "." + option.name);
            } else {
                if(component.getOption(option.name) != option.value) 
                    throw new compilerexception(option, error.DUPLICATE_DECLARATION,
                            "Option", option.name);
            }
        }
    }
    
    protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
        for(final astproperty property : primitive.getProperties()) 
        {
            if(primitive.getProperty(property.name) != property) 
                throw new compilerexception(property, error.DUPLICATE_DECLARATION,
                        "Property", property.name);
        }
        
        ArrayList<String> files = new ArrayList<String>();
        for(final astsource source : primitive.type.sources) 
        {
            String path = source.file.getAbsolutePath();

            if(! source.file.exists())
                throw new compilerexception(source, error.FILE_NOT_FOUND,
                        path);
            
            if(files.contains(path))
                throw new compilerexception(source, error.DUPLICATE_DECLARATION,
                        "Source", path);
            files.add(path);
        }
        
        if(files.size() == 0 && primitive.getProvideSize() != 0)
            throw new compilerexception(primitive.type, error.COMPONENT_WITHOUT_SOURCE,
                    primitive.type.fullyname);
    }
    
    protected void travelComposite(composite composite, Object data) throws compilerexception {
        // Check that specific primitive option not used in composite
        for(astoption option : composite.getOptions()) {
            if(option.name == OptionNames.memories || 
                    option.name == OptionNames.incdir)
                throw new compilerexception(option, error.OPTION_NOT_COMPATYIBLE_WITH_COMPOSITE,
                        option.name);
        }

        super.travelComposite(composite, data);
    }

}
