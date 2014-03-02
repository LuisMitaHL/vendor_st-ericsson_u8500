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
package tools.conf.graph;

import java.util.ArrayList;
import java.util.HashMap;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.*;

public class primitive extends primitiveraw {
    public astprimitive type;

    public primitive(astcontent _content, astprimitive _type, boolean _isFirstInstance) {
        super(_content, _type, _isFirstInstance);
        this.type = _type;
    }
    protected primitive(astbasefile _declaration, 
            String _nameinowner,  
            astprimitive _type,             
            SchedPriority _priority,
            boolean _isFirstInstance) {
        super(_declaration, _nameinowner, _type, _priority, _isFirstInstance);
        this.type = _type;
    }

    /* 
     * Attributes
     */
    protected ArrayList<astattribute> attributes = new ArrayList<astattribute>();
    protected HashMap<String, String> values = new HashMap<String, String>();

    /**
     * Add an attribute
     */
    public void addAttribute(astattribute attribute) {
        attributes.add(attribute);
    }

    /**
     * Return attribute according name
     */
    public astattribute getAttribute(String attrname) {
        for (final astattribute attribute : attributes) 
        {
            if(attribute.name.equals(attrname))
                return attribute;
        }
        return null;
    }

    /**
     * Get all attributes
     */
    public Iterable<astattribute> getAttributes() {
        return attributes;
    }

    /**
     * Set the value of a attribute
     */
    public void setAttributeValue(String attrname, String value) throws compilerexception {
        values.put(attrname, value);
    }
    
    /**
     * Get the value of a attribute
     */
    public String getAttributeValue(String attrname) {
        String value = values.get(attrname);
        return value;
    }

}
