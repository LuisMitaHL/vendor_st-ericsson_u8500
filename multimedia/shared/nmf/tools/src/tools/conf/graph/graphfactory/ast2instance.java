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
package tools.conf.graph.graphfactory;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Stack;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.targetfactory;
import tools.common.targetfactory.DomainNames;
import tools.common.configuration.Debug;
import tools.conf.ast.astattribute;
import tools.conf.ast.astbind;
import tools.conf.ast.astcomponent;
import tools.conf.ast.astcomposite;
import tools.conf.ast.astcontent;
import tools.conf.ast.astdefinition;
import tools.conf.ast.astoption;
import tools.conf.ast.astparameter;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astproperty;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.asttype;
import tools.conf.graph.component;
import tools.conf.graph.composite;
import tools.conf.graph.primitive;
import tools.conf.graph.primitiveraw;

public class ast2instance {
    File mostRecentFile = null;
    component component;
    
    public ast2instance(component _component) {
        component = _component;
    }

    /*
     * Grammatical error detected:
     *      primitive Primitive<X>
     *      type Type<X>
     *      type Type(x)
     * Semantic error detected:
     *      Primitive extends Composite
     *      Composite extends Primitive
     *      Component extends Type<X>
     *      C extends .... extends C
     *      C extends A, A
     *      C<X x> C<TYPE> !(TYPE: X)
     * 
     */
    void computeExtend(asttype implementsType, HashSet<String> extended) throws compilerexception {
        for(astdefinition extend : implementsType.getLocalExtends()) {
            asttype extendType  = tools.conf.confcompiler.Compile(extend.type, extend);
            
            if(! extended.contains(extendType.fullyname)) {
                extended.add(extendType.fullyname);
                computeExtend(extendType, extended);
            }
        }            
    }
    
    void checkDefinition(astbasefile context,
            asttype implementsType, 
            ArrayList<String> templateparameterdefinitions, 
            ArrayList<String> attributeparameterdefinitions) throws compilerexception {
        if(implementsType instanceof astcomponent) 
        {
            if(implementsType instanceof astcomposite) 
            {
                /*
                 * Check that template parameter number correspond between declaration and definition
                 */
                ArrayList<astparameter> templateparameterdeclarations = ((astcomposite)implementsType).getLocalTemplateParameters();
                if(
                        (templateparameterdefinitions == null && templateparameterdeclarations.size() != 0) ||
                        (templateparameterdefinitions != null && templateparameterdeclarations.size() != templateparameterdefinitions.size()))
                    throw new compilerexception(context, error.WRONG_TEMPLATE_NUMBER, 
                            templateparameterdefinitions.size(), implementsType.fullyname, 
                            templateparameterdeclarations.size());

                /*
                 * Check that component give as parameter extend template parameter definition !
                 */
                if(templateparameterdefinitions != null) {
                    for(int i = 0; i < templateparameterdefinitions.size(); i++) {
                        asttype paramType  = tools.conf.confcompiler.Compile(templateparameterdefinitions.get(i), context);
                        HashSet<String> extended = new HashSet<String>();
                        computeExtend(paramType, extended);
                        String waitedType = templateparameterdeclarations.get(i).type;
                        if(! extended.contains(waitedType))
                            throw new compilerexception(context, error.NOT_COMPATIBLE_TEMPLATE,
                                    i+1, waitedType, paramType.fullyname + ": " + extended);
                    }
                }
            } 
            else if(implementsType instanceof astprimitive) 
            {
                if(templateparameterdefinitions != null && templateparameterdefinitions.size() != 0)
                    throw new compilerexception(context, error.PRIMITIVE_WITH_TEMPLATE);
            } 

            ArrayList<astparameter> attributeparameterdeclarations = ((astcomponent)implementsType).getLocalAttributeParameters();
            if(attributeparameterdefinitions == null && attributeparameterdeclarations.size() != 0)
                throw new compilerexception(context, error.WRONG_ATTRIBUTE_NUMBER, 
                        0, implementsType.fullyname, 
                        attributeparameterdeclarations.size());

            if (attributeparameterdefinitions != null && attributeparameterdeclarations.size() != attributeparameterdefinitions.size())
                throw new compilerexception(context, error.WRONG_ATTRIBUTE_NUMBER, 
                        attributeparameterdefinitions.size(), implementsType.fullyname, 
                        attributeparameterdeclarations.size());
     
            /*
             * Check that attribute value definition is castable to attribute declaration.
             */
            if(attributeparameterdefinitions != null) {
                for(int i = 0; i < attributeparameterdefinitions.size(); i++) {
                    astparameter param = attributeparameterdeclarations.get(i);
                    astattribute.checkValue(context, param.type, attributeparameterdefinitions.get(i));
                }
            }
        }
        else 
        {
            if(templateparameterdefinitions != null && templateparameterdefinitions.size() != 0)
                throw new compilerexception(context, error.PRIMITIVE_WITH_TEMPLATE);
            
            if(attributeparameterdefinitions != null && attributeparameterdefinitions.size() != 0)
                throw new compilerexception(context, error.TYPE_WITH_ATTRIBUTE);
        }
    }

