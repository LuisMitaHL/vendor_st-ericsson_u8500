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

public enum error {
    // Error command line
    BC_NOT_SET                                  (1, "BC generator not set, specify -host2mpc, -mpc2host, -asynchronous, ...  options"),
    BC_PREFIX_NOT_SET                           (2, "Prefix name for functions array not setted (specify -n <prefixName>)"),
    INVALID_ENVIRONMENT                         (3, "%s", "Message"),
    INVALID_IDENTIFIER                          (4, "Invalid identifier '%s'", "id"),

    // Environment
    PIC_FOR_DYNAMIC_COMPONENT                   (11, "Can't compile dynamic component with --pic option (specify --executable as option)"),
    SYNTAXIC_ERROR                              (12, "Syntaxic error: %s", "..."), 
    LEXICAL_ERROR                               (13, "Lexical error: %s", "..."), 
    NOT_SUPPORTED                               (14, "'%s' not supported in '%s' compilation mode", "Feature", "C/C++"),
    ERROR_ON_WARNING                            (15, "Warning has been detected during C/C++ compilation"),

    // Component type
    // Free
    ADL_SOURCE_NOT_FOUND                        (22, "ADL source '%s' not found", "fileName"),
    WRONG_NAME_INFERRED                         (23, "Name infered from file <%s> mismatch with specified name <%s>", "inferedTemplateName", "specifiedTemplateName"),
    BAD_OPTION_TARGET                           (24, "Target named '%s' not recognized", "TARGET"),
    BAD_OPTION                                  (25, "Option named '%s' not recognized", "OptionName"),
    BAD_OPTION_VALUE                            (250,"Invalid value for option '%s' (value='%s')", "option", "value"),    
    ITF_PREFIX_EMPTY                            (26, "Provide prefix must not be empty"),
    INTERFACE_PROVIDE_TWICE                     (27, "Interface name '%s' provide twice in same component", "itfName"),
    INTERFACE_REQUIRE_TWICE                     (28, "Interface name '%s' require twice in same component", "itfName"),
    COMPONENT_NOT_REQUIRE_INTERFACE             (29, "Component <%s> don't require interface '%s'", "templateName", "itfName"),
    COMPONENT_NOT_PROVIDE_INTERFACE             (30, "Component <%s> don't provide interface '%s'", "templateName", "itfName"),
    DUPLICATE_DECLARATION                       (31, "%s '%s' defined twice", "Option/Property/Source...", "declarationName"),
    // free
    // free
    //INVALID_ATTRIBUTE                           (34, "Attribute '%s' is not valid", "attributeName"),
    OPTION_NOT_COMPATYIBLE_WITH_COMPOSITE       (35, "Option '%s' not available with composite component", "optionName"),
    ITF_PREFIX_MUST_START_LETTER                (36, "Prefix '%s' must start with a letter", "perfixValue"),
    ITF_INVALID_PREFIX                          (37, "Prefix '%s' must contains only letter or digit", "perfixValue"),
    ITF_PREFIX_IN_COMPOSITE                     (38, "Prefix can't be used in composite component which contains no sources"),
    IDENTIFIER_TOO_LARGE                        (39, "Identifier '%s' of %s too large than limit %d", "ident", "class", 0x123),
    FILE_NOT_FOUND                              (40, "File '%s' not found", "path"),
    LOOP_IN_EXTENDS                             (41, "Loop in extends stack %s not allowed", "extendsStack"),
    EXTENDS_TWICE                               (42, "Type name '%s' extend twice in same component", "typeName"),
    VALUE_TOO_LARGE                             (43, "Value '%s' of %s too large than limit %d", "xxx", "class", 0x123),
    COMPONENT_WITHOUT_SOURCE                    (44, "Primitive component '%s' has no source (src directory is empty)", "templateName"),
    ASSIGN_UNTYPED_ATTRIBUTE                    (45, "Can't set value to attribute declared without type"),
    PRIMITIVE_EXTENDS_COMPOSITE                 (46, "A primitive component can't extends a composite one"),
    COMPOSITE_EXTENDS_PRIMITIVE                 (47, "A composite component can't extends a primitive one"),
    WRONG_ATTRIBUTE_NUMBER                      (48, "Argument attribute pass (%d) mismatch with '%s' declaration (%d)", 99, "templateName", 99),
    WRONG_TEMPLATE_NUMBER                       (49, "Argument template pass (%d) mismatch with '%s' declaration (%d)", 99, "templateName", 99),
    PRIMITIVE_WITH_TEMPLATE                     (50, "A primitive component or component type can't be specialized with template"),
    TYPE_WITH_ATTRIBUTE                         (180, "A component type con't be specialized with attribute"),
    NOT_COMPATIBLE_TEMPLATE                     (181, "Template parameter definition #%d '%s' not extended by '%s'", 99, "templateName", "templateName"),
    
