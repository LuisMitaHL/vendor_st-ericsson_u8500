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

import java.util.*;
import java.io.*;

import tools.common.configuration;
import tools.common.configuration.Debug;
import tools.conf.ast.astsource.SourceType;

public class astprimitive extends astcomponent {

    public astprimitive(File _file, String _fullyname) {
        super(_file, _fullyname);
    }

    public astprimitive(astprimitive _other) throws IOException {
        super(_other);
        this.singleton = _other.singleton;

        String oldpattern = _other.fullyname.replace('.', '/') + ".nmf";
        String newpattern = fullyname.replace('.', '/') + ".nmf";

        // Copy source
        for(final astsource orig : _other.sources) {
            astsource dst = new astsource(
            		new File(configuration.tmpsrcdir, 
            				fullyname.replace('.', File.separatorChar) + File.separator + "src" + 
            				File.separator + cloneNumber + "-" + orig.file.getName()));
            if(dst.file.lastModified() < orig.file.lastModified()) {
                configuration.debug(Debug.step, "Copy " + orig.file.getPath() + " -> " + dst.file.getPath());
                
                // Create parent directory
                dst.file.getParentFile().mkdirs();

                /*
                 * Copy the content
                 */
                FileWriter fw = new FileWriter(dst.file);
                BufferedWriter out = new BufferedWriter(fw);
                FileReader fr = new FileReader(orig.file);
                BufferedReader in = new BufferedReader(fr);
                String line;
                while ((line = in.readLine()) != null) {
                    String newline = line.replaceFirst(oldpattern, newpattern);
                    out.write(newline);
                    out.newLine();
                }
                // Close it in order to copy file if necessary
                in.close();	
                fr.close();
                out.close();
                fw.close();
            }

            sources.add(dst);
        }

        this.attributes = _other.attributes;
    }

    /*
     * Sources
     * Binary Objects
     */
    public LinkedHashSet<astsource> sources = new LinkedHashSet<astsource>();
    
    public SourceType getSourceType() {
        for(final astsource source : sources) 
        {
            if(source.sourceType == SourceType.CPPCource)
                return SourceType.CPPCource;
        }
        return SourceType.CSource;
    }

    /* ----------------------------------------------------------------
     * Attributes
     * ---------------------------------------------------------------- */
    protected ArrayList<astattribute> attributes = new ArrayList<astattribute>();
    public void addAttribute(astattribute attribute) {
        attributes.add(attribute);
    }
    public Iterable<astattribute> getLocalAttributes() {
        return attributes;
    }
}
