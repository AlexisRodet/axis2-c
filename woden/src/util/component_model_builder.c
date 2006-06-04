/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <woden/util/axis2_woden_component_model_builder.h>
#include ../wsdl20/axis2_woden_constants.h"

typedef struct axis2_woden_component_model_builder_impl
{
    axis2_woden_component_model_builder_t component_model_builder;
    void *f_desc;
    axis2_array_list_t *f_schemas_done;
    axis2_array_list_t *f_interfaces_done;
    axis2_array_list_t *f_bindings_done;
    axis2_array_list_t *f_svcs_done;
    
}axis2_woden_component_model_builder_impl_t;

#define AXIS2_INTF_TO_IMPL(builder) \
    ((axis2_woden_component_model_builder_impl_t *) builder)

axis2_status_t AXIS2_CALL
axis2_woden_component_model_builder_free (
        axis2_woden_component_model_builder_t *builder, 
        const axis2_env_t *env); 

/*
 * Extract the element declarations from the given schema. 
 */
static axis2_status_t
build_element_decls(
        void *builder,
        const axis2_env_t *env,
        void *schema_def,
        axis2_url_t *type_system_uri);
    
/*
 * Extract the type definitions from the given schema. 
 */
static axis2_status_t
build_type_defs(
        void *builder,
        const axis2_env_t *env,
        void *schema_def,
        axis2_url_t *type_system_uri);

/* *******************************************************************************
 *  INTERFACE
 * ******************************************************************************/

/*
 * Initialize the Interface component and its child components from the
 * Interface Element and its child elements.
 */
static axis2_status_t
build_interfaces(
        void *builder,
        const axis2_env_t *env,
        void *desc);

static axis2_status_t
build_interface_faults(
        void *builder,
        const axis2_env_t *env,
        void *interface);

static axis2_status_t
build_interface_ops(
        void *builder,
        const axis2_env_t *env,
        void *interface);

static axis2_status_t
build_interface_fault_refs(
        void *builder,
        const axis2_env_t *env,
        void *op);

static axis2_status_t
build_interface_msg_refs(
        void *builder,
        const axis2_env_t *env,
        void *op);

/* *******************************************************************************
 *  BINDING
 * ******************************************************************************/

/*
 * Initialize the Binding component and its child components from the
 * Binding Element and its child elements.
 */
static axis2_status_t
build_bindings(
        void *builder,
        const axis2_env_t *env,
        void *desc);

static axis2_status_t
build_binding_faults(
        void *builder,
        const axis2_env_t *env,
        void *binding);

static axis2_status_t
build_binding_ops(
        void *builder,
        const axis2_env_t *env,
        void *binding);

static axis2_status_t
build_binding_fault_refs(
        void *builder,
        const axis2_env_t *env,
        void *op);

static axis2_status_t
build_binding_msg_refs(
        void *builder,
        const axis2_env_t *env,
        void *op);

static axis2_status_t
build_binding_exts(
        void *builder,
        const axis2_env_t *env,
        void *binding);

static axis2_status_t
build_binding_fault_ref_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_fault);

static axis2_status_t
build_binding_op_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_op);

static axis2_status_t
build_binding_msg_ref_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_msg_ref);
    
static axis2_status_t
build_binding_fault_ref_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_fault_ref);
    
/*
 * This helper method factors out common code for creating Component Extensions registered
 * in the Extension Registry. 
 */
static void *
create_component_exts(
        void *builder,
        const axis2_env_t *env,
        axis2_char_t *parent_class,
        void *parent_elem,
        axis2_url_t *ext_ns);

/* *******************************************************************************
 *  SERVICE
 * ******************************************************************************/
static axis2_status_t
build_svcs(
        void *builder,
        const axis2_env_t *env,
        void *desc);

/* *******************************************************************************
 *  PROPERTY
 * ******************************************************************************/

static axis2_status_t
build_properties(
        void *builder,
        const axis2_env_t *env,
        axis2_array_list_t *prop_els,
        void *parent);

/************************** End of function prototypes ************************/

axis2_woden_component_model_builder_t *AXIS2_CALL 
axis2_woden_component_model_builder_create(
        const axis2_env_t *env,
        void *desc)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    
   AXIS2_ENV_CHECK(env, NULL);
   
   builder_impl = (axis2_woden_component_model_builder_impl_t *) 
        AXIS2_MALLOC(env->allocator, 
        sizeof(axis2_woden_component_model_builder_impl_t));
   
   if(NULL == builder_impl)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE); 
        return NULL;
    }
    builder_impl->f_desc= NULL;
    builder_impl->f_schemas_done = NULL;
    builder_impl->f_interfaces_done = NULL;
    builder_impl->f_bindings_done = NULL;
    builder_impl->f_svcs_done = NULL;
    
    builder_impl->builder.ops = 
      AXIS2_MALLOC (env->allocator, 
                sizeof(axis2_woden_component_model_builder_ops_t));
   if(NULL == builder_impl->builder.ops)
    {
        axis2_woden_component_model_builder_free(&(builder_impl->builder), env);
      AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    

    builder_impl->builder.ops->free =  
        axis2_woden_component_model_builder_free;
    
    builder_impl->f_desc = desc;
    axis2_woden_component_model_builder_init_components(&(builder_impl->builder), env, 
            builder_impl->f_desc);
   return &(builder_impl->builder);
}

/***************************Function implementation****************************/

