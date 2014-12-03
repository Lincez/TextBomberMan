#include "../include/World.h"
#include <time.h>

Entity::Entity()
{
    for (int i = 0; i < COMP_AMOUNT; i++)
    {
        this->componentVector[i] = NULL;
        this->hasComponentVector[i] = false;
    }
}

Entity::~Entity()
{
    for (int i = 0; i < COMP_AMOUNT; i++)
    {
        if (this->hasComponent((Component)i))
        {
            delete (this->componentVector[i]);
        }
    }
}

void Entity::addComponent(ComponentBase* comp)
{
    if (this->hasComponent(comp->getId()))
    {
        removeComponent(comp->getId());
    }
    this->hasComponentVector[(int)comp->getId()] = true;
    this->componentVector[(int)comp->getId()] = comp;
}

void Entity::removeComponent(Component comp)
{
    if (this->hasComponent(comp))
    {
        delete (this->componentVector[(int)comp]);
        this->componentVector[(int)comp] = NULL;
        this->hasComponentVector[(int)comp] = false;
    }
}

bool Entity::hasComponent(Component comp)
{
    return this->hasComponentVector[(int)comp];
}

ComponentBase* Entity::getComponent(Component comp)
{
    return this->componentVector[(int)comp];
}

World::World()
{
    this->qtdeEntity = 0;

    this->entityList.initialize();
    this->entityList.setName("EntityList");

    mapOffset.x = mapOffset.y = 0;
    map.w = map.h = 20;
}
World::~World()
{
//    printf("Deletando World...\n");
    this->entityList.clearAndFree();
}

Entity* World::createEntity()
{
    this->qtdeEntity++;
//    this->entity = (Entity**) realloc(this->entity, this->qtdeEntity*sizeof(Entity*));
//    this->entity[this->qtdeEntity-1] = new Entity();
    Entity* ent = new Entity();
    this->entityList.add(ent);
    return ent;
}
Entity* World::insertEntity(Entity* ent)
{
    this->qtdeEntity++;
//    this->entity = (Entity**) realloc(this->entity, this->qtdeEntity*sizeof(Entity*));
//    this->entity[this->qtdeEntity-1] = new Entity();
    this->entityList.add(ent);
    return ent;
}

int World::removeEntity(int id)
{
    if (this->entityList.get(id) == NULL) return -1;
    delete (this->entityList.get(id));
    int rt = this->entityList.remove(id);
    this->qtdeEntity= this->entityList.getAmount();
    return rt;
}
int World::removeEntity(Entity* ent)
{
    int rt = this->entityList.remove(ent);
    this->qtdeEntity= this->entityList.getAmount();
    return rt;
}
void World::removeLater(Entity* ent)
{
    if (this->toRemoveList.contain(ent)) return;
    this->toRemoveList.add(ent);
}
void World::removeSystem()
{
    LIterator<Entity> it(&this->toRemoveList);
    while (it.hasNext())
    {
        Entity* ent = it.next();
        this->removeEntity(ent);
        delete ent;
    }
    this->toRemoveList.clear();
}
void World::insertSystem()
{
    LIterator<Entity> it(&this->toInsertList);
    while (it.hasNext())
    {
        this->insertEntity(it.next());
    }
    this->toInsertList.clear();
}
void World::insertLater(Entity* ent)
{
    this->toInsertList.add(ent);
}

void World::runSystems(int key)
{
    LIterator<Entity> it(&this->entityList);
    while (it.hasNext())
    {
        Entity* ent = it.next();
        controllerSystem(ent, key);
        movementSystem(ent);
        collisionSystem(ent);
        bombSystem(ent);
        bombFireSystem(ent);
    }

    removeSystem();
    insertSystem();

    this->auxFps++;
    if (this->lastSec != time(NULL))
    {
        this->lastSec = time(NULL);
        this->fps = this->auxFps;
        this->auxFps = 0;
    }
}

