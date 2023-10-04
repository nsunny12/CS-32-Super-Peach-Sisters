#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

#include "Actor.h"
#include <sstream>
#include <iomanip>

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    
}

int StudentWorld::init()
{
    //loads level
    int level = getLevel();
    Level lev(assetPath());
    
    ostringstream ll;
    ll << "level";
    ll.fill('0');
    ll << setw(2) << level << ".txt";
    
    string level_file = ll.str();
    Level::LoadResult result = lev.loadLevel(level_file);
    
    if(result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format){
        return GWSTATUS_LEVEL_ERROR;
    }
    else if(result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        //populates game with actors
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
        Level::GridEntry ge;
        ge = lev.getContentsOf(i, j); // x=5, y=10
        switch (ge)
        {
            case Level::peach:
                m_peach=new Peach(i*SPRITE_WIDTH, j*SPRITE_HEIGHT,this);
                break;
            case Level::block:
                m_actors.push_back(new Block(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this, Block::Goodie::none));
                break;
            case Level::pipe:
                m_actors.push_back(new Pipe(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
                break;
            case Level::flag:
                m_actors.push_back(new Flag(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
                break;
            case Level::mario:
                m_actors.push_back(new Mario(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
                break;
            case Level::goomba:
                m_actors.push_back(new Goomba(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
                break;
            case Level::koopa:
                m_actors.push_back(new Koopa(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
                break;
            case Level::flower_goodie_block:
                m_actors.push_back(new Block(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this, Block::Goodie::flower));
                break;
            case Level::mushroom_goodie_block:
                m_actors.push_back(new Block(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this, Block::Goodie::mushroom));
                break;
            case Level::star_goodie_block:
                m_actors.push_back(new Block(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this, Block::Goodie::star));
                break;
            case Level::piranha:
                m_actors.push_back(new Piranha(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
                break;
            case Level::empty:
                break;
            } 
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

//adds pointers to actors to list
void StudentWorld::addActor(Actor* actor){
    m_actors.push_back(actor); //is this right
}

int StudentWorld::move()
{
    
        //tells peach to do something
       int size = m_actors.size();
        m_peach->doSomething(); //should i check if this is alive

        // checks if peach reached any milestones
        list<Actor*>::iterator it = m_actors.begin();
    for (int i = 0; i<size+1; i++) { //double check
            if(!m_peach->getAliveStat()) {
                decLives();
                this->playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
            if(reachedMario()){ //make in student world
                this->playSound(SOUND_GAME_OVER);
                return GWSTATUS_PLAYER_WON;
            }
            if(completedLevel()){
                this->playSound(SOUND_FINISHED_LEVEL);
                setCompletedLevel(false);
                return GWSTATUS_FINISHED_LEVEL;
            }
            if(i == size)
                break;
            
            //tells remaining actors to do somthing
            if((*it)->getAliveStat()){
                (*it)->doSomething();
            }
            it++;
        }

    //Remove dead actors
    list<Actor*>::iterator p = m_actors.begin();
    while(p!=m_actors.end()){
        if((*p)->getAliveStat()==false){  //bad access
        delete *p;
        p=m_actors.erase(p);
    }
        else{
            p++;
        }
    }
    
    //update status on the text
    ostringstream status;
    status  << "Lives: " << setw(1) << setfill('0') << internal << getLives(); //is this the right setfills?
    status << "  Level: " << setw(2) << setfill('0') << internal << getLevel();
    status << "  Points: "<< setw(6) << setfill('0') << internal << getScore(); //is this right
    if(m_peach->hasShootPower()){
     status  << " ShootPower!";
    }
    if(m_peach->hasJumpPower()){
    status << " JumpPower!";
    }
    if(m_peach->hasStarPower()){
     status  << " StarPower!";
    }
    setGameStatText(status.str());
    
    return GWSTATUS_CONTINUE_GAME; //not sure
}

//basic function that checks if there an overlap between actors
bool StudentWorld::basicOverlap(int x, int y, int a, int b){
    int px=x; int py=y;
    int xx; int yy;
    int brx; int bry;
    int trx; int tryy;
    int tlx; int tly;
            xx=a; yy=b;
            brx=xx+SPRITE_WIDTH-1; bry=yy;
            trx=xx+SPRITE_WIDTH-1; tryy=yy+SPRITE_HEIGHT-1;
            tlx=xx; tly=yy+SPRITE_HEIGHT-1;
            //top right
            if((px<=xx && xx<=px+SPRITE_WIDTH-1) && (py<=yy && yy<=py+SPRITE_HEIGHT-1)){
                return true;
            }
            //top left
            if((px<=brx && brx<=px+SPRITE_WIDTH-1) && (py<=bry && bry<=py+SPRITE_HEIGHT-1)){
                return true;
            }
            //bottom left
            if((px<=trx && trx<=px+SPRITE_WIDTH-1) && (py<=tryy && tryy<=py+SPRITE_HEIGHT-1)){
                return true;
            }
            //bottom right
            if((px<=tlx && tlx<=px+SPRITE_WIDTH-1) && (py<=tly && tly<=py+SPRITE_HEIGHT-1)){
                return true;
            }
    return false;
}

//checks to see if there is overlap with an actor that can be damaged
bool StudentWorld::checkDamagableOverlap(int x, int y){
    int xx; int yy;
    list<Actor*>::iterator p;
    for (p = m_actors.begin(); p!= m_actors.end(); p++){
        if((*p)->isDamageable() && (*p)->getAliveStat()){
            xx=(*p)->getX(); yy=(*p)->getY();
            if(basicOverlap(x, y, xx, yy)){
                (*p)->damaged();
                return true;
            }
        }
    }
    return false;
}

//returns true if overlaps with an actor that blocks other actors
bool StudentWorld::overlapPeach(int x, int y){ //blocks and pipes
    //iterate through the container
    int xx; int yy;
    list<Actor*>::iterator p = m_actors.begin();
    for (p; p!= m_actors.end(); p++){
        if((*p)->canBlock()){
            xx=(*p)->getX(); yy=(*p)->getY();
            if(basicOverlap(x, y, xx, yy)){
                return true;
            }
        }
    }
    return false;
}

//checks for an overlap other actors and peach
bool StudentWorld::passOverlapPeach(int x, int y, Actor* a){
    int px=m_peach->getX(); int py=m_peach->getY();
    if(a->isAnEnemy()){
        setOverlapEnemy(true);
    }
    if(basicOverlap(px, py, x, y)){
        return true;
    }
    else{
        return false;
    }
}

//sets peach's powers
void StudentWorld::decodePower(int power){
    switch(power){
        case 1: //flower
            m_peach->setShootPower(true);
            break;
        case 2: //mushroom
            m_peach->setJumpPower(true);
            break;
        case 3: //star
            m_peach->setStarPower(true);
            break;
    }
}

//checks what power peach currently has
int StudentWorld::checkPower(int power){
    switch(power){
        case 1:
            if(m_peach->hasShootPower()){
                return 1;
            }
            break;
        case 2:
            if(m_peach->hasJumpPower()){
                return 2;
            }
            break;
        case 3:
            if(m_peach->hasStarPower()){
                return 3;
            }
    }
    return 0;
}
//checks to see if peach is within range of a piranha 
int StudentWorld::xCoordOverlap(int x){
    if(m_peach->getX()>x-8*SPRITE_HEIGHT && m_peach->getX()<x+8*SPRITE_HEIGHT){
        return 1;
    }
    else{
        return 0;
    }
}

int StudentWorld::yCoordOverlap(int y){
    if(m_peach->getY()<=y-1.5*SPRITE_HEIGHT || m_peach->getY()>=y+1.5*SPRITE_HEIGHT){
        return 0;
    }
    else{
        return 1;
    }
}

//checks to see if peach is to the left/right of an actor 
int StudentWorld::howFarAwayisPeach(int x){
    if(m_peach->getX()<x){
        return 1;
    }
    else{
        return 0;
    }
}

void StudentWorld::bonkObject(int x, int y, Actor* a){
    list<Actor*>::iterator p = m_actors.begin();
    for (p; p!= m_actors.end(); p++){
    if(a!=(*p)){
        
       int a=(*p)->getX(); int b=(*p)->getY();
        if(basicOverlap(x, y, a, b)){
            if((*p)->isAnEnemy()){ //idk if this is right 
                m_peach->bonk();
            }
//            Actor*  temp=*p;
//            int tempx=(*p)->getX();  int tempy=(*p)->getY();
            (*p)->bonk();
        }
    }
    }
}

int StudentWorld::PeachHitPts(int x){
    return m_peach->hitPoints(x);
}

void StudentWorld::cleanUp()
{
    delete m_peach;
    list<Actor*>::iterator p = m_actors.begin();
    list<Actor*>::iterator q = m_actors.begin();
        for (p; p!= m_actors.end(); p++) {
            q=p;
            delete *q;
            m_actors.erase(q);
        }
}

StudentWorld::~StudentWorld(){
    cleanUp();
}