axis2_status_t AXIS2_CALL 
axis2_woden_component_model_builder_free (
        axis2_woden_component_model_builder_t *builder, 
        const axis2_env_t *env)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    if(builder_impl->builder.ops)
    {
        AXIS2_FREE(env->allocator, builder_impl->
                builder.ops);
        builder_impl->builder.ops = NULL;
    }
    
    if(builder_impl)
    {
        AXIS2_FREE(env->allocator, builder_impl);
        builder_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
init_components(
        void *builder,
        const axis2_env_t *env,
        void *desc)
{
    axis2_array_list_t *includes = NULL;
    axis2_array_list_t *imports = NULL;
    int i = 0, size = 0;
    
    build_elements_and_types(builder, env);
    build_interfaces(builder, env);
    build_bindings(builder, env);
    build_svcs(builder, env);
    
    includes = AXIS2_WODEN_DESC_GET_INCLUDE_ELEMENTS(desc, env);
    size = AXIS2_ARRAY_LIST_SIZE(includes, env);
    for(i = 0; i < size; i++)
    {
        void *incl_desc = NULL;
        void *incl_el = NULL;

        incl_el = AXIS2_ARRAY_LIST_GET(includes, env, i);
        incl_desc = AXIS2_WODEN_INCLUDE_ELEMENT_GET_DESC_ELEMENT(incl_el, env);
        if(NULL != incl_desc)
        {
            init_components(builder, env, incl_desc);
        }
    }
     
    imports = AXIS2_WODEN_DESC_GET_IMPORT_ELEMENTS(desc, env);
    size = AXIS2_ARRAY_LIST_SIZE(imports, env);
    for(i = 0; i < size; i++)
    {
        void *imp_desc = NULL;
        void *imp_el = NULL;

        imp_el = AXIS2_ARRAY_LIST_GET(imports, env, i);
        imp_desc = AXIS2_WODEN_IMPORT_ELEMENT_GET_DESC_ELEMENT(imp_el, env);
        if(NULL != imp_desc)
        {
            init_components(builder, env, imp_desc);
        }
    }
}

/* *******************************************************************************
 *  TYPES
 * ******************************************************************************/

/*
 * Extract from the collections of in-scope schemas
 * the element declarations and type definitions.
 */
static axis2_status_t
build_elements_and_types(
        void *builder,
        const axis2_env_t *env,
        void *desc)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    void *types = NULL;
    axis2_url_t *type_system_uri = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, desc, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);

    types = AXIS2_WODEN_DESC_GET_TYPES_ELEMENT(desc, env);
    
    type_system_uri = axis2_url_parse_string(env, WODEN_TYPE_XSD_2001);
    
    if(NULL != types)
    {
        axis2_array_list_t *referenceable_schema_defs = NULL;
        int i = 0, size = 0;

        referenceable_schema_defs = 
            AXIS2_WODEN_TYPES_GET_REFERENCEABLE_SCHEMA_DEFS(types, env);
        if(referenceable_shcema_defs)
            size = AXIS2_ARRAY_LIST_SIZE(referenceable_schema_defs, env);
        for(i = 0; i < size; i++)
        {
            void *schema_def = NULL;
            int i = 0, size = 0;
            axis2_bool_t new_schema = AXIS2_TRUE;
            
            schema_def = AXIS2_ARRAY_LIST_GET(referenceable_schema_defs, env, i);
            if(builder_impl->f_schemas_done)
                size = AXIS2_ARRAY_LIST_SIZE(builder_impl->f_schemas_done, env);
            for(i = 0; i < size; i++)
            {
                void *schema_done = NULL;

                schema_done = AXIS2_ARRAY_LIST_GET(builder_impl->f_schemas_done, 
                        env, i);
                /* TODO Check the logic here */
                if(schema_def == schema_done)
                {
                    new_schema = AXIS2_FALSE;
                    break;
                }
            }
            /* if builder_impl->f_schemas_done does not contain schema_def */
            if(new_schema)
            {
                build_element_decls(builder, env, schema_def, type_system_uri);
                build_type_defs(builder, env, schema_def, type_system_uri);
                if(!builder_impl->f_schemas_done)
                {
                    builder_impl->f_schemas_done = axis2_array_list_create(env, 0);
                    if(!builder_impl->f_schemas_done)
                        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, 
                                AXIS2_FAILURE);
                }
                AXIS2_ARRAY_LIST_ADD(builder_impl->f_schemas_done, env, schema_def);
            }
        }
    }
}

/*
 * Extract the element declarations from the given schema. 
 */
static axis2_status_t
build_element_decls(
        void *builder,
        const axis2_env_t *env,
        void *schema_def,
        axis2_url_t *type_system_uri)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_char_t *schema_tns = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, schema_def, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, type_system_uri, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);

    schema_tns = AXIS2_XML_SCHEMA_GET_TARGET_NAMESPACE(schema_def, env);
    if(NULL != schema_tns)
    {
        void *element_table = NULL;
        axis2_array_list_t *qnames = NULL;
        int i = 0, size = 0;

        element_table = AXIS2_XML_SCHEMA_GET_ELEMENTS(schema_def, env);
        qnames = AXIS2_XML_SCHEMA_OBJ_TABLE_GET_NAMES(element_table, env);
        if(qnames)
            size = AXIS2_ARRAY_LIST_SIZE(qnames, env);
        for(i = 0; i < size; i++)
        {
            axis2_qname_t *qname = NULL;
            axis2_char_t *namespc = NULL;

            qname = (axis2_qname_t *) AXIS2_ARRAY_LIST_GET(qnames, env, i);
            if(qname)
                namespc = AXIS2_QNAME_GET_URI(qname, env);
            if(0 == AXIS2_STRCMP(namespc, schema_tns))
            {
                void *ed = NULL;

                ed = axis2_woden_element_decl_create(env);
                AXIS2_WODEN_ELEMENT_DECL_SET_QNAME(ed, env, qname);
                AXIS2_WODEN_ELEMENT_DECL_SET_SYSTEM(ed, env, type_system_uri);
                AXIS2_WODEN_ELEMENT_DECL_SET_CONTENT_MODEL(ed, env, 
                        WODEN_API_APACHE_WS_XS);
                AXIS2_WODEN_ELEMENT_DECL_SET_CONTENT(ed, env, 
                        AXIS2_XML_SCHEMA_OBJ_TABLE_GET_ITEM(element_table, env, 
                            qname));
                AXIS2_WODEN_DESC_ADD_TO_ALL_ELEMENT_DECLS(builder_impl->f_desc, 
                        env, ed);
            }
        }
    }
}
    
