#ifndef THREAD_H
#define THREAD_H
#include <windows.h>
#include <exception>
#include "../Vector/LString.h"
#include "../Exception/LException.h"

class LThreadException : public LException
{
    public:
        LThreadException(const char*s) : LException(s)
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
            LString *s = new LString("LThread: H(");
            (*s) += (int)this->getHandle();
            (*s) += "); Id(";
            (*s) += (int)this->getId();
            (*s) += ")";
            return (char*)s->c_str();
        }

    protected:
        void sleep(long t)
        {
            _sleep(t);
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
