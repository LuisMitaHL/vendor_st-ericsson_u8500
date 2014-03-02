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
package tools.common;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.StringTokenizer;

import tools.conf.ast.astcomponent;
import tools.idl.ast.Interface;
import tools.idl.ast.definedtype;

import tools.common.console.factory;
import tools.common.console.foregroundcolor;
import tools.common.targetfactory.CodingStyle;
import tools.common.mail.Attachements;

public class configuration {
    public enum validity {Vmandatory, Vswitch, Voptional};
    public static class option {
        String option, value, comment;
        validity validity;
        public option(String _option, String _value, String _comment, validity _validity) {
            option = _option;
            value = _value;
            comment = _comment;
            validity = _validity;
        }
    }
    /*----------------------------------------------------------------------*/
    /* Options management                                                   */
    /*----------------------------------------------------------------------*/
    private static void displayHelp(
            String tool,
            String filename,
            String message, 
            option options[],
            boolean hugehelp) {
        error("Usage: " + tool + " <switch(s)> " + filename);
        error("  " + message);

        error("  At least one of the following switches must be given:");
        error("    --help					Display information");
        error("    --version                Display version number");
        for(int i = 0; i < options.length; i++) {
            if(options[i].validity == validity.Vswitch) {
                String s = String.format("    %-25s%s", options[i].option + "  " + options[i].value, options[i].comment);
        		error(s);
        	}
        }
        error("");

        error("  The following switches must be given:");
        error("    --cpp                    Generate C++ project");
        error("    --target=target          Target name (default = $TARGET)");
        error("                             Allowed target: 'dsp24', 'dsp16', 'rvct',");
        error("                                'armgcc', 'unix', 'xp70'");
        error("    --srcpath=<path>         Source path (default = $SRCPATH separate by ';')");
        error("    --sharepath=<path>       Shared source path (default = $SHAREPATH separate by ';')");
        error("    --output=<dir>           Ouput directory (default = $BUILD)");
        error("        -o <dir>");
        error("    --stoponwarning          Stop compilation if warning meet in C/C++ compiler");
        for(int i = 0; i < options.length; i++) {
            if(options[i].validity == validity.Vmandatory) {
                String s = String.format("    %-25s%s", options[i].option + "  " + options[i].value, options[i].comment);
                error(s);
            }
        }
        error("");

        error("  The following switches are optional:");
        error("    --tmpoutput=<dir>        Directory where generate intermediaire files");     
        error("    --verbose                Run program in verbose mode");
        error("    --longpath               Generate small path for output");
        error("    -j <jobs>                Number of commands to run simultaneously");
        error("                                 (default = $NMF_JOBS or 1)");
        for(int i = 0; i < options.length; i++) {
            if(options[i].validity == validity.Voptional) {
                String s = String.format("    %-25s%s", options[i].option + "  " + options[i].value, options[i].comment);
                error(s);
            }
        }
        error("");
        
        if(hugehelp) {
        	error("  The following environment variables are also take into account:");
        	error("    $CPPFLAGS		C preprocessor flags");
        	error("    $CFLAGS			C Compilation flags");
            error("    $CXXFLAGS        CXX Compilation flags");
        	error("    $LDFLAGS			Link flags used during final executable linking");
            error("    $LIBLDFLAGS      Link flags used during intermediary library linking");
            error("    $GROUPLDFLAGS    Link flags used during linking and add in files group");
        	error("    $BC_CFLAGS		C/C++ Flags for Binding components");	
            error("");
        	error("    $NMF_DEBUG       Active NMF tools debugging");
        	error("         NMF_DEBUG = all | (debug ('|' debug)*)");
        	error("         debug=");
        	error("            step:   NK tools steps");
        	error("            env:    Operating System Environment");
        	error("            dep:    Dependencies computed");
        	error("            option: Compilation option computed");
        	error("            comp:   Component information");
            error("    $NMF_VERBOSE     Verbosity (preempted by option)");
            error("         false      To summarize executed command");
            error("         true       To fully display executed command");
            error("    $NMF_STOPONWARNING Control compilation if warning meet in C/C++ compiler (preempted by option)");
            error("         false      Don't stop");
            error("         true       Stop");
            error("    $ANSI_COLORS_DISABLED    To deactivate ouput colors");
        	error("");
        	error("    Specific to mmdsp (dsp16 or dsp24) target:");
        	error("      $MMDSPAC       MMDSP compiler executable (default = ac)");
            error("      $MMDSPAR       MMDSP archiver (default = mmdsp-ar)");
            error("      $MMDSPLD       MMDSP linker (default = mmdsp-ld)");
            error("    Specific to gcc target:");
            error("      $CC            Gnu C compiler executable (default = gcc)");
            error("      $CXX           Gnu C++ compiler executable (default = gcc)");
            error("      $ASM           Gnu ASM compiler executable (default = as)");
            error("      $AR            Gnu AR archiver executable (default = ar)");
            error("      $EXELD         Gnu LD executable linker executable (default = ld)");
            error("      $LIBLD         Gnu LD object linker executable (default = ld)");
            error("      Specific to STxp70 target:");
            error("        $CC            STxp70 compiler executable (default = stxp70cc)");
            error("        $EXELD         STxp70 executable linker executable (default = stxp70-ld)");
            error("        $LIBLD         STxp70 object linker executable (default = stxp70-ld)");
           error("      Specific to armgcc target:");
            error("        $CC            Arm Gnu C compiler executable (default = arm-none-eabi-gcc)");
            error("        $CXX           Arm Gnu C++ compiler executable (default = arm-none-eabi-g++)");
            error("        $ASM           Arm Gnu ASM compiler executable (default = arm-none-eabi-as)");
            error("        $AR            Arm Gnu AR archiver executable (default = arm-none-eabi-ar)");
            error("        $EXELD         Arm gnu executable LD executable (default = arm-none-eabi-ld)");
            error("        $LIBLD         Arm gnu object LD executable (default = arm-none-eabi-ld)");
            error("    Specific to rvct target:");
            error("        $CC            RVCT C compiler executable (default = armcc)");
            error("        $CXX           RVCT C++ compiler executable (default = armcpp)");
            error("        $ASM           RVCT ASM compiler executable (default = armasm)");
        	error("        $AR            RVCT archiver executable (default = armar)");
        	error("        $EXELD         RVCT executable linker executable (default = armlink)");
            error("        $LIBLD         RVCT object linker executable (default = armlink)");
      }
    }
    
