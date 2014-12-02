#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include "../Exception/LException.h"

class ExceptionLista : public LException
{
    public:
        ExceptionLista(const char*s, int pos_ = -1) : LException(s)
        {
            if (pos_ != -1)
            {
                strcat(this->details, "Posicao da Lista: ");
                char pos_S[16];
                sprintf(pos_S, "%d", pos_);
                strcat(this->details, pos_S);
            }
        }
        ExceptionLista(const char*s, char*listName, int pos_ = -1) : LException(s, listName)
        {
            strcat(this->details, "Nome da Lista: ");
            strcat(this->details, listName);

            if (pos_ != -1)
            {
                strcat(this->details, " = ");
                strcat(this->details, "Posicao da Lista: ");
                char pos_S[16];
                sprintf(pos_S, "%d", pos_);
                strcat(this->details, pos_S);
            }
        }
        virtual ~ExceptionLista() throw()
        {

        }
};

class OutOfBoundException : public ExceptionLista
{
    public:
        OutOfBoundException(char*listName, int pos_ = -1) : ExceptionLista("Out of Bound", listName)
        {
            strcat(this->details, "Nome da Lista: ");
            strcat(this->details, listName);

            if (pos_ != -1)
            {
                strcat(this->details, " = ");
                strcat(this->details, "Posicao da Lista: ");
                char pos_S[16];
                sprintf(pos_S, "%d", pos_);
                strcat(this->details, pos_S);
            }
        }
};

template<class T>
class List
{
    public:
        static const int FEEDBACK_ERROR = -1;
        static const int FEEDBACK_SUCCESS = 0;
        static const int FEEDBACK_DUPLICATE = 2;
        List()
        {
            this->initialize();
        }

        void initialize()
        {
            list = NULL;
            amount = 0;
            usableVector = NULL;
            flagStatus = 0;
            itPosition = 0;
            setName("Lista");
        }

        int add(T*object)
        {
            // Procura no usableVector por lugares vazios
            for (int i = 0; i < amount; i++)
            {
                if (!usableVector[i])
                {
                    list[i] = object;
                    usableVector[i] = true;
                    return i;
                }
            }

            // se não encontrar alloca um novo espaço
            pushBack(object);

            return getLastPosition();
        }
        int pushBack(T*object)
        {
            amount++;
            this->adjustList();
            list[getLastPosition()] = object;
            usableVector[getLastPosition()] = true;
            return getLastPosition();
        }
        T* get(int index)
        {
            if (!isInRange(index)) throw new OutOfBoundException(this->getName(), index);
            return list[index];
        }
        T* noThrowGet(int index)
        {
            if (!isInRange(index)) return NULL;
            return list[index];
        }
        T** getList() {return list;}

        bool isEmpty()
        {
            return (this->getAmount() == 0);
        }
        int getAmount()
        {
            int rt = 0;
            for (int i = 0; i < amount; i++)
            {
                if (usableVector[i]) rt++;
            }
            return rt;
        }
        int getLastPosition() {return amount-1;};

        bool contain(T*object)
        {
            resetIterator();
            while (hasNext())
            {
                if (next() == object) return true;
            }
            return false;
        }
        int search(T*object)
        {
            for (int i = 0; i < this->amount; i++)
            {
                if (this->get(i) == object) return i;
            }
            return FEEDBACK_ERROR;
        }

        int set(int pos, T*newObject)
        {
            if (!isInRange(pos)) return FEEDBACK_ERROR;
            this->list[pos] = newObject;
            this->usableVector[pos] = (list[pos] != NULL);
            return FEEDBACK_SUCCESS;
        }

        int remove(T*object)
        {
            return remove(this->search(object));
        }
        int remove(int index)
        {
            if (!isInRange(index)) return FEEDBACK_ERROR;
            this->usableVector[index] = false;
            this->list[index] = NULL;
            return FEEDBACK_SUCCESS;
        }
        int removeLined(int index)
        {
            if (!isInRange(index)) return FEEDBACK_ERROR;
            for (int i = index; i < (this->amount - 1); i++)
            {
                list[i] = list[i+1];
                usableVector[i] = usableVector[i+1];
            }

            amount--;
            return this->adjustList();
        }

        int adjustList()
        {
            if (amount == 0 && list && usableVector)
            {
                this->clear();
                return FEEDBACK_SUCCESS;
            }

            list = (T**) realloc(list, sizeof(T*) * amount);
            if (!list) flagStatus = FEEDBACK_ERROR;
            if (!list) throw new ExceptionLista("Erro na Criação da Lista", this->getName(), this->getAmount());

            usableVector = (bool*) realloc(usableVector, sizeof(bool) * amount);
            if (!usableVector) flagStatus = FEEDBACK_ERROR;
            if (!usableVector) throw new ExceptionLista("Erro na Criação da Lista", this->getName());

            return FEEDBACK_SUCCESS;
        }

        void clear()
        {
            if (this->list) free(this->list);
            if (this->usableVector) free(this->usableVector);
            this->initialize();
        }
        void clearAndFree()
        {
            for (int i = 0; i < amount; i++)
            {
                if (usableVector[i] && list[i])
                {
                    delete (list[i]);
                }
            }
            this->clear();
        }
        bool isInRange(int index)
        {
            if (index < 0 || index >= this->amount) return false;
            return true;
        }

        int amount;
        int getSize() {return amount;}
        T** list;
        bool* usableVector;

        int flagStatus;

        void setName(const char*s) {strcpy(name, s);};
        char* getName() {return (char*)name;};
        char name[1024];
        int itPosition;

        int getAmountRentSpace()
        {
            int rt = 0;
            for (int i = 0; i < amount; i++)
            {
                if (usableVector[i] == false) rt++;
            }
            return rt;
        }
        void normalizeVector()
        {
            for (int i = 0; i < amount; i++)
            {
                if (usableVector[i] == false)
                {
                    removeLined(i);
                }
            }
        }

        bool hasNext()
        {
            while (noThrowGet(itPosition) == NULL)
            {
                itPosition++;
                if (itPosition >= amount) return false;
            }
            return true;
        }
        T* next()
        {
            return get(itPosition++);
        }
        void resetIterator() {itPosition = 0;}
};

class Lista : public List<void>
{
    public:
        void clearAndFree()
        {
            for (int i = 0; i < amount; i++)
            {
                if (usableVector[i] && list[i])
                {
                    free(list[i]);
                }
            }
            this->clear();
        }
};

template <class T = void>
class LIterator
{
    public:
        LIterator(Lista* list_)
        {
            this->list = (List<T>*)list_;
            resetIterator();
        }
        LIterator(List<T>* list_)
        {
            this->list = list_;
            resetIterator();
        }
        LIterator(LIterator<T>* it)
        {
            this->list = it->list;
            resetIterator();
        }
        void resetIterator()
        {
            this->iterator = 0;
        }
        void endIterator()
        {
            this->iterator = list->amount;
        }
        bool hasNext()
        {
            while (list->noThrowGet(iterator) == NULL)
            {
                iterator++;
                if (iterator >= list->getSize()) return false;
            }
            return true;
        }
        T* first()
        {
            resetIterator();
            return next();
        }
        T* next()
        {
            //if (list->get(iterator) == NULL) throw new LException("barriou");
            return (T*) list->get(iterator++);
        }
        int getIteratorPosition()
        {
            return iterator;
        }
    protected:
        int iterator;
        List<T>* list;
};

#endif // LISTA_H
