#ifndef LMAP_H
#define LMAP_H
#include "LClasses/Lista/Lista.h"

class LComparable
{
    public:
    virtual bool isEqual(LComparable*) = 0;
    virtual bool operator==(LComparable*) = 0;
    virtual void print()=0;
};

template <class ObjectType>
class LMap
{
    public:
        static const int NOT_FOUND = -1;
        LMap()
        {
            this->keyList.initialize();
            this->objectList.initialize();
        }
        virtual ~LMap()
        {
            this->keyList.clear();
            this->objectList.clear();
        }

        void insert(LComparable* key, ObjectType* object)
        {
            if (this->contains(key)) return;
            objectList.add(object);
            keyList.add(key);
        }
        void remove(LComparable* key)
        {
            int position = this->getPosition(key);
            if (position == LMap::NOT_FOUND) return;
            keyList.remove(position);
            objectList.remove(position);
        }

        bool contains(LComparable* key)
        {
            LIterator<LComparable> it(&this->keyList);
            while (it.hasNext())
            {
                LComparable* ite = it.next();
                printf("comparing ");
                ite->print();
                printf("to given ");
                key->print();
                printf(": ");
//                if (key->isEqual(ite)) return true;
                if (ite->isEqual(key)) return true;
            }
            return false;
        }
        int getPosition(LComparable* key)
        {
            return (this->keyList.search(key));
        }

    protected:
        List<ObjectType> objectList;
        List<LComparable> keyList;
    private:
};

template <class ObjectType>
class LSimpleMap
{
    public:
        static const int INVALID_KEY = 0;
        static const int NOT_FOUND = -1;

        LSimpleMap()
        {
            keyList = NULL;
        }

        bool insert(int key, ObjectType* object)
        {
            if (key == INVALID_KEY) return false;
            if (this->contains(key)) return false;
            int pos = this->objectList.add(object);

            keyList = (int*) realloc(keyList, sizeof(int) * this->objectList.getSize());
            keyList[pos] = key;

            return true;
        }
        void remove(int key)
        {
            for (int i = 0; i < this->objectList.getSize(); i++)
            {
                if (keyList[i] == key)
                {
                    keyList[i] = this->INVALID_KEY;
                    objectList.remove(i);
                    return;
                }
            }
        }
        int getPosition(int key)
        {
            for (int i = 0; i < this->objectList.getSize(); i++)
            {
                if (keyList[i] == key)
                {
                    return i ;
                }
            }
            return this->NOT_FOUND;
        }
        bool contains(int key)
        {
            for (int i = 0; i < this->objectList.getSize(); i++)
            {
                if (keyList[i] == key)
                {
                    return true;
                }
            }
            return false;
        }
        ObjectType* getObject(int key)
        {
            for (int i = 0; i < this->objectList.getSize(); i++)
            {
                if (keyList[i] == key)
                {
                    return objectList.get(i);
                }
            }
            return NULL;
        }

        void clearAndFree()
        {
            this->objectList.clearAndFree();
            free(keyList);
        }
        List<ObjectType>* getList()
        {
            return &this->objectList;
        }

    protected:
        List<ObjectType> objectList;
        int* keyList;
};

#endif // LMAP_H
