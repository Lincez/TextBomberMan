#ifndef COMP_H
#define COMP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MOVEMENT_MASK   (COMP_POSITION | COMP_VELOCITY)
#define BOMB_MASK       (COMP_BOMB | COMP_POSITION | COMP_AABB | COMP_IMAGE)
#define PLAYER_MASK     (COMP_PLAYER | MOVEMENT_MASK | COMP_IMAGE | COMP_AABB)

#define L_visibility public

/** @info
    @todo Ao criar um novo component:
    >>> Criar um id unico para ele no enum Component
    >>> Fazer a classe herdar de ComponentBase
    >>> Reimplementar o metodo getId() para ele retornar o id previamente criado para ele
**/


typedef enum
{
    COMP_NONE = 0,
    COMP_POSITION,
    COMP_VELOCITY,
    COMP_IMAGE,
    COMP_AABB,
    COMP_CONTROLLER,
    COMP_AI,
    COMP_RECT,
    COMP_HERO,
    COMP_BOMB,
    COMP_BOMBFIRE,
    COMP_PLAYER,
    COMP_SOFT_WALL,
    COMP_POWER_UP,
    COMP_BOMB_KICKER,

    COMP_AMOUNT /// Sempre o ultimo
} Component;

const char* getCompName(Component comp);


class ComponentBase
{
    public:
    virtual Component getId(){return COMP_NONE;}
    virtual char* getString()
    {
        return (char*)"ComponentBase";
    }
    virtual ~ComponentBase()
    {
    }
};

typedef int dv ;

class Position: L_visibility ComponentBase
{
    public:
        Position()
        {
            x = y = lastX = lastY = renderX = renderY = 0;
        }
        Position(dv x_, dv y_) {this->x = x_; this->y=y_;}
    dv x;
    dv y;
    dv lastX;
    dv lastY;
    dv renderX;
    dv renderY;
    virtual Component getId(){return COMP_POSITION;}
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "Position: %d x %d", this->x, this->y);
        return string;
    }
};

class Velocity : L_visibility ComponentBase
{
    public:
        Velocity(dv x_ = 0, dv y_ = 0, bool infInertia = false)
        {
            this->x = x_;
            this->y = y_;
            this->infiniteInertia = infInertia;
        }
    dv x;
    dv y;
    bool infiniteInertia;
    virtual Component getId(){return COMP_VELOCITY;}
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "Velocity: %d x %d", this->x, this->y);
        return string;
    }
};

class Image : L_visibility ComponentBase
{
    public:
        Image(){letter[0] = 0; lAmount = 0; lAtual = 0;}
        Image(char l){letter[0] = l; lAmount = 1; lAtual = 0;}
        Image(const char* l)
        {
            strcpy(letter, l);
            lAmount = strlen(l);
            if (lAmount >= MAX_LETTER)
            {
                lAmount = MAX_LETTER;
                letter[MAX_LETTER] = '\0';
            }
            lAtual = 0;
            velToChange = 3;
        }
        static const int MAX_LETTER = 10;
    virtual Component getId(){return COMP_IMAGE;}
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "Image: %s", this->letter);
        return string;
    }

    char getLetter()
    {
        if (lAtual >= lAmount) lAtual = 0;
        if (toChange++ > velToChange)
        {
            toChange = 0;
            return letter[lAtual++];
        }
        else
            return letter[lAtual];
    }
    int velToChange;
    protected:
        char letter[Image::MAX_LETTER];
        int lAmount;
        int lAtual;
        int toChange;

};

class AABB: L_visibility ComponentBase
{
    public:
    AABB(){w = h = 0; this->barrier = true;}
    AABB(dv w_, dv h_, bool barrier_ = true)
    {
        this->w = w_;
        this->h = h_;
        this->barrier = barrier_;
    }
    virtual Component getId(){return COMP_AABB;}
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "AABB: %d x %d (%s)", this->w, this->h, (this->barrier?"barrier":"-"));
        return string;
    }
    dv w;
    dv h;
    bool barrier;
} ;

