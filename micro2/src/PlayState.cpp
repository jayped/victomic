#include "PlayState.h"
#include "PauseState.h"
#include <iostream>
#include <conio.h>
#include "libxl.h"
#include <math.h>
#include <cmath>


using namespace libxl;

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

PlayState::PlayState ()
{
    //Se inicializa el tablero.
    for(int i = 0; i < BOARDSIZE; i++ )
    {
        for( int j = 0; j < BOARDSIZE; j++ )
        {
            _boardInfo[i][j] = EMPTY;
        }
    }
}

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
  //_sceneMgr->addRenderQueueListener(new Ogre::OverlaySystem());


  _camera = _sceneMgr->getCamera("IntroCamera");
  //_camera->setPosition(Ogre::Vector3(10,10,2));
  _camera->setPosition(Ogre::Vector3(_posCamX,_posCamY,_posCamZ));
  _camera->lookAt(Ogre::Vector3(_posCamX,_posCamY,0));
  //_camera->setDirection(Ogre::Vector3(-0.006,-0.165,-0.9621));
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);

  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  // Nuevo background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
  
	double width = _viewport->getActualWidth();
	double height = _viewport->getActualHeight();
	_camera->setAspectRatio(width / height);

	_exitGame = false;
	
	// crea el escenario y el fondo;
	createScene();
	createStage();
	
	
	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay = _overlayMgr->getByName("Score");
	
	_score = 0;
	_scoreOverlay = _overlayMgr->getOverlayElement("scoreLabel");
	_scoreOverlay->setCaption("Score");
	_scoreOverlay = _overlayMgr->getOverlayElement("scoreValue");
	_scoreOverlay->setCaption(Ogre::StringConverter::toString(_score));
	
	overlay->show();
	

	_playerDirection = _stop;
	_storeDir = _stop;
	
}

