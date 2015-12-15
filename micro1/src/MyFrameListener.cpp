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
	_myWinner = 0; // 0.NO, 1.Player, 2.CPU
	_maxFires = 0;
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
	if(_keyboard->isKeyDown(OIS::KC_0)) // reset
	{
		if (_gameState == 1)
		{
			_gameState = 0;
			deleteBoard();
			_overlayManager->getByName("Title")->show();
		}
	}
	if(_keyboard->isKeyDown(OIS::KC_1)) // jugar
	{
		if (_gameState == 0)
		{
			_gameState = 1;
			createBoard();
			_overlayManager->getByName("Title")->hide();
			_overlayManager->getByName("Win")->hide();
			_overlayManager->getByName("Lose")->hide();
		}
	}

	//if(_keyboard->isKeyDown(OIS::KC_2)) return false;

	// Captura del raton
	_mouse->capture();
	int posx = _mouse->getMouseState().X.abs;
	int posy = _mouse->getMouseState().Y.abs;

	// Dibujado del cursor
	Ogre::OverlayElement *oe;
	oe = _overlayManager->getOverlayElement("cursor");
	oe->setLeft(posx);
	oe->setTop(posy);
	
	// Accion del raton
	bool mbleft = _mouse->getMouseState().buttonDown(OIS::MB_Left);
	if (mbleft) {
		Ogre::Ray r = setRayQuery(posx, posy, 1);
		Ogre::RaySceneQueryResult &result = _raySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator it;
		it = result.begin();
		//AGUA
		if (it != result.end())
		{
			if (it->movable->getParentSceneNode()->getName() != "nodeG")
			{
				Ogre::Vector3 pos = it->movable->getParentSceneNode()->getPosition();
				it->movable->detachFromParent();
				
				// tirada del CPU.
				int lResult = 0;
				do
				{
					lResult = playCPU();
					if (endGame()!=0)
					{
						_gameState = 0;
						deleteBoard();
						_overlayManager->getByName("Title")->show();
						_overlayManager->getByName("Lose")->show();
					}
				}
				while ((lResult == 1) && (_gameState==1));
			}
		}
		//TOCADO
		r = setRayQuery(posx, posy, 2);
		Ogre::RaySceneQueryResult &resultShip = _raySceneQuery->execute();
		it = resultShip.begin();
		if (it != resultShip.end())
		{
			if (it->movable->getParentSceneNode()->getName() != "nodeG")
			{
				
				Ogre::Vector3 pos = it->movable->getParentSceneNode()->getPosition();
				it->movable->detachFromParent();
				
				// Creacion de otro cubo para cuando haya barco.
				Ogre::Entity* ent1;
				ent1 = _sceneManager->createEntity("CuboBarco.mesh");
				ent1->setQueryFlags(4);
				Ogre::SceneNode* node1 = _sceneManager->createSceneNode();
				node1->attachObject(ent1);
				node1->translate(pos.x,pos.y,pos.z);
				_sceneManager->getRootSceneNode()->addChild(node1);
				
				_maxFires--;
				// no tira CPU
				if (endGame()!=0)
				{
					_gameState = 0;
					deleteBoard();
					_overlayManager->getByName("Title")->show();
					_overlayManager->getByName("Win")->show();

				}

			}
		}
		
	}
	
	return true;
}

