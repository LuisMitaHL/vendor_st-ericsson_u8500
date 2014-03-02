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

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.compiler.ac;
import tools.common.compiler.armgcc;
import tools.common.compiler.ccompiler;
import tools.common.compiler.symbian;
import tools.common.compiler.linux;
import tools.common.compiler.gcc;
import tools.common.compiler.rvct;
import tools.common.compiler.stxp70;
import tools.common.configuration.Debug;

public class targetfactory 
{
    /* 
     * Compilation flags
     */
    public static boolean staticPicCompilation = false;
    
    /*
     * Link type
     */
    public enum LinkType 
    {
        EXECUTABLE  (true), 
        PARTIAL     (true), 
        ARCHIVE     (false), 
        DLL         (false);
        
        public boolean uniqueLCCMethodName;
        
        LinkType(boolean _uniqueLCCMethodName)
        {
            uniqueLCCMethodName = _uniqueLCCMethodName;
        }
    }
    public static LinkType linktype = null;

    /*
     * Coding Style
     */
    public enum CodingStyle 
    {
        C       (ccompiler.C), 
        CPP     (ccompiler.CPP);
        
        public String SourceExtension;
        
        CodingStyle(String _SourceExtension)
        {
            SourceExtension = _SourceExtension;
        }
    }
    public static CodingStyle style = null;

    /*
     * Domain
     */
    @SuppressWarnings("unchecked")
    public enum DomainNames 
    { 
        MMDSP       (CodingStyle.C, 
                tools.generator.c.CWrapGenerator.class, tools.generator.c.CAssembleGenerator.class, tools.generator.c.MMDSP.MMDSPMetaDataGenerator.class,  
                tools.generator.c.MMDSP.MMDSPEventGenerator.class, tools.generator.c.MMDSP.MMDSPTraceGenerator.class, 
                null, tools.generator.c.MMDSP.MMDSPStubGenerator.class, tools.generator.c.MMDSP.MMDSPSkeletonGenerator.class), 
        HostEE      (CodingStyle.CPP, 
                tools.generator.cpp.CppWrapGenerator.class, tools.generator.cpp.hostee.HostEEAssembleGenerator.class, null,
                tools.generator.cpp.hostee.eventgenerator.class, tools.generator.cpp.hostee.tracegenerator.class, 
                tools.generator.cpp.hostee.HostEEBoundariesBinder.class, null, null), 
        EmbeddedC   (CodingStyle.C, 
                tools.generator.c.CWrapGenerator.class, tools.generator.c.CAssembleGenerator.class, null,
                null, null, 
                null, tools.generator.c.Embedded.EmbeddedStubGenerator.class, tools.generator.c.Embedded.EmbeddedSkeletonGenerator.class), 
        EmbeddedCpp (CodingStyle.CPP, 
                tools.generator.cpp.CppWrapGenerator.class, tools.generator.cpp.embedded.CppEmbeddedAssembleGenerator.class, null,
                null, null, 
                null, tools.generator.cpp.embedded.CppEmbeddedStubGenerator.class, tools.generator.cpp.embedded.CppEmbeddedSkeletonGenerator.class), 
        EECpp       (CodingStyle.CPP, 
                tools.generator.cpp.CppWrapGenerator.class, tools.generator.cpp.ee.CppEEAssembleGenerator.class, tools.generator.cpp.ee.CppEEMetaDataGenerator.class,
                tools.generator.cpp.ee.CppEEEventGenerator.class, null, 
                null, tools.generator.cpp.ee.CppEEStubGenerator.class, tools.generator.cpp.ee.CppEESkeletonGenerator.class);
        
        protected Class<tools.generator.api.WrapGeneratorInterface> wrapper;
        protected Class<tools.generator.api.AssembleGeneratorInterface> assembler;
        protected Class<tools.generator.api.MetaDataGeneratorInterface> metadata;
        protected Class<tools.generator.api.EventGeneratorInterface> event;
        protected Class<tools.generator.api.TraceGeneratorInterface> trace;
        protected Class<tools.generator.api.BoundariesBinderInterface> boundariesBinder;
        protected Class<tools.generator.api.StubGeneratorInterface> stubGenerator;
        protected Class<tools.generator.api.SkeletonGeneratorInterface> skeletonGenerator;
        protected CodingStyle style;

