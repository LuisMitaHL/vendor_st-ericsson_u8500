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
import java.util.LinkedHashMap;

import tools.common.compilerexception;
import tools.conf.ast.*;

public class composite extends component {
    public astcomposite type;

    public composite(astcontent _content, astcomposite _type) throws compilerexception {
        super(_content, _type, true);
        this.type = _type;
    }

    /*
     * Contents
     */
    protected ArrayList<astcontent> contents = new ArrayList<astcontent>();
    public Iterable<astcontent> getContents() {
        return contents;
    }
    public void addContent(astcontent content) {
        contents.add(content);
    }

    /*
     * Binds
     */
    protected ArrayList<astbind> binds = new ArrayList<astbind>();
    public Iterable<astbind> getBinds() {
        return binds;
    }
    public void addBind(astbind bind) {
        binds.add(bind);
    }

    /*
     * Content Controller
     */
    protected ArrayList<component> subcomponents = new ArrayList<component>();
    protected LinkedHashMap<String, primitiveraw> controllercomponents = new LinkedHashMap<String, primitiveraw>();

    public Iterable<component> getFcSubComponents() {
        return subcomponents;
    }
    public Iterable<primitiveraw> getFcControllerComponents() {
        return controllercomponents.values();
    }

    public void addFcSubComponent(component component) {
        component.owner = this;
        
        // Insert according order
        
        int index;
        for(index = 0; index < subcomponents.size(); index++)
        {
            if(component.order < subcomponents.get(index).order)
            {
                subcomponents.add(index, component);
                return;
            }
        }
        
        subcomponents.add(component);
    }
    
    public void addFcControllerComponent(primitiveraw component) {
        controllercomponents.put(component.nameinowner, component);
        component.owner = this;
    }

    public component getFcSubComponent(String scname) {
        if("this".equals(scname)) 
            return this;
        
        for(component sc : subcomponents)
        {
            if(sc.nameinowner.equals(scname))
                return sc;
        }
        
        return controllercomponents.get(scname);
    }

    /*
     * Component Controller 
     */
    public interfacereference getFcInterface(String servername) {
        astprovide provide = getProvide(astinterface.getNameOfInterfaceCollection(servername));
        if(provide != null) {
            for(final astbind sb : getBinds()) {
                if("this".equals(sb.from) && servername.equals(sb.fromitf)) {
                    // Return the first sub component that is astbind to our interface 
                    return getFcSubComponent(sb.to).getFcInterface(sb.toitf);
                }
            }
        }
        return null;
    }

    /*
     * Bases
     */
    public boolean isBaseSetted(Bases base) {
        for(final component comp : getFcSubComponents()) {
            if(comp.isBaseSetted(base))
                return true;
        }
        return false;
    }

}
