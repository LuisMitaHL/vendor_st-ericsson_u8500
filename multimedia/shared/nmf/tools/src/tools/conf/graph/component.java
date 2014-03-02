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

import tools.common.*;
import tools.common.targetfactory.TargetNames;
import tools.conf.ast.*;
import tools.conf.ast.astoption.OptionNames;

public abstract class component {
    public static class SchedPriority {
        public final static SchedPriority NotSetted = new SchedPriority("not setted");
        public final static SchedPriority Background = new SchedPriority("background");
        public final static SchedPriority Normal = new SchedPriority("normal");
        public final static SchedPriority Urgent = new SchedPriority("urgent");
        
        String  value;
        
        public SchedPriority(int _priority) {
            value = String.valueOf(_priority);
        }
        public SchedPriority(String _priority) {
            value = _priority;
        }

        public String getValue() {
            if("background".equals(value))
                return "0";
            if("normal".equals(value))
                return "1";
            if("urgent".equals(value))
                return "2";
            // 3 for RT
            // 4 for RT for CScall
            // 5 for RT for CScall
            if("io".equals(value))
                return "6";
            return value;
        }
        
        public int getOrdinal() {
            try 
            {
                return Integer.parseInt(getValue());
            } 
            catch (Exception e) 
            { 
            }
            
            return -1;
        }

        public String toString() 
        {
            return value;
        }
    };

    public astcomponent type;

    public composite owner;

    public String nameinowner;
    public int order = Integer.MAX_VALUE;
    
    public astbasefile declaration;

    public boolean isFirstInstance;
    
    public String localmpc;
    public SchedPriority priority;
    
    /**
     * Constructor
     * @param _owner
     * @param _nameinowner
     * @param _type
     * @throws illegalcontentexception
     * @throws compilerexception
     */
    public component(astcontent _content, astcomponent _type, boolean _isFirstInstance) {
        type = _type;

        // Add component in composite 
        if(_content != null) {
            nameinowner = _content.name;
            declaration = _content;
            localmpc = _content.mpc;
            priority = _content.priority;
            order = _content.order;
        } else {
            nameinowner = null;	
            declaration = null;
            priority = SchedPriority.Normal;
        }

        this.isFirstInstance = _isFirstInstance;
    }
    
    // Used by generated component 
    protected component(astbasefile _declaration, 
            String _nameinowner,  
            astcomponent _type, 
            SchedPriority _priority,
            boolean _isFirstInstance) {
        type = _type;
        nameinowner = _nameinowner; 
        declaration = _declaration;
        localmpc = null;
        priority = _priority;
        isFirstInstance = _isFirstInstance;
    }

    /*
     * Path
     */
    public String getPathName() {
        if(owner != null) {
            String parentpathname = owner.getPathName();
            if(parentpathname != null)
                return parentpathname + "/" + nameinowner;
        } 
        return nameinowner;
    }
    public String getPathName_() {
        String pathname = getPathName();
        if(pathname != null)
            return getPathName().replace('/', '_');
        return null;
    }
    public String getMPC() {
        if(localmpc == null && owner != null)
            return owner.getMPC();
        else if(localmpc == null)
        	return null;
        else
            return localmpc.intern();
    }
    
    /*
     * Abstract controllers 
     */
    public abstract interfacereference getFcInterface(String servername);
    
    /*
     * Binding Controller
     * Not, name must be serialized (without hex, oct, space, ...) before calling such methods.
     */
    public interfacereference lookupFcWithoutError(String clientname) {
        try {
            return lookupFc(clientname);
        } catch (illegalbindingexception e) {
            return null;
        }
    }

    public interfacereference lookupFc(String clientname) throws illegalbindingexception {
        if(owner != null) {
            // Search where the required interface is bound in the parent composite
            for(final astbind sb : owner.getBinds()) {
                if(nameinowner.equals(sb.from) && clientname.equals(sb.fromitf)) {
                    interfacereference ir;
                    if("this".equals(sb.to)) {
                        // This is bound outside composite
                        ir = owner.lookupFc(sb.toitf);
                    } else {
                        // This is bound to another subcomponent
                        ir = owner.getFcSubComponent(sb.to).getFcInterface(sb.toitf);
                    }
                    return ir;
                }
            }
            // A required interface is not bound
            return null;
        } else {
            // This is a top level required interface
            astrequire require = getRequire(astinterface.getNameOfInterfaceCollection(clientname));
            return new externalinterfacereference(require.name,  
                    astinterface.getIndexOfInterfaceCollection(clientname));
        }
    }
    