class Rect: L_visibility ComponentBase
{
    public:
    Rect(){w = h = 0;}
    Rect(dv w_, dv h_)
    {
        this->w = w_;
        this->h = h_;
    }
    virtual Component getId(){return COMP_RECT;}
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "Rect: %d x %d", this->w, this->h);
        return string;
    }
    dv w;
    dv h;
};

#define CONTROLLLER_ID_NONE 0
#define CONTROLLLER_ID_KEYBOARD 1
class Controller : L_visibility ComponentBase
{
    public:
    virtual Component getId(){return COMP_CONTROLLER;}
    int id;
    int up;
    int dn;
    int lt;
    int rt;
    virtual char* getString()
    {
        char* string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "Controller: %d (%c %c %c %c)", this->id,
                        this->up, this->dn,
                        this->lt, this->rt);
        return string;
    }

    int bomb;
};

class AI: L_visibility ComponentBase
{
    public:
    virtual Component getId(){return COMP_AI;}
    int lastMove;
    int waitFrames;
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "AI: lastMove: %d; waitFrames: %d", this->lastMove, this->waitFrames);
        return string;
    }
};

class Player: L_visibility ComponentBase
{
    public:
    Player()
    {
        this->bombAmount = 1;
        this->bombPower = 1;
        this->bombsPlaced = 0;
    }
    virtual Component getId(){return COMP_PLAYER;}
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "Player: bombAmount: %d; bombPower: %d", this->bombAmount, this->bombPower);
        return string;
    }
    int bombAmount;
    int bombPower;
    int bombsPlaced;
};

class Bomb: L_visibility ComponentBase
{
    public:
    Bomb(){}
    Bomb(Player* owner_)
    {
        this->timeToExplode = totalTimeToExplode;
        this->owner = owner_;
        this->power = owner->bombPower;
    }
    virtual Component getId(){return COMP_BOMB;}
    static const int totalTimeToExplode = 30;
    int timeToExplode;
    int power;
    Player* owner;
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "Bomb: toExplode: %d; Power: %d", this->timeToExplode, this->power);
        return string;
    }
};

typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;
class BombFire: L_visibility ComponentBase
{
    public:
    BombFire(Direction direction_,int power_)
    {
        this->direction = direction_;
        this->timeToVanish = totalTimeToVanish;
        this->power = power_;
        madeChild = false;
    }
    virtual Component getId(){return COMP_BOMBFIRE;}
    virtual char* getString()
    {
        char *string = (char*) calloc(sizeof(char), 128);
        sprintf(string, "BombFire: Dir: %d; toVanish: %d; Power: %d", this->direction, this->timeToVanish,
                this->power);
        return string;
    }
    static const int totalTimeToVanish = 10;

    Direction direction;
    int timeToVanish;
    int power;
    bool madeChild;
};

class Hero: L_visibility ComponentBase
{
    public:
    virtual Component getId(){return COMP_HERO;}
    virtual char* getString()
    {
        return (char*) "Hero";
    }
};

class SoftWall : L_visibility ComponentBase
{
    public:
    virtual Component getId(){return COMP_SOFT_WALL;}
    virtual char* getString()
    {
        return (char*) "Soft Wall";
    }
};

typedef enum
{
    P_ExtraBomb = 0,
    P_Fire,
    P_KickBomb,
    P_Amount
} PowerUpType;
class PowerUp : L_visibility ComponentBase
{
    public:
        PowerUp(PowerUpType type_)
        {
            this->type = type_;
        }
        virtual Component getId() {return COMP_POWER_UP;}
        PowerUpType type;
};
class BombKicker : L_visibility ComponentBase
{
    public:
        virtual Component getId() {return COMP_BOMB_KICKER;}
};

#endif
