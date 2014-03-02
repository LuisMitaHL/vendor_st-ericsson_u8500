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
package tools.conf.ast;

import java.util.ArrayList;
import java.util.StringTokenizer;
import java.io.File;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.util;
import tools.common.configuration.Debug;
import tools.conf.graph.component.Bases;

public abstract class astcomponent extends asttype {
    public String smallname;

    public boolean singleton = false;

    public astcomponent(File _file, String _fullyname) {
        super(_file, _fullyname);

        smallname = util.getLastName(_fullyname);

        cloneNumber = 0; // We are not cloned!
    };

    protected int NumberOfClone = 0;
    protected int cloneNumber; 
    public astcomponent(astcomponent _other) {
        this(_other.parsedFile, _other.fullyname + "_" + (++_other.NumberOfClone));
        this.cloneNumber = _other.NumberOfClone;
        this.requires = _other.requires;
        this.provides = _other.provides;
        this.properties = _other.properties;
        this.options = _other.options;
        this.extended = _other.extended;
    }

    /*
     * Attribute Parameters
     */
    public ArrayList<astparameter> attributeparameters = new ArrayList<astparameter>();
    
    public ArrayList<astparameter> getLocalAttributeParameters() {
        return attributeparameters;
    }

    /*
     * Option
     */
    protected ArrayList<astoption> options = new ArrayList<astoption>();
    
    public void setOption(astoption option) throws compilerexception {
        configuration.debug(Debug.comp, "Option " + option.name + "=" + option.value);

        //check if option value is valid
        if (option.name == astoption.OptionNames.CFLAGS) {
            ; // check CFLAGS
        } else if (option.name == astoption.OptionNames.CPPFLAGS) {
            ; // check CPPFLAGS
        } else if (option.name == astoption.OptionNames.FIXED_CFLAGS) {
            ; // check FIXED_CFLAGS
        } else if (option.name == astoption.OptionNames.incdir) {
            ; // check incdir
        } else if (option.name == astoption.OptionNames.LDFLAGS) {
            ; // check LDFLAGS
        } else if (option.name == astoption.OptionNames.LIBLDFLAGS) {
            ; // check LIBLDFLAGS
        } else if (option.name == astoption.OptionNames.GROUPLDFLAGS) {
            ; // check GROUPLDFLAGS
        } else if (option.name == astoption.OptionNames.memories) {
            if(! "none".equals(option.value)) {
                StringTokenizer st = new StringTokenizer(option.value, "|");
                while (st.hasMoreTokens()) {
                    String memory = st.nextToken();
                    try {
                        Bases.valueOf(memory);
                    } catch (IllegalArgumentException e) {
                        if(memory.contains("ROM"))
                            configuration.warning("Memory option '" + memory + "' in <" + fullyname + "> is deprecated!");
                        else
                            throw new compilerexception(error.BAD_OPTION_VALUE, "memories", memory);
                    }
                }
            }
        } else if (option.name == astoption.OptionNames.OVERWRITTEN_CFLAGS) {
            ; // check OVERWRITTEN_CFLAGS
        } else if (option.name == astoption.OptionNames.stack) {
            try {
                int val = Integer.parseInt(option.value);
                if ((val <= 0) || (val > 1000000)) // out of range, upper bound arbitrary, will be refused anyway by the CM
                    throw new compilerexception(error.BAD_OPTION_VALUE,
                            "stack", option.value);
            } catch (NumberFormatException e) {
                throw new compilerexception(error.BAD_OPTION_VALUE, "stack",
                        option.value);
            }
        } else {
            throw new compilerexception(error.BAD_OPTION, option.name); //no check performed, probably a new option
        }

        // TODO, check if the option wasn't already set
        boolean flag = false;
//        for (astoption o : options) {
//            if (o.name.equals(option.name) && o.target == option.target)
//                flag = true;
//        }
        if (flag)
            throw new compilerexception(error.DUPLICATE_DECLARATION, "Option", option.target + "." + option.name);
        else
            options.add(option);

    }

    public Iterable<astoption> getLocalOptions() {
        return options;
    }

}
