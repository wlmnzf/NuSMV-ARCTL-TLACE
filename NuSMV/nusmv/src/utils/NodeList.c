/**CSourceFile*****************************************************************

  FileName    [NodeList.c]

  PackageName [utils]

  Synopsis    [This is a class exporting a node_ptr based list, but with 
  a higher level and better performances]

  Description [You can pass from a NodeList to node_ptr by calling the
  method to_node_ptr. Viceversa is supported by calling the
  constructor create_from_list.  Notice that at the moment a minimal
  bunch of functionalities is exported ]

  SeeAlso     [NodeList.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``utils'' package of NuSMV version 2. 
  Copyright (C) 2003 by ITC-irst.

  NuSMV version 2 is free software; you can redistribute it and/or 
  modify it under the terms of the GNU Lesser General Public 
  License as published by the Free Software Foundation; either 
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.

  For more information of NuSMV see <http://nusmv.irst.itc.it>
  or email to <nusmv-users@irst.itc.it>.
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

******************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "NodeList.h"
#include "utils.h"
#include "assoc.h"

static char rcsid[] UTIL_UNUSED = "$Id: NodeList.c,v 1.1.2.6 2004/02/06 15:01:41 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Types definition                                                          */
/*---------------------------------------------------------------------------*/

typedef struct NodeList_TAG 
{
  node_ptr first;

  /* other useful members */
  node_ptr last;
  int len;

  /* for constant-time search */
  hash_ptr elems_hash; 

} NodeList;


/*---------------------------------------------------------------------------*/
/* Macros definition                                                         */
/*---------------------------------------------------------------------------*/
#define END_LIST_ITERATOR  LIST_ITER(Nil)


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void node_list_init ARGS((NodeList_ptr self, node_ptr list));
static void node_list_deinit ARGS((NodeList_ptr self));
static void node_list_copy ARGS((NodeList_ptr self, NodeList_ptr copy));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr NodeList_create()
{
  NodeList_ptr self = ALLOC(NodeList, 1);
  NODE_LIST_CHECK_INSTANCE(self);

  node_list_init(self, Nil);
  return self;
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr NodeList_create_from_list(node_ptr list)
{
  NodeList_ptr self = ALLOC(NodeList, 1);
  NODE_LIST_CHECK_INSTANCE(self);

  node_list_init(self, list);
  return self;
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void NodeList_destroy(NodeList_ptr self)
{
  NODE_LIST_CHECK_INSTANCE(self);
  node_list_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr NodeList_copy(const NodeList_ptr self)
{
  NodeList_ptr copy;
  NODE_LIST_CHECK_INSTANCE(self);
  
  copy = ALLOC(NodeList, 1);
  NODE_LIST_CHECK_INSTANCE(copy);

  node_list_copy(self, copy);
  return copy;
}


/**Function********************************************************************

  Synopsis           [Casts to node_ptr based list]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr NodeList_to_node_ptr(const NodeList_ptr self)
{
  NODE_LIST_CHECK_INSTANCE(self);
  return self->first;
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void NodeList_append(NodeList_ptr self, node_ptr elem)
{
  node_ptr new;
  int c; 

  NODE_LIST_CHECK_INSTANCE(self);
  
  new = cons(elem, Nil);
  
  if (self->last != Nil) {
    setcdr(self->last, new);
  } 

  self->last = new;  

  if (self->first == Nil) {
    self->first = self->last;
  }
  self->len += 1;  

  c = NodeList_count_element(self, elem);
  insert_assoc(self->elems_hash, elem, (node_ptr) (c+1));
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void NodeList_prepend(NodeList_ptr self, node_ptr elem)
{
  int c; 
  NODE_LIST_CHECK_INSTANCE(self);

  self->first = cons(elem, self->first);
  if (self->last == Nil)  self->last = self->first;
  self->len += 1;  

  c = NodeList_count_element(self, elem);
  insert_assoc(self->elems_hash, elem, (node_ptr) (c+1));
}


/**Function********************************************************************

  Synopsis           [Walks through the list, calling given funtion 
  for each element]

  Description        [Returns the number of visited nodes, which can be less
  than the total number of elements since foo can decide to interrupt
  the walking]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int NodeList_foreach(NodeList_ptr self, NODE_LIST_FOREACH_FUN_P foo,
		      void* user_data)
{
  ListIter_ptr iter; 
  boolean cont = true;
  int walks = 0;

  NODE_LIST_CHECK_INSTANCE(self);

  iter = NodeList_get_first_iter(self);
  while ( (! ListIter_is_end(iter)) && cont ) {
    cont = foo(self, iter, user_data);
    ++walks;
    iter = ListIter_get_next(iter);
  }  
  
  return walks;
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int NodeList_get_length(const NodeList_ptr self)
{
  NODE_LIST_CHECK_INSTANCE(self);
  return self->len;
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void NodeList_reverse(NodeList_ptr self)
{
  NODE_LIST_CHECK_INSTANCE(self);

  self->last = self->first;
  self->first = reverse(self->first);
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean NodeList_belongs_to(const NodeList_ptr self, node_ptr elem)
{
  NODE_LIST_CHECK_INSTANCE(self);
  
  return (find_assoc(self->elems_hash, elem) != Nil);
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int NodeList_count_element(const NodeList_ptr self, node_ptr elem)
{
  int res = 0;
  node_ptr c;
    
  NODE_LIST_CHECK_INSTANCE(self);
  
  c = find_assoc(self->elems_hash, elem);
  if (c != Nil) res = (int) c;
  
  return res;
}


/**Function********************************************************************

  Synopsis           [Returns the iterator pointing to the first element]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
ListIter_ptr NodeList_get_first_iter(const NodeList_ptr self)
{
  NODE_LIST_CHECK_INSTANCE(self);

  return LIST_ITER(self->first);
}


/**Function********************************************************************

  Synopsis           [Returns the element at the position pointed by iter]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr NodeList_get_value_at(const NodeList_ptr self, 
			       const ListIter_ptr iter)
{
  NODE_LIST_CHECK_INSTANCE(self);
  nusmv_assert(iter != END_LIST_ITERATOR);
  
  return car((node_ptr) iter);
}



/**Function********************************************************************

  Synopsis           [Returns the following iterator]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
ListIter_ptr ListIter_get_next(const ListIter_ptr self)
{
  nusmv_assert(self != END_LIST_ITERATOR);
  
  return LIST_ITER( cdr((node_ptr) self) );
}


/**Function********************************************************************

  Synopsis           [Returns true if the iteration is given up]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean ListIter_is_end(const ListIter_ptr self)
{
  return (self == END_LIST_ITERATOR);
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void node_list_init(NodeList_ptr self, node_ptr list) 
{
  node_ptr iter;
  self->first = list;
  self->last = list;
  self->len = 0;

  self->elems_hash = new_assoc();
  nusmv_assert(self->elems_hash != (hash_ptr) NULL);

  /* finds the last node */
  iter = list;
  while (iter != Nil) {
    int c; 
    node_ptr elem = car(iter);
    self->last = iter; 
    self->len += 1;  

    c = NodeList_count_element(self, elem);
    insert_assoc(self->elems_hash, elem, (node_ptr) (c+1));
    iter = cdr(iter);
  }
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void node_list_deinit(NodeList_ptr self)
{
  free_list(self->first);
  self->first = Nil;
  self->last = Nil;
  self->len = 0;  
  free_assoc(self->elems_hash);
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void node_list_copy(NodeList_ptr self, NodeList_ptr copy)
{
  node_list_init(copy, self->first);
} 
  
