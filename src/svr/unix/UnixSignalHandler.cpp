// Copyright 2011 Chris Sanchez
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
// ====================================================================
//

#include <svr/iSvrService.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <svr/unix/UnixSignalHandler.h>
#include <thd/ThdManager.h>
#include <pthread.h>
#include <signal.h>

ThdCriticalSection UnixSignalHandler::_instance_mutex;
UnixSignalHandler* UnixSignalHandler::_instance=NULL;

//
// This object must be instantiated in the first thread (ie main). 
// before any threads have been created. Otherwise, signals
// can be sent to any thread within a process, and this handler would, 
// therefore,  not be guaranteed to receive the intended signal.
//
UnixSignalHandler::UnixSignalHandler(RefCountedPtr<iSvrService> &itsService ):_itsService(itsService)
{
	// Initialize the mask
	sigemptyset(&_mask);

	// Add signals we want to handle
	sigaddset(&_mask,SIGINT);
	sigaddset(&_mask,SIGSTOP);
	sigaddset(&_mask,SIGQUIT);
	sigaddset(&_mask,SIGTERM);
	sigaddset(&_mask,SIGUSR1);
	sigaddset(&_mask,SIGUSR2);

	// Set the mask in current thread. This should be done in main before
	// any threads have been created. (signal mask is inherited 
	// thru subsqent new threads).
	pthread_sigmask(SIG_BLOCK,&_mask,NULL);

}

//
//This runner will handle signals sent to this thread.
//
unsigned UnixSignalHandler::run()
{
	int signal_sent;
	int rc;

	pthread_sigmask(SIG_UNBLOCK,&_mask,NULL);

	_current_action = IDLE;
	//
	// sigwait is a blocking call until a signal in mask is sent.
	//

	while ((_current_action != STOPPING) && !(sigwait(&_mask,&signal_sent)))
		switch(signal_sent)
		{

	    		case SIGINT:
	    		case SIGSTOP:
	    		case SIGQUIT:
	    		case SIGTERM:
				_current_action = STOPPING;
				// if stop returns error, we are not sure
				// the state of the service.  lets exit
				// the signal thread.
				if ((rc =_itsService->stop()) != -1)
					waitforstop(_itsService);
				break;
			case SIGUSR1:
				// we might want to turn on debugging
				break;
			case SIGUSR2:
				// we might want to turn off debugging
				break;
			default:
				break;
		}

	return 0;
				
}

//
//This instance ensures only one UnixSignalHandler object exits within a 
// process (ie Singleton). 
// The cavet to this design, is that the class user MUST delete
// the instance (at most one time). It would be simpler to use a local static
// variable which is destructed upon exit(commented below). 
// However, since we use RefCountedPtr,
// the static variable is deleted which causes SEGV. So lets go ahead and use
// a static pointer instead.
UnixSignalHandler* UnixSignalHandler::instance(RefCountedPtr<iSvrService> &itsService )
{
	// Make sure only one thread creates the instance.
	// Please note, only the main thread should invoke the creation
	// of the Signal handler object.
	ThdAutoCriticalSection critsection(_instance_mutex);

	// It would be better to return a local static instance. However, in
        // this environement the instance is deleted using RefCountedPtr. 
	// Hence, we must use a static pointer.
	// static UnixSignalHandler _instance(itsService);

	// check state of our instance.
	if (!_instance) _instance = new UnixSignalHandler(itsService);

	return _instance;	
}

void UnixSignalHandler::waitforstop(RefCountedPtr<iSvrService> &localService)
{
    //This function is a blocking call until the service has stopped
    while (!localService->isStopped())
    {
            ThdManager::Sleep( 1000 );
    }   

}
UnixSignalHandler::~UnixSignalHandler()
{
	// Clean up mask
	sigemptyset(&_mask);

	//
	// We might get multiple calls to our destructor. Lets make
	// sure we only delete one time and only one thread deletes the
	// instance.
	ThdAutoCriticalSection critsection(_instance_mutex);

	if (!_instance) 
	{
		delete _instance;
		_instance = NULL;
	}
}
