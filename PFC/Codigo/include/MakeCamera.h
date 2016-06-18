#ifndef MAKECAMERA_H
#define MAKECAMERA_H
#include <Ogre.h>
#include <math.h>

#define _posCamX 0
#define _posCamY 24
#define _posCamZ 50

#define _cameraMoveSpeed 200

using namespace Ogre;

class MakeCamera : public Ogre::Camera
{

	enum direction {_stop, _up, _down, _right, _left};

public:
	MakeCamera(const Ogre::String & name, Ogre::SceneManager *creator);
    ~MakeCamera();
    
	void init();
	bool isMoving();
	void move(double aSpeedRelative);
	void setCameraDirection(direction aDirection);
	void setMoving(int aDirection);
	int getCameraPosition();

protected:
	bool _isMoving;
	double _cameraAngle;
	direction _cameraDirection;
	int _cameraPosition; // 4 posiciones posibles en las 4 caras del cubo.

};
#endif