/*
 * Extract the type definitions from the given schema. 
 */
static axis2_status_t
build_type_defs(
        void *builder,
        const axis2_env_t *env,
        void *schema_def,
        axis2_url_t *type_system_uri)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_char_t *schema_tns = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, schema_def, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, type_system_uri, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);

    schema_tns = AXIS2_XML_SCHEMA_GET_TARGET_NAMESPACE(schema_def, env);
    if(NULL != schema_tns)
    {
        void *type_table = NULL;
        axis2_array_list_t *qnames = NULL;
        int i = 0, size = 0;

        type_table = AXIS2_XML_SCHEMA_GET_SCHEMA_TYPES(schema_def, env);
        qnames = AXIS2_XML_SCHEMA_OBJ_TABLE_GET_NAMES(element_table, env);
        if(qnames)
            size = AXIS2_ARRAY_LIST_SIZE(qnames, env);
        for(i = 0; i < size; i++)
        {
            axis2_qname_t *qname = NULL;
            axis2_char_t *namespc = NULL;

            qname = (axis2_qname_t *) AXIS2_ARRAY_LIST_GET(qnames, env, i);
            if(qname)
                namespc = AXIS2_QNAME_GET_URI(qname, env);
            if(0 == AXIS2_STRCMP(namespc, schema_tns))
            {
                void *td = NULL;

                td = axis2_woden_type_def_create(env);
                AXIS2_WODEN_TYPE_DEF_SET_QNAME(td, env, qname);
                AXIS2_WODEN_TYPE_DEF_SET_SYSTEM(td, env, type_system_uri);
                AXIS2_WODEN_TYPE_DEF_SET_CONTENT_MODEL(td, env, 
                        WODEN_API_APACHE_WS_XS);
                AXIS2_WODEN_TYPE_DEF_SET_CONTENT(td, env, 
                        AXIS2_XML_SCHEMA_OBJ_TABLE_GET_ITEM(type_table, env, 
                            qname));
                AXIS2_WODEN_DESC_ADD_TO_ALL_TYPE_DEFS(builder_impl->f_desc, 
                        env, td);
            }
        }
    }
}
 

/* *******************************************************************************
 *  INTERFACE
 * ******************************************************************************/

/*
 * Initialize the Interface component and its child components from the
 * Interface Element and its child elements.
 */
