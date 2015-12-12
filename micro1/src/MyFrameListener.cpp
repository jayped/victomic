#include "MyFrameListener.h"

MyFrameListener::MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneNode* node, Ogre::OverlayManager* oveMan, Ogre::SceneManager* sceMan) {
	OIS::ParamList param;
	size_t windowHandle; // ogre maneja las ventanas como entero
	std::ostringstream wHandleStr; // ois maneja las ventanas como string

	// para usar la camara y los elementos en el famelistener
	_camera = cam;
	_node = node;
	_overlayManager = oveMan;
	_win = win;
	_sceneManager = sceMan;

	win->getCustomAttribute("WINDOW", &windowHandle); // obtenemos el manejador de la ventana desde win
	wHandleStr << windowHandle; // pasamos el manejador, obtenido como entero, a un string preparado para ois
	param.insert(std::make_pair("WINDOW", wHandleStr.str())); // se añade a la lista de parametros de ois

	_inputManager = OIS::InputManager::createInputSystem(param);
	_keyboard = static_cast<OIS::Keyboard*>
		(_inputManager->createInputObject(OIS::OISKeyboard, false));
	_mouse = static_cast<OIS::Mouse*>
		(_inputManager->createInputObject(OIS::OISMouse, false));

	_mouse->getMouseState().width = win->getWidth();
	_mouse->getMouseState().height = win->getHeight();
	_raySceneQuery = _sceneManager->createRayQuery(Ogre::Ray());

	_gameState = 0; // menu
	
}

MyFrameListener::~MyFrameListener() {
	// Ogre no elimina los objetos OIS
	_inputManager->destroyInputObject(_keyboard);
	_inputManager->destroyInputObject(_mouse);
	OIS::InputManager::destroyInputSystem(_inputManager);
}

Ogre::Ray MyFrameListener::setRayQuery(int posx, int posy, int mask)
{
	Ogre::Ray rayMouse = _camera->getCameraToViewportRay(posx/float(_win->getWidth()),posy/float(_win->getHeight()));
	_raySceneQuery->setRay(rayMouse);
	_raySceneQuery->setSortByDistance(true);
	_raySceneQuery->setQueryMask(mask);
	return (rayMouse);
}

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt) {
	
	Ogre::Vector3 vt(0,0,0);
	Ogre::Real deltaT = evt.timeSinceLastFrame;
	Ogre::Real tSpeed = 20.0;
	int fps = 1.0 / deltaT;

	_keyboard->capture();
	if(_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
	if(_keyboard->isKeyDown(OIS::KC_0)) return false;
	if(_keyboard->isKeyDown(OIS::KC_1))
	{
		if (_gameState == 0)
		{
			_gameState = 1;
			createBoard();
		}
	}

	if(_keyboard->isKeyDown(OIS::KC_2)) return false;

	// Mouse capture
	_mouse->capture();
	int posx = _mouse->getMouseState().X.abs;
	int posy = _mouse->getMouseState().Y.abs;

	// Cursor drawing
	Ogre::OverlayElement *oe;
	oe = _overlayManager->getOverlayElement("cursor");
	oe->setLeft(posx);
	oe->setTop(posy);
	
	// Mouse action
	bool mbleft = _mouse->getMouseState().buttonDown(OIS::MB_Left);
	if (mbleft) {
		Ogre::Ray r = setRayQuery(posx, posy, 1);
		Ogre::RaySceneQueryResult &result = _raySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator it;
		it = result.begin();

		if (it != result.end())
		{
			if (it->movable->getParentSceneNode()->getName() != "nodeG")
			{
				Ogre::Vector3 pos = it->movable->getParentSceneNode()->getPosition();
				it->movable->detachFromParent();
				/*
				Ogre::Entity* ent1;
				ent1 = _sceneManager->createEntity("cube.mesh");
				Ogre::SceneNode* node1 = _sceneManager->createSceneNode();
				node1->attachObject(ent1);
				node1->translate(pos.x,pos.y,pos.z);
				_sceneManager->getRootSceneNode()->addChild(node1);
				*/
			}
		}
	}
	
	return true;
}

bool MyFrameListener::createBoard()
{
	
	const int numShips = 5;
	const int xMAX = 10;
	const int yMAX = 10;
	
	int myX=0;
	int myY=0;
	int myShips[numShips] = {1, 2, 3, 4 ,5};
	int myBoard[xMAX][yMAX];
	
	// Reset
	for (int i=0; i<xMAX; i++)
	{
		for (int j=0; j<yMAX; j++)
		{
			myBoard[i][j]=0;
		}
	}

	for (int i=0; i<numShips; i++)
	{
		int myX = rand()%10;
		int myY = rand()%10;

		myBoard[myX][myY] = 1;
	}

	// Creacion del tablero.
	for (int i=0; i<xMAX; i++)
	{
		for (int j=0; j<yMAX; j++)
		{
			Ogre::Entity* ent1;
			if (myBoard[i][j] == 1)
				ent1 = _sceneManager->createEntity("cube.mesh");
			else
				ent1 = _sceneManager->createEntity("Cubepam.mesh");
			ent1->setQueryFlags(myBoard[i][j]?0:1);

			Ogre::SceneNode* node1 = _sceneManager->createSceneNode();
			node1->attachObject(ent1);
			node1->translate(i*2.2,j*2.2,-50);
			
			_sceneManager->getRootSceneNode()->addChild(node1);
		}
	}


	return true;
}