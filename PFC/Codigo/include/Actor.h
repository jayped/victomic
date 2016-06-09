#ifndef ACTOR_H
#define ACTOR_H
#include <Ogre.h>
#include <btBulletDynamicsCommon.h>

class Actor : public Ogre::SceneNode
{
  
public:
    //Actor(){}
    Actor(Ogre::SceneManager *creator);
    Actor(Ogre::SceneManager *creator, const Ogre::String & name);
    ~Actor();
    
	void init();
	void setRigitBody(btRigidBody *myRigitBody);
	btRigidBody* getRigitBody();
	void destroyRigitBody();

private:
	btRigidBody* _fallRigidBody;
	
};
#endif