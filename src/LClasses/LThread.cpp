#include "../../include/LClasses/LClasses.h"

LThread::LThread()
{
    this->handle = NULL;
    this->id = 0;
    this->status = THREAD_NOT_STARTED;
}

LThread::~LThread()
{
    this->stop();
}

LPTHREAD_START_ROUTINE runLThread(void*s)
{
    ((LThread*)s)->setStatus(LThread::THREAD_RUNNING);
    ((LThread*)s)->run();
    ((LThread*)s)->setStatus(LThread::THREAD_FINISHED);
    return 0;
}

void LThread::start()
{
    if (this->status == THREAD_RUNNING) throw new LThreadException("Thread already running");

    // Create the thread to begin execution on its own.

    this->handle = CreateThread
    (
        NULL,                        // default security attributes
        0,                              // use default stack size
        (LPTHREAD_START_ROUTINE)runLThread,                      // thread function name
        (void*)this,                       // argument to thread function
        0,                           // use default creation flags
        &this->id                          // returns the thread identifier
    );

    if (this->handle == NULL) throw new LThreadException("Falha na criacao da LThread");
}

void LThread::stop()
{
    this->setStatus(LThread::THREAD_FINISHED);
    if (this->handle)
        CloseHandle(this->handle);
    this->handle = NULL;
}