    public static ArrayList<String> parseOption(
            String args[],
            String tool,
            String filename,
            String message, 
            option options[]) throws compilerexception {
        ArrayList<String> argl = new ArrayList<String>();

        if (args.length == 0)
        	throw new compilerexception(error.INVALID_ENVIRONMENT, "No input component");

        /*
         * Parse options
         */
        debug(Debug.env, Arrays.asList(args).toString());

        for(int i = 0; i < args.length; i++) {
            String option = args[i];
            String value = (i+1 < args.length) ? args[i+1] : "";
            if(option.startsWith("--srcpath=")) {
                convertPath(option.substring(10), srcdir = new ArrayList<File>());
            } else if(option.startsWith("--sharepath=")) {
                convertPath(option.substring(12), sharedir = new ArrayList<File>());
            } else if(option.startsWith("--dumperror")) {
                error.dumpError();
                System.exit(0);
            } else if("-o".equals(option)) {
                dstdir = getDirectory(value);
                i++;
            } else if(option.startsWith("--output=")) {
                dstdir = getDirectory(option.substring(9));
            } else if("-j".equals(option)) {
                numberOfJobs = Integer.parseInt(value);
                i++;
            } else if(option.startsWith("--tmpoutput=")) {
                tmpbasedir = getDirectory(option.substring(12));
            } else if("--cpp".equals(option)) {
                targetfactory.style = CodingStyle.CPP;
            } else if(option.startsWith("--target=")) {
                targetfactory.SettedTARGET = option.substring(9);
            } else if(option.equals("--version")) {
                error("NMF Version: " + tools.version.major + "." + tools.version.minor + "." + tools.version.patch);
                System.exit(0);
            } else if(option.equals("--longpath")) {
                longpath = true;
            } else if(option.equals("--help")) {
                displayHelp(tool, filename, message, options, true);
                System.exit(0);
            } else if(option.equals("--verbose")) {
                verbose = true;
            } else if(option.equals("--stoponwarning")) {
                stoponwarning = true;
            } else if(option.startsWith("-")) {
                // Search if option exists and display help if not
                boolean foundoption = false;
                for(int j = 0; j < options.length; j++) {
                    if((options[j].option.endsWith("=") && option.startsWith(options[j].option)) ||
                            option.equals(options[j].option)) {
                        foundoption = true;
                        break;
                    }
                }
                if(foundoption)
                    argl.add(option);
                else {
                    displayHelp(tool, filename, message, options, false);
                    throw new compilerexception(error.INVALID_ENVIRONMENT, "Unrecognized option '" + option + "'");
                }
            } else {
                argl.add(option);
            }
        }
        
        // Set default values to environment value if not set through commmand line
        if(dstdir == null) {
            String dir = getEnvDefault("BUILD", "build");
            dstdir = getDirectory(dir);
        }
        debug(Debug.env, "$BUILD=" + dstdir);

        if(srcdir == null) {
            String path = getEnvDefault("SRCPATH", "");
            convertPath(path, srcdir = new ArrayList<File>());
        }
        debug(Debug.env, "$SRCPATH=" + srcdir);

        if(sharedir == null) {
            String path = getEnvDefault("SHAREPATH", "");
            convertPath(path, sharedir = new ArrayList<File>());
        }
        debug(Debug.env, "$SHAREPATH=" + sharedir);

        if(tmpbasedir == null) {
        	tmpbasedir = new File(dstdir, "tmp");
        }
        debug(Debug.env, "tmpoutput=" + tmpbasedir);

        if(numberOfJobs <= 0)
            throw new compilerexception(error.INVALID_ENVIRONMENT, "Variable environment 'NMF_JOBS' or command line '-j' must be striclty positive");

        return argl;
    }