void World::movementSystem(Entity* ent)
{
    // Checking for MOVEMENT_MASK
    if (ent->hasComponent(COMP_POSITION) &&
        ent->hasComponent(COMP_VELOCITY))
    {
        Position* position = (Position*) ent->getComponent(COMP_POSITION);
        Velocity* velocity = (Velocity*) ent->getComponent(COMP_VELOCITY);
        position->lastX = position->x;
        position->lastY = position->y;
        position->x += velocity->x;
        position->y += velocity->y;

        if (velocity->infiniteInertia == false)
        {
            if (velocity->x > 0) velocity->x -= 1;
            else if (velocity->x < 0) velocity->x += 1;
            if (velocity->y > 0) velocity->y -= 1;
            else if (velocity->y < 0) velocity->y += 1;
        }

        if (position->x < 0) position->x = 0;
        if (position->y < 0) position->y = 0;
        if (position->x >= this->map.w) position->x = this->map.w - 1;
        if (position->y >= this->map.h) position->y = this->map.h - 1;
    }

}

void World::controllerSystem(Entity* ent, int key)
{
    // Checking for COMP_CONTROLLER and MOVEMENT_MASK
    if (ent->hasComponent(COMP_CONTROLLER) &&
        (ent->hasComponent(COMP_VELOCITY)) &&
        (ent->hasComponent(COMP_POSITION)))
    {
        Controller* controller = (Controller*) ent->getComponent(COMP_CONTROLLER);
        Position* position = (Position*) ent->getComponent(COMP_POSITION);
        Velocity* velocity = (Velocity*) ent->getComponent(COMP_VELOCITY);
        if (key == controller->up)
        {
            velocity->y -= 1;
        }
        else if (key == controller->dn)
        {
            velocity->y += 1;
        }
        else if (key == controller->lt)
        {
            velocity->x -= 1;
        }
        else if (key == controller->rt)
        {
            velocity->x += 1;
        }
        else if (key == controller->bomb)
        {
            Player* player = (Player*) ent->getComponent(COMP_PLAYER);
            if (player != NULL)
                this->createBomb(position, player);
        }

        // checking for COMP_HERO
        if (ent->hasComponent(COMP_HERO))
        {
            if (key == 'j') mapOffset.x++;
            if (key == 'l') mapOffset.x--;
            if (key == 'i') mapOffset.y++;
            if (key == 'k') mapOffset.y--;

            if (position->renderX < 10) mapOffset.x ++;
            if (position->renderX > BORDER_W - 10 ) mapOffset.x --;

            if (position->renderY < 10) mapOffset.y ++;
            if (position->renderY > BORDER_H - 10 ) mapOffset.y --;

//                printf("%d x %d\n", mapOffset.x, mapOffset.y);
            if (mapOffset.x > 1) mapOffset.x = 1;
            if (mapOffset.y > 1) mapOffset.y = 1;
        }
    }
    else if (ent->hasComponent(COMP_AI))
    if (ent->hasComponent(COMP_VELOCITY))
    if (ent->hasComponent(COMP_POSITION))
    {
        AI* ai = (AI*) ent->getComponent(COMP_AI);
//            Position* position = (Position*) ent->getComponent(COMP_POSITION);
        Velocity* velocity = (Velocity*) ent->getComponent(COMP_VELOCITY);

        if (ai->lastMove == 0)
        {
            ai->lastMove = ai->waitFrames+1;
            if (rand()%3 == 0)
            {
                switch (rand()%4)
                {
                    case 0:
                        velocity->x -= 1;
                        break;
                    case 1:
                        velocity->x += 1;
                        break;
                    case 2:
                        velocity->y -= 1;
                        break;
                    case 3:
                        velocity->y += 1;
                        break;
                }
            }
            else switch (rand()%4)
            {
                case 0:
                    velocity->x -= 1;
                    break;
                case 1:
                    velocity->x += 1;
                    break;
                case 2:
                    velocity->y -= 1;
                    break;
                case 3:
                    velocity->y += 1;
                    break;
            }
        }
        ai->lastMove--;
    }

}

