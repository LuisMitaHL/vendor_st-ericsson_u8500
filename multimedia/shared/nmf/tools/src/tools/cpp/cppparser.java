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
package tools.cpp;

import java.io.*;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.TreeSet;

import tools.common.*;
import tools.common.cache.file;
import tools.cpp.parser.CPPParser;
import tools.cpp.parser.ParseException;
import tools.cpp.parser.TokenMgrError;

public class cppparser extends configuration {
    
    public static class path implements Comparable<path>
    {
        public File path;
        public String filename;
        public File file;
        public path(File _path, String _filename) 
        {
            path = _path;
            filename = _filename;
            file = new File(path, filename);
        }
        
        public boolean exists() 
        {
            return file.exists();
        }
        
        public String toString() 
        {
            return file.toString();
        }

        public int compareTo(path o) {
            return file.compareTo(o.file);
        }
    }

    private static Hashtable<String, TreeSet<path>> CFiles = new Hashtable<String, TreeSet<path>>();
    
    static path seachDependency(File parent, String include, ArrayList<File> directories) {
        for(File dir : directories) 
        {
            path incfile = new path(dir, include);
            if(incfile.exists()) 
                return incfile;
        }

        // Try if relative to "."
        if(parent != null) 
        {
            path incfile = new path(parent, include);
            if(incfile.exists()) 
                return incfile;
        }
        
        return null;
    }
    
    static TreeSet<path> computeDependencies(
            File src, 
            ArrayList<File> directories) throws compilerexception, IOException {
        TreeSet<path> files = CFiles.get(src.getCanonicalPath());
        if(files == null) {
            debug(Debug.step, "Preprocess " + src.getCanonicalPath());
            
            try {
                // Parse the C file
                ArrayList<String> includeAbsolutes = new ArrayList<String>();
                ArrayList<String> includeRelatives = new ArrayList<String>();

                FileInputStream inputfile = new FileInputStream(src);
                CPPParser parser = new CPPParser(inputfile);
                parser.Parse(includeAbsolutes, includeRelatives);
                inputfile.close();                
                
                // Add it in the database
                files = new TreeSet<path>();
                CFiles.put(src.getCanonicalPath(), files);

                // Recursively compute include file
                for(String include : includeAbsolutes) 
                {
                    path incfile = seachDependency(null, include, directories);
                    if(incfile != null) {
                        files.add(incfile);
                        files.addAll(computeDependencies(incfile.file, directories));
                    }
                }
                for(String include : includeRelatives) 
                {   path incfile = seachDependency(src.getParentFile(), include, directories);
                    if(incfile != null) {
                        files.add(incfile);
                        files.addAll(computeDependencies(incfile.file, directories));
                    }
                }

                debug(Debug.dep, "DEPENDENCY " + src + " <- "  + files);
            } catch (ParseException e) {
                throw new compilerexception(new astbasefile(src, 0, 0), 
                        error.SYNTAXIC_ERROR, e.getMessage());
            } catch (TokenMgrError e) {
                throw new compilerexception(new astbasefile(src, 0, 0),  
                        error.LEXICAL_ERROR, e.getMessage());
            }
        } else {
            debug(Debug.step, "Reuse " + src.getCanonicalPath());
        }
        
        return files;
    }

    /*
    if(src.getName().endsWith("." + ASM)) {
        // Nothing to do
        return null;
    } else {
        ArrayList<String> argc;

        argc = new ArrayList<String>();
        argc.add(GCC);
        addList(getFlags(TFlags.CPPFLAGS), argc);
        addAsInclude(configuration.sharedir, argc);
        addAsInclude(configuration.srcdir, argc);
        addAsInclude(configuration.tmpbasedir, argc);
        addAsInclude(configuration.tmpcontextdir, argc);
        argc.add("-MM");
        argc.add("-MT" + configuration.toMakePath(obj));
        argc.add("-MF" + configuration.toMakePath(dep));
        argc.add(configuration.toMakePath(src));

        return exec.startArgc(Debug.env, argc, ErrorType.AlaGnu);
    }
    */
    public static TreeSet<path> createDependencies(
            File src) throws compilerexception {
        try {
            // Compute -I included C directory
            ArrayList<File> directories = new ArrayList<File>();
            directories.addAll(configuration.sharedir);
            directories.addAll(configuration.srcdir);
            directories.add(configuration.tmpsrcdir);

            // Write dependency in file
            return computeDependencies(src, directories);
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
            return null;
        }
    }

    public static void createDependenciesFile(
            File src, 
            File dep) throws compilerexception {
        try {
            // Compute dependencies
            TreeSet<path> files = createDependencies(src);
            
            // Write dependency in file
            dep.getParentFile().mkdirs();
            PrintStream out = new PrintStream(dep);
            out.println(src.getAbsolutePath());
            for(path incfile : files) {
                out.println(incfile.file.getAbsolutePath());
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static boolean needRecompile(File obj, File dep) {
        if(! obj.exists()) {
            configuration.debug(Debug.dep, "First compile " + configuration.toMakePath(obj));
            return true;
        }
        if(! dep.exists()) {
            configuration.debug(Debug.dep, "Recompile " + configuration.toMakePath(obj) + " (no dependency file)");
            return true;
        }

        try {
            FileReader fr = new  FileReader(dep);
            BufferedReader reader = new BufferedReader(fr);

            // Read entire file
            String include = null;
            while((include = reader.readLine()) != null) {
                if(file.getModifiedDate(include) > obj.lastModified()) {
                    configuration.debug(Debug.dep, "Recompile " + configuration.toMakePath(obj) + " (" + include + ")");
                    return true;
                }
            }
        } catch(FileNotFoundException e) {
            configuration.debug(Debug.dep, "Recompile " + configuration.toMakePath(obj) + " (dependency computation failed)");
            return true;
        } catch(IOException e) {
            configuration.debug(Debug.dep, "Recompile " + configuration.toMakePath(obj) + " (dependency computation failed)");
            return true;
        }

        configuration.debug(Debug.dep, "Don't need to recompile " + configuration.toMakePath(obj));
        return false;
    }
}
