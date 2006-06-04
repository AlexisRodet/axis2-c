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

#ifndef GUTHTHILA_ERROR_H
#define GUTHTHILA_ERROR_H

#include <guththila_defines.h>
#include <guththila_allocator.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum guththila_status_codes
    {
        GUTHTHILA_FAILURE = 0,
        GUTHTHILA_SUCCESS
    } guththila_status_codes_t;

    typedef enum guththila_error_codes
    {
        GUTHTHILA_ERROR_NONE = 0,
        GUTHTHILA_ERROR_NO_MEMORY,
        GUTHTHILA_ERROR_INVALID_NULL_PARAMETER,
        GUTHTHILA_ERROR_INVALID_ITERATOR_STATE,
        GUTHTHILA_ERROR_INVALID_NODE_TYPE,
        GUTHTHILA_STREAM_WRITER_ERROR_NOT_IN_GUTHTHILA_START_ELEMENT,
        GUTHTHILA_STREAM_WRITER_ERROR_WRITING_TO_STREAM,
        GUTHTHILA_STREAM_WRITER_ERROR_STREAM_STRUCT_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_LOCAL_NAME_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_guththila_namespace_t_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_PREFIX_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_guththila_namespace_t_NOT_DECLARED,
        GUTHTHILA_STREAM_WRITER_ERROR_guththila_element_t_guththila_stack_t_EMPTY,
        GUTHTHILA_STREAM_WRITER_ERROR_ILLEGAL_STATE,
        GUTHTHILA_STREAM_WRITER_ERROR_GUTHTHILA_COMMENT_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_ILLEGAL_GUTHTHILA_COMMENT,
        GUTHTHILA_STREAM_WRITER_ERROR_PROCESSING_INSTRUCTION_TARGET_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_CDATA_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_ILLEGAL_CDATA,
        GUTHTHILA_STREAM_WRITER_ERROR_DTD_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_ENTITY_REF_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_ILLEGAL_XML_VERSION,
        GUTHTHILA_STREAM_WRITER_ERROR_TEXT_NULL,
        GUTHTHILA_STREAM_WRITER_ERROR_ILLEGAL_PREFIX,
        GUTHTHILA_STREAM_WRITER_ERROR_OUT_OF_MEMORY,
        GUTHTHILA_STREAM_WRITER_ERROR_FILE_NOT_FOUND,
        GUTHTHILA_STREAM_READER_ERROR_OUT_OF_MEMORY,
   GUTHTHILA_ERROR_INVALID_ENCODING_DECLARATION,
   GUTHTHILA_ERROR_UNEXPECTED_UTF16_EOF,
   GUTHTHILA_ERROR_UNEXPECTED_EOF,
   GUTHTHILA_ERROR_PROCESS_EQUAL,
   GUTHTHILA_ERROR_INCORRECT_VERSION_INFO,
   GUTHTHILA_ERROR_INCORRECT_XML_DECLARATION,
   GUTHTHILA_ERROR_VERSION_INFO_NOT_FOUND,
   GUTHTHILA_ERROR_ENCODING_DECLARATION_ERROR,
   GUTHTHILA_ERROR_STANDALONE_ERROR_IN_YES,
   GUTHTHILA_ERROR_STANDALONE_ERROR_IN_NO,
   GUTHTHILA_ERROR_STANDALONE_ERROR_YES_OR_NO_NOT_AVAILABLE,
   GUTHTHILA_ERROR_MISSING_GREATER_SIGN_IN_XML_DECLARATION,
   GUTHTHILA_ERROR_INVALID_NAME_STARTING_CHARACTER,
   GUTHTHILA_ERROR_QUOTES_NOT_FOUND_BEFORE_ATTRIBUTE_VALUE,
   GUTHTHILA_ERROR_EMPTY_ELEMENT_NOT_CLOSED,
   GUTHTHILA_ERROR_END_TAG_NOT_CLOSED,
   GUTHTHILA_ERROR_MORE_HYPENS_OCCURED_IN_COMMENT,
   GUTHTHILA_ERROR_TOKENIZE_ERROR,
   GUTHTHILA_ERROR_INVALID_TOKEN_TYPE,
   GUTHTHILA_ERROR_NULL_ATTRIBUTE_NAME,
   GUTHTHILA_ERROR_NULL_ATTRIBUTE_VALUE,
   GUTHTHILA_ERROR_NULL_ATTRIBUTE_PREFIX,
   GUTHTHILA_ERROR_REQUESTED_NUMBER_GREATER_THAN_STACK_SIZE
    } guththila_error_codes_t;



    typedef struct guththila_error guththila_error_t;
    struct guththila_error_ops;


GUTHTHILA_DECLARE_DATA typedef struct guththila_error_ops
{
    guththila_char_t *(GUTHTHILA_CALL * get_message) ();
    int (GUTHTHILA_CALL *free)(guththila_error_t *error);
} guththila_error_ops_t;

struct guththila_error
{
    struct guththila_error_ops *ops;
    int errorno;
} ;


GUTHTHILA_DECLARE (guththila_error_t *)
guththila_error_create (guththila_allocator_t * allocator);


#define GUTHTHILA_ERROR_GET_MESSAGE(error) ((error)->ops->get_message())

#define GUTHTHILA_ERROR_FREE(error) ((error)->ops->free(error))


#ifdef __cplusplus
}
#endif


#endif                          /* GUTHTHILA_ERROR_H */
