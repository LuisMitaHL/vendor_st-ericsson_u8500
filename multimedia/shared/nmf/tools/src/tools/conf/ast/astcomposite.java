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
import java.util.ArrayList;

public class astcomposite extends astcomponent {
	public String ldflags;
	
	public astcomposite(File _file, String _fullyname) {
		super(_file, _fullyname);
	}

	/*
	 * Template Parameters
	 */
    public ArrayList<astparameter> templateparameters = new ArrayList<astparameter>();
    
    public ArrayList<astparameter> getLocalTemplateParameters() {
        return templateparameters;
    }
   
	/*
	 * Binds
	 */
	protected ArrayList<astbind> binds = new ArrayList<astbind>();
	
	public void addLocalBind(astbind bind) {
		binds.add(bind);
	}
	public Iterable<astbind> getLocalBinds() {
		return binds;
	}
	
	/*
	 * Contents
	 */
	protected ArrayList<astcontent> contents = new ArrayList<astcontent>();
	
	public void addLocalContent(astcontent sc) {
		contents.add(sc);
	}
	public Iterable<astcontent> getLocalContents() {
		return contents;
	}
}