    HashMap<String, String> ComputeParameters(
            ArrayList<astparameter> parameterdeclarations, 
            ArrayList<String> parameterdefinitions) throws compilerexception {
        HashMap<String, String> parameters = new HashMap<String, String>();

        if(parameterdefinitions != null) {
            for(int i = 0; i < parameterdefinitions.size(); i++) {
                astparameter param = parameterdeclarations.get(i);

                parameters.put(param.name, parameterdefinitions.get(i));
            } 
        }

        return parameters;
    }

    String ComputeParameter(HashMap<String, String> parameters, String param) {
        if(parameters != null) 
        {
            String overloadedvalue = parameters.get(param);

            if(overloadedvalue != null) {
                configuration.debug(Debug.comp, "Replace " + param + " with " + overloadedvalue);

                return overloadedvalue;
            } 
        }

        return param;
    }

    void copyDependencies(
            asttype _type, 
            Stack<asttype> stack, 
            Stack<asttype> cumul,
            HashMap<String, String> templateparameters,
            HashMap<String, String> attributeparameters) throws compilerexception {
        mostRecentFile = configuration.getMostRecent(mostRecentFile, _type.parsedFile);

        for(astdefinition extend : _type.getLocalExtends()) {
            // Check that type not extends twice on same component
            for(astdefinition extendbis : _type.getLocalExtends()) {
                if(extend != extendbis && extend.type.equals(extendbis.type))
                    throw new compilerexception(_type, error.EXTENDS_TWICE,
                            extend.type);
            }

            asttype implementsType = tools.conf.confcompiler.Compile(extend.type, extend);
            checkDefinition(extend, implementsType, extend.templates, extend.attributes);

            // Check that there is no extends in loop
            if(stack.contains(implementsType))
                throw new compilerexception(_type, error.LOOP_IN_EXTENDS,
                        stack);
            stack.push(implementsType);

            // Compute low level parameter according our parameters
            ArrayList<String> _templateparameters = new ArrayList<String>();
            ArrayList<String> _attributeparameters = new ArrayList<String>();
            for(String param : extend.templates) {
                _templateparameters.add(ComputeParameter(templateparameters, param));
            }
            for(String param : extend.attributes) {
                _attributeparameters.add(ComputeParameter(attributeparameters, param));
            }

            // Don't add type already extended
            if(! cumul.contains(implementsType)) {
                cumul.add(implementsType);
                if(component instanceof composite) {
                    // Don't override primitive in composite
                    if(implementsType instanceof astprimitive)
                        throw new compilerexception(_type, error.COMPOSITE_EXTENDS_PRIMITIVE);
                    else if(implementsType instanceof astcomposite)
                        copyCompositeDependencies((astcomposite)implementsType, stack, cumul, _templateparameters, _attributeparameters);
                    else{
                        copyDependencies(implementsType, stack, cumul, new HashMap<String, String>(), new HashMap<String, String>());
                    }
                } else {
                    if(implementsType instanceof astcomposite)
                        throw new compilerexception(_type, error.PRIMITIVE_EXTENDS_COMPOSITE);
                    else if(implementsType instanceof astprimitive) {
                        copyPrimitiveDependencies((astprimitive)implementsType, stack, cumul, _attributeparameters);
                    } else {
                        copyDependencies(implementsType, stack, cumul, new HashMap<String, String>(), new HashMap<String, String>());
                    }
                }
            }

            stack.pop();
        }

        for (astrequire require : _type.getLocalRequires()) {
            component.addRequire(require);
        }

        for (astprovide provide : _type.getLocalProvides()) {
            component.addProvide(provide);
        }

       for (astproperty property : _type.getLocalProperty()) {
            astproperty origproperty = component.getProperty(property.name);
            if(origproperty != null) {
                // Overwrite property if already defined
                origproperty.setCValue(property.getCValue());
            } else {
                component.addProperty(property);
            }
        }
    }

