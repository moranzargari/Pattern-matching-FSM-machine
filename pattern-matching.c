//calling libraries
#include "pattern-matching.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//-------------------------------------declerations of functions-----------------------------------------------//
pm_state_t* creatNewState(pm_int_t id,pm_int_t depth);
slist_t* queueDepthOne(pm_t *fsm);
int forEachR(pm_state_t *r,slist_t *transR,slist_t *q);
void addMatch(pm_state_t *fstate,int i,slist_t *q);
void delete(pm_state_t *st);
void freelist(pm_state_t *s,slist_destroy_t d,slist_destroy_t d1);
slist_t* creatNewList();
int Error(void* pointer);
int ErrorFsm(pm_t *fsm);
int ErrorState(pm_state_t *state);
int checkfail(pm_state_t *s);

//-------------------------------------defines-----------------------------------------------//
#define CONT 0
#define FAIL -1
#ifndef PATTERN_MATCHING_C
#define PATTERN_MATCHING_C


//============================================================================================FUNCTIONS===================================================================================//

//----------------------------------pm_init-----------------------------------------------//
/* Initializes the fsm parameters (the fsm itself sould be allocated).  Returns 0 on success, -1 on failure. 
*  this function should init zero state
*/
int pm_init(pm_t *fsm){
	
	if(Error(fsm)==FAIL)
		return FAIL;
	fsm->zerostate =(pm_state_t*)malloc(sizeof(pm_state_t));
	if(Error(fsm->zerostate)==FAIL)
		return FAIL;
	fsm->zerostate->id=0;
	fsm->zerostate->depth=0;
	fsm->zerostate->fail=NULL;
	fsm->zerostate->output=creatNewList();
	fsm->zerostate->_transitions=creatNewList();
	if(Error(fsm->zerostate->output)==FAIL || Error(fsm->zerostate->_transitions)==FAIL)
		return FAIL;
	fsm->newstate=1;
return CONT;
}
//----------------------------------pm_addstring-----------------------------------------------//
/* Adds a new string to the fsm, given that the string is of length n. 
   Returns 0 on success, -1 on failure.*/
int pm_addstring(pm_t *fsm,unsigned char *str, size_t n){
	int i;
	if(ErrorFsm(fsm)==FAIL || n==0 || Error(str)==FAIL || n!=strlen(str))
		return FAIL;
	pm_state_t* curr=fsm->zerostate;
	for(i=0;i<n;i++)
	{
		pm_state_t* temp=pm_goto_get(curr,str[i]);	
		if(temp==NULL)
		{
			fsm->newstate++;
			pm_state_t* newState=creatNewState(fsm->newstate-1,(curr->depth)+1);
			if(Error(newState)==FAIL)
			{
				fsm->newstate--;
				return FAIL;
			}
			printf("Allocating state %d\n",newState->id);
			if((pm_goto_set(curr,str[i],newState))==FAIL)
				return FAIL;
			curr=newState;
		}
		else
			curr=temp;
	}
if(slist_head(curr->output)==NULL)
{
	if(slist_append(curr->output,str)==FAIL)
		return FAIL;
}				
return CONT;
}
//----------------------------------pm_makeFSM-----------------------------------------------//
/* Finalizes construction by setting up the failrue transitions, as
   well as the goto transitions of the zerostate. 
   Returns 0 on success, -1 on failure.*/
int pm_makeFSM(pm_t *fsm){
	
	if(ErrorFsm(fsm)==FAIL)
		return FAIL;
	slist_t *q=queueDepthOne(fsm);
	if(Error(q)==FAIL)
		return FAIL;
	while(slist_head(q)!=NULL)
	{
		pm_state_t *r=(pm_state_t *)slist_pop_first(q);
		if(Error(r)==FAIL)
			return FAIL;
		if(r->_transitions!=NULL && slist_head(r->_transitions)!=NULL)
		{
			if(forEachR(r,r->_transitions,q)==FAIL)
				return FAIL;
		}
	}
free(q);
return CONT;
}
//----------------------------------pm_goto_set-----------------------------------------------//
/* Set a transition arrow from this from_state, via a symbol, to a
   to_state. will be used in the pm_addstring and pm_makeFSM functions.
   Returns 0 on success, -1 on failure.*/  