static axis2_status_t
build_interfaces(
        void *builder,
        const axis2_env_t *env,
        void *desc)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *interface_els = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, desc, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);

    interface_els = AXIS2_WODEN_DESC_GET_INTERFACE_ELEMENTS(desc, env);
    if(NULL != interface_els)
    {
        size = AXIS2_ARRAY_LIST_SIZE(interface_els, env);
    }
    for(i = 0; i < size; i++)
    {
        void *int_impl = NULL;

        int_impl = AXIS2_ARRAY_LIST_GET(interface_els, env, i);
        if(AXIS2_TRUE != AXIS2_ARRAY_LIST_CONTAINS(interface_els, env, int_impl))
        {
            axis2_array_list_t *property_els = NULL;
            AXIS2_WODEN_DESC_ADD_TO_ALL_INTERFACES(builder_impl->f_desc, 
                    env, int_impl);
            property_els = AXIS2_WODEN_INTERFACE_GET_PROPERTY_ELEMENTS(int_impl, 
                    env);
            build_properties(builder, env, property_els, int_impl);
            build_interface_faults(builder, env, int_impl);
            build_interface_ops(builder, env, int_impl);
            if(!builder_impl->f_interfaces_done)
            {
                builder_impl->f_interfaces_done = axis2_array_list_create(env, 0);
                if(!builder_impl->f_interfaces_done)
                    AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, 
                            AXIS2_FAILURE);
            }
            AXIS2_ARRAY_LIST_ADD(builder_impl->f_interfaces_done, env, int_impl);
        }
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_interface_faults(
        void *builder,
        const axis2_env_t *env,
        void *interface)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *faults = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, interface, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    interface = axis2_woden_interface_to_interface_element(interface, env);
    faults = AXIS2_WODEN_INTERFACE_ELEMENT_GET_INTERFACE_FAULT_ELEMENTS(
            interface, env);
    if(faults)
        size = AXIS2_ARRAY_LIST_SIZE(faults, env);
    for(i = 0; i < size; i++)
    {
        void *fault = NULL;
        axis2_array_list_t *property_elements = NULL;
        
        fault = AXIS2_ARRAY_LIST_GET(faults, env, i);
        property_elements = AXIS2_WODEN_INTERFACE_FAULT_GET_PROPERTY_ELEMENTS(
                fault, env);
        build_properties(builder, env, property_elements, fault);
        qname = AXIS2_WODEN_INTERFACE_FAULT_GET_ELEMENT_QNAME(fault, env);
        if(NULL ! = qname)
        {
            void *element_decl = NULL;
            
            element_decl = AXIS2_WODEN_DESC_GET_ELEMENT_DECL(
                    builder_impl->f_desc, env, qname);
            AXIS2_WODEN_INTERFACE_FAULT_SET_ELEMENT_DECL(fault, env, element_decl);
        }
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_interface_ops(
        void *builder,
        const axis2_env_t *env,
        void *interface)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *ops = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, interface, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    interface = axis2_woden_interface_to_interface_element(interface, env);
    ops = AXIS2_WODEN_INTERFACE_ELEMENT_GET_INTERFACE_OP_ELEMENTS(
            interface, env);
    if(ops)
        size = AXIS2_ARRAY_LIST_SIZE(ops, env);
    for(i = 0; i < size; i++)
    {
        void *op = NULL;
        axis2_array_list_t *property_elements = NULL;
        
        op = AXIS2_ARRAY_LIST_GET(ops, env, i);
        property_elements = AXIS2_WODEN_INTERFACE_OP_GET_PROPERTY_ELEMENTS(
                op, env);
        build_properties(builder, env, property_elements, op);
        build_interface_fault_refs(builder, env, op);
        build_interface_msg_refs(builder, env, op);
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_interface_fault_refs(
        void *builder,
        const axis2_env_t *env,
        void *op)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *fault_refs = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, op, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    op = axis2_woden__interface_op_to_interface_op_element(op, env);
    fault_refs = AXIS2_WODEN_INTERFACE_OP_ELEMENT_GET_FAULT_REF_ELEMENTS(
            op, env);
    if(fault_refs)
        size = AXIS2_ARRAY_LIST_SIZE(fault_refs, env);
    for(i = 0; i < size; i++)
    {
        void *fault_ref = NULL;
        axis2_array_list_t *property_elements = NULL;
        
        fault_ref = AXIS2_ARRAY_LIST_GET(fault_refs, env, i);
        property_elements = AXIS2_WODEN_INTERFACE_FAULT_REF_GET_PROPERTY_ELEMENTS(
                fault_ref, env);
        build_properties(builder, env, property_elements, fault_ref);
        qname = AXIS2_WODEN_INTERFACE_FAULT_REF_GET_REF(fault_ref, env);
        if(NULL ! = qname)
        {
            void *interface = NULL;
            void *interface_fault = NULL;
            
            interface = AXIS2_WODEN_INTERFACE_OP_GET_PARENT(op, env);
            interface_fault = AXIS2_WODEN_INTERFACE_GET_INTERFACE_FAULT(interface, env, qname)
            AXIS2_WODEN_INTERFACE_FAULT_REF_SET_INTERFACE_FAULT(fault_ref, env, interface_fault);
        }
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_interface_msg_refs(
        void *builder,
        const axis2_env_t *env,
        void *op)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *msgs = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, op, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    op = axis2_woden_interface_op_to_interface_op_element(op, env);
    msgs = AXIS2_WODEN_INTERFACE_OP_ELEMENT_GET_MSG_REF_ELEMENTS(
            op, env);
    if(fault_refs)
        size = AXIS2_ARRAY_LIST_SIZE(msgs, env);
    for(i = 0; i < size; i++)
    {
        void *msg = NULL;
        axis2_array_list_t *property_elements = NULL;
        axis2_char_t *msg_content_model = NULL;
        
        fault_ref = AXIS2_ARRAY_LIST_GET(fault_refs, env, i);
        property_elements = AXIS2_WODEN_INTERFACE_FAULT_REF_GET_PROPERTY_ELEMENTS(
                msg, env);
        build_properties(builder, env, property_elements, msg);
        msg_content_model = AXIS2_WODEN_INTERFACE_MSG_REF_GET_MSG_CONTENT_MODEL(
                msg, env);
        if(0 == AXIS2_STRCMP(WODEN_NMTOKEN_ELEMENT, msg_content_model))
        {
            axis2_qname_t *qname = NULL;

            qname = AXIS2_WODEN_INTERFACE_MSG_REF_GET_ELEMENT_QNAME(msg, env);
            if(qname)
            {
                void *element_decl = AXIS2_WODEN_DESC_GET_ELEMENT_DECL(
                        builder_impl->f_desc, env, qname);
                AXIS2_WODEN_INTERFACE_MSG_REF_SET_ELEMENT_DECLARATION(msg, env, 
                        element_decl); 
            }
        }
    }
    return AXIS2_SUCCESS;
}

/* *******************************************************************************
 *  BINDING
 * ******************************************************************************/

/*
 * Initialize the Binding component and its child components from the
 * Binding Element and its child elements.
 */
static axis2_status_t
build_bindings(
        void *builder,
        const axis2_env_t *env,
        void *desc)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *binding_els = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, desc, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);

    binding_els = AXIS2_WODEN_DESC_GET_BINDING_ELEMENTS(desc, env);
    if(binding_els)
        size = AXIS2_ARRAY_LIST_SIZE(binding_els, env);
    for(i = 0; i < size; i++)
    {
        void *bind_impl = NULL;

        bind_impl = AXIS2_ARRAY_LIST_GET(binding_els, env, i);
        if(builder_impl->f_bindings_done && AXIS2_TRUE != 
                AXIS2_ARRAY_LIST_CONTAINS(builder_impl->f_bindings_done, 
                    env, bind_impl))
        {
            axis2_array_list_t *property_elements = NULL;

            AXIS2_WODEN_DESC_ADD_TO_ALL_BINDINGS(builder_impl->f_desc, 
                    env, bind_impl);
            property_elements = AXIS2_WODEN_BINDING_GET_PROPERTY_ELEMENTS(
                    bind_impl, env);
            build_properties(builder, env, property_elements, bind_impl);
            build_binding_faults(builder, env, bind_impl);
            build_binding_ops(builder, env, bind_impl);
            build_binding_extensions(builder, env, bind_impl);
            if(!builder_impl->f_bindings_done)
            {
                builder_impl->f_bindings_done = axis2_array_list_create(env, 0);
                if(!builder_impl->f_bindings_done)
                    AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, 
                            AXIS2_FAILURE);
            }
            AXIS2_ARRAY_LIST_ADD(builder_impl->f_bindings_done, env, bind_impl);

        }
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_binding_faults(
        void *builder,
        const axis2_env_t *env,
        void *binding)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *faults = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, binding, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    binding = axis2_woden_binding_to_binding_element(binding, env);
    faults = AXIS2_WODEN_BINDING_ELEMENT_GET_BINDING_FAULT_ELEMENTS(
            binding, env);
    if(faults)
        size = AXIS2_ARRAY_LIST_SIZE(faults, env);
    for(i = 0; i < size; i++)
    {
        void *fault = NULL;
        axis2_array_list_t *property_elements = NULL;
        
        fault = AXIS2_ARRAY_LIST_GET(faults, env, i);
        property_elements = AXIS2_WODEN_BINDING_FAULT_GET_PROPERTY_ELEMENTS(
                fault, env);
        build_properties(builder, env, property_elements, fault);
        build_binding_fault_extensions(builder, env, fault);
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_binding_ops(
        void *builder,
        const axis2_env_t *env,
        void *binding)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *ops = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, binding, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    binding = axis2_woden_binding_to_binding_element(binding, env);
    ops = AXIS2_WODEN_BINDING_ELEMENT_GET_BINDING_OP_ELEMENTS(
            binding, env);
    if(ops)
        size = AXIS2_ARRAY_LIST_SIZE(ops, env);
    for(i = 0; i < size; i++)
    {
        void *op = NULL;
        axis2_array_list_t *property_elements = NULL;
        
        op = AXIS2_ARRAY_LIST_GET(ops, env, i);
        property_elements = AXIS2_WODEN_INTERFACE_OP_GET_PROPERTY_ELEMENTS(
                op, env);
        build_properties(builder, env, property_elements, op);
        build_binding_fault_refs(builder, env, op);
        build_binding_msg_refs(builder, env, op);
        build_binding_op_extensions(builder, env, op);
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_binding_fault_refs(
        void *builder,
        const axis2_env_t *env,
        void *op)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *fault_refs = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, op, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    op = axis2_woden_binding_op_to_binding_op_element(op, env);
    fault_refs = AXIS2_WODEN_BINDING_OP_ELEMENT_GET_FAULT_REF_ELEMENTS(
            op, env);
    if(fault_refs)
        size = AXIS2_ARRAY_LIST_SIZE(fault_refs, env);
    for(i = 0; i < size; i++)
    {
        void *fault_ref = NULL;
        axis2_array_list_t *property_elements = NULL;
        
        fault_ref = AXIS2_ARRAY_LIST_GET(fault_refs, env, i);
        property_elements = AXIS2_WODEN_INTERFACE_FAULT_REF_GET_PROPERTY_ELEMENTS(
                fault_ref, env);
        build_properties(builder, env, property_elements, fault_ref);
        build_binding_fault_ref_extensions(builder, env, fault_ref);
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_binding_msg_refs(
        void *builder,
        const axis2_env_t *env,
        void *op)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *msgs = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, op, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
    
    op = axis2_woden_binding_op_to_binding_op_element(op, env);
    msgs = AXIS2_WODEN_BINDING_OP_ELEMENT_GET_MSG_REF_ELEMENTS(
            op, env);
    if(fault_refs)
        size = AXIS2_ARRAY_LIST_SIZE(msgs, env);
    for(i = 0; i < size; i++)
    {
        void *msg = NULL;
        axis2_array_list_t *property_elements = NULL;
        axis2_char_t *msg_content_model = NULL;
        
        fault_ref = AXIS2_ARRAY_LIST_GET(fault_refs, env, i);
        property_elements = AXIS2_WODEN_INTERFACE_FAULT_REF_GET_PROPERTY_ELEMENTS(
                msg, env);
        build_properties(builder, env, property_elements, msg);
        build_binding_msg_ref_extensions(builder, env, msg);
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_binding_exts(
        void *builder,
        const axis2_env_t *env,
        void *binding)
{
    axis2_woden_ext_registry_t *er = NULL;
    axis2_array_list_t *ext_namespcs = NULL;
    int i = 0, size = 0;
    void *parent_element = NULL;
    axis2_url_t *binding_type = NULL;
    axis2_char_t *binding_type_str = NULL;

    er = AXIS2_WODEN_DESC_GET_EXT_REGISTRY(builder_impl->f_desc, env);
    ext_namespcs = AXIS2_WODEN_EXT_REGISTRY_QUERY_COMPONENT_EXT_NAMESPACES(er, 
            env, "binding");
    
    /*
     * First, create a Component Extensions object for each registered extension namespace 
     * used with this binding. 
     */
    size = AXIS2_ARRAY_LIST_SIZE(ext_namespcs, env);
    for(i = 0; i < size; i++)
    {
        axis2_url_t *ext_ns = NULL;
        axis2_bool_t temp1 = AXIS2_FALSE;
        axis2_bool_t temp2 = AXIS2_FALSE;
        
        ext_ns = AXIS2_ARRAY_LIST_GET(ext_namespcs, env, i);
        binding = axis2_woden_binding_to_attr_extensible(
                binding, env);
        temp1 = AXIS2_WODEN_ATTR_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                binding, env, ext_ns);
        binding = axis2_woden_binding_to_element_extensible(
                binding, env);
        temp2 = AXIS2_WODEN_ELEMENT_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                binding, env, ext_ns);
        if(AXIS2_TRUE == temp1 || AXIS2_TRUE == temp2)
        {
            void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding", 
                    bind_fault, ext_ns);
            binding = axis2_woden_binding_to_wsdl_obj(binding, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(binding, env, ext_ns, comp_ext);
        }
    }
    
    /*
     * Second, apply the rules from WSDL 2.0 Part 2 Adjuncts spec for default values for SOAP and HTTP 
     * extension properties to create ComponentExtension objects even if the related extension attributes
     * or elements do not exist in the WSDL. 
     * 
     * TODO chg this hardcoded behaviour so that any default rules for SOAP, HTTP or user-defined
     * extensions can be registered in some way and interpreted here at run time.
     */
    binding_type = AXIS2_WODEN_BINDING_GET_TYPE(binding, env);
    binding_type_str = AXIS2_URL_TO_EXTERNAL_FORM(binding_type, env);
    if(0 == AXIS2_STRCMP(AXIS2_WODEN_URI_NS_SOAP, binding_type_str))
    {
        void *sbe = NULL;

        /* If the binding type is SOAP, the {soap version} property defaults to "1.2" so if a
         * a SOAP Binding Extensions object has not already been created, create one now to handle
         * this default value.
         */
        binding = axis2_woden_binding_to_wsdl_component(binding, env);
        if(NULL == AXIS2_WODEN_WSDL_COMPONENT_GET_COMPONENT_EXTS_FOR_NAMESPACE(binding, 
                    env, AXIS2_WODEN_URI_NS_SOAP))
        {
            void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding", 
                    binding, AXIS2_WODEN_URI_NS_SOAP);
            binding = axis2_woden_binding_to_wsdl_obj(binding, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(binding, env, 
                    AXIS2_WODEN_URI_NS_SOAP, comp_ext);
        }
        sbe = AXIS2_WODEN_WSDL_COMPONENT_GET_COMPONENT_EXTS_FOR_NAMESPACE(binding, 
                    env, AXIS2_WODEN_URI_NS_SOAP)
        /* TODO Complete this */
    }
    return AXIS2_SUCCESS;
}


static axis2_status_t
build_binding_fault_ref_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_fault)
{
    axis2_woden_ext_registry_t *er = NULL;
    axis2_array_list_t *ext_namespcs = NULL;
    int i = 0, size = 0;
    void *parent_element = NULL;
    axis2_url_t *binding_type = NULL;
    axis2_char_t *binding_type_str = NULL;

    er = AXIS2_WODEN_DESC_GET_EXT_REGISTRY(builder_impl->f_desc, env);
    ext_namespcs = AXIS2_WODEN_EXT_REGISTRY_QUERY_COMPONENT_EXT_NAMESPACES(er, 
            env, "binding_fault");
    
    /*
     * First, create a Component Extensions object for each registered extension namespace 
     * used with this binding fault. 
     */
    size = AXIS2_ARRAY_LIST_SIZE(ext_namespcs, env);
    for(i = 0; i < size; i++)
    {
        axis2_url_t *ext_ns = NULL;
        axis2_bool_t temp1 = AXIS2_FALSE;
        axis2_bool_t temp2 = AXIS2_FALSE;
        
        ext_ns = AXIS2_ARRAY_LIST_GET(ext_namespcs, env, i);
        bind_fault = axis2_woden_binding_fault_to_attr_extensible(
                bind_fault, env);
        temp1 = AXIS2_WODEN_ATTR_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_fault, env, ext_ns);
        bind_fault = axis2_woden_binding_fault_to_element_extensible(
                bind_fault, env);
        temp2 = AXIS2_WODEN_ELEMENT_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_fault, env, ext_ns);
        if(AXIS2_TRUE == temp1 || AXIS2_TRUE == temp2)
        {
            void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding_fault", 
                    bind_fault, ext_ns);
            bind_fault = axis2_woden_binding_fault_to_wsdl_obj(bind_fault, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(bind_fault, env, ext_ns, comp_ext);
        }
    }
    
    /*
     * Second, apply the rules from WSDL 2.0 Part 2 Adjuncts spec for default values for SOAP and HTTP 
     * extension properties to create ComponentExtension objects even if the related extension attributes
     * or elements do not exist in the WSDL. 
     * 
     * TODO chg this hardcoded behaviour so that any default rules for SOAP, HTTP or user-defined
     * extensions can be registered in some way and interpreted here at run time.
     */
    bind_fault = axis2_woden_binding_fault_to_nested_element(bind_fault);
    parent_element = AXIS2_WODEN_NESTED_ELEMENT_GET_PARENT_ELEMENT(bind_fault, env);
    parent_element = axis2_woden_binding_to_binding_element(parent_element, env);
    binding_type = AXIS2_WODEN_BINDING_ELEMENT_GET_TYPE(parent_element, env);
    binding_type_str = AXIS2_URL_TO_EXTERNAL_FORM(binding_type, env);
    if(0 == AXIS2_STRCMP(AXIS2_WODEN_URI_NS_SOAP, binding_type_str))
    {
        /* If the binding type is SOAP, the {soap fault code} and {soap fault subcodes} properties
         * default to xs:token "#any", so if a SOAP Binding Fault Extensions object has not already 
         * been created, create one now to handle these default values.
         */
        bind_fault = axis2_woden_binding_fault_to_wsdl_component(bind_fault, env);
        if(NULL == AXIS2_WODEN_WSDL_COMPONENT_GET_COMPONENT_EXTS_FOR_NAMESPACE(bind_fault, 
                    env, AXIS2_WODEN_URI_NS_SOAP))
        {
            void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding_fault", 
                    bind_fault, AXIS2_WODEN_URI_NS_SOAP);
            bind_fault = axis2_woden_binding_fault_to_wsdl_obj(bind_fault, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(bind_fault, env, 
                    AXIS2_WODEN_URI_NS_SOAP, comp_ext);
        }
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
build_binding_op_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_op)
{
    axis2_ext_registry_t *er = NULL;

    er = AXIS2_WODEN_DESC_GET_EXT_REGISTRY(builder_impl->f_desc, env);
    ext_namespcs = AXIS2_WODEN_EXT_REGISTRY_QUERY_COMPONENT_EXT_NAMESPACES(er, env, "binding_op");
    if(ext_namespcs)
        size = AXIS2_ARRAY_LIST_SIZE(ext_namespcs, env);
    /*
     * First, create a Component Extensions object for each registered extension 
     * namespace used with this binding operation. 
     */
    for(i = 0; i < size; i++)
    {
        axis2_url_t *ext_ns = NULL;
        axis2_bool_t temp1 = AXIS2_FALSE;
        axis2_bool_t temp2 = AXIS2_FALSE;

        ext_ns = AXIS2_ARRAY_LIST_GET(ext_namespcs, env, i);
        bind_op = axis2_woden_binding_op_to_attr_extensible(
                bind_op, env);
        temp1 = AXIS2_WODEN_ATTR_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_op, env, ext_ns);
        bind_op = axis2_woden_binding_op_to_element_extensible(
                bind_op, env);
        temp2 = AXIS2_WODEN_ELEMENT_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_op, env, ext_ns);
        if(AXIS2_TRUE == temp1 || AXIS2_TRUE == temp2)
        {
            void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding_op", 
                    bind_op, ext_ns);
            binding_op = axis2_woden_binding_op_to_wsdl_obj(binding_op, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(bind_op, env, 
                    ext_ns, comp_ext);
        }
    }
    /*
     * Second, apply the rules from WSDL 2.0 Part 2 Adjuncts spec for default values for SOAP and HTTP 
     * extension properties to create Component Extension objects even if the related extension attributes
     * or elements do not exist in the WSDL. 
     * 
     * TODO chg this hardcoded behaviour so that any default rules for SOAP, HTTP or user-defined
     * extensions can be registered in some way and interpreted here at run time.
     */
    bind_op = axis2_woden_binding_op_to_nested_element(bind_op);
    parent_element = AXIS2_WODEN_NESTED_ELEMENT_GET_PARENT_ELEMENT(bind_op, env);
    parent_element = axis2_woden_binding_to_binding_element(parent_element, env);
    binding_type = AXIS2_WODEN_BINDING_ELEMENT_GET_TYPE(parent_element, env);
    binding_type_str = AXIS2_URL_TO_EXTERNAL_FORM(binding_type, env);
    if(0 == AXIS2_STRCMP(AXIS2_WODEN_URI_NS_SOAP, binding_type_str))
    {
        /*
         * If the binding type is HTTP then the {http input serialization} and {http output serialization} 
         * properties have default values defined by the HTTP Binding rules in the WSDL 2.0 Part 2 Adjuncts
         * spec, the {http fault serialization} property has the default "application/xml" and the 
         * {http query parameter separator default} property has the default "&". So if an 
         * HTTP Binding Operation Extensions object has not already been created, create one now to handle 
         * these default values.
         */
        bind_op = axis2_woden_binding_op_to_wsdl_component(bind_op, env);
        if(NULL == AXIS2_WODEN_WSDL_COMPONENT_GET_COMPONENT_EXTS_FOR_NAMESPACE(bind_op, 
                    env, AXIS2_WODEN_URI_NS_SOAP))
        {
            /*void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding_op", 
                    bind_op, AXIS2_WODEN_URI_NS_SOAP);
            bind_op = axis2_woden_binding_op_to_wsdl_obj(bind_op, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(bind_op, env, 
                    AXIS2_WODEN_URI_NS_SOAP, comp_ext);*/
        }
    }
    return AXIS2_SUCCESS;
}
    

