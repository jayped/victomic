#ifndef MAKECAMERA_H
#define MAKECAMERA_H
#include <Ogre.h>

class MakeCamera : public Ogre::Camera
{
  
public:
	MakeCamera(const Ogre::Camera & camera);
    MakeCamera(const Ogre::String & name, Ogre::SceneManager *creator);
    ~MakeCamera();
    
	void init();

private:
	
};
#endif