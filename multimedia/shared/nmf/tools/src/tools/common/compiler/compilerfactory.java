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
package tools.common.compiler;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.targetfactory;
import tools.conf.graph.component;

public class compilerfactory {
    public static ccompiler getRawComponent() throws compilerexception 
    {
        return targetfactory.getCompiler();
    }
    
    public static ccompiler getCCompiler(component component) throws compilerexception {
        ccompiler cc = getRawComponent();

        // Get default compile option
        cc.CPPFLAGS.setFlags(configuration.getEnvDefault("CPPFLAGS", ""));
        cc.CFLAGS.setFlags(configuration.getEnvDefault("CFLAGS", ""));
        cc.CXXFLAGS.setFlags(configuration.getEnvDefault("CXXFLAGS", ""));
        cc.ASMFLAGS.setFlags(configuration.getEnvDefault("ASMFLAGS", ""));
        cc.LDFLAGS.setFlags(configuration.getEnvDefault("LDFLAGS", ""));
        cc.LIBLDFLAGS.setFlags(configuration.getEnvDefault("LIBLDFLAGS", ""));
        cc.GROUPLDFLAGS.setFlags(configuration.getEnvDefault("GROUPLDFLAGS", ""));

        cc.setDefaultFlags(component);
        
        return cc;
    }

    public static ccompiler getCCompiler(ccompiler parent) throws compilerexception {
        ccompiler cc = getRawComponent();

        cc.CPPFLAGS.setFlags(parent.CPPFLAGS);
        cc.CFLAGS.setFlags(parent.CFLAGS);
        cc.CXXFLAGS.setFlags(parent.CXXFLAGS);
        cc.ASMFLAGS.setFlags(parent.ASMFLAGS);
        cc.LDFLAGS.setFlags(parent.LDFLAGS);
        cc.LIBLDFLAGS.setFlags(parent.LIBLDFLAGS);
        cc.GROUPLDFLAGS.setFlags(parent.GROUPLDFLAGS);
        
        cc.setDefaultFlags(parent);

        return cc;
    }

}
