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
import tools.conf.ast.astprimitive;
import tools.conf.graph.*;
import tools.idl.idlcompiler;
import tools.idl.ast.Interface;
import tools.idl.ast.Method;
import tools.idl.ast.Parameter;
import tools.idl.ast.TypePointer;
import tools.idl.ast.TypeTypedef;
import tools.idl.ast.TypeVoid;
import tools.idl.ast.include;

public class bc extends primitive {    
    static int bcNumber = 0;
    
    protected bc(astbasefile _declaration,             
            SchedPriority _priority,
            astprimitive _type) {
        super(_declaration, "bc" + (bcNumber++), _type, _priority, true);
    }
    protected bc(astbasefile _declaration,             
            String _name,
            SchedPriority _priority,
            astprimitive _type) {
        super(_declaration, _name, _type, _priority, true);
    }

    public static Interface getItfCom(Interface itf) throws compilerexception {
        Interface itfcom;

        tools.idl.parser.Token token = new tools.idl.parser.Token();
        itfcom = new Interface(itf.file, "_com." + itf.name);
        itfcom.includes.add(new include(itf.file, token, "communication/api/descriptor.idt"));
        for(int j = 0; j < itf.methods.size(); j++) {
            Method mdo =  itf.methods.get(j);
            Method md = new Method(itf.file, token, mdo.name, new TypeVoid(itf.file, token));
            md.addParameter(new Parameter(token, "event", new TypePointer(itf.file, token, new TypeTypedef(itf.file, token, "t_remote_event"), null)));
            itfcom.methods.add(md);
        }
        idlcompiler.addInterface(itfcom);

        return itfcom;
    }
}
