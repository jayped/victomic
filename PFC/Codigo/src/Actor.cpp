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
	int x=0;
}
void Actor::init(int aType, int aActorID)
{

	_currentSpeed=0.0F;
	_state = _stay;
	_actorID = aActorID;

	Ogre::Root *_root = Ogre::Root::getSingletonPtr();
	Ogre::SceneManager *_sceneMgr = _root->getSceneManager("SceneManager");
	Ogre::Entity *_actorEntity;
	_gameMgr = GameManager::getSingletonPtr();
		
	_activated = false;
	_counter = 1;
	_type = aType;
	_dead = false;

	switch (aType)
	{
		case 10: //Nori

				// Particulas de caminar
				smokeParticle = _sceneMgr->createParticleSystem("Smoke", "Smoke");
				smokeParticle->setEmitting(false);
				particleNode = this->createChildSceneNode("Particle");
				particleNode->attachObject(smokeParticle);

				// Particulas de saltar
				jumpParticle = _sceneMgr->createParticleSystem("jumpSmoke", "jumpSmoke");
				jumpParticle->setEmitting(false);
				jumpParticleNode = this->createChildSceneNode("ParticleJump");
				jumpParticleNode->attachObject(jumpParticle);

				//animación Nori
				_actorEntity = reinterpret_cast<Ogre::Entity *>(this->getAttachedObject((int)0));
				_animation = _actorEntity->getAnimationState("Walking");
				_animation->setEnabled(false);
				break;
		case 1: // Bloque Suelo de Escenario
			break;
		case 2: // Bloque Construccion de Escenario
			break;
		case 3: // Piedra movil
			break;
		case 4: // Bloque fragil
				smokeParticle = _sceneMgr->createParticleSystem("FragileRock"+std::to_string(_actorID), "FragileRock");
				particleNode = this->createChildSceneNode("FragileRockParticle"+std::to_string(_actorID));
				smokeParticle->setEmitting(false);
				particleNode->attachObject(smokeParticle);
				break;
		case 5: // Interruptor
			break;
		case 6: // Bloque transparente
				this->getAttachedObject((int)0)->setCastShadows(false);
				break;
		case 7: // Bloque solido
			break;
		case 8: // Meta de escenario
				smokeParticle = _sceneMgr->createParticleSystem("GoalParticle"+std::to_string(_actorID), "Goal");
				particleNode = this->createChildSceneNode("GoalParticle"+std::to_string(_actorID));
				smokeParticle->setEmitting(false);
				particleNode->attachObject(smokeParticle);
			break;
	}
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
	////////////////////////////
	// Particulas para desplazamiento de nori.
	////////////////////////////
	//particleNode->setPosition(this->getPosition());
	if (!_activated && _state==_stay)
	{
		_gameMgr->playWalk();
		_activated=true;
	}

	smokeParticle->setEmitting(true);
	//_gameMgr->playWalk();

	// animacion de nori
	_animation->setEnabled(true);
	_animation->setLoop(true);

	jumpParticleNode->setPosition(Ogre::Vector3(0,0,0));

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
		_fallRigidBody->translate(btVector3(0,0,-_moveSpeed*_moveSpeedRelative));
	}
	// left
	if (lDirection == lLeft)
	{
		_fallRigidBody->translate(btVector3(-_moveSpeed*_moveSpeedRelative,0,0));
	}
	// down
	if (lDirection == lDown)
	{
		_fallRigidBody->translate(btVector3(0,0,_moveSpeed*_moveSpeedRelative));
	}
	// right
	if (lDirection == lRight)
	{
		_fallRigidBody->translate(btVector3(_moveSpeed*_moveSpeedRelative,0,0));
	}

		// up
	if (lDirection == _up)
	{
		jumpParticleNode->setPosition(jumpParticleNode->getPosition().x, jumpParticleNode->getPosition().y, -.5);
	}
	// left
	if (lDirection == _left)
	{
		jumpParticleNode->setPosition(.5, jumpParticleNode->getPosition().y, jumpParticleNode->getPosition().z);
	}
	// down
	if (lDirection == _down)
	{
		jumpParticleNode->setPosition(jumpParticleNode->getPosition().x, jumpParticleNode->getPosition().y, .5);
	}
	// right
	if (lDirection == _right)
	{
		jumpParticleNode->setPosition(-.5, jumpParticleNode->getPosition().y, jumpParticleNode->getPosition().z);
	}

}

