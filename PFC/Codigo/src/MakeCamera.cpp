#include "MakeCamera.h"
MakeCamera::MakeCamera(const Ogre::String & name, Ogre::SceneManager *creator):Ogre::Camera(name,creator)
{
}

MakeCamera::~MakeCamera()
{
}

void
MakeCamera::init()
{
	this->setPosition(Ogre::Vector3(_posCamX,_posCamY,_posCamZ));
	this->lookAt(Ogre::Vector3(_posCamX,_posCamY,0));
	this->setNearClipDistance(5);
	this->setFarClipDistance(10000);
	_isMoving=false;
	_cameraAngle = 0;
	_cameraDirection = _stop;
}

void
MakeCamera::setCameraDirection(direction aDirection)
{
	_cameraDirection = aDirection;
}

void
MakeCamera::setMoving(int aDirection)
{
	if (_isMoving==false)
	{
		_isMoving = true;
	}
	_cameraDirection = (direction) aDirection;
}

bool
MakeCamera::isMoving()
{
	return _isMoving;
}

void
MakeCamera::move()
{
		if (_cameraDirection==_right)
			_cameraAngle+=0.1;
		else if (_cameraDirection==_left)
			_cameraAngle-=0.1;
		
		double posx = (Math::Sin(Math::AngleUnitsToRadians(_cameraAngle))*50);
		double posz = (Math::Cos(Math::AngleUnitsToRadians(_cameraAngle))*50);

		this->setPosition(Ogre::Vector3(posx,_posCamY,posz));
		double modulo;

		if (_cameraDirection==_right)
			modulo = (int)Math::Ceil(_cameraAngle) % 90;
		else if (_cameraDirection==_left)
			modulo = (int)Math::Floor(_cameraAngle) % 90;
		
		//if ( (_cameraAngle<(90.0+0.05)) && (_cameraAngle>(90.0-0.05)) )
		if ( (modulo==0) )
		{
			if (_cameraDirection==_right)
				_cameraAngle = Math::Ceil(_cameraAngle);
			else if (_cameraDirection==_left)
				_cameraAngle = Math::Floor(_cameraAngle);
			_isMoving=false;
		}
}


