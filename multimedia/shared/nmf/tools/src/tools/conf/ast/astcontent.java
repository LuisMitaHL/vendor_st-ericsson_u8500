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

import java.io.File;
import java.util.Comparator;

import tools.common.astbasefile;
import tools.conf.graph.component.SchedPriority;

public class astcontent extends astbasefile {
    
    public static class contentcomparator implements Comparator<astcontent> {
        public int compare(astcontent a, astcontent b) {
            return a.order - b.order;
        }
    }
    
	public String name;
	
	public astdefinition typedefinition;
	
	public String mpc = null;
    public SchedPriority priority = SchedPriority.Normal;
    public int order = Integer.MAX_VALUE;
	
    public astcontent(File _file, int line, int column, astdefinition _type, String _name) {
        super(_file, line, column);
        this.typedefinition = _type;
        this.name = _name;
    }
    
    public astcontent(File _file, int line, int column, String _type, String _name) {
        this(_file, line, column, 
                new astdefinition(_file, line, column, _type), 
                _name);
    }
    
    public astcontent(String _type, astcontent other) {
        super(other.parsedFile, other.line, other.column);
        this.typedefinition = new astdefinition(other.parsedFile, other.line, other.column, _type);
        this.name = other.name;
        this.mpc = other.mpc;
        this.priority = other.priority;
        this.order = other.order;
    }

}