void
PlayState::exit ()
{
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{
  // Se restaura el background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
	_playerPosition = _nodePlayer->getPosition();

	checkBalls();

	// desechar el truncado, porque no vale al no ser numeros exactos. solucionado con el epsilon.
	//double truncX = truncPosition(_playerPosition.x);
	//double truncY = truncPosition(_playerPosition.y);
	double truncX = _playerPosition.x;
	double truncY = _playerPosition.y;

	double newPosX = truncX;
    double newPosY = truncY;

	switch (_storeDir)
	{	
		case _up:
			if ((fmod(truncX, 2)>-_epsilon) && (fmod(truncX, 2)<_epsilon))
			{
				_playerDirection=_up;
				_storeDir=_stop;
				updatePlayer();
			}
			break;
		case _down:
			if ((fmod(truncX, 2)>-_epsilon) && (fmod(truncX, 2)<_epsilon))
			{
				_playerDirection=_down;
				_storeDir=_stop;
				updatePlayer();
			}
			break;
		case _right:
			if ((fmod(truncY, 2)>-_epsilon) && (fmod(truncY, 2)<_epsilon))
			{
				_playerDirection=_right;
				_storeDir=_stop;
				updatePlayer();
			}
			break;
		case _left:
			if ((fmod(truncY, 2)>-_epsilon) && (fmod(truncY, 2)<_epsilon))
			{
				_playerDirection=_left;
				_storeDir=_stop;
				updatePlayer();
			}
			break;
		default:
			break;
	}

	int boardPosX = newPosX/2-1;
    int boardPosY = abs(newPosY/2)-1;   

	switch (_playerDirection)
	{	
		case _up:
			//_nodePlayer->setPosition(truncX,truncY+0.05,_playerPosition.z);
			newPosY = truncY+0.05;
            boardPosY = abs(newPosY/2)-1; 
			break;
		case _down:
			//_nodePlayer->setPosition(truncX,truncY-0.05,_playerPosition.z);
			newPosY = truncY-0.05;
            boardPosY = abs(newPosY/2); 
			break;
		case _right:
			//_nodePlayer->setPosition(truncX+0.05,truncY,_playerPosition.z);
            newPosX = truncX+0.05;
            boardPosX=newPosX/2;
			break;
		case _left:
			//_nodePlayer->setPosition(truncX-0.05,truncY,_playerPosition.z);
            newPosX = truncX-0.05;
            boardPosX=newPosX/2-1;
			break;
		default:
			break;
	}
	
	if( _playerDirection!=_stop && _boardInfo[(int)boardPosY][(int)boardPosX] != WALL )
    {
        //En la nueva posición no encontraremos una pared, actualizamos posición.
        //Si hubiera pared, paramos hasta nueva pulsación
        _nodePlayer->setPosition(newPosX,newPosY,_playerPosition.z);    
           
        if( _boardInfo[(int)boardPosY][(int)boardPosX] == BALL )
        {
            //la nueva posición tiene una bola, nos la comemos!
        }
    }
    else
    {
        if( _playerDirection!=_stop )
        {   
            switch (_playerDirection)
	        {	
		        case _up:
                     _nodePlayer->setPosition((boardPosX+1)*2,((boardPosY+2)*-2),_playerPosition.z);
			        break;
		        case _down:
                    _nodePlayer->setPosition((boardPosX+1)*2,((boardPosY)*-2),_playerPosition.z);
			        break;
		        case _right:
			        _nodePlayer->setPosition((boardPosX)*2,((boardPosY+1)*-2),_playerPosition.z);
			        break;
		        case _left:
                   _nodePlayer->setPosition((boardPosX+2)*2,((boardPosY+1)*-2),_playerPosition.z);
			        break;
		        default:
			        break;
	        }   
            _playerDirection =_stop;
        }    
    }

	//Ogre::Real deltaT = evt.timeSinceLastFrame;
	//int fps = 1.0 / deltaT;
	_scoreOverlay = _overlayMgr->getOverlayElement("scoreLabel");
	_scoreOverlay->setCaption("Score");
	
	//if (_arrayNodeBalls.size() == 160) popState();

	return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  // Tecla p --> PauseState.
  if (e.key == OIS::KC_P) {
    pushState(PauseState::getSingletonPtr());
  }
  if (e.key == OIS::KC_UP) {
	//_viewport->setBackgroundColour(Ogre::ColourValue(1.0, 1.0, 0.0));
	//Ogre::Vector3 pos = _nodePlayer->getPosition();
	//_nodePlayer->setPosition(pos.x,pos.y+0.1,pos.z);
	//_playerDirection = _up;
	  _storeDir = _up;
  }
  if (e.key == OIS::KC_DOWN) {
	//_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 1.0));
	//Ogre::Vector3 pos = _nodePlayer->getPosition();
	//_nodePlayer->setPosition(pos.x,pos.y-0.1,pos.z);
	//_playerDirection = _down;
	  _storeDir = _down;
  }
  if (e.key == OIS::KC_LEFT) {
	//_viewport->setBackgroundColour(Ogre::ColourValue(1.0, 0.0, 1.0));
	//Ogre::Vector3 pos = _nodePlayer->getPosition();
	//_nodePlayer->setPosition(pos.x-0.1,pos.y,pos.z);
	//_playerDirection = _left;
	  _storeDir = _left;
  }
  if (e.key == OIS::KC_RIGHT) {
	//_viewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 1.0));
	//Ogre::Vector3 pos = _nodePlayer->getPosition();
	//_nodePlayer->setPosition(pos.x+0.1,pos.y,pos.z);
	//_playerDirection = _right;
	  _storeDir = _right;
  }
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }
}

void
PlayState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

// Adding methods
void
PlayState::createScene()
{
	// imagen de fondo
	//_sceneMgr->setSkyDome(true, "skyMat", 5, 1);
	/*
	// Creacion del plano
	Ogre::Plane pl1(Ogre::Vector3::UNIT_Y,-5);
	Ogre::MeshManager::getSingleton().createPlane("pl1",
	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		pl1,200,200,1,1,true,1,20,20,Ogre::Vector3::UNIT_Z);

	// Añadir el plano a la escena
	Ogre::SceneNode* nodeG = _sceneMgr->createSceneNode("nodeG");
	Ogre::Entity* grEnt = _sceneMgr->createEntity("pEnt", "pl1");
	grEnt->setMaterialName("Ground");
	nodeG->attachObject(grEnt);
	*/
	// Sombras
	_sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Light* light = _sceneMgr->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));
	//nodeG->attachObject(light);
	//_sceneMgr->getRootSceneNode()->addChild(nodeG);

	// personaje

	Ogre::Entity* ent1;
	ent1 = _sceneMgr->createEntity("player.mesh");
	_nodePlayer = _sceneMgr->createSceneNode();
	_nodePlayer->attachObject(ent1);
	_nodePlayer->translate(22,-26,0);
	_sceneMgr->getRootSceneNode()->addChild(_nodePlayer);

}


