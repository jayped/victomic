#include "Actor.h"
Actor::Actor(Ogre::SceneManager *creator):Ogre::SceneNode(creator)
{
}

Actor::Actor(Ogre::SceneManager *creator, const Ogre::String & name):Ogre::SceneNode(creator,name)
{
}
Actor::~Actor()
{
}
void Actor::init()
{
}

void
Actor::setRigitBody(btRigidBody *myRigitBody)
{
	
	_fallRigidBody = myRigitBody;
}

btRigidBody*
Actor::getRigitBody()
{
	return _fallRigidBody;
}

void
destroyRigitBody()
{
	//_fallRigidBody
}