    /*----------------------------------------------------------------------*/
    /* Trace management                                                     */
    /*----------------------------------------------------------------------*/
    /**
     * Debug levels
     */
    public enum Debug {
        always,
        error,      
        step, 		//!< NMF tools steps
        env,		//!< Operating System Environment 
        dep, 		//!< Dependencies computed
        option, 	//!< Compilation option computed
        comp		//!< Component information
    };
	
    protected static HashMap<Debug, foregroundcolor> debugColor = new HashMap<Debug, foregroundcolor>();
    protected static HashSet<Debug> debugSetted = new HashSet<Debug>();
    public static boolean stoponwarning = Boolean.parseBoolean(getEnvDefault("NMF_STOPONWARNING", "false"));;
    public static boolean verbose = Boolean.parseBoolean(getEnvDefault("NMF_VERBOSE", "false"));

    static {
        // Set Color
        debugColor.put(Debug.error, foregroundcolor.LIGHT_RED);
        debugColor.put(Debug.step, foregroundcolor.LIGHT_BLUE);
        debugColor.put(Debug.env, foregroundcolor.LIGHT_CYAN);
        debugColor.put(Debug.dep, foregroundcolor.LIGHT_GREEN);
        debugColor.put(Debug.option, foregroundcolor.LIGHT_CYAN);
        debugColor.put(Debug.comp, foregroundcolor.LIGHT_MAGENTA);

        /* 
         * Active trace mechanism
         */
        String NMF_DEBUG = getEnvDefault("NMF_DEBUG", "").toLowerCase();

        // Set debug level
        if("all".equals(NMF_DEBUG)) {
            // Set all value to be debugged
            for(Debug d: Debug.values())
                debugSetted.add(d);
        } else {
            StringTokenizer st = new StringTokenizer(NMF_DEBUG, "|");
            while (st.hasMoreTokens()) {
                try {
                    debugSetted.add(Debug.valueOf(st.nextToken()));
                } catch(IllegalArgumentException e) {
                    error("Invalid $NMF_DEBUG=" + NMF_DEBUG); 
                    error("    $NMF_DEBUG       Active NMF tools debugging");
                    error("         NMF_DEBUG = all | (debug ('|' debug)*)");
                    error("         debug=");
                    error("            step:   NK tools steps");
                    error("            env:    Operating System Environment");
                    error("            dep:    Dependencies computed");
                    error("            option: Compilation option computed");
                    error("            comp:   Component information");
                    System.exit(-1);
                }
            }
            debugSetted.add(Debug.always);
        }
        
    }
   
    /**
     * Test if a debug level is active
     * @param debug Debug level
     * @return
     */
    public static boolean isDebugged(Debug debug) {
        return debugSetted.contains(debug);
    }