bool MyFrameListener::createBoard()
{
	int myX=0;
	int myY=0;
	
	// iniciar longitud barcos. (1,2,3,4,5)
	for (int i=0; i<_NUMSHIPS; i++)
	{
		myShips[i] = i+1;
		_maxFires += i+1;
	}

	// Colocacion de los barcos
	placeShips(myShips, myBoard);
	
	// Creacion del tablero CPU
	for (int i=0; i<_XMAX; i++)
	{
		for (int j=0; j<_YMAX; j++)
		{
			Ogre::Entity* ent1;
			if (myBoard[i][j] == 1)
			{
				ent1 = _sceneManager->createEntity("CuboAgua.mesh");
				ent1->setQueryFlags(2);
			}
			else
			{
				ent1 = _sceneManager->createEntity("CuboAgua.mesh");
				ent1->setQueryFlags(1);
			}

			Ogre::SceneNode* node1 = _sceneManager->createSceneNode();
			node1->attachObject(ent1);
			node1->translate(i*2.2,j*2.2,-50);
			
			_sceneManager->getRootSceneNode()->addChild(node1);
		}
	}
	
	placeShips(myShips, myBoardPlayer);
	// creacion del tablero player
	for (int i=0; i<_XMAX; i++)
	{
		for (int j=0; j<_YMAX; j++)
		{
			Ogre::Entity* ent1;
			if (myBoardPlayer[i][j] == 1)
				ent1 = _sceneManager->createEntity("CuboBarco.mesh");
			else
				ent1 = _sceneManager->createEntity("CuboAgua.mesh");
			ent1->setQueryFlags(4);

			Ogre::SceneNode* node1 = _sceneManager->createSceneNode();
			node1->attachObject(ent1);
			node1->setPosition(_posXBoardPlayer+(i*2.2),_posYBoardPlayer,_posZBoardPlayer+(j*2.2));
			
			// añadir nodo para ir eliminando por parte de la CPU
			_playerNodeBoard[i][j] = node1;
			_sceneManager->getRootSceneNode()->addChild(node1);
		}
	}

	return true;
}

bool MyFrameListener::deleteBoard()
{
	for (int i=0; i<_XMAX; i++)
	{
		for (int j=0; j<_YMAX; j++)
		{
			myBoard[i][j]=0;
		}
	}
	for (int i=0; i<_XMAX; i++)
	{
		for (int j=0; j<_YMAX; j++)
		{
			myBoardPlayer[i][j]=0;
		}
	}

	_maxFires=0;

	_sceneManager->getRootSceneNode()->removeAndDestroyAllChildren();
	_sceneManager->clearScene();

	// Añadir el plano a la escena
	// Creacion del plano
	Ogre::Plane pl1(Ogre::Vector3::UNIT_Y,-5);
	Ogre::MeshManager::getSingleton().createPlane("pl1",
	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	pl1,200,200,1,1,true,1,20,20,Ogre::Vector3::UNIT_Z);

	// Añadir el plano a la escena
	Ogre::SceneNode* nodeG = _sceneManager->createSceneNode("nodeG");
	Ogre::Entity* grEnt = _sceneManager->createEntity("pEnt", "pl1");
	grEnt->setMaterialName("Ground");
	nodeG->attachObject(grEnt);

	// Sombras
	_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Light* light = _sceneManager->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));
	nodeG->attachObject(light);
	_sceneManager->getRootSceneNode()->addChild(nodeG);


	return true;
}

bool MyFrameListener::placeShips(int aShips[_NUMSHIPS], int aBoard[_XMAX][_YMAX])
{
	// Reset
	for (int i=0; i<_XMAX; i++)
	{
		for (int j=0; j<_YMAX; j++)
		{
			aBoard[i][j]=0;
		}
	}
	
	// semilla de rand()
	SYSTEMTIME st;
	GetSystemTime(&st);
	srand(st.wMilliseconds);

	for (int i=0; i<_NUMSHIPS; i++)
	{
		int myX = 0;
		int myY = 0;
		int shipLenght = aShips[i];
		int shipPointer = 0;
		int shipDirection = 0;

		// Posicion aleatoria no ocupada por barco
		do
		{
			myX = rand()%_XMAX;
			myY = rand()%_YMAX;
			shipDirection = getShipDirection(myX, myY, shipLenght, aBoard); // 0-derecha, 1-abajo, 2-izquierda, 3-derecha, 4-error;
		}
		while (/*(aBoard[myX][myY] == 1) || */shipDirection==4);
		
		// colocacion de barco dependiendo de la direccion
		switch (shipDirection)
		{
			case 0: // derecha
				while (shipLenght>0)
				{
					aBoard[myX+shipPointer][myY] = 1;
					shipPointer++;
					shipLenght--;
				}
				break;
			case 1: // abajo
				while (shipLenght>0)
				{
					aBoard[myX][myY+shipPointer] = 1;
					shipPointer++;
					shipLenght--;
				}
				break;
			case 2: // izquierda
				while (shipLenght>0)
				{
					aBoard[myX-shipPointer][myY] = 1;
					shipPointer++;
					shipLenght--;
				}
				break;
			case 3: // arriba
				while (shipLenght>0)
				{
					aBoard[myX][myY-shipPointer] = 1;
					shipPointer++;
					shipLenght--;
				}
				break;
		};

	}
	return true;
}

