#ifndef ACTOR_H
#define ACTOR_H
#include <Ogre.h>
#include <btBulletDynamicsCommon.h>

class Can : public Ogre::SceneNode
{
  
public:
    //Actor(){}
    Can(Ogre::SceneManager *creator);
    Can(Ogre::SceneManager *creator, const Ogre::String & name);
    ~Can();
    
	void init();
	void setRigitBody(btRigidBody *myRigitBody);
	btRigidBody* getRigitBody();
	void destroyRigitBody();

private:
	btRigidBody* _fallRigidBody;
	
};
#endif