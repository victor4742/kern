#include <types.h>
#include <lib.h>
#include <synchprobs.h>
#include <synch.h>
#include <clock.h>
#include "opt-A1.h"
/* 
 * This simple default synchronization mechanism allows only creature at a time to
 * eat.   The globalCatMouseSem is used as a a lock.   We use a semaphore
 * rather than a lock so that this code will work even before locks are implemented.
 */

/* 
 * Replace this default synchronization mechanism with your own (better) mechanism
 * needed for your solution.   Your mechanism may use any of the available synchronzation
 * primitives, e.g., semaphores, locks, condition variables.   You are also free to 
 * declare other global variables if your solution requires them.
 */

/*
 * replace this with declarations of any synchronization and other variables you need here
 */
static struct semaphore *globalCatMouseSem;
#if OPT_A1

	int mouseEating =0;
	int mouseWaiting = 0;
	int catEating = 0;
	int catWaiting = 0;
	int state = 0;
	int availableBowls;
	int totalBowls;
	static char *bowlsArray;
	static struct semaphore *mutex;
#endif

/* 
 * The CatMouse simulation will call this function once before any cat or
 * mouse tries to each.
 *
 * You can use it to initialize synchronization and other variables.
 * 
 * parameters: the number of bowls
 */
void
catmouse_sync_init(int bowls)
{
  #if OPT_A1
	totalBowls = bowls;
  	availableBowls = bowls;
	bowlsArray = kmalloc(bowls * sizeof(char));
	if(bowlsArray == NULL){
	   kfree(bowlsArray);
	   return;
	}
	mutex = sem_create("critical",1);
	if(mutex == NULL){
	   kfree(mutex);
	   return;
	}
	int i;
	for(i=0; i < bowls; i++){
		bowlsArray[i] = '$';
	}
  #endif

  
  /* replace this default implementation with your own implementation of catmouse_sync_init */

  (void)bowls; /* keep the compiler from complaining about unused parameters */
  globalCatMouseSem = sem_create("globalCatMouseSem",1);
  if (globalCatMouseSem == NULL) {
    panic("could not create global CatMouse synchronization semaphore");
  }
  return;
}

/* 
 * The CatMouse simulation will call this function once after all cat
 * and mouse simulations are finished.
 *
 * You can use it to clean up any synchronization and other variables.
 *
 * parameters: the number of bowls
 */
void
catmouse_sync_cleanup(int bowls)
{
  /* replace this default implementation with your own implementation of catmouse_sync_cleanup */
  #if OPT_A1
	kfree(bowlsArray);
	kfree(mutex);
  #endif 
  (void)bowls; /* keep the compiler from complaining about unused parameters */
  KASSERT(globalCatMouseSem != NULL);
  sem_destroy(globalCatMouseSem);
}


/*
 * The CatMouse simulation will call this function each time a cat wants
 * to eat, before it eats.
 * This function should cause the calling thread (a cat simulation thread)
 * to block until it is OK for a cat to eat at the specified bowl.
 *
 * parameter: the number of the bowl at which the cat is trying to eat
 *             legal bowl numbers are 1..NumBowls
 *
 * return value: none
 */

void
cat_before_eating(unsigned int bowl) 
{
  /* replace this default implementation with your own implementation of cat_before_eating */
  #if OPT_A1
	P(mutex);
  	if(state == 0){
	   state = 1;    // state 1 -> catState
        }
   	
	while(state != 1   ||   mouseWaiting == 1 || availableBowls == 0 || bowlsArray[bowl] != '$'){
		
	    
		catWaiting = 1;
		V(mutex);
		kprintf("Hello from inside cat %d" , state);
		clocksleep(2);
	  	P(mutex);

	}
	catWaiting =0;
	availableBowls--;
	bowlsArray[bowl] = 'c';
	V(mutex);

  #else


  
  (void)bowl;  /* keep the compiler from complaining about an unused parameter */
  #endif
KASSERT(globalCatMouseSem != NULL);
  P(globalCatMouseSem);
  
}

/*
 * The CatMouse simulation will call this function each time a cat finishes
 * eating.
 *
 * You can use this function to wake up other creatures that may have been
 * waiting to eat until this cat finished.
 *
 * parameter: the number of the bowl at which the cat is finishing eating.
 *             legal bowl numbers are 1..NumBowls
 *
 * return value: none
 */

void
cat_after_eating(unsigned int bowl) 
{
  #if OPT_A1
      P(mutex);
      availableBowls++;
      bowlsArray[bowl] = '$'; 
      if(availableBowls == totalBowls){
	state = 2;
      }
      V(mutex);
  #else

  /* replace this default implementation with your own implementation of cat_after_eating */
  (void)bowl;  /* keep the compiler from complaining about an unused parameter */
  #endif
  KASSERT(globalCatMouseSem != NULL);
  V(globalCatMouseSem);
}

/*
 * The CatMouse simulation will call this function each time a mouse wants
 * to eat, before it eats.
 * This function should cause the calling thread (a mouse simulation thread)
 * to block until it is OK for a mouse to eat at the specified bowl.
 *
 * parameter: the number of the bowl at which the mouse is trying to eat
 *             legal bowl numbers are 1..NumBowls
 *
 * return value: none
 */

void
mouse_before_eating(unsigned int bowl) 
{
  #if OPT_A1
	P(mutex);
	if( state == 0 ){
		state =2;
	}

	while(state != 2 || catWaiting == 1 || availableBowls == 0 || bowlsArray[bowl] != '$'){
		V(mutex);
		mouseWaiting = 1;
		kprintf( "inside the While :%d \n", state);
		clocksleep(2);
		P(mutex);
	}
	kprintf("Outside the WHile:%d \n", state);
	mouseWaiting = 0;
	availableBowls--;
	bowlsArray[bowl] = 'm';
	V(mutex);
  #else


 
  /* replace this default implementation with your own implementation of mouse_before_eating */
  (void)bowl;  /* keep the compiler from complaining about an unused parameter */
 #endif
  KASSERT(globalCatMouseSem != NULL);
  P(globalCatMouseSem);
}

/*
 * The CatMouse simulation will call this function each time a mouse finishes
 * eating.
 *
 * You can use this function to wake up other creatures that may have been
 * waiting to eat until this mouse finished.
 *
 * parameter: the number of the bowl at which the mouse is finishing eating.
 *             legal bowl numbers are 1..NumBowls
 *
 * return value: none
 */

void
mouse_after_eating(unsigned int bowl) 
{

  #if OPT_A1
	P(mutex);
	bowlsArray[bowl] = '$';
	availableBowls++;
	if(availableBowls == totalBowls){
	    state =1;
	}
	V(mutex);
  #else
  /* replace this default implementation with your own implementation of mouse_after_eating */
  (void)bowl;  /* keep the compiler from complaining about an unused parameter */
  #endif 
 KASSERT(globalCatMouseSem != NULL);
  V(globalCatMouseSem);
}
