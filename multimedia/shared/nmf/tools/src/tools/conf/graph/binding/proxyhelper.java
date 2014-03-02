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
package tools.conf.graph.binding;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astcomponent;
import tools.conf.ast.astcontent;
import tools.conf.ast.astprimitive;
import tools.conf.graph.primitiveraw;

public class proxyhelper extends primitiveraw implements proxy { 
    protected static astprimitive getProxy(String name) throws compilerexception {
        astprimitive proxy = new astprimitive(null, name);
        return proxy;
    }

    public proxyhelper(astcontent _content) throws compilerexception {
        super(_content, getProxy("_virtual"), true);
    }
    
    public proxyhelper(astcontent _content, astcomponent _type) throws compilerexception {
        super(_content, _type, true);
    }

    public static int nbproxy = 0;
    
    public proxyhelper(astbasefile dec, String name) throws compilerexception {
        super(new astcontent(dec.parsedFile, dec.line, dec.column, name, "p" + (nbproxy++)), getProxy(name), true);
    }

    public proxyhelper(astbasefile dec, String name, String _localname) throws compilerexception {
        super(new astcontent(dec.parsedFile, dec.line, dec.column, name, _localname), getProxy(name), true);
    }

}
