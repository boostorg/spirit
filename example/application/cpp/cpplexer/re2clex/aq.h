/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001 Daniel C. Nuffer
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#ifndef AQ_H
#define AQ_H

#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {
namespace re2clex {

typedef unsigned int aq_stdelement;

typedef struct tag_aq_queuetype
{
    size_t head;
    size_t tail;
    size_t size;
    size_t max_size;
    aq_stdelement* queue;
} aq_queuetype;

typedef aq_queuetype* aq_queue;

int aq_enqueue(aq_queue q, aq_stdelement e);
int aq_enqueue_front(aq_queue q, aq_stdelement e);
int aq_serve(aq_queue q, aq_stdelement *e);
int aq_pop(aq_queue q);
#define AQ_EMPTY(q) (q->size == 0)
#define AQ_FULL(q) (q->size == q->max_size)
aq_queue aq_create(void);
void aq_terminate(aq_queue q);
int aq_grow(aq_queue q);

///////////////////////////////////////////////////////////////////////////////
}   // namespace re2clex
}   // namespace cpplexer

#endif // AQ_H
