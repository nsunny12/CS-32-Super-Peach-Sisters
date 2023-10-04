#include "Actor.h"
#include "StudentWorld.h"


Actor::Actor(int imageID, int startX, int startY, int dir, int depth, double size, StudentWorld* world):GraphObject(imageID, startX, startY, dir, depth, size) {
    m_wrld=world;
}

void Actor::doSomething(){}
bool Actor::setAlive(bool alive){
    m_alive=alive;
    return m_alive;
}
bool Actor::getAliveStat(){
    return m_alive;
}
void Actor::bonk(){}
bool Actor::isDamageable(){
    return false;
}
void Actor::damaged(){}
bool Actor::canBlock(){
    return false;
}
bool Actor::isAnEnemy(){
    return false;
}
StudentWorld* Actor::getWorld(){
    return m_wrld;
}
Actor::~Actor(){}

//PEACH
Peach::Peach(int startX, int startY, StudentWorld* world):m_hit_points(1), m_StarPower(false), m_JumpPower(false), m_ShootPower(false), m_temp_invince(false), Actor(IID_PEACH, startX, startY, 0, 0, 1, world){
}

void Peach::temporarilyInvincible(){
    if(getWorld()->overlapPeach(getX(), getY()) && getWorld()->OverlapEnemy()==true && (hasJumpPower()||hasShootPower())){
        m_temp_invince=true;
        if(m_ticks_tempInvince>0){
        m_ticks_tempInvince--;
        }
        else{
            m_temp_invince=false;
            m_ticks_tempInvince=10;
        }
        
    }
}

void Peach::initiateJump(){
    if(m_jump_initiated==true && remaining_jump_distance>0){
        if(!getWorld()->overlapPeach(getX(), getY()+4)){
            moveTo(getX(), getY()+4);
            remaining_jump_distance--;
        }
        else{
            getWorld()->bonkObject(getX(), getY()+4, this);
            remaining_jump_distance=0;
            m_jump_initiated=false;
        }
    }
    else{
        int i=0; int count=0;
        while(i<=3){
        if(!getWorld()->overlapPeach(getX(), getY()-i)){
            count++;
        }
            i++;
        }
        if(count==4){
            moveTo(getX(), getY()-4);
        }
    }
}

void Peach::doSomething(){
    if(hasStarPower()){
        if(m_ticks_starPower>0){
            m_ticks_starPower--;
        }
        else{
            setStarPower(false);
            m_ticks_starPower=150;
        }
    }
    
    //check for temporary invincibility
    temporarilyInvincible();
    initiateJump();
    
    if(time_to_recharge_before_next_fire>0){
        time_to_recharge_before_next_fire --;
    }
    
   getWorld()->bonkObject(getX(), getY(), this);
    
    int ch;
    if(getWorld()->getKey(ch)){
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                setDirection(180);
                if(!getWorld()->overlapPeach(getX()-4, getY())){
                moveTo(getX()-4, getY());
                }
                else{
                   getWorld()->bonkObject(getX()-4, getY(), this);
                }
                break;
            case KEY_PRESS_RIGHT:
                setDirection(0);
                if(!getWorld()->overlapPeach(getX()+4, getY())){
                moveTo(getX()+4, getY());
                }
                else{
                    getWorld()->bonkObject(getX()+4, getY(), this);
                }
                break;
            case KEY_PRESS_UP:
                if(m_jump_initiated==false){
                    m_jump_initiated=true;
                }
                getWorld()->playSound(SOUND_PLAYER_JUMP);
                if(getWorld()->overlapPeach(getX(), getY()-1)){
                     if(!hasJumpPower()){
                    remaining_jump_distance=8;
                    }
                    else{
                        remaining_jump_distance=12;
                    }
                }
                break;
            case KEY_PRESS_SPACE:
                if(!hasShootPower() || time_to_recharge_before_next_fire>0){
                    return;
                }
                else{
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    time_to_recharge_before_next_fire=8;
                    int dir=getDirection();
                    if(dir==0){
                        getWorld()->addActor(new PeachFireball(getX(), getY(), dir, getWorld()));
                    }
                    else{
                        getWorld()->addActor(new PeachFireball(getX(), getY(), dir, getWorld()));
                    }
                }

                break;
        }
    }
}

