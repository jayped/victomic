#include "MakeCamera.h"
MakeCamera::MakeCamera(const Ogre::Camera & camera):Ogre::Camera(camera)
{
}

MakeCamera::MakeCamera(const Ogre::String & name, Ogre::SceneManager *creator):Ogre::Camera(name,creator)
{
}

MakeCamera::~MakeCamera()
{
}

void
MakeCamera::init()
{
}