    // Interface type
    IDL_SOURCE_NOT_FOUND                        (51, "Interface Description Language source '%s' not found", "fileName"),
    IDT_SOURCE_NOT_FOUND                        (52, "Interface Declaration Type source '%s' not found", "fileName"),
    ONLY_IDT_CAN_BE_INCLUDED                    (53, "Only inclusion of .idt file allowed"),
    ENUM_MUST_BE_DEFINED                        (54, "Enumeration '%s' not defined in interface not allowed here", "enumName"),
    TYPEDEF_MUST_BE_DEFINED                     (55, "Typedef '%s' not defined in interface not allowed here\nKnow types are:%s", "typedefName", "knowTypes"),
    ITF_MUST_BE_DEFINED                         (56, "Interface '%s' not defined in interface not allowed here", "itfType"),
    STRUCT_MUST_BE_DEFINED                      (57, "Structure '%s' not defined in interface not allowed here", "structName"),
    CONSTANT_MUST_BE_DEFINED                    (58, "Constant '%s' not defined in interface not allowed here", "identifierName"),
    IDT_ENUMERATOR_ALREADY_DEFINED              (59, "Redeclaration of enumerator '%s'", "enumeratorName"),
    IDT_TYPE_ALREADY_DEFINED                    (60, "Redeclaration of type '%s'", "typeName"),
    IDT_PARAM_ALREADY_DEFINED                   (61, "Redeclaration of parameter '%s'", "parameterName"),
    UNMANAGED_IN_MANAGED                        (62, "Use of unmanaged type '%s' in managed interface", "type"),

    // Type
    CAST_NEGATIVE_TO_UNSIGNED                   (81, "Assigning negative value to unsigned type not allowed"),
    CAST_NOTSTRING_TO_STRING                    (82, "Assigned '%s' to string type not allowed", "value"), 
    CAST_STRING_TO_NOTSTRING                    (83, "Assigned string to this type not allowed"),
    CAST_NOTCHAR_TO_CHAR                        (84, "Assigned this value to character type not allowed"),
    CAST_CHAR_TO_NOTCHAR                        (85, "Assigned char to this type not allowed"),
    TYPE_UNKNOWN_UINT                           (86, "Unknown unsigned integer size"),
    TYPE_UNKNOWN_SINT                           (87, "Unknown signed integer size"),
    TYPE_UNKOWN                                 (88, "Unknown primitive type"),
    TYPE_NOT_HANDLE_BY_TARGET                   (89, "Attribute type '%s' not handle by target '%s'", "C-type", "TARGET"),
    INTEGER_TOO_BIG                             (90, "Integer too large than %d allowed by type '%s'", 0xFFF, "C-type"),
    BAD_INTEGER                                 (91, "Integer format '%s' not recognized or too long", 0x123),
    STRING_TOO_LARGE                            (32, "Constant string '%s' too large than limit %d", "xxx", 0x123),
    
    // Composition
    C_INSTANTIATE_TWICE_IN_CPP                  (101, "C Primitive component instantiate twice in C++ project"),
    MPC_SPECIFIED_IN_C                          (102, "Can't specify an MPC in C project"),
    LOOP_IN_NETWORK                             (103, "Loop in instantiation stack %s not allowed", "instantiationStack"),
    SUBCOMPONENT_INSTANTIATE_TWICE              (104, "Sub component '%s' already exist in '%s'", "componentName", "pathName"),
    SUBCOMPONENT_NOT_EXIST                      (105, "Sub component '%s' don't exist in composite template <%s>", "componentName", "templateName"),
    PROVIDE_BIND_TO_REQUIRE                     (106, "Can't bind directly composite provide to composite require (this.x -> this.y)"),
    INSTANTIATE_TYPE                            (107, "Can't instantiate abstract component or component type"),
    
