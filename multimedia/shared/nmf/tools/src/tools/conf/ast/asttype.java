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
import java.io.File;

import tools.common.astbasefile;

public class asttype extends astbasefile {
    public String fullyname;
    public String fullyname_;   // fullyname_ = fullyname.replace('.', '_')

    public asttype(File _file, String _fullyname) {
        super(_file, 0, 0);
        fullyname = _fullyname;
        fullyname_ = fullyname.replace('.', '_');   
    };

    public String toString() {
        return fullyname;
    }

    /*
     * Extends
     */
    protected ArrayList<astdefinition> extended = new ArrayList<astdefinition>();
    
    public void addExtend(astdefinition extend) {
        extended.add(extend);
    }
    public Iterable<astdefinition> getLocalExtends() {
        return extended;
    }

    /*
     * Requires
     */
    protected ArrayList<astrequire> requires = new ArrayList<astrequire>();
    
    public void addRequire(astrequire require) {
        requires.add(require);
    }
    public Iterable<astrequire> getLocalRequires() {
        return requires;
    }

    /*
     * Provides
     */
    protected ArrayList<astprovide> provides = new ArrayList<astprovide>();
    
    public void addProvide(astprovide provide) {
        provides.add(provide);
    }
    public Iterable<astprovide> getLocalProvides() {
        return provides;
    }

    /*
     * Properties
     */
    protected ArrayList<astproperty> properties = new ArrayList<astproperty>();
    
    public void addProperty(astproperty property) {
        properties.add(property);
    }
    public Iterable<astproperty> getLocalProperty() {
        return properties;
    }
}
