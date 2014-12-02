#ifndef THREAD_H
#define THREAD_H

#include <windows.h>
#include <exception>
#include <stdio.h>

class LThreadException //: public LException
{
    public:
        LThreadException(const char*s)// : LException(s)
        {
        }
};

class LThread
{
    public:
        static const int THREAD_FINISHED = 0;
        static const int THREAD_RUNNING = 1;
        static const int THREAD_NOT_STARTED = -1;
        LThread();
        virtual ~LThread();

        void start();

        virtual void run()=0;

        int getStatus() {return this->status;};
        void setStatus(int i) {this->status = i;};

        DWORD getId() {return this->id;};
        HANDLE getHandle() {return this->handle;};

        char* toLString()
        {
            char *s = (char*) calloc(sizeof(char), 128);
            sprintf(s, "Thread: H(%d); Id(%d);", (int)this->getHandle(), (int)this->getId());
            return (char*)s;
        }

    protected:
        void sleep(long t)
        {
//            _sleep(t);
        }
        int status;
    private:
        HANDLE handle;
        DWORD id;

        void stop();

};

template<void (*F)()>
class LWorkerThread : public LThread
{
    void run() { F(); }
};

#endif // THREAD_H
