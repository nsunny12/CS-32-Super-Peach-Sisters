#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Peach;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  virtual ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
    
    bool setCompletedLevel(bool com){
        m_complete=com;
        return m_complete;
    }
    bool completedLevel(){
        return m_complete;
    }
    bool setReachedMario(bool x){
        m_finished=x;
        return m_finished;
    }
    bool reachedMario(){
        return m_finished;
    }
    bool setOverlapEnemy(bool x){
            enemy=x;
            return enemy;
    }
    bool OverlapEnemy(){
            return enemy;
    }
    
    void addActor(Actor* actor);
    
    void decodePower(int power);
    int checkPower(int power);
    
    //seperate for x and y
    int xCoordOverlap(int x); //0 for right, 1 for left
    int yCoordOverlap(int y);
    int howFarAwayisPeach(int x);
    
    void bonkObject(int x, int y, Actor* a);
    bool basicOverlap(int x, int y, int a, int b);
    bool overlapPeach(int x, int y);
    bool passOverlapPeach(int x, int y, Actor* a);
    bool checkDamagableOverlap(int x, int y);
    int PeachHitPts(int x);
    
    
private:
    Peach* m_peach;
    std::list<Actor*> m_actors;
    bool m_complete=false;
    bool m_finished=false;
    
    bool enemy=false;
    bool blocker=false;
    
};

#endif // STUDENTWORLD_H_