        DomainNames(CodingStyle _style, 
                Class<?> _wrapper, Class<?> _assembler, Class<?> _metadata, 
                Class<?> _event, Class<?> _trace, 
                Class<?> _boundariesBinder, Class<?> _stubGenerator, Class<?> _skeletonGenerator) 
        {
            style = _style;
            wrapper = (Class<tools.generator.api.WrapGeneratorInterface>)_wrapper;
            assembler = (Class<tools.generator.api.AssembleGeneratorInterface>)_assembler;
            metadata = (Class<tools.generator.api.MetaDataGeneratorInterface>)_metadata;
            event = (Class<tools.generator.api.EventGeneratorInterface>)_event;
            trace = (Class<tools.generator.api.TraceGeneratorInterface>)_trace;
            boundariesBinder = (Class<tools.generator.api.BoundariesBinderInterface>)_boundariesBinder;
            stubGenerator = (Class<tools.generator.api.StubGeneratorInterface>)_stubGenerator;
            skeletonGenerator = (Class<tools.generator.api.SkeletonGeneratorInterface>)_skeletonGenerator;
        }

    }
    public static DomainNames domain = null;

    /*
     * Target
     */
    @SuppressWarnings("unchecked")
    public enum TargetNames
    {
        dsp24       (LinkType.DLL, ac.class, 
                DomainNames.MMDSP, null), 
        dsp16       (LinkType.DLL, ac.class,
                DomainNames.MMDSP, null), 
        rvct        (LinkType.EXECUTABLE, rvct.class, 
                DomainNames.EmbeddedC, DomainNames.HostEE), 
        armgcc      (LinkType.EXECUTABLE, armgcc.class, 
                DomainNames.EmbeddedC, DomainNames.HostEE), 
        unix        (LinkType.EXECUTABLE, gcc.class, 
                DomainNames.EmbeddedC, DomainNames.HostEE), 
        xp70        (LinkType.EXECUTABLE, stxp70.class, 
                DomainNames.EmbeddedC, null), 
        symbian     (LinkType.EXECUTABLE, symbian.class,
                DomainNames.EmbeddedC, DomainNames.HostEE),        
        linux       (LinkType.EXECUTABLE, linux.class,
                DomainNames.EmbeddedC, DomainNames.HostEE),
                
        armee       (LinkType.DLL, rvct.class, 
                null, DomainNames.EECpp),
        armcpp      (LinkType.PARTIAL, rvct.class,
                null, DomainNames.EmbeddedCpp);

        protected Class<ccompiler> compiler;
        
        protected DomainNames[] domains = new DomainNames[2];

        protected LinkType defaultLinkType;

        TargetNames(
                LinkType _defaultLinkType, Class<?> _compiler, 
                DomainNames _domainC, DomainNames _domainCPP) 
        {
            defaultLinkType = _defaultLinkType;
            compiler = (Class<ccompiler>)_compiler;
            domains[0] = _domainC;
            domains[1] = _domainCPP;
        }
        
    };

    public static TargetNames TARGET;
    public static String SettedTARGET = configuration.getEnvDefault("TARGET", "unknown");

    public static void setTarget() throws compilerexception 
    {
        try 
        {
            TARGET = TargetNames.valueOf(SettedTARGET);
            configuration.debug(Debug.env, "TargetNames = " + TARGET);
            
            if(style == null)
            {
                if(TARGET.domains[CodingStyle.C.ordinal()] == null && 
                        TARGET.domains[CodingStyle.CPP.ordinal()] != null)
                    style = CodingStyle.CPP;
                else
                    style = CodingStyle.C;
            }
            configuration.debug(Debug.env, "CodingStyle = " + style);

            domain = TARGET.domains[style.ordinal()];
            configuration.debug(Debug.env, "DomainName = " + domain);

            if(linktype == null)
            {
                linktype = TARGET.defaultLinkType;
            }
            configuration.debug(Debug.env, "LinkType = " + linktype);
        } 
        catch(IllegalArgumentException e) 
        {
            throw new compilerexception(error.INVALID_ENVIRONMENT, 
                    "Target  '" + SettedTARGET +"' compiled with '" + style + "' in '" + linktype + "' not supported");
        }
    }
    