    // Bind error
    STATIC_REQUIRED_NOT_BIND_TO_SINGLETON       (121, "Static required interface can only be bound to singleton component"),
    STATIC_INTERFACE_REQUIRE_TWICE              (122, "Static interface type '%s' can't be required twice by same component", "itfType"),
    REQUIRED_NOT_BINDED                         (123, "Sub-component '%s' <%s> require interface '%s' <%s> not bound", "localName", "templateName", "itfName", "itfType"),
    TEMPLATE_NOT_BINDED                         (124, "Composite template <%s> provide interface '%s' <%s> not bound to a sub-component", "templateName", "itfName", "itfType"),
    COMPOSITE_REQUIRE_ALREADY_BINDED            (125, "Composite 'this.%s' already bound", "itfname"),
    INTERFACE_MISMATCH                          (126, "Interface type mismatch '%s' != '%s'", "itfType1", "itfType2"),
    INTERFACE_ALREADY_BINDED                    (127, "Interface '%s' already bound", "itfName"),
    STATIC_INTERFACE_AS_COLLECTION              (128, "Static interface is not compatible with interface collection"),
    COMPOSITE_REQUIRE_NOT_BINDED                (129, "Composite required interface 'this.%s' not bound", "itfName"),
    DISTRIBUTE_BIND_MUSTBE_ASYNCHRONOUS         (130, "Distributed bind between component '%s' and '%s' on different core must be asynchronous", "templateName", "templateName"),
    CALL_MUSTNOTBE_ASYNCHRONOUS                 (131, "Useless asynchronous in %s of top level composite", "Call(back)"),

    // Interface error
    NAME_RESERVED_FOR_LIFECYCLE                 (141, "Interface name '%s' must be of interface type '%s'", "interfaceName", "interfaceType"),
    LCC_CANT_BE_REQUIRED                        (142, "LifeCycle interface '%s' can't be required by component", "interfaceType"),
    // Free
    // Free
    LCC_WITH_COLLECTION                         (145, "LifeCycle interface '%s' can't be a collection", "interfaceType"),
    // Free
    // Free
    // Free
    COLLECTION_OUT_OF_RANGE                     (149, "Number element in collection '%s' must be in range ]0...255]", "itfName"),
    IRQ_ITF_TYPE_WRONG                          (150, "Interrupt interface must be of interface type 'exception.handler'"),
    IRQ_ITF_CAN_BE_COLLECTION                   (151, "Interrupt interface can't be a collection"),
    IRQ_LINE_OUT_OF_RANGE                       (152, "Interrupt line must be in range [%d...%d] or reserved {%s}", 0, 255, "set"),
    WRONG_COLLECTION_USE                        (153, "A index must be specify for binding a interface collection '%s.%s[]'", "templateName", "itfName"),
    COLLECTION_INDEX_OUT_OF_RANGE               (154, "Index must be in range 0..%d for interface collection '%s[]'", 255, "itfName"),
    WRONG_COLLECTION_INDEX_USE                  (155, "A standard interface '%s' must not be used with a index", "itfName"),
    LCC_ITF_WITH_COMPOSITE                      (156, "Lifecycle interface can't be provide manually by composite component"),
    IRQ_NOT_SUPPORTED                           (157, "Interrupt not supported by target"),

    // Communication
    COMS_TOO_BIG_TYPE                           (171, "Type longer than 32bits not yet allowed for distributed communication"),
    COMS_UNKNOWN_TYPE                           (172, "Type '%s' of unknown size used in distributed communication", "C-type"),
    COMS_RETURN_VALUE_NOT_HANDLED               (173, "Only method with no return allowed in asynchronouse (distributed) communication"),
    COMS_OUT_OF_FIFO_SIZE_LIMIT                 (174, "Fifo size out of range [1 .. 256]"),
    PRIORITY_NOT_SUPPORTED                      (175, "Priority %s not supported [0..%d] [background, normal, urgent]", "1234", 2),
    COMS_OUT_OF_SUBPRIORITY_LIMIT               (176, "Fifo size out of range [0 .. %d]", 3),

    // End
    TOOL_ERROR(255, "Error generated by other tools");
    
    int id;
    String format;
    String message;
    
    error(int _id, String _format, Object ... args) {
        id = _id;
        format = _format;
        message = String.format(format, args);
    }

    public String getFormatString(Object ...args) {
        String message =  String.format("[#" + id + "] " + format + ".", args);
        if(id < 10)
            message = message + configuration.eol + "\t(use '--help' for informations)";
        return message;
    }
    
    public static void dumpError() {
        for (error e : error.values()) {
            System.out.println(
                    String.format("[%4d] %s", e.id, e.message));
        }
    }
};