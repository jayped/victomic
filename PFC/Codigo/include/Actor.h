#ifndef ACTOR_H
#define ACTOR_H
#include <Ogre.h>
#include <btBulletDynamicsCommon.h>
#define _jumpX 0 //-15
#define _jumpY 9
#define _jumpZ 0
#define _moveSpeed .01 // .003 sin sombras .004 800x600. hay que hacerlo respecto de los frames

class Actor : public Ogre::SceneNode
{
	enum direction {_stop, _up, _down, _right, _left};

public:
    //Actor(){}
    Actor(Ogre::SceneManager *creator);
    Actor(Ogre::SceneManager *creator, const Ogre::String & name);
    ~Actor();
    
	enum states {_stay, _jumping, _falling};
	void init();
	void setRigitBody(btRigidBody *myRigitBody);
	btRigidBody* getRigitBody();
	void destroyRigitBody();
	void move(int aDirection, int aCameraPosition);
	void stop();
	void orientate(bool aRotate[], int aCameraPosition);
	void jump();
	void resetSpeed();
	states getState();

private:
	btRigidBody* _fallRigidBody;
	double _currentSpeed;
	states _state;
	Ogre::ParticleSystem *smokeParticle;
	Ogre::SceneNode *particleNode;
	Ogre::ParticleSystem *jumpParticle;
	Ogre::SceneNode *jumpParticleNode;
};
#endif