    /**
     * Display a debug message if corresponding debug level is active
     * @param debug Debug level
     * @param message
     */
    public static void debug(Debug debug, String message) {
        if(debugSetted.contains(debug)) {
            foregroundcolor fg = debugColor.get(debug);
            if(fg != null) {
                factory.getOutConsole().setForegroundColor(fg);
                System.out.println(message);
                factory.getOutConsole().resetColors();
            } else {
                System.out.println(message);
            }
        }
    }

    /**
     * Display colored warning
     * @param message Warning message to be displayed
     */
    public static void warning(String message) {
        factory.getErrConsole().setForegroundColor(foregroundcolor.LIGHT_GREEN);
        System.err.println(message);
        factory.getErrConsole().resetColors();
    }

    /**
     * Display colored warning
     * @param e
     */
   public static void warning(compilerexception e) {
        factory.getErrConsole().setForegroundColor(foregroundcolor.LIGHT_GREEN);
        if(e.parsedFile != null)
            System.err.print(e.parsedFile.getPath() + ":");
        if(e.line != 0)
            System.err.print(e.line + ":");
        System.err.println(e.getMessage());
        factory.getErrConsole().resetColors();
    }

    /**
     * Display colored error
     * @param e
     */
    public static void error(compilerexception e) {
        factory.getErrConsole().setForegroundColor(foregroundcolor.LIGHT_RED);
        if(e.parsedFile != null)
            System.err.print(e.parsedFile.getPath() + ":");
        if(e.line != 0)
            System.err.print(e.line + ":");
        System.err.println(e.getMessage());
        factory.getErrConsole().resetColors();
    }

    /**
     * Display colored error
     * @param message  Warning error to be displayed
     */
    public static void error(String message) {
        factory.getErrConsole().setForegroundColor(foregroundcolor.LIGHT_RED);
        System.err.println(message);
        factory.getErrConsole().resetColors();
    }

    /*----------------------------------------------------------------------*/
    /* Environment variables management                                     */
    /*----------------------------------------------------------------------*/

    /**
     * Return a environment variable and if not filled return the default value.
     * @param var Environment variable name
     * @param def Default value
     * @return Environment variable value
     */
    public static String getEnvDefault(String var, String def) {
        String value = System.getenv(var);
        if(value == null || "".equals(value)) value = def;	
        //System.out.println(var + " = " + value);
        return value;
    }

    /*----------------------------------------------------------------------*/
    /* File management                                                      */
    /*----------------------------------------------------------------------*/
    public final static String pathSeparator = ";";
    public final static String eol = System.getProperty("line.separator", "\n");

    public static boolean longpath = false;
    
    public static File dstdir = null;
    public static File tmpbasedir = null;
    public static File tmpsrcdir = null;
    public static File tmpobjdir = null;
    public static ArrayList<File> srcdir = null;
    public static ArrayList<File> sharedir = null;

    private static HashMap<String, String> lastnames = null;
    private static int lastnameid;
    
    public static void startContext(String fullyname) {
        tmpobjdir = new File(tmpbasedir, fullyname.replace(".", "_") + File.separator + "obj");
        tmpsrcdir = new File(tmpbasedir, fullyname.replace(".", "_") + File.separator + "src");
        lastnames = new HashMap<String, String>();
        lastnameid = 0;
    }
    public static void startContext() {
        tmpobjdir = tmpbasedir;
        tmpsrcdir = tmpbasedir;
        lastnames = new HashMap<String, String>();
        lastnameid = 0;
    }
    public static void stopContext() {
        tmpobjdir = null;
        tmpsrcdir = null;
        lastnames = null;
        Attachements.reset();
  }


    /**
     * Convert directory to raw OS file system conventions!!!!
     */
    protected static File getDirectory(String dirname) {
        String windirname = dirname;
        if(getEnvDefault("OS", "").toLowerCase().contains("windows")) {
            // We are on MingW: Shell path must be converted to Windows path
            // /x/yyyyyyyy -> x:/yyyyyyyy
            if(dirname.charAt(0) == '/' && 
                    (Character.toLowerCase(dirname.charAt(1)) >= 'a' && 
                            Character.toLowerCase(dirname.charAt(1)) >= 'a') &&
                            dirname.charAt(2) == '/') {
                windirname = dirname.charAt(1) + ":" + File.separator + 
                dirname.substring(3).replace('/', File.separatorChar);
                debug(Debug.env, "DIR " + dirname + " -> " + windirname);
            }
        } 
        return new File(windirname);
    }
    
