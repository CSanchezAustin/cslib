#include<thd/ThdAutoLock.h>
#include <pat/PatRunnable.h>

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
// used to install a signal handler to intercept signals sent to the process.
// This class is designed around the Singleton Pattern. And uses the
// thread runnable interface.
class UnixSignalHandler : public iRunnable
{
public:

    unsigned run();

    // Keep track of current action
    enum action {IDLE,STOPPING} _current_action;

    // Use 'instance' method to retrive and/or create the object.
    static UnixSignalHandler* instance(RefCountedPtr<iSvrService> &itsService);
    ~UnixSignalHandler();

private:
    // This is private to prevent local instanitation.
    UnixSignalHandler(RefCountedPtr<iSvrService> &itsService ); 


    RefCountedPtr<iSvrService> _itsService;
    sigset_t _mask;
    void waitforstop(RefCountedPtr<iSvrService> &localService );

    static ThdCriticalSection _instance_mutex;
    static UnixSignalHandler* _instance;

};