static axis2_status_t
build_binding_msg_ref_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_msg_ref)
{
    axis2_ext_registry_t *er = NULL;

    er = AXIS2_WODEN_DESC_GET_EXT_REGISTRY(builder_impl->f_desc, env);
    ext_namespcs = AXIS2_WODEN_EXT_REGISTRY_QUERY_COMPONENT_EXT_NAMESPACES(er, env, "binding_msg_ref");
    if(ext_namespcs)
        size = AXIS2_ARRAY_LIST_SIZE(ext_namespcs, env);
    for(i = 0; i < size; i++)
    {
        axis2_url_t *ext_ns = NULL;
        axis2_bool_t temp1 = AXIS2_FALSE;
        axis2_bool_t temp2 = AXIS2_FALSE;

        ext_ns = AXIS2_ARRAY_LIST_GET(ext_namespcs, env, i);
        bind_msg_ref = axis2_woden_binding_msg_ref_to_attr_extensible(
                bind_msg_ref, env);
        temp1 = AXIS2_WODEN_ATTR_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_msg_ref, env, ext_ns);
        bind_msg_ref = axis2_woden_binding_msg_ref_to_element_extensible(
                bind_msg_ref, env);
        temp2 = AXIS2_WODEN_ELEMENT_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_msg_ref, env, ext_ns);
        if(AXIS2_TRUE == temp1 || AXIS2_TRUE == temp2)
        {
            void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding_msg_ref", 
                    bind_msg_ref, ext_ns);
            bind_msg_ref = axis2_woden_binding_msg_ref_to_wsdl_obj(bind_msg_ref, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(bind_msg_ref, env, 
                    ext_ns, comp_ext);
        }
    }
    return AXIS2_SUCCESS;
}
    