    /*
     * Bases
     */
    public enum Bases { 
        YRAM        ("__sb__data2", null), 
        
        EXT24RAM    ("__sb__data5", null), 
        EXT16RAM    ("__sb__data6", null), 
        EMB24RAM    ("__sb__data8", null), 
        EMB16RAM    ("__sb__data9", null),

        EXT24ROM    ("__sb__cst_data5", "__sb__cst_data5__ptr"), 
        EXT16ROM    ("__sb__cst_data6", "__sb__cst_data6__ptr"), 
        EMB24ROM    ("__sb__cst_data8", "__sb__cst_data8__ptr"), 
        EMB16ROM    ("__sb__cst_data9", "__sb__cst_data9__ptr");

        public String elfSymbolStart, elfSymbolPointer;
        Bases(String _elfSymbolStart, String _elfSymbolPointer) {
            elfSymbolStart = _elfSymbolStart;
            elfSymbolPointer = _elfSymbolPointer;
        }
     };

    public abstract boolean isBaseSetted(Bases base);

    public int numberBaseSetted() {
        int nb = 0;
        for (Bases base : Bases.values()) 
            if(isBaseSetted(base))
                nb++;
        return nb;
    }

    /* 
     * Introspection
     */
    protected ArrayList<astrequire> requires = new ArrayList<astrequire>();
    protected ArrayList<astprovide> provides = new ArrayList<astprovide>();
    protected ArrayList<astproperty> properties = new ArrayList<astproperty>();

    public void addRequire(astrequire require) {
        requires.add(require);
    }
    public void addProvide(astprovide provide) {
        provides.add(provide);
    }
    public void addProperty(astproperty property) {
        properties.add(property);
    }
    public void addOption(astoption option) {
        options.add(option);
    }

    /**
     * Return first required interface with this name.
     * @param itfname the searched required interface name
     * @return required interface
     */
    public astrequire getRequire(String itfname) {
        for (astrequire require : requires) 
        {
            if(itfname.equals(require.name))
                return require;
        }
        return null;
    }
    
    /**
     * Return first required interface with this type.
     * @param type the searched required interface type
     * @return required interface
     */
    public astrequire getRequireByType(String type) {
        for (astrequire require : requires) 
        {
            if(type.equals(require.type))
                return require;
        }
        return null;
    }

    /**
     * Get all required interfaces.
     * @return required interfaces
     */
    public Iterable<astrequire> getRequires() {
        return requires;
    }
    
    /**
     * Get the number of required interfaces.
     * @return number of required interfaces
     */
    public int getRequireSize() {
        return requires.size();
    }

    /**
     * Return the provided interface i.
     * @param itfname name of the searched provided interface
     * @return the provided interface
     */
    public astprovide getProvide(String itfname) {
        for (final astprovide provide : provides) 
        {
            if(itfname.equals(provide.name))
                return provide;
        }
        return null;
    }
    
    public int getProvideIndex(String itfname) {
        int idx = 0;
        for (final astprovide provide : provides) 
        {
            if(itfname.equals(provide.name))
                return idx;
            idx++;
        }
        return 0xDEAD;
    }

    /**
     * Get all provided interfaces.
     * @return provided interfaces
     */
    public Iterable<astprovide> getProvides() {
        return provides;
    }
    /**
     * Get the number of provided interfaces.
     * @return number of provided interfaces
     */
    public int getProvideSize() {
        return provides.size();
    }

    /**
     * Return the property.
     * @param propname name of the searched property
     * @return the property
     */
    public astproperty getProperty(String propname) {
        for(final astproperty property : properties) {
            if(propname.equals(property.name))
                return property;
        }
        return null;
    }

    /**
     * Get all properties.
     * @return properties
     */
    public Iterable<astproperty> getProperties() {
        return properties;
    }
    
    /*
     * Option
     */
    protected ArrayList<astoption> options = new ArrayList<astoption>();

    public Iterable<astoption> getOptions() {
        return options;
    }

    public String getOption(TargetNames target, OptionNames name) {
        // Lookup in option if target.name option set
        for(final astoption option : options) 
        {
            if(target == option.target && name == option.name)
                return option.value;
        }
        // Take default option if existing
        return getOption(name);
    }
    
    public String getOption(OptionNames name) {
        // Lookup in option if target.name option set
        for(final astoption option : options) 
        {
            if(option.target == null && name == option.name) 
                return option.value;
        }
        return null;
    }
    
    /**
     * @see java.lang.Object#toString()
     */
    public String toString() {
        return "'" + type.fullyname + "'";
    }


}
