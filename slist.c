//calling libraries
#include <stdio.h>
#include "slist.h"
#include <stdlib.h>
#include <string.h>

//-------------------------------------declerations of functions-----------------------------------------------//
int isEmpty(slist_t *list);
slist_node_t *creatNode(void* data);

//-------------------------------------defines----------------------------------------------------------------//
#define CONT 0
#define FAIL -1
#ifndef SLIST_C
#define SLIST_C


//============================================================================================FUNCTIONS===================================================================================//

//----------------------------------slist_init-----------------------------------------------//
/** Initialize a single linked list
	\param list - the list to initialize */
void slist_init(slist_t *list)
{	
	if(list==NULL)
		return;
	slist_head(list)=NULL;
	slist_tail(list)=NULL;
	slist_size(list)=0;
}
//----------------------------------slist_destroy-----------------------------------------------//
/** Destroy and de-allocate the memory hold by a list
	\param list - a pointer to an existing list
	\param dealloc flag that indicates whether stored data should also be de-allocated */
void slist_destroy(slist_t *list,slist_destroy_t d)
{
	if(list==NULL)
		return;
	while(slist_head(list)!=NULL)
	{
		slist_node_t *temp;
		temp=slist_head(list);
		if(d==SLIST_FREE_DATA)
		{
			free(slist_data(temp));
		}
		slist_head(list)=slist_next(slist_head(list));
		free(temp);
		temp=NULL;
	}
}
//----------------------------------slist_pop_first-----------------------------------------------//
/** Pop the first element in the list
	\param list - a pointer to a list
	\return a pointer to the data of the element, or NULL if the list is empty */
void *slist_pop_first(slist_t *list)
{
	if(list==NULL)
		return NULL;
	if(isEmpty(list)==0)
		return NULL;
	else
	{
		slist_node_t *temp;
		temp=slist_head(list);
		void* data=slist_data(temp);
		slist_head(list)=slist_next(slist_head(list));
		if(slist_head(list)==NULL)
			slist_init(list);
		else
			slist_size(list)-=1;
		free(temp);
		temp=NULL;
		return data;
	}
}
//----------------------------------slist_append-----------------------------------------------//
/** Append data to list (add as last node of the list)
	\param list - a pointer to a list
	\param data - the data to place in the list
	\return 0 on success, or -1 on failure */
int slist_append(slist_t *list,void *data)
{
	slist_node_t *new_node=creatNode(data);
	if(new_node==NULL || list==NULL)
		return -1;
	slist_next(new_node)=NULL;
	if(isEmpty(list)==0)
		slist_head(list)=slist_tail(list)=new_node;
	else
	{
		slist_next(slist_tail(list))=new_node;
		slist_tail(list)=slist_next(slist_tail(list));
	}
	slist_size(list)+=1;

return CONT;
}
//----------------------------------slist_prepend-----------------------------------------------//
/** Prepend data to list (add as first node of the list)
	\param list - a pointer to list
	\param data - the data to place in the list
	\return 0 on success, or -1 on failure
*/
int slist_prepend(slist_t *list,void *data)
{
	slist_node_t *new_node=creatNode(data);
	if(new_node==NULL || list==NULL)
		return FAIL;
	if(isEmpty(list)==0)	
		slist_head(list)=slist_tail(list)=new_node;
	else
	{
		slist_next(new_node)=slist_head(list);
		slist_head(list)=new_node;
	}
	slist_size(list)+=1;

return 0;
}
//----------------------------------slist_append_list-----------------------------------------------//
/** \brief Append elements from the second list to the first list, use the slist_append function.
	you can assume that the data of the lists were not allocated and thus should not be deallocated in destroy 
	(the destroy for these lists will use the SLIST_LEAVE_DATA flag)
	\param to a pointer to the destination list
	\param from a pointer to the source list
	\return 0 on success, or -1 on failure
*/
int slist_append_list(slist_t*list1, slist_t*list2)
{
	if(list1==NULL || list2==NULL)
		return FAIL;
	slist_node_t *temp=slist_head(list2);
	while(temp!=NULL)
	{
		if((slist_append(list1,slist_data(temp))==-1))
			return FAIL;
		temp=slist_next(temp);
	}
return CONT;
}
//----------------------------------isEmpty-----------------------------------------------//
/*check if the list is Empty in case it allocated */
int isEmpty(slist_t *list)
{
	if(slist_head(list)==NULL)
		return 0;
return 1;
}
//----------------------------------creatNode-----------------------------------------------//
/*the function creats an new node and return -1 in case this list or the new node
is NULL . the function return's the node that created.*/
slist_node_t *creatNode(void* data)
{
	slist_node_t *new_node=(slist_node_t*)malloc(sizeof(slist_node_t));
	slist_data(new_node)=data;
return new_node;
}

#endif
