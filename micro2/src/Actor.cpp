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
    if(dir!=_stop)
    {
        //dirección aleatoria, menos arriba
        float random = Ogre::Math::RangeRandom(2.00, 4.99);
        _actorDirection = (direction)(int)floor( random );
    }
    else
    {
        _actorDirection = dir;   
    }
    _storeDir = _stop;
    if( speed == 0.0 )
    {
        //velocidad aleatoria
        _speed =  Ogre::Math::RangeRandom(0.08, 0.15);
    }
    else
    {        
        _speed = speed;
    }
}

void Actor::setDirection(const direction& dir)
{
    if( dir != _random )
    {
        _actorDirection = dir; 
    }
    else
    {
        float random = Ogre::Math::RangeRandom(1.00, 4.99);
        _actorDirection = (direction)(int)floor( random );
    }
}
