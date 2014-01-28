#ifndef MULTITHREAD_H
#define MULTITHREAD_H
#include <map>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <process.h>
using namespace std;
class multithread
{
    public:
        multithread()
        {
            number_threads=1;
        }

        ~multithread()
        {
            int i=1;
            while(i<number_threads)
            {
                if(maintainMap[i]==true)
                    {

                        WaitForSingleObject(handleMap[i], INFINITE );
                    }
                i++;
            }
        }
        void registerWait(int processID,bool wait=true)
        {
            maintainMap[processID]=wait;
        }

template <typename T>
        int create( void(*function_name)(void *),T argument,bool wait=true)
        {
            handleMap[number_threads]=(HANDLE)_beginthread(function_name,0,(void*)argument);
            registerWait(number_threads,wait);
            number_threads++;
            return number_threads-1;
        }

        HANDLE getHandle(int processID)
        {
            return handleMap[processID];
        }
        bool isRegisterWait(int processID)
        {
            return maintainMap[processID];
        }

        int getProcessID(HANDLE handle)


        {
            int i=1;
            while(i<number_threads)
                if(handleMap[i]==handle)
                    return i;
            return -1;
        }
        bool createLock(int lockID)
        {
            if(criticalSections.find(lockID)==criticalSections.end())
            {

                InitializeCriticalSection(&criticalSections[lockID]);

                return true;
            }
            return false;
        }
        void destroyLock(int lockID)
        {

            DeleteCriticalSection(&criticalSections[lockID]);
        }
        void lock(int lockID)
        {

            EnterCriticalSection(&criticalSections[lockID]);
        }
        void unlock(int lockID)
        {
            LeaveCriticalSection(&criticalSections[lockID]);
        }
        bool tryLock(int lockID)
        {
            return TryEnterCriticalSection(&criticalSections[lockID]);
        }
    protected:
    private:
        int number_threads;
        map<int,HANDLE> handleMap;
        map<int,CRITICAL_SECTION>criticalSections;
        map<int,bool> maintainMap;
};
//create the thread object
static multithread thread;
#endif // MULTITHREAD_H
