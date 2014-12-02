#ifndef SYSTEM_H
#define SYSTEM_H

#include "World.h"

class System
{
    public:
        System();
        virtual ~System();
    protected:
        World* world;
        WorldRun * wRun;
        void loop();

        void makeWall(int x, int y, int w, int h);

    private:
};

#endif // SYSTEM_H
