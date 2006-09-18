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

#include <woden_endpoint_element.h>

axis2_status_t AXIS2_CALL
woden_endpoint_element_resolve_methods(
    woden_endpoint_element_t *endpoint_element,
    const axis2_env_t *env,
    axis2_hash_t *methods)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, methods, AXIS2_FAILURE);

    endpoint_element->ops->free = axis2_hash_get(methods, "free",
            AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->type = axis2_hash_get(methods, "type",
            AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->set_name = axis2_hash_get(methods,
            "set_name", AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->get_name = axis2_hash_get(methods,
            "get_name", AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->set_binding_qname = axis2_hash_get(methods,
            "set_binding_qname", AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->get_binding_qname = axis2_hash_get(methods,
            "get_binding_qname", AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->get_binding_element = axis2_hash_get(methods,
            "get_binding_element", AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->set_address = axis2_hash_get(methods,
            "set_address", AXIS2_HASH_KEY_STRING);
    endpoint_element->ops->get_address = axis2_hash_get(methods,
            "get_address", AXIS2_HASH_KEY_STRING);

    return AXIS2_SUCCESS;
}

