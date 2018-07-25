# Pattern-matching-FSM-machine

slist.c - Implementation of a generic linked list.
pattern-matching.c - implementation of a patterns matching automat.

 Its efficiecny is O(n), when n is the length of the pattern.

how to use it:
* allocate the pm variable
* use the init function to init it.
* add patterns to the automat using the addString function
* complete construction by using the makeFSM function
* make a matches variable and send to it the result of the search function
* Do something with the list of the matches...
* destroy pm with the destroy function
* Don't forget to free the allocated pm pointer


remarks:
Additional functions for pattern_matching.:
* createNewState- the method creats new state.
* queueDepthOne- the method append to list all states depth 1.
* foEach- auxiliary function to "makeFSM".
* addMatch- the method add to list all matches.
* delete - auxiliary function to "Destroy".
* freelist - auxiliary function to "Destroy".
* createNewList - the methode creats new list.
* Error - handle general failures.
* ErrorFsm - handle fsm failures.
* ErrorState - handle state failures.
* checkfail - handle situation when the search called befor makeFSM.

Additional functions for slist.c:
* isEmpty - handle a special case when a linked list is allocated but empty.
* creatNode - creates new node.

defines:
* CONT - equal to 0, use to Announce success.
* FAIL - equal to -1, use to Announce failure.
