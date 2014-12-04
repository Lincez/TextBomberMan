#include "../include/System.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

System::System()
{
    printf("running...\n");
    printf("Setting World...\n");
    this->world = new World();
    this->wRun = new WorldRun();
    this->wRun->setWorld(this->world);

//     Criando Heroi
    Entity* hero = this->world->createEntity();
    // Positioning
    Position* position = new Position;
    position->x = position->y = 1;
    hero->addComponent(position);
    // Setting Velocity
    Velocity* velocity = new Velocity;
    velocity->x = 0;
    velocity->y = 0;
    hero->addComponent(velocity);
    // Setting Image
    Image* image = new Image('H');
    hero->addComponent(image);
    // Setting AABB
    AABB* aabb = new AABB;
    aabb->w = 1;
    aabb->h = 1;
    hero->addComponent(aabb);
    // Setting Controller
    Controller* controller = new Controller;
    controller->up = 'w';
    controller->dn = 's';
    controller->lt = 'a';
    controller->rt = 'd';
    controller->bomb = 'l';
    hero->addComponent(controller);
    // Setting Player
    Player* player = new Player();
    player->bombAmount = 5;
    hero->addComponent(player);
    hero->addComponent(new BombKicker);

    makeWall(0, 0, 20, 1); // Cima
    makeWall(0, 1, 1, 20); // Esquerda
    makeWall(1, 20, 20, 1); // Baixo
    makeWall(20, 0, 1, 20); // Direita
    bool wall[20][20];
    memset(wall, 0, sizeof(wall));
    for (int i = 2; i < 20; i=i+2)
    {
        for (int j = 2; j < 20; j=j+2)
        {
            wall[i][j] = true;
            makeWall(i, j, 1, 1); // intermediarias
        }
    }
    for (int i = 2; i < 19; i++)
    {
        for (int j = 2; j < 19; j++)
        {
            if (wall[i][j] == false)
            {
                wall[i][j] = true;
                Entity* softWall = new Entity();
                softWall->addComponent(new Position(i, j));
                softWall->addComponent(new Image('#'));
                softWall->addComponent(new AABB(1, 1));
                softWall->addComponent(new SoftWall());
                this->world->insertEntity(softWall);
            }
        }
    }


    try
    {
        loop();
    }
    catch (...)
    {
        printf("\n\tERROR\n");
    }

}

void System::makeWall(int x, int y, int w, int h)
{
    // Making upper wall
    Entity* wall = this->world->createEntity();
    // positioning
    Position *position = new Position;
    position->x = x;
    position->y = y;
    wall->addComponent(position);
    // AABBing
    AABB* aabb = new AABB;
    aabb->w = w;
    aabb->h = h;
    wall->addComponent(aabb);
    // Rect'ing
    Rect* rect = new Rect;
    rect->w = w;
    rect->h = h;
    wall->addComponent(rect);
    // Setting Image
    Image* image = new Image('X');
    wall->addComponent(image);
}

System::~System()
{
//    printf("Deletando System...\n");
    delete this->world;
}

void System::loop()
{
    bool running = true;
    this->wRun->start();
    while (running)
    {
        int key = getch();

        this->wRun->setKey(key);

//        printf("%d\n", key);
//        this->world->runSystems(key);
//        this->world->render();

        switch (key)
        {
            case 27:
                running = false;
                break;
        }
    }
    this->wRun->setRunning(false);
    delete this->wRun;
}