    /**
     * Convert String path -> Array List path
     */
    protected static void convertPath(String pathStr, ArrayList<File> path) {
        StringTokenizer st = new StringTokenizer(pathStr, pathSeparator);
        while (st.hasMoreTokens()) {
            path.add(getDirectory(st.nextToken()));
        }
    }
    
    /**
     * Search a file into the sources path
     * @param filename File name
     * @return Founded file (null if not found)
     */
    public static File getFile(String filename) {
        for(int i = 0; i < srcdir.size(); i++) {
            File file = new File(srcdir.get(i),  filename);
            if(file.exists()) 
                return file;
        }
        File file = new File(tmpsrcdir,  filename);
        if(file.exists()) 
            return file;
        return new File(filename);
    }

    /**
     * Search a file into the sources path or shared path.
     * @param filename File name
     * @return Founded file (null if not found)
     */
    public static File getSharedFile(String filename) {
        for(int i = 0; i < srcdir.size(); i++) {
            File file = new File(srcdir.get(i),  filename);
            if(file.exists()) 
                return file;
        }
        for(int i = 0; i < sharedir.size(); i++) {
            File file = new File(sharedir.get(i),  filename);
            if(file.exists()) 
                return file;
        }
        return new File(filename);
    }

    /**
     * Get generated files
     * @param filename
     * @return
     */
    protected static File getFinalFile(String filename) {
        File file = new File(dstdir, filename);
        return file;
    }
    protected static File getBinaryFile(String filename) {
    	assert (tmpobjdir != null) : "Component context no set";
    	
        File file = new File(tmpobjdir, filename);
        return file;
    }
    public static File getSourceFile(String filename) {
        assert (tmpsrcdir != null) : "Component context no set";

        File file = new File(tmpsrcdir, filename);
        return file;
    }

    public static File getComponentFinalExt(astcomponent component, String extension) {
        return getFinalFile(component.fullyname.replace('.', File.separatorChar) + "." + extension);
    }
    public static File getComponentTemporaryExt(astcomponent component, String extension) {
        return getSourceFile(component.fullyname.replace('.', File.separatorChar) + "." + extension);
    }
    
    public static File getComponentUniqueTemporaryFile(astcomponent component, String lastname) {
        String exitingcomponent = lastnames.get(lastname);
        if(component.fullyname.equals(exitingcomponent))
        {
            // Do nothing
        }
        else
        {
            if(exitingcomponent != null)
            {
                // Same file exist for another component -> create a new filename
                lastname = lastnameid + "_" + lastname;
                lastnameid++;
            }
            lastnames.put(lastname, component.fullyname);
        } 

        return getComponentTemporaryFile(component, lastname);
    }
    public static File getComponentTemporaryFile(astcomponent component, String filename) {
        return getSourceFile(component.fullyname.replace('.', File.separatorChar) + File.separator + filename);
    }
    public static File getComponentBinaryFile(astcomponent component, String filename) {
        return getBinaryFile(component.fullyname.replace('.', File.separatorChar) + File.separator + filename);
    }
    public static File getComponentTemporaryUniqueFile(astcomponent component, String extension) {
        return getBinaryFile(component.fullyname.replace('.', '_') + '.' + extension);
    }

    public static File getInterfaceExt(boolean inFinalDirectory, String subdirectory, Interface itf, String extension) {
        String filename = itf.name.replace('.', File.separatorChar) + "." + extension;
        filename = subdirectory + filename;
        if(inFinalDirectory)
            return getFinalFile(filename);
        else
            return getSourceFile(filename);
    }
    public static File getIdtExt(boolean inFinalDirectory, String subdirectory, definedtype idt, String extension) {
        String filename = idt.nameUsedAsInclude + "." + extension;
        filename = subdirectory + filename;
        if(inFinalDirectory)
            return getFinalFile(filename);
        else
            return getSourceFile(filename);
    }

    public static String toMakePath(File file) {
        return file.getPath().replace('\\', '/');
    }

