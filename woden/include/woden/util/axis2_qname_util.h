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

#ifndef AXIS2_QNAME_UTIL_H
#define AXIS2_QNAME_UTIL_H

#include <axis2_qname.h>
#include <axis2_om.h>

#ifdef __cplusplus
extern "C"
{
#endif

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
axis2_qname_util_matches(
        const axis2_env_t *env,
        axis2_qname_t *qname,
        axis2_om_node_t *node);
        
AXIS2_EXTERN axis2_qname_t * AXIS2_CALL
axis2_qname_util_new_qname(
        const axis2_env_t *env,
        axis2_om_node_t *node);

#ifdef __cplusplus
}
#endif
#endif /* AXIS2_QNAME_UTIL_H */
