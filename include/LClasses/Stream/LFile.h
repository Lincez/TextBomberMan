#ifndef LFILE_H
#define LFILE_H

#include <stdio.h>
#include "../Vector/LString.h"
#include "LStream.h"

class LFileException : public LException
{
    public:
        LFileException(const char*s, const char*d=NULL) : LException(s, d)
        {
        }
};

class OpenFileException : public LFileException
{
    public:
        OpenFileException(const char*fileName) : LFileException("file open failed")
        {
            if (fileName != NULL)
            {
                details = "\"";
                details += fileName;
                details += "\"";
                details += " could not be open";
            }
        }
};

class InvalidOpenModeException : public LFileException
{
    public:
        InvalidOpenModeException(const char*fileName, int mode_) : LFileException(fileName, *new String(mode_))
        {
        }
};

class WriteFileException : public LFileException
{
    public:
        WriteFileException() : LFileException("File write failed")
        {

        }
};

class FileCreationFailure : public LFileException
{
    public:
        FileCreationFailure(const char*fileName) : LFileException("File could not be created", fileName)
        {

        }
};

class LFile
{
    public:
        static const int TEXT_MODE  = 1;
        static const int BYTE_MODE  = 2;
        static const int READ_MODE  = 4;
        static const int WRITE_MODE = 8;
        static const int APPEND_MODE = 16;
        LFile()
        {
            file = NULL;
            this->seek = SEEK_SET;
        }
        LFile(LString*name)
        {
            file = NULL;
            this->seek = SEEK_SET;
            setFileName(name);
        }
        LFile(const char*name)
        {
            file = NULL;
            this->seek = SEEK_SET;
            setFileName(name);
        }
        void setFileName(LString* s)
        {
            if (s == NULL) return;
            setFileName(s->c_str());
        }
        void setFileName(const char*s)
        {
            this->fileName = s;
        }
        const char* getFileName()
        {
            return this->fileName.c_str();
        }
        bool exists()
        {
            FILE* f = fopen(getFileName(), "r");
            if (f != NULL) fclose(f);
            return (f != NULL);
        }
        void createFile()
        {
            if (exists()) return;
            FILE* f = fopen(fileName, "w");
            if (f != NULL) fclose(f);
            if (!exists()) throw new FileCreationFailure(fileName);
        }
        void createNewFile()
        {
            this->close();
            this->remove();
            this->createFile();
        }
        bool isOpen()
        {
            return (this->file!=NULL);
        }
        bool seekFile(long seek_)
        {
            this->seek = seek_;
            if (!isOpen()) return false;
            return (fseek(getFile(), seek_, SEEK_SET) == 0);
        }
        void rewind()
        {
            seekFile(0);
        }
        long getLength()
        {
            bool b  = isOpen();
            unsigned long s = this->seek;
            close();

            open(BYTE_MODE + READ_MODE);
            fseek(getFile(), 0, SEEK_END);
            long rt = ftell(getFile());
            close();

            if (b)
            {
                this->open(this->getOpenMode());
                seekFile(s);
            }
            return rt;
        }
        long copy(LFile* origin) throw (LFileException)
        {
            if (!origin->exists()) throw new LFileException("Original File does not exists");
            if (fileName.equals(origin->getFileName())) return 0;
            long counter = 0;
            createFile();
            open(WRITE_MODE + BYTE_MODE);
            origin->open(READ_MODE + BYTE_MODE);

            char* d = (char*) calloc(sizeof(char), 1);
            size_t len = fread(d, sizeof(char), 1, origin->getFile());
            while (len != 0)
            {
                counter++;
                fwrite(d, sizeof(char), 1, this->getFile());
                len = fread(d, sizeof(char), 1, origin->getFile());
            }
            origin->close();
            close();

            return counter;
        }
        void remove()
        {
            this->close();
            if (!this->exists()) return;
            std::remove(this->getFileName());
        }
        void rename(const char* name_)
        {
            bool b = this->isOpen();
            this->close();
            std::rename(this->getFileName(), name_);
            this->setFileName(name_);
            if (b) this->open(this->getOpenMode());

        }
        void flushFile()
        {
            if (isOpen())
                fflush(this->file);
        }
        LTextInputStream* getTextInput();   /*** IMPLEMENTADO NO FIM DO ARQUIVO ***/
        LTextOutputStream* getTextOutput(); /*** IMPLEMENTADO NO FIM DO ARQUIVO ***/
        LOutputStream* getOutput();
        LInputStream* getInput();
        int getOpenMode() {return this->openMode;}
        const char* getExtension()
        {
            LString* ext = new LString;
            size_t i = fileName.length();
            while (fileName.at(i) != '.')
            {
                if (i == 0) throw new LFileException("File has no extension");
                i--;
            }
            i++;
            while (i < fileName.length())
            {
                (*ext) += fileName.at(i);
                i++;
            }

            return ext->c_str();
        }
        long getSeek() {return this->seek;}
        virtual ~LFile()
        {
            close();
        }
    protected:
        FILE* getFile()
        {
            return this->file;
        }
        void open(int mode_) // throw (LFileException, OpenFileException)
        {
            if (fileName.length() == 0) throw new LFileException("LFile has no name");
            if (!exists()) if (!(mode_ & WRITE_MODE)) throw new OpenFileException(getFileName());
            if (this->getOpenMode() != mode_)
            {
                this->close();
                file = fopen(getFileName(), solveMode(mode_));
                if (file == NULL) throw new OpenFileException(getFileName());
                this->openMode = mode_;
            }
            else
            {
                /// File already open on this mode;
            }
        }
        void close()
        {
            if ((isOpen()) && (file!=stdout))
            {
                this->seek = ftell(getFile());
                fclose(file);
                file = NULL;
            }
            openMode = 0;
        }
        const char* solveMode(int mode_) // throw (LFileException)
        {
            if (mode_ & TEXT_MODE)
            {
                if (mode_ & READ_MODE)
                {
                    return "rt";
                }
                else if (mode_ & WRITE_MODE)
                {
                    return "wt";
                }
                else if (mode_ & APPEND_MODE)
                {
                    return "at";
                }
            }
            else if (mode_ & BYTE_MODE)
            {
                if (mode_ & READ_MODE)
                {
                    return "rb";
                }
                else if (mode_ & WRITE_MODE)
                {
                    return "r+b";
                }
                else if (mode_ & APPEND_MODE)
                {
                    return "ab";
                }
            }
            throw new InvalidOpenModeException(getFileName(), mode_);
        }
        String fileName;
        FILE*file;
        long seek;
        int openMode;
};

