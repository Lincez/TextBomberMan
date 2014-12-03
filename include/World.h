#ifndef WORLD_H
#define WORLD_H

#include "Components.h"
#include "Thread.h"

#include <vector>

#ifndef repeat
#define repeat(varName, times) for (unsigned varName = 0; varName < times; varName++)
#endif

class Entity
{
    public:
        Entity();
        ~Entity();
        static const int TOTAL_ENTITY = (int)COMP_AMOUNT;

        void addComponent(ComponentBase* comp);

        void removeComponent(Component comp);

        bool hasComponent(Component comp);

        ComponentBase* getComponent(Component comp);

    protected:

        ComponentBase* componentVector[Entity::TOTAL_ENTITY];
        bool hasComponentVector[Entity::TOTAL_ENTITY];
};

class World
{
    public:
        World();
        ~World();

        void render();

        void runSystems(int key);
        void movementSystem(Entity* ent);
        void controllerSystem(Entity* ent, int key);
        void collisionSystem(Entity* ent);
        void collided(Entity* ent1, Entity* ent2);
        void bombSystem(Entity* ent);
        void bombFireSystem(Entity* ent);
        void analiseBombSystem(Entity* ent1, Entity* ent2);
        void powerUpToEntity(Entity* ent, Entity* PowerUp);

        void removeSystem();
        void removeLater(Entity* ent);
        void insertSystem();
        void insertLater(Entity* ent);

        Entity* createEntity();
        Entity* insertEntity(Entity* ent);
        int removeEntity(int id);
        int removeEntity(Entity* ent);

        Entity* createBomb(Position* position, Player* player);
        Entity* createBombFire(Position* position, int power, Direction dir);

        Entity* get(int pos)
        {
            return (entityList[pos]);
        }

    protected:

        int fps;
        int auxFps;
        int lastSec;

//        Entity** entity;
        std::vector<Entity*> entityList;

        std::vector<Entity*> toRemoveList;
        std::vector<Entity*> toInsertList;

        int qtdeEntity;

        static const int BORDER_W = 30;
        static const int BORDER_H = 21;
        Position mapOffset;
        Rect map;

};

class WorldRenderer : public LThread
{
    public:
        WorldRenderer(World* world_)
        {
            this->world = world_;
            this->running = true;
        }
        void run()
        {
            while (running)
            {
                this->world->render();
                for (int i = 0; i < 9999999; i++)
                    for (int j = 0; j < 3; j++);
            }
        }
        void stop()
        {
            running = false;
        }
    protected:
        World* world;
        bool running;
};

class WorldRun : public LThread
{
    public:
        WorldRun()
        {
            this->world = NULL;
            this->running = false;
            this->key = 0;
        }
        void run()
        {
            this->running = true;
            WorldRenderer* renderer = new WorldRenderer(this->world);
            renderer->start();

            while (this->running)
            {
                try
                {
                    world->runSystems(this->key);
                }
                catch (std::exception& e)
                {
                    renderer->stop();
                    system("cls");
                    printf("%s\n\n", e.what());
                    system("pause");
                    this->running = false;
                }


                this->key = 0;

                for (int i = 0; i < 9999999; i++)
                    for (int j = 0; j < 5; j++);
            }

            renderer->stop();
            delete renderer;
        }
        void setWorld(World* world_)
        {
            this->world = world_;
        }
        void setKey(int k_)
        {
            this->key = k_;
        }
        void setRunning(bool b)
        {
            this->running = b;
        }
    protected:
        World* world;
        int key;
        bool running;
};

class Map
{
    public:
        Map();
        World* getWorld();
    protected:
        World* world;

};

#endif // WORLD_H