void World::render()
{
    char screen[BORDER_W*BORDER_H+1];
    memset(screen, ' ', BORDER_W*BORDER_H);
    screen[BORDER_H*BORDER_W] = '\0';
    Entity* ent;

    LIterator<Entity> it(&this->entityList);
    while (it.hasNext())
    {
        ent = it.next();
        int renderPosX;
        int renderPosY;
        int renderPos;
        // Checking for (COMP_POSITION | COMP_IMAGE | COMP_RECT)
        if (ent->hasComponent(COMP_RECT))
        {
            if (ent->hasComponent(COMP_POSITION))
            if (ent->hasComponent(COMP_IMAGE))
            {
                Position* position = (Position*) ent->getComponent(COMP_POSITION);
                Rect* rect = (Rect*) ent->getComponent(COMP_RECT);
                Image* image = (Image*) ent->getComponent(COMP_IMAGE);

                for (int x = 0; x < rect->w; x++)
                {
                    for (int y = 0; y < rect->h; y++)
                    {
                        renderPosX = (position->x + x + mapOffset.x);
                        renderPosY = (position->y + y + mapOffset.y);
                        renderPos = renderPosX + renderPosY*BORDER_W;
                        if (renderPosX < BORDER_W && renderPosX >= 0)
                        if (renderPosY < BORDER_H && renderPosY >= 0)
                        if (renderPos >= 0 && renderPos < (BORDER_H*BORDER_W))
                        {
                            screen[renderPos] = image->getLetter();
                        }
                    }
                }
            }
        }
        else if (ent->hasComponent(COMP_POSITION) &&
                 ent->hasComponent(COMP_IMAGE))
        {
            Position* position = (Position*) ent->getComponent(COMP_POSITION);
            Image* image = (Image*) ent->getComponent(COMP_IMAGE);

            renderPosX = position->x + mapOffset.x;
            renderPosY = position->y + mapOffset.y;

            renderPos = renderPosX + renderPosY*BORDER_W;
            if (renderPosX < BORDER_W && renderPosX >= 0)
            if (renderPosY < BORDER_H && renderPosY >= 0)
            if (renderPos >= 0 && renderPos < (BORDER_H*BORDER_W))
            {
                screen[renderPos] = image->getLetter();
            }

            position->renderX = renderPosX;
            position->renderY = renderPosY;
        }
    }

    system("cls");
    Entity* hero = this->get(0);
    Player* player = (Player*) hero->getComponent(COMP_PLAYER);
    printf("%d fps | %d ents | f:%d b:%d\n", this->fps, this->qtdeEntity, player->bombPower, player->bombAmount);
    for (int i = 0; i < BORDER_W; i++)
        printf("-");
    if (BORDER_W == 80) printf(screen);
    else
    {
        printf("--\n");
        char lineScreen[BORDER_W+1];
        memset(lineScreen, 0, sizeof(lineScreen));
        for (int i = 0; i < BORDER_H; i++)
        {
            for (int off = 0; off < BORDER_W; off++)
            {
                lineScreen[off] = screen[off + i*BORDER_W];
            }
            printf("|%s|\n", lineScreen);
        }
    }
    for (int i = 0; i < BORDER_W; i++)
        printf("-");
    if (BORDER_W < 80) printf("--\n");

return;
    int i=0;
    it.resetIterator();
    while (it.hasNext())
    {
        printf("%d:\n", i++);
        Entity* ent = it.next();
        for (int i = 0; i < COMP_AMOUNT; i++)
        {
            if (ent->hasComponent((Component)i))
            {
                ComponentBase* comp = ent->getComponent((Component)i);
                printf("%s\n", comp->getString());
            }
        }
    }
}

void World::collisionSystem(Entity* ent1)
{
    // checking for (COMP_AABB | COMP_POSITION))
    if (ent1->hasComponent(COMP_AABB) && ent1->hasComponent(COMP_POSITION))
    {
        AABB* aabb1 = (AABB*) ent1->getComponent(COMP_AABB);
        Position* position1 = (Position*) ent1->getComponent(COMP_POSITION);

        LIterator<Entity> it2(&this->entityList);
        bool canStart = false;
        Entity* ent2;
        while (it2.hasNext())
        {
            ent2 = it2.next();
            if (ent2->hasComponent(COMP_AABB) && ent2->hasComponent(COMP_POSITION) && canStart)
            {
                AABB* aabb2 = (AABB*) ent2->getComponent(COMP_AABB);
                Position* position2 = (Position*) ent2->getComponent(COMP_POSITION);
                if (!(  (position2->x > position1->x + aabb1->w - 1)||
                        (position2->x < position1->x - aabb2->w + 1)||
                        (position2->y > position1->y + aabb1->h - 1)||
                        (position2->y < position1->y - aabb2->h + 1)))
                    {       /** Colisão ocorreu **/

//                            if (ent1->hasComponent(COMP_IMAGE) && ent2->hasComponent(COMP_IMAGE))
//                            {
//                                Image* image1 = (Image*) ent1->getComponent(COMP_IMAGE);
//                                Image* image2 = (Image*) ent2->getComponent(COMP_IMAGE);
//                                printf(" %c - %c\n", image1->letter, image2->letter);
//                            }
                        this->collided(ent1, ent2);


                    }
            }

            if (!canStart) // Só começa a testar a partir da entidade que vier depois de ent1 na entityList;
            {
                if (ent1 == ent2) canStart = true;
            }
        }
    }
}

