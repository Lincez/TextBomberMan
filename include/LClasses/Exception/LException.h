#ifndef LEXCEPTION_H
#define LEXCEPTION_H

#define DEBUG_MODE_ON 1

#include <exception>
#include <string.h>
#include <stdio.h>

class LException : public std::exception
{
    public:
        LException()
        {
            strcpy(this->details, "");
        }
        LException(const char*s, const char*d = NULL)
        {
            #ifdef DEBUG_MODE_ON
            FILE* file = fopen("debug.txt", "at");
            fprintf(file, "%s\n%s\n", s, d);
            fclose(file);
            #endif

            strcpy(this->msg, s);
            if (d != NULL)
                strcpy(this->details, d);
        }
        virtual ~LException() throw(){}

        char*getMsg() {return this->toString();};
        void setMsg(char*s) {strcpy(msg, s);};

        virtual bool equals(LException*e)
        {
            return (strcmp(e->getString(), (this->getString())) == 0);
        }
        virtual bool equals(char*s)
        {
            return (strcmp(this->getString(), s) == 0);
        }
        char* getString() {return msg;}

        operator char*(void)
        {
            return this->toString();
        }

        char details[1024];

    protected:
        char msg[1024];

        virtual char*toString()
        {
            return (char*)this->msg;
        }
    private:
};

void printf(LException*);

#endif // EXCEPTION_H
