#ifndef ACTOR_H
#define ACTOR_H
#include <Ogre.h>
#include <btBulletDynamicsCommon.h>
#define _jumpX 0 //-15
#define _jumpY 9
#define _jumpZ 0
#define _moveSpeed .003

class Actor : public Ogre::SceneNode
{
	enum direction {_stop, _up, _down, _right, _left};
	
public:
    //Actor(){}
    Actor(Ogre::SceneManager *creator);
    Actor(Ogre::SceneManager *creator, const Ogre::String & name);
    ~Actor();
    
	void init();
	void setRigitBody(btRigidBody *myRigitBody);
	btRigidBody* getRigitBody();
	void destroyRigitBody();
	void move(int aDirection);
	void orientate(bool aRotate[]);
	void jump();

private:
	btRigidBody* _fallRigidBody;
	
};
#endif