    public enum TypeHeader {makefile, RvctAsm, C};
    protected static void addHeader(TypeHeader header, PrintStream out)
    {
        switch(header)
        {
        case makefile:
            out.println("# Copyright (C) ST-Ericsson SA 2010. All rights reserved.");
            out.println("# This code is ST-Ericsson proprietary and confidential.");
            out.println("# Any use of the code for whatever purpose is subject to");
            out.println("# specific written permission of ST-Ericsson SA.");
            out.println();
            break;
        case RvctAsm:
            out.println("; Copyright (C) ST-Ericsson SA 2010. All rights reserved.");
            out.println("; This code is ST-Ericsson proprietary and confidential.");
            out.println("; Any use of the code for whatever purpose is subject to");
            out.println("; specific written permission of ST-Ericsson SA.");
            out.println();
            break;
        case C:
            out.println("/*");
            out.println(" * Copyright (C) ST-Ericsson SA 2010. All rights reserved.");
            out.println(" * This code is ST-Ericsson proprietary and confidential.");
            out.println(" * Any use of the code for whatever purpose is subject to");
            out.println(" * specific written permission of ST-Ericsson SA.");
            out.println(" */");
            out.println();

            out.println("/*****************************************************************************/");
            out.println("/**");
            out.println(" * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.");
            out.println(" */");
            out.println();
            break;
        }
    }

    public static PrintStream forceRegeneration(File target) 
    {
        return forceRegeneration(target, TypeHeader.C);
    }
    
    public static PrintStream forceRegeneration(File target, TypeHeader header) 
    {
        debug(Debug.dep, "Generate " + target.getPath());
        try {
            target.getParentFile().mkdirs();
            PrintStream stream = new PrintStream(new FileOutputStream(target));
            if(stream != null)
                addHeader(header, stream);
            return stream;
        } catch(FileNotFoundException e) {
            error(e.getMessage());
            //e.printStackTrace();
            System.exit(255);
        }
        return null;
    }
    
    public static PrintStream needRegeneration(File source, File target) 
    {
        return needRegeneration(source, target, TypeHeader.C);
    }
        
    public static PrintStream needRegeneration(File source, File target, TypeHeader header) 
    {
        assert (source != null) : "Null source file (target = " + target + ")";
        assert (target != null) : "Null target file (source = " + source + ")";

        if(target.lastModified() >= source.lastModified()) {
            //debug(Debug.dep, "Don't need to regenerate " + target.getPath() + " (" + source.getPath() + ")");
        } else {
            debug(Debug.dep, "Generate " + target.getPath() + " (" + source.getPath() + " modified )");
            try {
                target.getParentFile().mkdirs();
                PrintStream stream = new PrintStream(new FileOutputStream(target));
                if(stream != null)
                    addHeader(header, stream);
                return stream;
            } catch(FileNotFoundException e) {
                error(e.getMessage());
                //e.printStackTrace();
                System.exit(255);
            }
        }
        return null;
    }
    
    public static File getMostRecent(File file1, File file2) {
        if(file1 == null)
            return file2;
        if(file2 == null)
            return file1;
        if(file1.lastModified() < file2.lastModified()) 
            return file2;
        return file1;
    }

    public static void copyFile(File in, File out) {
        if(out.lastModified() >= in.lastModified()) {
            debug(Debug.dep, "Don't need to copy " + in.getPath() + " -> " + out.getPath());
        } else {
            debug(Debug.dep, "Copy " + in.getPath() + " -> " + out.getPath());
            try {
                // Create parent directory file
                out.getParentFile().mkdirs();
                
                // Copy the content
                FileInputStream fis  = new FileInputStream(in);
                FileOutputStream fos = new FileOutputStream(out);
                byte[] buf = new byte[1024];
                int i = 0;
                while((i=fis.read(buf))!=-1) {
                    fos.write(buf, 0, i);
                }
                fis.close();
                fos.close();
            } catch(IOException e) {
                error(e.getMessage());
                //e.printStackTrace();
                System.exit(255);
            }
        }
    }
    
    /*
     * Compilation management
     */
    public configuration() {
	try {
	    numberOfJobs = Integer.parseInt(getEnvDefault("NMF_JOBS", "1"));
	}
	catch(NumberFormatException e) {
	    error(e.getMessage());
	    error("NMF_JOBS must be an integer !!");
	    System.exit(255);
	}
    }

    public static int numberOfJobs = 1;
}
