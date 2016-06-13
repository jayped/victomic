#ifndef MAKECAMERA_H
#define MAKECAMERA_H
#include <Ogre.h>
#include <math.h>

#define _posCamX 0
#define _posCamY 24
#define _posCamZ 50

using namespace Ogre;

class MakeCamera : public Ogre::Camera
{

	enum direction {_stop, _up, _down, _right, _left};

public:
	MakeCamera(const Ogre::String & name, Ogre::SceneManager *creator);
    ~MakeCamera();
    
	void init();
	bool isMoving();
	void move();
	void setCameraDirection(direction aDirection);
	void setMoving(int aDirection);

protected:
	bool _isMoving;
	double _cameraAngle;
	direction _cameraDirection;
};
#endif