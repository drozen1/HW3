#ifndef __THREAD_H
#define __THREAD_H

#include "../Part1/Headers.hpp"
#include "vector"

class Thread
{
public:
	Thread(uint thread_id) 
	{
        this->m_thread_id=thread_id;
		// Only places thread_id 
	} 
	virtual ~Thread() {} // Does nothing 

	/** Returns true if the thread was successfully started, false if there was an error starting the thread */
	// Creates the internal thread via pthread_create 
	bool start()
	{
       int ret= pthread_create(&this->m_thread,NULL,this->entry_func,this);
        if(ret==0){
            return true;
        }
        return false;
	}

	/** Will not return until the internal thread has exited. */
	void join()
	{
        pthread_join(this->m_thread,NULL);
	}

	/** Returns the thread_id **/
	uint thread_id()
	{
		return m_thread_id;
	}
protected:
	// Implement this method in your subclass with the code you want your thread to run. 
	virtual void thread_workload() = 0;
	uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use

private:
	static void * entry_func(void * thread) { ((Thread *)thread)->thread_workload(); return NULL; }
	pthread_t m_thread;
};

#endif
