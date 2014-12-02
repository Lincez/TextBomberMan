#include "../include/MainMenu.h"
#include "../include/LMap.h"
#include "../include/Components.h"
#include <stdio.h>

class Numbers : public LComparable
{
    public:
    Numbers()
    {
        value = COMP_NONE;
    }
    bool isEqual(Numbers*x)
    {
        printf("is %d equal to %d\n", this->value, x->value);
        return (this->value == x->value);
    }
    bool isEqual(LComparable*x)
    {
        return this->isEqual((Numbers*)x);
    }
    bool operator==(LComparable*x)
    {
        return (this->isEqual(x));
    }
    Component value;

    void print()
    {
        printf(" (%d) ", this->value);
    }
};

class Pair
{
    public:
        Numbers* number;
        ComponentBase* component;
};

class Ent
{
    public:
        Ent()
        {

        }

        Pair* convertId(Component comp)
        {
            Pair* pair = new Pair;
            pair->number = new Numbers();
            pair->number->value = comp;
            switch (comp)
            {
                case COMP_NONE:
                    break;
                case COMP_AABB:
                    pair->component = new AABB;
                    break;
                case COMP_POSITION:
                    pair->component = new Position;
                    break;
                case COMP_VELOCITY:
                    pair->component = new Velocity;
                    break;
                case COMP_IMAGE:
                    pair->component = new Image;
                    break;
                case COMP_CONTROLLER:
                    pair->component = new Controller;
                    break;
                case COMP_AI:
                    pair->component = new AI;
                    break;
                case COMP_RECT:
                    pair->component = new Rect;
                    break;
                case COMP_HERO:
                    pair->component = new Hero;
                    break;
                case COMP_BOMB:
                    pair->component = new Bomb;
                    break;
                default:
                    pair->component = NULL;
                    break;
            }
            return pair;
        }

        void addComponent(ComponentBase* comp)
        {
            if (!this->compMap.insert(comp->getId(), comp))
            {
                delete comp;
            }
        }

        void removeComponent(Component comp)
        {
            this->compMap.remove(comp);
        }

        bool hasComponent(Component comp)
        {
            printf("searching for %d\n", comp);
            int pos = this->compMap.getPosition(comp);
            printf("position: %d\n", pos);
            return (pos != -1);

            return this->compMap.contains(comp);
        }

        LSimpleMap<ComponentBase> compMap;
};

void test(Ent ent1, Ent ent2)
{
    if (ent1.hasComponent(COMP_POSITION))
    {
        printf("ent1 tem Position\n");
    }
    else
    {
        printf("ent1 nao tem Position\n");
    }

    if (ent2.hasComponent(COMP_POSITION))
    {
        printf("ent2 tem Position\n");
    }
    else
    {
        printf("ent2 nao tem Position\n");
    }
    if (ent1.hasComponent(COMP_VELOCITY))
    {
        printf("ent1 tem Image\n");
    }
    else
    {
        printf("ent1 nao tem Image\n");
    }

    if (ent2.hasComponent(COMP_VELOCITY))
    {
        printf("ent2 tem Image\n");
    }
    else
    {
        printf("ent2 nao tem Image\n");
    }
}

MainMenu::MainMenu()
{
    printf("testing...\n");

    Ent ent1, ent2;

    printf("Entidades criadas\n");

    ent1.addComponent(new Position);
    printf("Colocado Position na entidade 1\n");
    ent2.addComponent(new Velocity);
    printf("Colocado Velocity na entidade 2\n");
    ent1.addComponent(new Hero);
    printf("Colocado Hero na entidade 1\n");

    printf("...\n");

//    test(ent1, ent2);

    printf("ent1 %stem position\n", (ent1.hasComponent(COMP_POSITION)?"":"nao "));
    printf("ent1 %stem velocity\n", (ent1.hasComponent(COMP_VELOCITY)?"":"nao "));
    printf("\n");
    printf("ent2 %stem position\n", (ent2.hasComponent(COMP_POSITION)?"":"nao "));
    printf("ent2 %stem velocity\n", (ent2.hasComponent(COMP_VELOCITY)?"":"nao "));
}

int MainMenu::run()
{
    bool b = true;

    while (b)
    {

    }

    return 0;
}

MainMenu::~MainMenu()
{
    //dtor
}



