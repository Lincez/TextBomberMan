#ifndef LSTREAM_H
#define LSTREAM_H

#include "../Lista/Lista.h"

class LStreamException : public LException /// Classe-Mãe de todas as classes de Exceção lançadas por LStream e seus derivados
{
    public:
        LStreamException(){}
        LStreamException(char*s, char*d = NULL) : LException(s, d)
        {
        }
};

class EndOfStreamException : public LStreamException
{
    public:
        EndOfStreamException()
        {
            setMsg("Stream Ended");
        }
};

class LStream
{
    public:
        LStream* getStream()
        {
            return (LStream*) this;
        }
        virtual ~LStream()
        {
            closeStream();
        }
        virtual void closeStream() {}
};

class LInputStream : public LStream
{
    public:
        virtual void* read(size_t len) = 0;
        LString* readString(size_t len)
        {
            return new LString((char*)read(len));
        }
        Lista* readList(size_t objectSize, int amount)
        {
            Lista* list = new Lista;
            for (int i = 0; i < amount; i++)
                list->add(read(objectSize));
            return list;
        }
        void skip(size_t len)
        {
            free(read(len));
        }
        LInputStream* getInputStream()
        {
            return (LInputStream*) this;
        }
};

class LOutputStream : public LStream
{
    public:
        virtual void write(const void*, size_t len) = 0;
        void writeList(Lista* list, size_t objectSize)
        {
            list->resetIterator();
            while(list->hasNext())
            {
                this->write(list->next(), objectSize);
            }
        }
        LOutputStream* getOutputStream()
        {
            return (LOutputStream*) this;
        }
};

class LTextOutputStream : public LOutputStream
{
    public:
        LTextOutputStream* getTextOutputStream()
        {
            return (LTextOutputStream*) this;
        }
        virtual void write(const void*, size_t len) {}
        virtual void write(const char* str)  = 0;
        virtual void append(const char* str) = 0;

        void writeLine(const char* s)
        {
            LString string = s;
            if (!string.endsWith("\n"))
                string += "\n";
            write(string.c_str());
        }
        void write(LString* s)
        {
            write(s->c_str());
        }
        void appendLine(const char*str = "")
        {
            LString string = str;
            if (!string.endsWith("\n"))
                string += "\n";
            append(string.c_str());
        }
        void writeLine(LString*s = NULL)
        {
            if (s != NULL)
                writeLine(s->c_str());
            else
                writeLine("\n");
        }
        void writeStringList(Lista* list)
        {
            list->resetIterator();
            while(list->hasNext())
            {
                String* s = (String*) list->next();
                writeLine(s);
            }
        }
};

class LTextInputStream : public LInputStream
{
    public:
        LTextInputStream* getTextInputStream()
        {
            return (LTextInputStream*) this;
        }
        virtual char* read(size_t len) = 0;
        virtual int getChar() = 0;
        LString* readLine()
        {
            LString* rt = new LString;
            while (true)
            {
                char c = getChar();
                (*rt) += c;

                if (c == '\n') break;
                if (c == '\r') break;
                if (c == '\0') break;
                if (c == EOF) break;
            }
            if (rt->at(0) == EOF) throw new EndOfStreamException;
            return rt;
        }
        Lista* readStringList()
        {
            Lista* stringList = new Lista;
            while (true)
            {
                try
                {
                    stringList->add(readLine());
                }
                catch (EndOfStreamException* e)
                {
                    break;
                }
            }
            return stringList;
        }
};

#endif // LSTREAM_H
