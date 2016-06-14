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
Actor::move(int aDirection)
{
	direction lDirection = (direction) aDirection;
	_fallRigidBody->activate(true);
	btQuaternion btq;
	btTransform transform;

	// up
	if (lDirection == _up)
	{
		_fallRigidBody->translate(btVector3(0,0,-_moveSpeed));
		//btq = _fallRigidBody->getOrientation();
		transform = _fallRigidBody->getWorldTransform();
		//btq.setRotation(btVector3(0,1,0),Ogre::Math::PI);
	}
	if (lDirection == _left)
	{
		_fallRigidBody->translate(btVector3(-_moveSpeed,0,0));
		//btq = _fallRigidBody->getOrientation();
		transform = _fallRigidBody->getWorldTransform();
		//btq.setRotation(btVector3(0,1,0),(3.0/2.0) * Ogre::Math::PI);
	}
	if (lDirection == _down)
	{
		_fallRigidBody->translate(btVector3(0,0,_moveSpeed));
		//btq = _fallRigidBody->getOrientation();
		transform = _fallRigidBody->getWorldTransform();
		//btq.setRotation(btVector3(0,1,0),0);
	}
	if (lDirection == _right)
	{
		_fallRigidBody->translate(btVector3(_moveSpeed,0,0));
		//btq = _fallRigidBody->getOrientation();
		transform = _fallRigidBody->getWorldTransform();
		//btq.setRotation(btVector3(0,1,0),Ogre::Math::PI/2);
	}


	//transform.setRotation(btq);
	_fallRigidBody->activate(true);
	_fallRigidBody->setWorldTransform(transform);
}

void
Actor::orientate(bool aRotate[])
{
	btQuaternion btq;
	btTransform transform;
	Ogre::Real lRotate=0;

	// down
	if ( (aRotate[0]) && !(aRotate[1]) && !(aRotate[2]) && !(aRotate[3])) lRotate = 0;
	// down right
	if ( (aRotate[0]) && (aRotate[1]) && !(aRotate[2]) && !(aRotate[3])) lRotate = Ogre::Math::PI/4;
	// down left
	if ( (aRotate[0]) && !(aRotate[1]) && !(aRotate[2]) && (aRotate[3])) lRotate = (7.0/4.0) * Ogre::Math::PI;
	// right
	if ( !(aRotate[0]) && (aRotate[1]) && !(aRotate[2]) && !(aRotate[3])) lRotate = Ogre::Math::PI/2;
	// right up
	if ( !(aRotate[0]) && (aRotate[1]) && (aRotate[2]) && !(aRotate[3])) lRotate = (3.0/4.0) * Ogre::Math::PI;
	// up
	if ( !(aRotate[0]) && !(aRotate[1]) && (aRotate[2]) && !(aRotate[3])) lRotate = Ogre::Math::PI;
	// up left
	if ( !(aRotate[0]) && !(aRotate[1]) && (aRotate[2]) && (aRotate[3])) lRotate = (5.0/4.0) * Ogre::Math::PI;
	// left
	if ( !(aRotate[0]) && !(aRotate[1]) && !(aRotate[2]) && (aRotate[3])) lRotate = (3.0/2.0) * Ogre::Math::PI;
	
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

/*
void
destroyRigitBody()
{
	//_fallRigidBody
}*/

