#include "Actor.h"
Actor::Actor(Ogre::SceneManager *creator):Ogre::SceneNode(creator)
{
    _actorDirection = _stop;
    _storeDir = _stop;
}

Actor::Actor(Ogre::SceneManager *creator, const Ogre::String & name):Ogre::SceneNode(creator,name)
{
    _actorDirection = _stop;
    _storeDir = _stop;
    _speed = 0.5;
}
Actor::~Actor()
{
}
void Actor::init( direction dir, float speed )
{
    _actorDirection = dir;   
    _storeDir = _stop;
    _speed = speed;
}