void Peach::bonk(){
    if(hasStarPower() || m_temp_invince==true){
        return;
    }
    else{
        m_hit_points--;
        m_ticks_tempInvince=10;
            setShootPower(false);
            setJumpPower(false);
        if(m_hit_points>0){
            getWorld()->playSound(SOUND_PLAYER_HURT);
        }
        if(m_hit_points<=0){
            setAlive(false);
        }
    }
}

//PIPE
Pipe::Pipe(int startX, int startY, StudentWorld* world):Obstacle(IID_PIPE, startX, startY, world){
    
}

void Obstacle::bonk(){
    getWorld()->playSound(SOUND_PLAYER_BONK);
}

Obstacle::~Obstacle(){}

//BLOCK
Block::Block(int startX, int startY, StudentWorld* world, Goodie type):Obstacle(IID_BLOCK, startX, startY, world){
    m_goodie_type=type;
}

void Block::bonk(){
    if(m_goodie_type!=none){
        setGoodie(true);
    }
    
    if(!holdGoodie() || beenBonked()){
        getWorld()->playSound(SOUND_PLAYER_BONK);
    }
    else{
        getWorld()->playSound(SOUND_POWERUP_APPEARS);
        if(m_goodie_type==flower){
            getWorld()->addActor(new Flower(getX(), getY()+8, getWorld()));
        }
        if(m_goodie_type==mushroom){
            getWorld()->addActor(new Mushroom(getX(), getY()+8, getWorld()));
        }
        if(m_goodie_type==star){
            getWorld()->addActor(new Star(getX(), getY()+8, getWorld()));
        }
        setBeenBonked(true);
        }
}
//Enemy
void Enemy::doSomething(){
    if(!getAliveStat()){
        return;
    }
    if(getWorld()->passOverlapPeach(getX(), getY(), this)){
        //bonk peach
        return;
    }
    bool inside=false;
    if(getDirection()==0){
        if(getWorld()->overlapPeach(getX()+1, getY())){ //blocks in x dir
            inside=true;
            setDirection(180);
            
        }
        else{
            if(!getWorld()->overlapPeach(getX()+(1*SPRITE_WIDTH), getY()-1)){ //no block underneath
                inside=true;
                setDirection(180);
            }
        }
    }
    if(inside!=true && getDirection()==180){
        if(getWorld()->overlapPeach(getX()-1, getY())){
            setDirection(0);
        }
        else{
            if(!getWorld()->overlapPeach(getX()-(1*SPRITE_WIDTH), getY()-1)){
                setDirection(0);
            }
        }
    }
    
    if(getDirection()==0){
        if(getWorld()->overlapPeach(getX()+1, getY())){
            return;
        }
        else{
            if(getWorld()->overlapPeach(getX()+(1*SPRITE_WIDTH), getY()-1)){
                moveTo(getX()+1, getY());
            }
    }
    }
    else{
        if(getWorld()->overlapPeach(getX()-1, getY())){
            return;
        }
        else{
            if(getWorld()->overlapPeach(getX()-(1*SPRITE_WIDTH), getY()-1)){
                moveTo(getX()-1, getY());
            }
        }
    }
}

void Enemy::bonk(){
    if(getWorld()->checkPower(3)==3){
        getWorld()->playSound(SOUND_PLAYER_KICK);
        getWorld()->increaseScore(100);
        setAlive(false);
        
    }
}

void Enemy::damaged(){
    getWorld()->increaseScore(100);
    setAlive(false);
    if(m_Koopa_damge_activate==1){
        int dir=getDirection();
        getWorld()->addActor(new Shell(getX(), getY(), dir, getWorld()));
    }
}

Enemy::~Enemy(){}
//GOOMBA
Goomba::Goomba(int startX, int startY, StudentWorld* world): Enemy(IID_GOOMBA, startX, startY, world, 0){
}

//Koopa
Koopa::Koopa(int startX, int startY, StudentWorld* world): Enemy(IID_KOOPA, startX, startY, world, 1){
}

Piranha::Piranha(int startX, int startY, StudentWorld* world):Enemy(IID_PIRANHA, startX, startY, world, 0), m_firing_delay(0){
}

