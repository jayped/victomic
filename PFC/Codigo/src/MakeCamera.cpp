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
	_cameraPosition = 0;
	_cameraDistance = _posCamZ;
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

		if ( (direction) aDirection == _right) _cameraPosition++;
		if ( (direction) aDirection == _left) _cameraPosition--;
	
		if ((_cameraPosition) > 3) _cameraPosition=0;
		if ((_cameraPosition) < 0) _cameraPosition=3;

		_cameraDirection = (direction) aDirection;
	}

}

bool
MakeCamera::isMoving()
{
	return _isMoving;
}

int
MakeCamera::getCameraPosition()
{
	return _cameraPosition;
}

void
MakeCamera::setCameraDistance(int aCameraDistance)
{
	_cameraDistance=aCameraDistance;
}


void
MakeCamera::move(double aSpeedRelative)
{
		// En ejecuciones a baja resolucion, a veces llegan velocidades a 0.0 y esto hace que la camara no se mueva
		// y salte de golpe a la siguiente posicion fija. De esta forma, fallarian los calculos de la camara
		// y provocaria fallos en movimiento del personaje. con el forzado de velocidad positiva, se soluciona.
		aSpeedRelative+=0.00001;

		if (_cameraDirection==_right)
			_cameraAngle+=_cameraMoveSpeed*aSpeedRelative;//0.4; // 0.1 sin sombras 0.15 800x600. hay que hacerlo respecto de los frames
		else if (_cameraDirection==_left)
			_cameraAngle-=_cameraMoveSpeed*aSpeedRelative;//0.4; // 0.1 sin sombras 0.15 800x600. hay que hacerlo respecto de los frames

		double posx = (Math::Sin(Math::AngleUnitsToRadians(_cameraAngle))*/*_cameraDistance*/_cameraDistance);
		double posz = (Math::Cos(Math::AngleUnitsToRadians(_cameraAngle))*/*_cameraDistance*/_cameraDistance);

		this->setPosition(Ogre::Vector3(posx,this->getPosition().y,posz));
		double modulo;

		if (_cameraDirection==_right)
			modulo = (int)Math::Ceil(_cameraAngle) % 90;
		else if (_cameraDirection==_left)
			modulo = (int)Math::Floor(_cameraAngle) % 90;
		
		if ( (modulo==0) )
		{
			if (_cameraDirection==_right)
				_cameraAngle = Math::Ceil(_cameraAngle);
			else if (_cameraDirection==_left)
				_cameraAngle = Math::Floor(_cameraAngle);
			_isMoving=false;
		}
		
		// centrado de la camara para quitar el epsilon.
		if (!_isMoving)
		{
			if (_cameraPosition==0)this->setPosition(Ogre::Vector3(0,this->getPosition().y,_cameraDistance));
			if (_cameraPosition==1)this->setPosition(Ogre::Vector3(_cameraDistance,this->getPosition().y,0));
			if (_cameraPosition==2)this->setPosition(Ogre::Vector3(0,this->getPosition().y,-_cameraDistance));
			if (_cameraPosition==3)this->setPosition(Ogre::Vector3(-_cameraDistance,this->getPosition().y,0));
		}
}


