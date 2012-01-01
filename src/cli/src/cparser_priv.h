/**
 * \file     cparser_priv.h
 * \brief    Private parser definitions.
 * \version  \verbatim $Id: cparser_priv.h 156 2011-09-27 09:13:36Z henry $ \endverbatim
 */
/*
 * Copyright (c) 2008-2009, 2011, Henry Kwok
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the project nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY HENRY KWOK ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL HENRY KWOK BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CPARSER_PRIV_H__
#define __CPARSER_PRIV_H__

#include "cparser.h"
#include "cparser_token.h"

/**
 * A node in the parser tree. It has a node type which determines
 * what type of token is accepted.
 */
struct cparser_node_ {
    cparser_node_type_t   type;      /**< Token type */ 
    uint32_t              flags;     /**< Flags */
    void                  *param;    /**< Token-dependent parameter */
    char                  *desc;     /**< A per-node description string */
    /** Pointer to the next sibling in the same level of the tree */
    cparser_node_t        *sibling;
    /** Pointer to all its children in the next level of the tree */
    cparser_node_t        *children;
};

#define CPARSER_NODE_FLAGS_OPT_START          (1 << 0)
#define CPARSER_NODE_FLAGS_OPT_END            (1 << 1)
#define CPARSER_NODE_FLAGS_OPT_PARTIAL        (1 << 2)
#define CPARSER_NODE_FLAGS_HIDDEN             (1 << 3)

#define VALID_PARSER(p)  (p)

#define NODE_USABLE(p,n) (((p)->is_privileged_mode) ||                  \
                          (!((n)->flags & CPARSER_NODE_FLAGS_HIDDEN)))

/** Return the current line index */
#define CURRENT_LINE(p)  ((p)->cur_line)

typedef struct cparser_list_node_ cparser_list_node_t;

/**
 * \struct   cparser_list_node_t
 * \brief    A special list node used for LIST token.
 * \details  Each list node holds one keyword for a LIST token. They
 *           are singlely linked. The last keyword has a NULL next pointer.
 */
struct cparser_list_node_ {
    cparser_list_node_t   *next;
    const char            *keyword;
};

/**
 * \brief    Print the CLI prompt.
 * \details  If in privileged mode, prepend a '+'.
 *
 * \param    parser Pointer to the parser structure.
 */
void cparser_print_prompt(const cparser_t *parser);

#endif /* __CPARSER_PRIV_H__ */