class LFileByte : public LFile, public LInputStream, public LOutputStream
{
    public:
        LFileByte()
        {
        }
        LFileByte(const char*s)
        {
            setFileName(s);
        }
        LFileByte(LString*s)
        {
            setFileName(s);
        }
        void write(const void* data, size_t len)
        {
            this->open(WRITE_MODE + BYTE_MODE);
            if (fwrite(data, len, 1, getFile()) != 1)
            {
                throw new WriteFileException;
            }
        }
        void* read(size_t len)
        {
            this->open(READ_MODE + BYTE_MODE);
            void* data = calloc(len, 1);
            fread(data, len, 1, getFile());
            return data;
        }
};

class LFileText : public LFile, public LTextInputStream, public LTextOutputStream
{
    public:
        LFileText()
        {
            appendMode = false;
        }
        LFileText(const char*s, bool append_ = false) : LFile(s)
        {
            this->appendMode = append_;
        }
        LFileText(LString*s, bool append_ = false) : LFile(s)
        {
            this->appendMode = append_;
        }

        /*** INPUT / OUTPUT Methods ***/
        int getChar()
        {
            open(READ_MODE + TEXT_MODE);
            int c = getc(getFile());
            return (c & 0xFF);
        }
        int putChar(int c)
        {
            open(APPEND_MODE + TEXT_MODE);
            int rt = putc(c, getFile());
            return (rt);
        }
        char* read(size_t len)
        {
            open((READ_MODE + TEXT_MODE));
            char* data = (char*) calloc(sizeof(char), len);
            for (size_t i = 0; i < len; i++)
            {
                data[i] = getc(getFile());
            }
            close();
            return data;
        }
        virtual void write(const char*s)// throw(OpenFileException, WriteFileException)
        {
            int thisMode = (appendMode?APPEND_MODE:WRITE_MODE) + TEXT_MODE;
            open(thisMode);
            fprintf(this->getFile(), (const char*)s);
        }
        void append(const char*s)
        {
            open(APPEND_MODE + TEXT_MODE);
            fprintf(this->getFile(), (const char*)s);
        }
        /******************************/
    protected:
        void closeStream()
        {
            this->close();
        }
        bool appendMode;
};

class LTextStream : public LFileText
{
    public:
        LTextStream(FILE*f)
        {
            this->file = f;
        }
        void write(const char*s)
        {
            fprintf(getFile(), s);
        }

        ~LTextStream()
        {
            this->file = NULL;
        }
};

#endif // LFILE_H