static axis2_status_t
build_binding_fault_ref_exts(
        void *builder,
        const axis2_env_t *env,
        void *bind_fault_ref)
{
    axis2_ext_registry_t *er = NULL;

    er = AXIS2_WODEN_DESC_GET_EXT_REGISTRY(builder_impl->f_desc, env);
    ext_namespcs = AXIS2_WODEN_EXT_REGISTRY_QUERY_COMPONENT_EXT_NAMESPACES(er, env, "binding_fault_ref");
    if(ext_namespcs)
        size = AXIS2_ARRAY_LIST_SIZE(ext_namespcs, env);
    for(i = 0; i < size; i++)
    {
        axis2_url_t *ext_ns = NULL;
        axis2_bool_t temp1 = AXIS2_FALSE;
        axis2_bool_t temp2 = AXIS2_FALSE;

        ext_ns = AXIS2_ARRAY_LIST_GET(ext_namespcs, env, i);
        bind_fault_ref = axis2_woden_binding_fault_ref_to_attr_extensible(
                bind_fault_ref, env);
        temp1 = AXIS2_WODEN_ATTR_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_fault_ref, env, ext_ns);
        bind_fault_ref = axis2_woden_binding_fault_ref_to_element_extensible(
                bind_fault_ref, env);
        temp2 = AXIS2_WODEN_ELEMENT_EXTENSIBLE_HAS_EXT_ATTRS_FOR_NAMESPACE(
                bind_fault_ref, env, ext_ns);
        if(AXIS2_TRUE == temp1 || AXIS2_TRUE == temp2)
        {
            void *comp_ext = NULL;

            comp_ext = create_component_exts(builder, env, "binding_fault_ref", 
                    bind_fault_ref, ext_ns);
            bind_fault_ref = axis2_woden_binding_fault_ref_to_wsdl_obj(bind_fault_ref, env);
            AXIS2_WODEN_WSDL_OBJ_SET_COMPONENT_EXTS(bind_fault_ref, env, 
                    ext_ns, comp_ext);
        }
    }
    return AXIS2_SUCCESS;
}
    