int pm_goto_set(pm_state_t *from_state,
			   unsigned char symbol,
			   pm_state_t *to_state){
	
	pm_labeled_edge_t* edge=(pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));
	if(ErrorState(from_state)==FAIL || Error(edge)==FAIL || Error(to_state)==FAIL)
		return FAIL;
	edge->label=symbol;
	edge->state=to_state;
	if(slist_append(from_state->_transitions,edge)==FAIL)
		return FAIL;
	printf("%d -> %c -> %d\n",from_state->id,symbol,to_state->id);
return CONT;
}
//----------------------------------pm_goto_get-----------------------------------------------//
/* Returns the transition state.  If no such state exists, returns NULL. 
   will be used in pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions. */
pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol){
	if(ErrorState(state)==FAIL)
		return NULL;
	slist_node_t *temp=slist_head(state->_transitions);
	while(temp!=NULL){
		pm_labeled_edge_t* edge=(pm_labeled_edge_t*)slist_data(temp);
		if(edge->label==symbol)
			return edge->state;
		temp=slist_next(temp);
	}
return NULL;
}
//----------------------------------pm_fsm_search-----------------------------------------------//
/* Search for matches in a string of size n in the FSM. 
   if there are no matches return empty list */
 slist_t* pm_fsm_search(pm_state_t *root,unsigned char *text,size_t n)
{
	
	int i;
	if(ErrorState(root)==FAIL || Error(text)==FAIL || n==0 || n!=strlen(text))
		return NULL;
	if(checkfail(root)==FAIL)
		return NULL;
	slist_t *q =creatNewList();
	if(Error(q)==FAIL)
		return NULL;
	pm_state_t *s=root;
	for(i=0;i<n;i++)
	{
		while(pm_goto_get(s,text[i])==NULL)
		{
			if((i+1)!=n)
			{
				if(s->id==0)
					i+=1;
				else
					s=s->fail;
			}
			else
				break;
				
		}
		if(pm_goto_get(s,text[i])!=NULL)
			s=pm_goto_get(s,text[i]);
		if(slist_head(s->output)!=NULL)
		{
			addMatch(s,i,q);
			if(slist_head(s->_transitions)==NULL)
				s=s->fail;
		}
	}
return q;
}
//----------------------------------pm_destroy-----------------------------------------------//
/* Destroys the fsm, deallocating memory. */
void pm_destroy(pm_t *fsm)
{
	if(ErrorFsm(fsm)==FAIL)
		return;
	slist_node_t *temp=slist_head(fsm->zerostate->_transitions);
	while(temp!=NULL)
	{
		pm_labeled_edge_t* edge=(pm_labeled_edge_t*)slist_data(temp);
		delete(edge->state);
		temp=slist_next(temp);
	}
freelist(fsm->zerostate,SLIST_FREE_DATA,SLIST_LEAVE_DATA);
}
//----------------------------------delete-----------------------------------------------//
/*an reference function. the destroy function use's this function to recursivly free all alocated memory */
void delete(pm_state_t *st)
{
	if(slist_head(st->_transitions)==NULL)
		freelist(st,SLIST_LEAVE_DATA,SLIST_LEAVE_DATA);
	else
	{
		slist_node_t *temp=slist_head(st->_transitions);
		while(temp!=NULL)
		{
			pm_labeled_edge_t* edge=(pm_labeled_edge_t*)slist_data(temp);
			delete(edge->state);
			temp=slist_next(temp);
		}
		freelist(st,SLIST_FREE_DATA,SLIST_LEAVE_DATA);
	}
}
//----------------------------------creatNewState-----------------------------------------------//
/*this method creates new state*/
pm_state_t* creatNewState(pm_int_t id,pm_int_t depth)
{
	pm_state_t* newState=(pm_state_t*)calloc(1,sizeof(pm_state_t));
	if(Error(newState)==FAIL)
		return NULL;
	newState->id=id;
	newState->depth=depth;
	newState->output=creatNewList();
	newState->_transitions=creatNewList();
	if(Error(newState->output)==FAIL || Error(newState->_transitions)==FAIL)
		return NULL;
return newState;
}
//----------------------------------queueDepthOne-----------------------------------------------//
/* this function append to list 'q' all states with depth 1*/
slist_t* queueDepthOne(pm_t *fsm)
{
	slist_t *q =creatNewList();
	if(Error(q)==FAIL)
		return NULL; 
	slist_node_t *temp=slist_head(fsm->zerostate->_transitions);
	while(temp!=NULL)
	{
		pm_labeled_edge_t* edge=(pm_labeled_edge_t*)slist_data(temp);
		edge->state->fail=fsm->zerostate;
		if(slist_append(q,edge->state)==FAIL)
			return NULL;
		temp=slist_next(temp);	
	}
return q;
}
//----------------------------------forEachR-----------------------------------------------//
/*a reference function to the makeFSM function*/
int forEachR(pm_state_t *r,slist_t *transR,slist_t *q)
{
	slist_node_t *temp=slist_head(transR);
	while(temp!=NULL)
	{
		pm_labeled_edge_t* edge=(pm_labeled_edge_t*)slist_data(temp);
		if(slist_append(q,edge->state)==FAIL)
			return FAIL;
		pm_state_t* fatherState=r->fail;
		while(pm_goto_get(fatherState,edge->label)==NULL)
		{
			if(fatherState->id==0)
			{
				edge->state->fail=fatherState;
				break;
			}
			else
				fatherState=fatherState->fail;
		}
		if(fatherState->id==0 && pm_goto_get(fatherState,edge->label)!=NULL)
			edge->state->fail=pm_goto_get(fatherState,edge->label);	
		if(fatherState->id!=0)
			edge->state->fail=pm_goto_get(fatherState,edge->label);
		if((slist_append_list(edge->state->output,(edge->state->fail)->output))==FAIL)
			return FAIL;
		if(edge->state->fail->id!=0)
			printf("Setting f(%d) = %d\n",edge->state->id,edge->state->fail->id);
		temp=slist_next(temp);	
	}
return CONT;
}
//----------------------------------addMatch-----------------------------------------------//
/*this method append to list 'q' the matched pattern than function "search" found*/
void addMatch(pm_state_t *fstate,int i,slist_t *q)
{
	slist_node_t *temp=slist_head(fstate->output);
	while(temp!=NULL)
	{
		pm_match_t* match=(pm_match_t*)malloc(sizeof(pm_match_t));
		if(Error(match)==FAIL)
			return;
		match->pattern=slist_data(temp);
		match->start_pos=i-(strlen(slist_data(temp))-1);
		match->end_pos=i;
		match->fstate=fstate;
		if(slist_append(q,match)==FAIL)
			return;
		temp=slist_next(temp);
		printf("Pattern: %s, start at: %d, ends at: %d, last state = %d\n",match->pattern,match->start_pos,match->end_pos,match->fstate->id);
	}
}
//----------------------------------freelist-----------------------------------------------//
/*a reference function to "destroy" to avoid duplicate code*/
void freelist(pm_state_t *s,slist_destroy_t d,slist_destroy_t d1)
{
	slist_destroy(s->_transitions,d);
	slist_destroy(s->output,d1);
	free(s->output);
	free(s->_transitions);
	free(s);
}
//----------------------------------creatNewList-----------------------------------------------//
/*the function creates new list--to avoid duplicate code*/
slist_t* creatNewList()
{
	slist_t* q=(slist_t*)malloc(sizeof(slist_t));
	if(Error(q)==FAIL)
		return NULL;
	slist_init(q);
return q;
}
//----------------------------------Error-----------------------------------------------//
/* this is a general failure function to pointers --to avoid duplicate code*/
int Error(void* pointer)
{
	if(pointer==NULL)
		return FAIL;
return CONT;
}
//----------------------------------ErrorFsm-----------------------------------------------//
/* this is a fsm failure function --to avoid duplicate code*/
int ErrorFsm(pm_t *fsm)
{
	if(Error(fsm)==FAIL || Error(fsm->zerostate)==FAIL || Error(fsm->zerostate->_transitions)==FAIL)
		return FAIL;
return CONT;
}
//----------------------------------ErrorState-----------------------------------------------//
/* this is a state failure function --to avoid duplicate code*/
int ErrorState(pm_state_t *state)
{
	if(Error(state)==FAIL || Error(state->_transitions)==FAIL)
		return FAIL;
return CONT;
}
//----------------------------------checkfail-----------------------------------------------//
/*check in case the function search called before makeFSM*/
int checkfail(pm_state_t *s)
{
	if(s->id==0 && slist_head(s->_transitions)!=NULL)
	{
		if(((pm_labeled_edge_t*)slist_data(slist_head(s->_transitions)))->state->fail==NULL)
			return FAIL;
	}
	else if(s->id!=0 && s->fail==NULL)
		return FAIL;
return CONT;
}
#endif
