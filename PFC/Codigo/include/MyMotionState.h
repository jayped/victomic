#include <Ogre.h>
#include <btBulletDynamicsCommon.h>

class MyMotionState : public btMotionState {
protected:
  Ogre::SceneNode* _visibleobj;
  btTransform _pos;

public:
  MyMotionState(const btTransform &initialpos, Ogre::SceneNode* node);
  virtual ~MyMotionState();
  void setNode(Ogre::SceneNode* node);
  virtual void getWorldTransform(btTransform &worldTrans) const;
  virtual void setWorldTransform(const btTransform &worldTrans);
};
