#include "../include/LMap.h"
/*
template <class ObjectType>
LMap<ObjectType>::LMap()
{
    keylist.initialize();
    objectList.initialize();
}

template <class ObjectType>
void LMap<ObjectType>::insert(LComparable* key, ObjectType* object)
{
    if (this->contains(key)) return;
    objectList.add(object);
    keyList.add(key);
}

template <class ObjectType>
bool LMap<ObjectType>::contains(LComparable* key)
{
    LIterator<LComparable> it(this->keyList);
    while (it.hasNext())
    {
        if (key->isEqual(it.next())) return true;
    }
    return false;
}

template <class ObjectType>
void LMap<ObjectType>::remove(LComparable* key)
{
    int position = this->getPosition(key);
    if (position == LMap::NOT_FOUND) return;
    keyList.remove(position);
    objectList.remove(position);
}

template <class ObjectType>
int LMap<ObjectType>::getPosition(LComparable* key)
{
    return (this->keyList.search(key));
}

template <class ObjectType>
LMap<ObjectType>::~LMap()
{
    this->keyList.clear();
    this->objectList.clear();
}
*/