void World::collided(Entity* ent1, Entity* ent2)
{
    AABB* aabb1 = (AABB*) ent1->getComponent(COMP_AABB);
    AABB* aabb2 = (AABB*) ent2->getComponent(COMP_AABB);

    if (ent1->hasComponent(COMP_POSITION) && ent1->hasComponent(COMP_VELOCITY) && aabb2->barrier)
    {
        Position* position = (Position*) ent1->getComponent(COMP_POSITION);
        position->x = position->lastX;
        position->y = position->lastY;
    }
    if (ent2->hasComponent(COMP_POSITION) && ent2->hasComponent(COMP_VELOCITY) && aabb1->barrier)
    {
        Position* position = (Position*) ent2->getComponent(COMP_POSITION);
        position->x = position->lastX;
        position->y = position->lastY;
    }

    if (ent1->hasComponent(COMP_BOMB_KICKER))
    {
        if (ent2->hasComponent(COMP_BOMB))
        {
            Position* playerPosition = (Position*) ent1->getComponent(COMP_POSITION);
            Position* bombPosition = (Position*) ent2->getComponent(COMP_POSITION);
            int x = 0, y = 0;
            if (playerPosition->x > bombPosition->x)
            {
                x = -1;
            }
            else if (playerPosition->x < bombPosition->x)
            {
                x = 1;
            }
            else if (playerPosition->y > bombPosition->y)
            {
                y = -1;
            }
            else if (playerPosition->y < bombPosition->y)
            {
                y = 1;
            }

            ent2->addComponent(new Velocity(x, y, true));
        }
    }

    if (ent1->hasComponent(COMP_BOMB))
    {
        if (ent2->hasComponent(COMP_POWER_UP))
        {
            removeLater(ent2);
        }
        else
        {
            ent1->removeComponent(COMP_VELOCITY);
        }
    }
    if (ent2->hasComponent(COMP_BOMB))
    {
        if (ent1->hasComponent(COMP_POWER_UP))
        {
            removeLater(ent1);
        }
        if (ent1->hasComponent(COMP_PLAYER))
        {

        }
        else
        {
            ent2->removeComponent(COMP_VELOCITY);
        }
    }

    if (ent2->hasComponent(COMP_POWER_UP))
    {
        powerUpToEntity(ent1, ent2);
    }

    analiseBombSystem(ent1, ent2);
    analiseBombSystem(ent2, ent1);
}

void World::analiseBombSystem(Entity* ent1, Entity* ent2)
{
    if (ent1->hasComponent(COMP_BOMBFIRE))
    {
        if (ent2->hasComponent(COMP_BOMB))
        {
            Bomb* bomb = (Bomb*) ent2->getComponent(COMP_BOMB);
            bomb->timeToExplode = 0;
        }
        else if (ent2->hasComponent(COMP_SOFT_WALL))
        {
            int oddOfPowerUp = 30; // %
            if (rand()%100 < oddOfPowerUp)
            {
                Position* position = (Position*) ent2->getComponent(COMP_POSITION);
                Entity* entity = new Entity();
                entity->addComponent(new Position(position->x, position->y));
                entity->addComponent(new AABB(1, 1, false));
                PowerUpType type = (PowerUpType)(rand()%P_Amount);
                entity->addComponent(new PowerUp(type));
                this->insertLater(entity);
                switch (type)
                {
                    case P_ExtraBomb:
                        entity->addComponent(new Image("O+"));
                        break;
                    case P_Fire:
                        entity->addComponent(new Image("f+"));
                        break;
                    case P_KickBomb:
                        entity->addComponent(new Image("kK"));
                        break;
                    default:
                        break;
                }
            }


            removeLater(ent2);
        }
        else if (ent2->hasComponent(COMP_POWER_UP))
        {
            removeLater(ent2);
        }
        removeLater(ent1);
    }
}

