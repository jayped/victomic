#ifndef ACTOR_H
#define ACTOR_H
#include <Ogre.h>
class Actor : public Ogre::SceneNode
{
  
public:
    enum direction {_stop, _up, _down, _right, _left};
    //Actor(){}
    Actor(Ogre::SceneManager *creator);
    Actor(Ogre::SceneManager *creator, const Ogre::String & name);
    ~Actor();
    //por defecto, la llamada a init sin argumentos, inicializará la dirección
    //a _stop y la velocidad a 0.5
    void init( direction dir = _stop, float speed = 0.5 );
    direction getDirection() const { return _actorDirection; }
    direction getStoreDir() const { return _storeDir; }
    float getSpeed() const { return _speed; }
    void setDirection(const direction& dir){ _actorDirection = dir; }
    void setStoreDir(const direction& dir){ _storeDir = dir;}
    void setSpeed( const float& speed ){ _speed = speed;}
private:
   direction _actorDirection;
   direction _storeDir;
   float _speed;
};
#endif