    protected static void raiseError(String generator) throws compilerexception
    {
        throw new compilerexception(error.INVALID_ENVIRONMENT, 
                generator + " generator for domain '" + TARGET.domains[style.ordinal()] + "'/'" + style + "' of target'" + TARGET +"' not supported");
    }
    
    public static ccompiler getCompiler() throws compilerexception 
    {
        try {
            return TARGET.compiler.newInstance();
        } catch (Exception e) {
            throw new compilerexception(error.INVALID_ENVIRONMENT, 
                    "Compiling of target  '" + TARGET +"' in '" + style + "' not supported");
        }
    }
    
    public static tools.generator.api.WrapGeneratorInterface getWrapper() throws compilerexception 
    {
        try {
            return domain.wrapper.newInstance();
        } catch (Exception e) {
            throw new compilerexception(error.INVALID_ENVIRONMENT, 
                    "Wrapper generator for target  '" + TARGET +"' in '" + style + "' not supported");
        }
    }

    public static tools.generator.api.AssembleGeneratorInterface getAssembler() throws compilerexception 
    {
        try {
            return domain.assembler.newInstance();
        } catch (Exception e) {
            throw new compilerexception(error.INVALID_ENVIRONMENT, 
                    "Assemble generator for target  '" + TARGET +"' in '" + style + "' not supported");
        }
    }
    
    public static tools.generator.api.MetaDataGeneratorInterface getMetaDataGenerator() throws compilerexception {
        if(domain.metadata != null)
        {
            try {
                return domain.metadata.newInstance();
            } catch (Exception e) {
                throw new compilerexception(error.INVALID_ENVIRONMENT, 
                        "Meta data of target  '" + TARGET +"' can't be instantiated");
            }
        } 
        else
            return null;
    }

    public static tools.generator.api.EventGeneratorInterface getEvent() throws compilerexception 
    {
        try {
            return domain.event.newInstance();
        } catch (Exception e) {
            configuration.error(e.toString());
            raiseError("Event");
        }
        return null;
    }

    public static tools.generator.api.TraceGeneratorInterface getTrace() throws compilerexception 
    {
        try {
            return domain.trace.newInstance();
        } catch (Exception e) {
            raiseError("Trace");
        }
        return null;
    }

    public static tools.generator.api.BoundariesBinderInterface getBoundariesBinder() throws compilerexception {
        if(domain.boundariesBinder != null)
        {
            try {
                return domain.boundariesBinder.newInstance();
            } catch (Exception e) {
                throw new compilerexception(error.INVALID_ENVIRONMENT, 
                        "Boundaries binder of target  '" + TARGET +"' in '" + style + "' not supported");
            }
        } 
        else
            return null;
    }

    public static tools.generator.api.StubGeneratorInterface getStubGenerator() throws compilerexception {
        if(domain.stubGenerator != null)
        {
            try {
                return domain.stubGenerator.newInstance();
            } catch (Exception e) {
                throw new compilerexception(error.INVALID_ENVIRONMENT, 
                        "Stub generator for target  '" + TARGET +"' in '" + style + "' not supported");
            }
        } 
        else
            return null;
    }
    
    public static tools.generator.api.SkeletonGeneratorInterface getSkeletonGenerator() throws compilerexception {
        if(domain.skeletonGenerator != null)
        {
            try {
                return domain.skeletonGenerator.newInstance();
            } catch (Exception e) {
                throw new compilerexception(error.INVALID_ENVIRONMENT, 
                        "Skeleton generator for target  '" + TARGET +"' in '" + style + "' not supported");
            }
        } 
        else
            return null;
    }
}