int MyFrameListener::getShipDirection(int aMyX, int aMyY, int aShipLenght, int aBoard[_XMAX][_YMAX])
{
	
	int shipDirection = 0; // 0-derecha, 1-abajo, 2-izquierda, 3-derecha;

	// comprobar si hay barco en medio o se sale del tablero
	// Derecha
	if(aMyX+aShipLenght<_XMAX)
	{
		for (int i=0; i<aShipLenght; i++)
		{
			if (aBoard[aMyX+i][aMyY] == 1)
			{
				shipDirection = 1;
			}
		}
	}
	else
	{
		shipDirection = 1;
	}

	// Abajo
	if ((aMyY+aShipLenght<_YMAX) && shipDirection==1)
	{
		for (int i=0; i<aShipLenght; i++)
		{
			if (aBoard[aMyX][aMyY+i] == 1)
			{
				shipDirection = 2;
			}
		}
	}
	else if (shipDirection==1)
	{
		shipDirection = 2;
	}

	// Izquierda
	if ((aMyX-aShipLenght>=0) && shipDirection==2)
	{
		for (int i=0; i<aShipLenght; i++)
		{
			if (aBoard[aMyX-i][aMyY] == 1)
			{
				shipDirection = 3;
			}
		}
	}
	else if (shipDirection==2)
	{
		shipDirection = 3;
	}

	// Arriba
	if ((aMyY-aShipLenght>=0) && shipDirection==3)
	{
		for (int i=0; i<aShipLenght; i++)
		{
			if (aBoard[aMyX][aMyY-i] == 1)
			{
				shipDirection = 4;
			}
		}
	}
	else if (shipDirection==3)
	{
		shipDirection = 4;
	}
	
	return shipDirection;
}

int MyFrameListener::playCPU()
{
	int i=0;
	int j=0;
	int result = 0; // 0.agua, 1.impacto
	do
	{
		i=rand()%10;
		j=rand()%10;
	}
	while(myBoardPlayer[i][j]==2);
				
	if (myBoardPlayer[i][j]==1) // barco.
	{
		Ogre::Vector3 pos = _playerNodeBoard[i][j]->getPosition();
		_playerNodeBoard[i][j]->detachObject((unsigned short)0);

		// Creacion de otro cubo para cuando haya barco.
		Ogre::Entity* ent1;
		ent1 = _sceneManager->createEntity("CuboBarcoRojo.mesh");
		ent1->setQueryFlags(4);
		Ogre::SceneNode* node1 = _sceneManager->createSceneNode();
		node1->attachObject(ent1);
		node1->translate(pos.x,pos.y,pos.z);
		_sceneManager->getRootSceneNode()->addChild(node1);
		result = 1;
	}
	else if (myBoardPlayer[i][j]==0) // agua.
	{
		_playerNodeBoard[i][j]->detachObject((unsigned short)0);
	}
	
	myBoardPlayer[i][j]=2;
	return result;
}

int MyFrameListener::endGame()
{
	int lWinner = 0; // 0.no, 1.Player, 2.CPU

	// ganador player?
	
	if (_maxFires==0)
	{
		lWinner = 1;
	}

	if (lWinner==0)
	{
		// ganador CPU?
		lWinner = 2;
		for (int i=0; i<_XMAX; i++)
		{
			for (int j=0; j<_YMAX; j++)
			{
				if (myBoardPlayer[i][j]==1)
				{
					lWinner = 0;
				}
			}
		}
	}
	return lWinner;

}


