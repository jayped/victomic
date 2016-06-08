#include "Can.h"
Can::Can(Ogre::SceneManager *creator):Ogre::SceneNode(creator)
{
}

Can::Can(Ogre::SceneManager *creator, const Ogre::String & name):Ogre::SceneNode(creator,name)
{
}
Can::~Can()
{
}
void Can::init()
{
}

void
Can::setRigitBody(btRigidBody *myRigitBody)
{
	
	_fallRigidBody = myRigitBody;
}

btRigidBody*
Can::getRigitBody()
{
	return _fallRigidBody;
}

void
destroyRigitBody()
{
	//_fallRigidBody
}