void World::powerUpToEntity(Entity* ent, Entity* powerUp)
{
    if (ent->hasComponent(COMP_PLAYER))
    {
        Player* player = (Player*) ent->getComponent(COMP_PLAYER);
        if (powerUp->hasComponent(COMP_POWER_UP))
        {
            PowerUpType type = ((PowerUp*)powerUp->getComponent(COMP_POWER_UP))->type;
            switch (type)
            {
                case P_ExtraBomb:
                    player->bombAmount++;
                    break;
                case P_Fire:
                    player->bombPower++;
                    break;
                case P_KickBomb:
                    ent->addComponent(new BombKicker);
                    break;
                default:
                #warning revise this
                    break;
            }
        }
        this->removeLater(powerUp);
    }
}

void World::bombSystem(Entity* ent)
{
    if (ent->hasComponent(COMP_BOMB))
    {
        Bomb *bomb = (Bomb*) ent->getComponent(COMP_BOMB);
        Position* position = (Position*) ent->getComponent(COMP_POSITION);
        bomb->timeToExplode--;
        if (bomb->timeToExplode < 0)
        {
            bomb->owner->bombsPlaced--;
            this->removeLater(ent);
            printf("%c", 7);

            this->insertLater(this->createBombFire(position, bomb->power, DIR_DOWN));
            this->insertLater(this->createBombFire(position, bomb->power, DIR_UP));
            this->insertLater(this->createBombFire(position, bomb->power, DIR_LEFT));
            this->insertLater(this->createBombFire(position, bomb->power, DIR_RIGHT));
        }
    }

}

void World::bombFireSystem(Entity* ent)
{
    if (this->toRemoveList.contain(ent)) return;
    if (ent->hasComponent(COMP_BOMBFIRE) &&
        ent->hasComponent(COMP_POSITION))
    {
        Position* position = (Position*) ent->getComponent(COMP_POSITION);
        BombFire* bFire = (BombFire*) ent->getComponent(COMP_BOMBFIRE);
        bFire->timeToVanish--;

        if (bFire->power > 0 && !bFire->madeChild)
        {
            this->insertLater(this->createBombFire(position, bFire->power - 1, bFire->direction));
            bFire->madeChild = true;
        }

        if (bFire->timeToVanish < 0)
        {
            this->removeLater(ent);
        }
    }
}

Entity* World::createBomb(Position* playerPosition, Player* player)
{
    if (player->bombsPlaced == player->bombAmount) return NULL;
    player->bombsPlaced++;

    Entity* bomb = this->createEntity();
    bomb->addComponent(new Position(playerPosition->x, playerPosition->y));
    bomb->addComponent(new Bomb(player));
    bomb->addComponent(new AABB(1, 1));
    bomb->addComponent(new Image("O0"));



    return bomb;
}
Entity* World::createBombFire(Position* position, int power, Direction dir)
{
    Entity* bombFire = new Entity();
//    Entity* bombFire = this->createEntity();
    Position* newPos = new Position(position->x, position->y);
    newPos->lastX = position->x;
    newPos->lastY = position->y;
    bombFire->addComponent(newPos);
    bombFire->addComponent(new Image("\\|/-"));
    ((Image*)bombFire->getComponent(COMP_IMAGE))->velToChange = 0;
    bombFire->addComponent(new BombFire(dir, power));
    bombFire->addComponent(new AABB(1, 1));
    switch (dir)
    {
        case DIR_DOWN:
            newPos->y++;
            break;
        case DIR_UP:
            newPos->y--;
            break;
        case DIR_LEFT:
            newPos->x--;
            break;
        case DIR_RIGHT:
            newPos->x++;
            break;
    }
    return bombFire;
}

Map::Map()
{
    this->world = NULL;
}
World* Map::getWorld()
{
    return (this->world);
}

