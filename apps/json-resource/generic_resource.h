/*
 * Copyright (c) 2014-15, Ralf Schlatterbeck Open Source Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \defgroup Generic CoAP Resource Handler
 *
 * This factors the boilerplate code necessary for defining a resource
 * together with the necessary handler. Currently this supports
 * text/plain and application/json and outputs the resource with its
 * name in json format. This may change in the future as more CoRE
 * standards (e.g. see RFC 6690) get defined.
 *
 * @{
 */


/**
 * \file
 *      Generic CoAP Resource Handler
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 */

#define STR__(s) #s
#define STR_(s) STR__(s)

#define MAX_GET_STRING_LENGTH   100
#define MAX_URI_STRING_LENGTH    30
#define MAX_QUERY_STRING_LENGTH  30

/*
 * A macro that extends the resource definition and also sets up the
 * necessary handler function that calls format/parse routines that
 * convert from/to string (fs and ts). The ts function needs to return
 * the length written, similar to sprintf.
 * The content type definitions ct="0 5" are text/plain and
 * application/json, respectively, see rfc7252 Table 9 p. 91.
 * Yes, this *is* a hack. But I hate boilerplate code.
 */

#define GENERIC_RESOURCE(name, title, unit, is_str, fs, ts)                \
  static void name##_get_handler                                           \
    ( void *request                                                        \
    , void *response                                                       \
    , uint8_t *buffer                                                      \
    , uint16_t ps                                                          \
    , int32_t *offset                                                      \
    )                                                                      \
  {                                                                        \
    generic_get_handler                                                    \
      (request, response, buffer, ps, offset, STR_(name), is_str, ts);     \
  }                                                                        \
  static void name##_put_handler                                           \
    ( void *request                                                        \
    , void *response                                                       \
    , uint8_t *buffer                                                      \
    , uint16_t ps                                                          \
    , int32_t *offset                                                      \
    )                                                                      \
  {                                                                        \
    generic_put_handler                                                    \
      (request, response, buffer, ps, offset, STR_(name), fs);             \
  }                                                                        \
                                                                           \
  RESOURCE ( res_##name                                                    \
           , "title=\"" STR_(title) "\""                                   \
             ";rt=UCUM:\"" STR_(unit) "\""                                 \
             ";ct=\"0 5\""                                                 \
           , (ts) ? name##_get_handler : NULL                              \
           , NULL /* POST */                                               \
           , (fs) ? name##_put_handler : NULL                              \
           , NULL /* DELETE */                                             \
           )

/* Ignore constant pointer tests above */
#pragma GCC diagnostic ignored "-Waddress"

/**
 * \brief Parse a resource in json format
 * \param bytes: Input string received via coap
 * \param len: Length of input string
 * \param name: Name to search in json input
 * \param buf: Output buffer
 * \param buflen: Output buffer length
 * \return 0 for success, -1 for error
 *
 * If compiled with DEBUG also prints the error encountered
 */
extern int8_t json_parse_variable
  (const uint8_t *bytes, size_t len, char *name, char *buf, size_t buflen);

/**
 * \brief Generic coap GET resource handler
 * \param name: The name of the variable in json
 * \param to_str: Application method to format value for output;
 *        the function may chose to format differently for coap or text
 * The other parameters are the same as a normal resource handler
 * This helps avoid boilerplate code for request handlers
 *
 * The callback functions get the name of the parameter as a first
 * argument, this allows to re-use the same function for different
 * parameters.  In addition it gets a buffer and the size of the buffer.
 * It needs to return the number of bytes output, similar to sprintf.
 */
extern void generic_get_handler
  ( void *request
  , void *response
  , uint8_t *buffer
  , uint16_t preferred_size
  , int32_t *offset
  , char *name
  , int is_str
  , size_t (*to_str)
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
  );

/**
 * \brief Generic coap PUT resource handler
 * \param name: The name of the variable in json
 * \param from_str: Application method to parse value from string
 *        and act on it, may be NULL in which case the resource only
 *        supports GET not PUT
 * The other parameters are the same as a normal resource handler
 * This helps avoid boilerplate code for request handlers
 *
 * The callback functions get the name of the parameter as a first
 * argument, this allows to re-use the same function for different
 * parameters. The from_str in addition gets the string to parse.
 */
extern void generic_put_handler
  ( void *request
  , void *response
  , uint8_t *buffer
  , uint16_t preferred_size
  , int32_t *offset
  , char *name
  , int (*from_str)
    (const char *name, const char *uri, const char *query, const char *s)
  );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

/** @} */

