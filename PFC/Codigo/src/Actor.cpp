#include "Actor.h"
Actor::Actor(Ogre::SceneManager *creator):Ogre::SceneNode(creator)
{
	_currentSpeed=0.0F;
}

Actor::Actor(Ogre::SceneManager *creator, const Ogre::String & name):Ogre::SceneNode(creator,name)
{
	_currentSpeed=0.0F;
}
Actor::~Actor()
{
}
void Actor::init()
{
	_currentSpeed=0.0F;
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
Actor::move(int aDirection, int aCameraPosition)
{
	direction lDirection = (direction) aDirection;
	_fallRigidBody->activate(true);
	
	direction lUp;
	direction lLeft;
	direction lDown;
	direction lRight;

	switch (aCameraPosition)
	{
	case 0:
		lUp=_up;lLeft=_left;lDown=_down;lRight=_right;
		break;
	case 1:
		lUp=_right;lLeft=_up;lDown=_left;lRight=_down;
		break;
	case 2:
		lUp=_down;lLeft=_right;lDown=_up;lRight=_left;
		break;
	case 3:
		lUp=_left;lLeft=_down;lDown=_right;lRight=_up;
		break;
	}

	// up
	if (lDirection == lUp)
	{
		_fallRigidBody->translate(btVector3(0,0,-_moveSpeed));
	}
	// left
	if (lDirection == lLeft)
	{
		_fallRigidBody->translate(btVector3(-_moveSpeed,0,0));
	}
	// down
	if (lDirection == lDown)
	{
		_fallRigidBody->translate(btVector3(0,0,_moveSpeed));
	}
	// right
	if (lDirection == lRight)
	{
		_fallRigidBody->translate(btVector3(_moveSpeed,0,0));
	}
}

void
Actor::orientate(bool aRotate[], int aCameraPosition)
{
	btQuaternion btq;
	btTransform transform;
	Ogre::Real lRotate=0;
	
	int lUp;
	int lLeft;
	int lDown;
	int lRight;

	switch (aCameraPosition)
	{
	case 0:
		lUp=2;lLeft=3;lDown=0;lRight=1;
		break;
	case 1:
		lUp=1;lLeft=2;lDown=3;lRight=0;
		break;
	case 2:
		lUp=0;lLeft=1;lDown=2;lRight=3;
		break;
	case 3:
		lUp=3;lLeft=0;lDown=1;lRight=2;
		break;
	}

	// down
	if ( (aRotate[lDown]) && !(aRotate[lRight]) && !(aRotate[lUp]) && !(aRotate[lLeft])) lRotate = 0;
	// down right
	if ( (aRotate[lDown]) && (aRotate[lRight]) && !(aRotate[lUp]) && !(aRotate[lLeft])) lRotate = Ogre::Math::PI/4;
	// down left
	if ( (aRotate[lDown]) && !(aRotate[lRight]) && !(aRotate[lUp]) && (aRotate[lLeft])) lRotate = (7.0/4.0) * Ogre::Math::PI;
	// right
	if ( !(aRotate[lDown]) && (aRotate[lRight]) && !(aRotate[lUp]) && !(aRotate[lLeft])) lRotate = Ogre::Math::PI/2;
	// right up
	if ( !(aRotate[lDown]) && (aRotate[lRight]) && (aRotate[lUp]) && !(aRotate[lLeft])) lRotate = (3.0/4.0) * Ogre::Math::PI;
	// up
	if ( !(aRotate[lDown]) && !(aRotate[lRight]) && (aRotate[lUp]) && !(aRotate[lLeft])) lRotate = Ogre::Math::PI;
	// up left
	if ( !(aRotate[lDown]) && !(aRotate[lRight]) && (aRotate[lUp]) && (aRotate[lLeft])) lRotate = (5.0/4.0) * Ogre::Math::PI;
	// left
	if ( !(aRotate[lDown]) && !(aRotate[lRight]) && !(aRotate[lUp]) && (aRotate[lLeft])) lRotate = (3.0/2.0) * Ogre::Math::PI;
	
	btq = _fallRigidBody->getOrientation();
	btq.setRotation(btVector3(0,1,0),lRotate);
	transform = _fallRigidBody->getWorldTransform();
	transform.setRotation(btq);

	_fallRigidBody->activate(true);
	_fallRigidBody->setWorldTransform(transform);
}

void
Actor::jump()
{
	btVector3 impulse(_jumpX,_jumpY,_jumpZ);
	_fallRigidBody->activate(true);
	_fallRigidBody->applyCentralImpulse(impulse);
}

void Actor::resetSpeed()
{
	_currentSpeed=0.0F;
}

/*
void
destroyRigitBody()
{
	//_fallRigidBody
}*/