/*
 * This helper method factors out common code for creating Component Extensions registered
 * in the Extension Registry. 
 */
static void *
create_component_exts(
        void *builder,
        const axis2_env_t *env,
        axis2_char_t *parent_class,
        void *parent_elem,
        axis2_url_t *ext_ns)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_woden_ext_registry_t *er = NULL;
    void *comp_ext = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, parent_class, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, parent_elem, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, ext_ns, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);
        
    er = AXIS2_WODEN_DESC_GET_EXT_REGISTRY(builder_impl->f_desc, env);
    comp_ext = AXIS2_WODEN_EXT_REGISTRY_QUERY_COMPONENT_EXT(er, env, 
            parent_class, ext_ns);
    AXIS2_WODEN_COMPONENT_EXTS_INIT(comp_ext, env, parent_elem, ext_ns);
    
    return comp_ext;
}


/* *******************************************************************************
 *  SERVICE
 * ******************************************************************************/
static axis2_status_t
build_svcs(
        void *builder,
        const axis2_env_t *env,
        void *desc)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    axis2_array_list_t *svcs = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, desc, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);

    svcs = AXIS2_WODEN_DESC_GET_SVC_ELEMENTS(desc, env);
    if(svcs)
        size = AXIS2_ARRAY_LIST_SIZE(svcs, env);
    for(i = 0; i < size; i++)
    {
        void *svc = NULL;

        svc = AXIS2_ARRAY_LIST_GET(svcs, env, i);
        if(AXIS2_TRUE != AXIS2_ARRAY_LIST_CONTAINS(builder_impl->f_svcs_done, 
                    env, svc))
        {
            AXIS2_WODEN_DESC_ADD_TO_ALL_SVCS(builder_impl->f_desc, env, svc);
            if(!builder_impl->f_svcs_done)
            {
                builder_impl->f_svcs_done = axis2_array_list_create(env, 0);
                if(!builder_impl->f_svcs_done)
                {
                    AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, 
                            AXIS2_FAILURE);
                    return AXIS2_FAILURE;
                }
            }
            AXIS2_ARRAY_LIST_ADD(builder_impl->f_svcs_done, env, svc);
        }
    }
    return AXIS2_SUCCESS;
}


/* *******************************************************************************
 *  PROPERTY
 * ******************************************************************************/

static axis2_status_t
build_properties(
        void *builder,
        const axis2_env_t *env,
        axis2_array_list_t *prop_els,
        void *parent)
{
    axis2_woden_component_model_builder_impl_t *builder_impl = NULL;
    int i = 0, size = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, prop_els, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, parent, AXIS2_FAILURE);
    builder_impl = AXIS2_INTF_TO_IMPL(builder);

    size = AXIS2_ARRAY_LIST_SIZE(prop_els, env);
    for(i = 0; i < size; i++)
    {
        void *prop = NULL;
        axis2_qname_t *qname = NULL;

        prop = AXIS2_ARRAY_LIST_GET(props_el, env, i);
        qname = AXIS2_WODEN_PROPERTY_GET_CONSTRAINT_QNAME(prop, env);
        if(NULL != qname)
        {
            void *value = NULL;

            value = AXIS2_WODEN_DESC_GET_TYPE_DEF(builder_impl->f_desc, env, qname);
            AXIS2_WODEN_PROPERTY_SET_VALUE_CONSTRAINT(prop, env, value);
        }
    }
    return AXIS2_SUCCESS;
}

