
#include "include/System.h"
#include "include/LMap.h"
#include "include/MainMenu.h"

#include <list>
#include <map>

const char* getCompName(Component comp)
{
    switch(comp)
    {
        case COMP_AABB:
            return "AABB";
        case COMP_AI:
            return "AI";
        case COMP_BOMB:
            return "Bomb";
        case COMP_BOMBFIRE:
            return "BombFire";
        case COMP_CONTROLLER:
            return "Controller";
        case COMP_IMAGE:
            return "Image";
        case COMP_POSITION:
            return "Position";
        case COMP_VELOCITY:
            return "Velocity";
        case COMP_RECT:
            return "Rect";
        case COMP_PLAYER:
            return "Player";
        case COMP_NONE:
            return "Base";
        default:
            return "Other";
    }
}

int main()
{
    try
    {
        delete new System;
    }
    catch (LException* e)
    {
        printf("%s\n%s\n", e->getString(), e->details);
    }

    return 0;
}
