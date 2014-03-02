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

import java.util.HashMap;
import java.util.StringTokenizer;

import tools.common.astbasefile;
import tools.conf.ast.*;
import tools.conf.ast.astoption.OptionNames;

public class primitiveraw extends component {
    public String debugComment; 

    public primitiveraw(astcontent _content, astcomponent _type, boolean _isFirstInstance) {
        super(_content, _type, _isFirstInstance);
        this.debugComment = null;
    }
    protected primitiveraw(astbasefile _declaration, 
            String _nameinowner,  
            astcomponent _type,             
            SchedPriority _priority,
            boolean _isFirstInstance) {
        super(_declaration, _nameinowner, _type, _priority, _isFirstInstance);
        this.debugComment = null;
    }

    /*
     * Component Controller
     */
    HashMap<String, interfacereference> providedInterfaces = new HashMap<String, interfacereference>();
    public interfacereference getFcInterface(String servername) {
        interfacereference ir =  providedInterfaces.get(servername);
        if(ir == null) {
            astprovide provide = getProvide(astinterface.getNameOfInterfaceCollection(servername));
            if(provide != null) {
                ir = new interfacereference(this, provide.name,  
                        astinterface.getIndexOfInterfaceCollection(servername));
                providedInterfaces.put(servername, ir);
            } 
        }
        return ir;
    }

    /*
     * Base information
     */
     public boolean isBaseSetted(Bases base) {
       String memoriesValue = getOption(OptionNames.memories);
       if(memoriesValue == null || "none".equals(memoriesValue)) {
           return false;
       } else {
           StringTokenizer st = new StringTokenizer(memoriesValue, "|");
           while (st.hasMoreTokens()) {
               String memory = st.nextToken();
               if(base.toString().equals(memory))
                   return true;
           }
       }
       return false;
   }

}