void
PlayState::createStage()
{
	
	Book* book = xlCreateBook();
    Format* format;
	Color color;
	int inicio = 1;
	int fin = 21;
	const char* s;
	double r;
	
						
	if(book)
    {
        if(book->load("example.xls"))
        {
            Sheet* sheet = book->getSheet(0);
            if(sheet)
            {
				for (int i=1; i<fin; i++)
				{
					for (int j=1; j<fin; j++)
					{
						
						s = sheet->readStr(j, i, &format);
						
						color = format->patternForegroundColor();
						if (color == Color::COLOR_OCEANBLUE_CF)
						{
							Ogre::Entity* ent1;
							ent1 = _sceneMgr->createEntity("wall.mesh");
							Ogre::SceneNode* node1 = _sceneMgr->createSceneNode();
							node1->attachObject(ent1);
							//node1->setPosition(-9.4321+(i*2),20.6799-(j*2),-60.9970);
							node1->setPosition((i*2),-(j*2),0);
							_sceneMgr->getRootSceneNode()->addChild(node1);
                            _boardInfo[j-1][i-1] = WALL;
						}
						
						if (color == Color::COLOR_BLACK)
						{
							Ogre::Entity* ent1;
							ent1 = _sceneMgr->createEntity("ball.mesh");
							Ogre::SceneNode* node1 = _sceneMgr->createSceneNode();
							node1->attachObject(ent1);
							//node1->setPosition(-9.4321+(i*2),20.6799-(j*2),-60.9970);
							node1->setPosition((i*2),-(j*2),0);
							_sceneMgr->getRootSceneNode()->addChild(node1);
							_arrayNodeBalls.push_back(node1);
                            _boardInfo[j-1][i-1] = BALL;
						}
						
					}
				}
            }
        }
        else
        {
            std::cout << "At first run generate !" << std::endl;
        }
        book->release();
    }
}

void
PlayState::checkBalls()
{
	double _ballPositionX = 0;
	double _ballPositionY = 0;
	
	std::list<Ogre::SceneNode*>::iterator list_iter = _arrayNodeBalls.begin();
	while (list_iter != _arrayNodeBalls.end())
	{
		_playerPosition = _nodePlayer->getPosition();
		_ballPositionX = (*list_iter)->getPosition().x;
		_ballPositionY = (*list_iter)->getPosition().y;

		if ( ( abs(_playerPosition.x - _ballPositionX) < _epsilon) &&
			 ( abs(_playerPosition.y - _ballPositionY) < _epsilon))
		{	 
			(*list_iter)->detachObject((unsigned short)0);
			//std::list<Ogre::SceneNode*>::iterator delElement = list_iter;
			list_iter = _arrayNodeBalls.erase(list_iter);
			_score++;
			_scoreOverlay = _overlayMgr->getOverlayElement("scoreValue");
			_scoreOverlay->setCaption(Ogre::StringConverter::toString(_score));
		}
		list_iter++;
	}

}

double // Eliminar. no esta en uso.
PlayState::truncPosition(double aPosition)
{
	int truncado = aPosition * 100;
	double devuelta = truncado/100.0;
	return devuelta;
	
}

void
PlayState::updatePlayer()
{
	_nodePlayer->setOrientation(_nodePlayer->getInitialOrientation());

	switch (_playerDirection)
	{
		case _up:
				_nodePlayer->roll(Ogre::Degree(90));
				break;
		case _down:
				_nodePlayer->roll(Ogre::Degree(-90));
				break;
		case _right:
				_nodePlayer->roll(Ogre::Degree(0));
				break;
		case _left:
				_nodePlayer->roll(Ogre::Degree(180));
				break;
	}

}

// End Adding methods
PlayState*
PlayState::getSingletonPtr ()
{
return msSingleton;
}

PlayState&
PlayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