void
Actor::stop()
{
	if (_activated)
	{
		_gameMgr->stopWalk();
		_activated=false;
	}
	smokeParticle->setEmitting(false);
	_animation->setEnabled(false);
	_animation->setTimePosition(0.0);
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
	if (_state == _stay)
	{
		btVector3 impulse(_jumpX,_jumpY,_jumpZ);
		_fallRigidBody->activate(true);
		_fallRigidBody->applyCentralImpulse(impulse);
		_gameMgr->stopWalk();
		_gameMgr->playJump();
	}
}

void
Actor::goal()
{
	if (_type==10)
	{
	jumpParticle->setEmitting(false);
	btVector3 impulse(_jumpX,_jumpY,_jumpZ);
	_fallRigidBody->clearForces();
	_fallRigidBody->activate(true);
	_fallRigidBody->applyCentralImpulse(impulse);
	_fallRigidBody->setAngularVelocity(btVector3(0,3,0));
	_gameMgr->playGoal();

	}
	else
		smokeParticle->setEmitting(true);
	//_fallRigidBody->applyTorqueImpulse(btVector3(0,5,0));
}

void Actor::resetSpeed()
{
	_currentSpeed=0.0F;
}

Actor::states
Actor::updateState(Ogre::Real aAnimationTime)
{
	btVector3 linearVelocity = getRigitBody()->getLinearVelocity();
	btScalar y=linearVelocity.getY();

	// Callendo
	if (y<=-_stayEpsilon){
		_state = _falling;
		_gameMgr->stopWalk();
	}
	
	// Parado
	if ( ((y>-_stayEpsilon) && (y<_stayEpsilon)) && (_state==_stay))
	{
		jumpParticle->setEmitting(false);
	}
	
	// Cerca del suelo, y próximo a parar.
	if ( ((y>-_stayEpsilon) && (y<_stayEpsilon)) && (_state==_falling))
	{
		_activated=false;
		_gameMgr->playEndJump();
		_state = _stay;
		jumpParticle->setEmitting(true);
	}

	// Saltando
	if (y>=_stayEpsilon) _state = _jumping;

	if (_state != _stay)
	{
		smokeParticle->setEmitting(false);
		_animation->setEnabled(false);
		_animation->setTimePosition(0.0);
	}	

    if (_animation->getEnabled() && !_animation->hasEnded()) 
    {
		_animation->addTime(aAnimationTime);
    } 

	return _state;

}

void
Actor::setSpeedRelative(double aSpeedRelative)
{
	_moveSpeedRelative = aSpeedRelative;
}

void
Actor::setAnimationTime(Ogre::Real aAnimationTime)
{
    if (_animation->getEnabled() && !_animation->hasEnded()) 
    {
		_animation->addTime(aAnimationTime);
    } 
}

void
Actor::setActivated(bool aActivated)
{
	_activated = aActivated;
}

bool
Actor::getActivated()
{
	return _activated;
}

double
Actor::getCounter()
{
	return _counter;
}

void
Actor::updateCounter(double aDeltaT)
{
	_counter-=aDeltaT;
}

int
Actor::getType()
{
	return _type;
}

int
Actor::getActorID()
{
	return _actorID;
}

void
Actor::generateParticles()
{
	smokeParticle->setEmitting(true);
}


/*
void
destroyRigitBody()
{
	//_fallRigidBody
}*/

