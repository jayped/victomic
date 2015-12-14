#include <Ogre.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OIS/OIS.h>
#include <windows.h>
#include "GameTimer.h"
#define _posXBoardPlayer -0.4321
#define _posYBoardPlayer -3.6799
#define _posZBoardPlayer -49.9970

class MyFrameListener : public Ogre::FrameListener {
private:
	OIS::InputManager* _inputManager;
	OIS::Keyboard* _keyboard;
	OIS::Mouse* _mouse;
	Ogre::Camera* _camera;
	Ogre::SceneNode* _node;
	Ogre::OverlayManager* _overlayManager;
	Ogre::RenderWindow* _win;
	Ogre::RaySceneQuery *_raySceneQuery;
	Ogre::SceneManager* _sceneManager;
	int _gameState;
	static const int _XMAX = 10;
	static const int _YMAX = 10;
	static const int _NUMSHIPS = 5;
	//static const double _posXBoardPlayer = -0.4321;
	//static const double _posYBoardPlayer = -3.6799;
	//static const double _posZBoardPlayer = -49.9970;
	
	Ogre::SceneNode* _playerNodeBoard[_XMAX][_YMAX]; // [!] QUITAR

	GameTimer theGameTimer;

public:
	MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneNode* node,Ogre::OverlayManager* oveMan,Ogre::SceneManager* sceMan);
	~MyFrameListener();
	bool frameStarted(const Ogre::FrameEvent& evt);
	Ogre::Ray setRayQuery(int posx, int posy, int mask);
	bool MyFrameListener::createBoard();
	bool MyFrameListener::placeShips(int aShips[_NUMSHIPS], int aBoard[_XMAX][_YMAX]);
	int MyFrameListener::getShipDirection(int aMyX, int aMyY, int aShipLenght, int aBoard[_XMAX][_YMAX]);
};
