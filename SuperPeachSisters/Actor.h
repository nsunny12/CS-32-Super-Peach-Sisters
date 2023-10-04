#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

class Actor: public GraphObject
{
private:
    StudentWorld* m_wrld;
    bool m_alive=true;
public:
    Actor(int imageID, int startX, int startY, int dir, int depth, double size, StudentWorld* world);
    virtual void doSomething()=0;
    virtual bool setAlive(bool alive);
    virtual bool getAliveStat();
    virtual void bonk();
    virtual bool isDamageable();
    virtual void damaged();
    virtual bool canBlock();
    virtual bool isAnEnemy();
    StudentWorld* getWorld();
    virtual ~Actor()=0;
};

class Peach: public Actor{
private:
    bool m_reached_mario=false;
    bool m_completed_level=false;
    bool m_jump_initiated=false;
    int m_ticks_starPower=150; 
    int m_ticks_tempInvince=10;
    int remaining_jump_distance=0;
    int time_to_recharge_before_next_fire=0;
    bool m_StarPower;
    bool m_JumpPower;
    bool m_ShootPower;
    bool m_temp_invince;
    int m_hit_points=1;
public:
    Peach(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    void temporarilyInvincible();
    virtual void bonk();
    
    bool setStarPower(bool star){
           m_StarPower=star;
           return m_StarPower;
    }
       bool hasStarPower(){
           return m_StarPower;
    }
    
    bool setShootPower(bool shoot){
        m_ShootPower=shoot;
        return m_ShootPower;
    }
    bool hasShootPower(){
        return m_ShootPower;
    }
    bool setJumpPower(bool jump){
        m_JumpPower=jump;
        return m_JumpPower;
    }
    bool hasJumpPower(){
        return m_JumpPower;
    }
    
    void initiateJump();
    virtual bool isDamageable(){
        return true;
    }
    
    virtual void damaged(){
        bonk();
    }
    int hitPoints(int x){
        m_hit_points=x;
        return m_hit_points;
    }
    virtual ~Peach(){}
    
};
//BLOCK AND PIPE
//OBSTACLE
class Obstacle: public Actor{
public:
    Obstacle(int image, int startX, int startY, StudentWorld* world): Actor(image, startX, startY, 0, 2, 1, world){
    }
    virtual bool canBlock(){
        return true;
    }
    virtual void doSomething(){}
    virtual void bonk();
    virtual ~Obstacle()=0;
};

class Block: public Obstacle{
private:
    bool m_hold;
    bool m_released;
    bool m_hasFlower;
    bool m_hasMushroom;
    bool m_hasStar;
    int m_goodie_type;
public:
    enum Goodie{none, flower, mushroom, star};
    Block(int startX, int startY, StudentWorld* world, Goodie type);
    
    virtual void bonk();
    bool setGoodie(bool x){
        m_hold=x;
        return m_hold;
    }
    
    bool holdGoodie(){
        return m_hold;
    }
    bool setBeenBonked(bool x){
        m_released=x;
        return m_released;
    }
    bool beenBonked(){
        return m_released;
    }
    virtual ~Block(){}
};

class Pipe: public Obstacle{
public:
    Pipe(int startX, int startY, StudentWorld* world);
    virtual ~Pipe(){}
};
//END OF OBSTACLE

//CLASS ENEMY
class Enemy:public Actor{
private:
    int m_Koopa_damge_activate;
public:
    Enemy(int image, int startX, int startY, StudentWorld* world, int koopaDamage):Actor(image, startX, startY, randInt(0,1)*180, 1, 0, world), m_Koopa_damge_activate(koopaDamage){}
    virtual void doSomething();
    virtual void bonk();
    virtual void damaged();
    virtual bool isAnEnemy(){
        return true;
    }
    virtual bool isDamageable(){
        return true;
    }
    virtual ~Enemy()=0;
};

class Goomba: public Enemy{
public:
    Goomba(int startX, int startY, StudentWorld* world);
    virtual ~Goomba(){}
};

class Koopa: public Enemy{
public:
    Koopa(int startX, int startY, StudentWorld* world);
    virtual ~Koopa(){}
};

class Piranha: public Enemy{
private:
    int m_firing_delay;
public:
    Piranha(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual ~Piranha(){}
};
//END OF ENEMY

//LEVELENDER
class LevelEnder:public Actor{ //rename
private:
    int m_reachedMario;
public:
    LevelEnder(int image, int startX, int startY, StudentWorld* world, int reachMario):Actor(image, startX, startY, 0, 1, 1, world), m_reachedMario(reachMario){
    }
    virtual void doSomething();
    virtual ~LevelEnder()=0;
};

class Flag: public LevelEnder{
private:
    bool m_complete=false;
    bool m_finished=false;
public:
    Flag(int startX, int startY, StudentWorld* world);
    virtual ~Flag(){}
};

class Mario: public LevelEnder{
public:
    Mario(int startX, int startY, StudentWorld* world);
    virtual ~Mario(){}
};
//END OF LEVELENDER

//GOODIES
class Goodies: public Actor{
private:
    int m_increaseScore;
    int m_decodePow;
    int m_hitPoints;
public:
    Goodies(int image, int startX, int startY, StudentWorld* world, int increaseScore, int decodePow, int hitPoints):Actor(image, startX, startY, 0, 1, 1, world), m_increaseScore(increaseScore), m_decodePow(decodePow), m_hitPoints(hitPoints) {
    }
    virtual void doSomething();
    virtual ~Goodies();
};

class Flower: public Goodies{
public:
    Flower(int startX, int startY, StudentWorld* world);
    virtual ~Flower(){}
};

class Mushroom: public Goodies{
public:
    Mushroom(int startX, int startY, StudentWorld* world);
    virtual ~Mushroom(){}
};

class Star: public Goodies{
public:
    Star(int startX, int startY, StudentWorld* world);
    virtual ~Star(){}
};
//END OF GOODIES

//PROJECTILES
class Projectiles:public Actor{
private:
    int m_piranha_ds;
public:
    Projectiles(int image, int startX, int startY, int dir, StudentWorld* world, int piranhaDS):Actor(image, startX, startY, dir, 1, 1, world), m_piranha_ds(piranhaDS){}
    virtual bool isAnEnemy(){
        return false;
    }
    virtual void doSomething();
    virtual ~Projectiles()=0;
};

class PiranhaFireball: public Projectiles{
private:
    int m_firing_delay;
public:
    PiranhaFireball(int startX, int startY, int dir, StudentWorld* world);
    virtual bool isAnEnemy(){
        return true;
    }
    virtual ~PiranhaFireball(){}
};

class PeachFireball: public Projectiles{
public:
    PeachFireball(int startX, int startY, int dir, StudentWorld* world);
    virtual ~PeachFireball(){}
};

class Shell:public Projectiles{
public:
    Shell(int startX, int startY, int dir, StudentWorld* world);
    virtual ~Shell(){}
};
#endif // ACTOR_H_