void Piranha::doSomething(){
    if(getAliveStat()==false){
        return;
    }
    increaseAnimationNumber();

    if(getWorld()->passOverlapPeach(getX(), getY(), this)){
        return;
    }
    if(getWorld()->yCoordOverlap(getY())==0){
        return;
    }
    else{
        setDirection(getWorld()->howFarAwayisPeach(getX())*180);
        if(m_firing_delay>0){
            m_firing_delay--;
            return;
        }
        else{
            if(getWorld()->xCoordOverlap(getX())==1){
                int dir=getDirection();
                getWorld()->addActor(new PiranhaFireball(getX(), getY(), dir, getWorld()));
                getWorld()->playSound(SOUND_PIRANHA_FIRE);
                m_firing_delay=40;
            }
        }
    }
}

//END OF ENEMY

//LEVEL ENDER
void LevelEnder::doSomething(){
    if(getAliveStat()==false){
        return;
    }
    else{
        if(getWorld()->passOverlapPeach(getX(),getY(),this)){
            getWorld()->increaseScore(1000);
            setAlive(false);
            if(m_reachedMario==1){
                getWorld()->setReachedMario(true);
            }
            else{
            getWorld()->setCompletedLevel(true);
            }
        }
    }
}

LevelEnder::~LevelEnder(){}

//FLAG
Flag::Flag(int startX, int startY, StudentWorld* world):LevelEnder(IID_FLAG, startX, startY, world,0){
}


Mario::Mario(int startX, int startY, StudentWorld* world):LevelEnder(IID_MARIO, startX, startY, world,1){
}

//LEVEL ENDER

//GOODIE
void Goodies::doSomething(){
    if(getWorld()->passOverlapPeach(getX(),getY(),this)){ //check if i need to check if its alive or not
        getWorld()->increaseScore(m_increaseScore); //diferences in score and power
        getWorld()->decodePower(m_decodePow);
        getWorld()->PeachHitPts(m_hitPoints);
        setAlive(false);
        getWorld()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }
    else{
        if(!getWorld()->overlapPeach(getX(), getY()-2)){
            moveTo(getX(), getY()-2);
        }
    }
        if(getDirection()==0){
            if(getWorld()->overlapPeach(getX()+2, getY())){
                setDirection(180);
                return;
            }
            else{
                moveTo(getX()+2, getY());
            }
        }
        if(getDirection()==180){ //fix
            if(getWorld()->overlapPeach(getX()-2, getY())){
                setDirection(0);
                return;
            }
            else{
                moveTo(getX()-2, getY());
            }
        }
}

Goodies::~Goodies(){}
//FLOWER
Flower::Flower(int startX, int startY, StudentWorld* world):Goodies(IID_FLOWER, startX, startY, world, 50, 1, 2){
}


//mushroom
Mushroom::Mushroom(int startX, int startY, StudentWorld* world):Goodies(IID_MUSHROOM, startX, startY, world, 75, 2, 2){
}


//star
Star::Star(int startX, int startY, StudentWorld* world):Goodies(IID_STAR, startX, startY, world, 100, 3, 0){
}
//END OF GOODIES

//PROJECTILES
void Projectiles::doSomething(){
    bool firstcheck=false;
    if(m_piranha_ds==0){
        if(getWorld()->checkDamagableOverlap(getX(), getY())){
            firstcheck=true;
            setAlive(false);
            return;
        }
    }
    else{
        if(getWorld()->passOverlapPeach(getX(), getY(), this)){
            firstcheck=true;
            setAlive(false);
            return;
        }
    }
    
    if(firstcheck==false){
        if(!getWorld()->overlapPeach(getX(), getY()-1)){ //is this check right
            moveTo(getX(), getY()-2);
        }
    }
    
    int dir=getDirection();
    if(dir==0){
        if(getWorld()->overlapPeach(getX()+2, getY())){
            setAlive(false);
            return;
        }
        else{
            moveTo(getX()+2, getY());
        }
    }
    if(dir==180){
        if(getWorld()->overlapPeach(getX()-2, getY())){
            setAlive(false);
            return;
        }
        else{
            moveTo(getX()-2, getY());
        }
    }
}

Projectiles::~Projectiles(){}

PiranhaFireball::PiranhaFireball(int startX, int startY, int dir, StudentWorld* world):Projectiles(IID_PIRANHA_FIRE, startX, startY, dir,world, 1){
}

//PEACHFIREBALL
PeachFireball::PeachFireball(int startX, int startY, int dir, StudentWorld* world):Projectiles(IID_PEACH_FIRE, startX, startY, dir, world, 0){
    
}

Shell::Shell(int startX, int startY, int dir, StudentWorld* world):Projectiles(IID_SHELL, startX, startY, dir, world, 0){
}
//END OF PROJECTILES