    void copyComponentDependencies(
            astcomponent _component, 
            Stack<asttype> stack, 
            Stack<asttype> cumul,
            HashMap<String, String> templateparameters,
            HashMap<String, String> attributeparameters) throws compilerexception {
        copyDependencies(_component, stack, cumul, templateparameters, attributeparameters);
        for (astoption option : _component.getLocalOptions()) {
            component.addOption(option);
        }
    }

    /**
     * Create a primitive from every component template
     */
    void copyPrimitiveRawDependencies(
            astcomponent _component, 
            Stack<asttype> stack, 
            Stack<asttype> cumul, 
            ArrayList<String> _attributeparameters) throws compilerexception {
        HashMap<String, String> attributeparameters = ComputeParameters(_component.getLocalAttributeParameters(), _attributeparameters);

        copyComponentDependencies(_component, 
                stack, cumul, 
                null, attributeparameters);
    }

    /**
     * Create a primitive from a primitive template
     */
    void copyPrimitiveDependencies(
            astprimitive _primitive, 
            Stack<asttype> stack, 
            Stack<asttype> cumul, 
            ArrayList<String> _attributeparameters) throws compilerexception {
        HashMap<String, String> attributeparameters = ComputeParameters(_primitive.getLocalAttributeParameters(), _attributeparameters);

        copyComponentDependencies(_primitive, 
                stack, cumul, 
                null, attributeparameters);

        primitive primitive = (primitive)component;
        for(astattribute attribute : _primitive.getLocalAttributes()) {
            primitive.addAttribute(attribute);

            primitive.setAttributeValue(attribute.name, 
                    ComputeParameter(attributeparameters, attribute.initvalue));
        }
    }

    void copyCompositeDependencies(
            astcomposite _composite, 
            Stack<asttype> stack, 
            Stack<asttype> cumul, 
            ArrayList<String> _templateparameters, 
            ArrayList<String> _attributeparameters) throws compilerexception {
        HashMap<String, String> templateparameters = ComputeParameters(_composite.getLocalTemplateParameters(), _templateparameters);
        HashMap<String, String> attributeparameters = ComputeParameters(_composite.getLocalAttributeParameters(), _attributeparameters);
        composite composite = (composite)component;

        copyComponentDependencies(_composite,
                stack, cumul, 
                templateparameters, attributeparameters);

        /*
         * Compute content according template
         */
        for(astcontent content : _composite.getLocalContents()) {
            astcontent newcontent = new astcontent(ComputeParameter(templateparameters, content.typedefinition.type), content);
            
            for(String param : content.typedefinition.templates) {
                newcontent.typedefinition.templates.add(ComputeParameter(templateparameters, param));
            }
            
            for(String param : content.typedefinition.attributes) {
                newcontent.typedefinition.attributes.add(ComputeParameter(attributeparameters, param));
            }

            composite.addContent(newcontent);
        }

        /*
         * Compute binding
         */
        for(astbind bind : _composite.getLocalBinds()) {
            composite.addBind(new astbind(bind));
        }
    }

    public void createCanonical(
            astbasefile context,
            ArrayList<String> templateparameters,
            ArrayList<String> attributeparameters) throws compilerexception {
        if(component instanceof primitive) 
        {
            primitive primitive = (primitive)component;

            checkDefinition(context, primitive.type, 
                    null, attributeparameters);

            // We create a primitive from a primitive template
            copyPrimitiveDependencies(primitive.type, 
                    new Stack<asttype>(), new Stack<asttype>(),
                    attributeparameters);

            // On MMDSP
            if(targetfactory.domain == DomainNames.MMDSP) {
                component.addRequire(new astrequire(null, 0, 0, "rtos.api.runtime", "_runtime"));
            }
        } 
        else if(component instanceof primitiveraw) 
        {
            primitiveraw primitive = (primitiveraw)component;

            checkDefinition(context, primitive.type, 
                    null, attributeparameters);

            // We create a primitive from a composite template in order to create a black box!
            copyPrimitiveRawDependencies(component.type, 
                    new Stack<asttype>(), new Stack<asttype>(),
                    attributeparameters);
        } 
        else 
        {
            composite composite = (composite)component;

            checkDefinition(context, composite.type, 
                    templateparameters, attributeparameters);
            copyCompositeDependencies(composite.type, 
                    new Stack<asttype>(), new Stack<asttype>(),
                    templateparameters, 
                    attributeparameters);

        }
    }
}