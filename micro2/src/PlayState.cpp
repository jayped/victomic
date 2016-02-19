#include "ReplayState.h"
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
    _root = 0;
    isInitialMove = true;
    isStop = false;
    //Se inicializa el tablero.
    for(int i = 0; i < BOARDSIZE; i++ )
    {
        for( int j = 0; j < BOARDSIZE; j++ )
        {
            _boardInfo[i][j] = EMPTY;
            _arrayNodeBalls[i][j] = 0;
        }
    }
}

void
PlayState::enter ()
{
	if (!_root)
	{
		_root = Ogre::Root::getSingletonPtr();
		_gameMgr = GameManager::getSingletonPtr();

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
	}
     isStop = false;
	_exitGame = false;
	 isInitialMove = true;

	// crea el escenario y el fondo;
	createScene();
	createStage();
	_gameMgr->playIntro();

	
	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
  
	_overlay = _overlayMgr->getByName("Score");
	
	_score = 0;
	_hiscore = _gameMgr->_hiscore;
	_introCounter=0;
    _timerGhostDir = 0;

	_scoreOverlay = _overlayMgr->getOverlayElement("scoreLabel");
	_scoreOverlay->setCaption("Score");
	_scoreOverlay = _overlayMgr->getOverlayElement("scoreValue");
	_scoreOverlay->setCaption(Ogre::StringConverter::toString(_score));
	
	_scoreOverlay = _overlayMgr->getOverlayElement("scoreLabelHi");
	_scoreOverlay->setCaption("HiScore");
	_scoreOverlay = _overlayMgr->getOverlayElement("hiscoreValueHi");
	_scoreOverlay->setCaption(Ogre::StringConverter::toString(_hiscore));

	_overlay->show();


}

void
PlayState::exit ()
{
    std::list<Actor*>::iterator pos = _nodesGhost.begin();;

    while( pos != _nodesGhost.end() )
    {
        (*pos)->detachObject((unsigned short)0);
        pos =_nodesGhost.erase( pos );
    }
    _nodesGhost.clear();	    
    
    for(int i = 0; i < BOARDSIZE; i++ )
    {
        for( int j = 0; j < BOARDSIZE; j++ )
        {
            _boardInfo[i][j] = EMPTY;
            if( _arrayNodeBalls[i][j] != 0 )
            {
                _arrayNodeBalls[i][j]->detachObject((unsigned short)0);
                //delete _arrayNodeBalls[i][j];
                _arrayNodeBalls[i][j] = 0;           
            }  
        }
    }
    
    _sceneMgr->clearScene();
    _overlay->hide();
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
	Ogre::Real deltaT = evt.timeSinceLastFrame;
	_introCounter = _introCounter + evt.timeSinceLastFrame;
    

	if (_introCounter>4.5)
	{
        _timerGhostDir +=  evt.timeSinceLastFrame;
        if( _timerGhostDir > Ogre::Math::RangeRandom(3.0, 5.0) )
        {
            std::list<Actor*>::iterator pos;
            pos = _nodesGhost.begin();
            while( pos != _nodesGhost.end())
            {
                (*pos)->setStoreDir(Actor::direction::_random);
                pos++;
            }
            _timerGhostDir = 0;
        }
		movingGhostsInitial();
		movingNewPos( _nodePlayer );
        checkGhost();
        
	}

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
	  _nodePlayer->setStoreDir (Actor::direction(_up));

  }
  if (e.key == OIS::KC_DOWN) {
	  _nodePlayer->setStoreDir (Actor::direction(_down));
  }
  if (e.key == OIS::KC_LEFT) {
	   _nodePlayer->setStoreDir (Actor::direction(_left)); 
  }
  if (e.key == OIS::KC_RIGHT) {
	 _nodePlayer->setStoreDir (Actor::direction(_right)); 
  }

  // Fin de juego y Replay
  if (e.key == OIS::KC_Z) {
	
	//_sceneMgr->getRootSceneNode()->removeAndDestroyAllChildren();
	//_sceneMgr->clearScene();
	
	//_gameMgr->updateScore(_score);
	//_arrayNodeBalls.clear();
	//changeState(ReplayState::getSingletonPtr());
  }


}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  if (e.key == OIS::KC_ESCAPE) {
    _root->getAutoCreatedWindow()->removeAllViewports();
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
	_nodePlayer = reinterpret_cast<Actor *>(_sceneMgr->createSceneNode());
    _nodePlayer->init(Actor::direction(_stop), 0.10F);
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
	bool spawnDetected=false;
						
	if(book)
    {
        if(book->load("example.xls"))
        {
            Sheet* sheet = book->getSheet(0);
            if(sheet)
            {
				// hiscore
				_hiscore = sheet->readNum(1,24);

				for (int i=1; i<fin; i++)
				{
					for (int j=1; j<fin; j++)
					{
						
						s = sheet->readStr(j, i, &format);
						
						color = format->patternForegroundColor();
						if (color == Color::COLOR_OCEANBLUE_CF)
						{
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
							_arrayNodeBalls[j-1][i-1] = node1;
                            _boardInfo[j-1][i-1] = BALL;
						}

                        if(color == Color::COLOR_LIGHTTURQUOISE_CF)
                        {
                            //Detectado Spawn. Colocar fantasmas en la misma posición.
                            if( !spawnDetected )
                            {
                                spawnDetected = true;
                                createRedGhost((i*2), -(j*2), 0);
                                //createWhiteGhost((i*2), -(j*2), 0);
                                //createBlueGhost((i*2), -(j*2), 0);
                                createOrangeGhost((i*2), -(j*2), 0);
                                //createGreenGhost((i*2), -(j*2), 0);
                                createPinkGhost((i*2), -(j*2), 0);
                                createCyanGhost((i*2), -(j*2), 0);
                            }
                            _boardInfo[j-1][i-1] = SPAWN;
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

		// Obtiene el codigo para saber que tipo de muro es y lo solicita.
	for (int i=0; i<20; i++)
	{
		for (int j=0; j<20; j++)
		{
			if (_boardInfo[j][i] == WALL)
			{
				int localWall = 0;

				// Vecino arriba
				if (j==0) // fila de arriba
				{
					localWall += 0;
				}
				else
				{
					if (_boardInfo[j-1][i] == WALL)
					{
						localWall += 1;
					}
					else
					{
						localWall += 0;
					}
				}

				localWall = localWall << 1;
				// Vecino derecha
				if (i==19) // columna de derecha
				{
					localWall += 0;
				}
				else
				{
					if (_boardInfo[j][i+1] == WALL)
					{
						localWall += 1;
					}
					else
					{
						localWall += 0;
					}
				}

				localWall = localWall << 1;
				// Vecino abajo
				if (j==19) // fila de abajo
				{
					localWall += 0;
				}
				else
				{
					if (_boardInfo[j+1][i] == WALL)
					{
						localWall += 1;
					}
					else
					{
						localWall += 0;
					}
				}

				localWall = localWall << 1;
				// Vecino izquierda
				if (i==0) // columna de derecha
				{
					localWall += 0;
				}
				else
				{
					if (_boardInfo[j][i-1] == WALL)
					{
						localWall += 1;
					}
					else
					{
						localWall += 0;
					}
				}
			
				// inclusion del muro
				Ogre::SceneNode* node1 = getWallEntity(localWall);
				node1->setPosition(((i+1)*2),-((j+1)*2),0);
				_sceneMgr->getRootSceneNode()->addChild(node1);
			}
		}
	}
}

Ogre::SceneNode*
PlayState::getWallEntity(int aType)
{

	Ogre::Entity* ent1 = _sceneMgr->createEntity("wall.mesh");;
	Ogre::SceneNode* node1 = _sceneMgr->createSceneNode();

	switch (aType)
	{
		case 0:
			ent1 = _sceneMgr->createEntity("wall.mesh");
			node1 = _sceneMgr->createSceneNode();
			break;
		case 1:
			ent1 = _sceneMgr->createEntity("cornerWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(90));
			break;
		case 2:
			ent1 = _sceneMgr->createEntity("cornerWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(180));
			break;
		case 3:
			ent1 = _sceneMgr->createEntity("lWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(180));
			break;
		case 4:
			ent1 = _sceneMgr->createEntity("cornerWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(-90));
			break;
		case 5:
			ent1 = _sceneMgr->createEntity("parallelwall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(90));
			break;
		case 6:
			ent1 = _sceneMgr->createEntity("lWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(-90));
			break;
		case 7:
			ent1 = _sceneMgr->createEntity("onewall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(-90));
			break;
		case 8:
			ent1 = _sceneMgr->createEntity("cornerWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(0));
			break;
		case 9:
			ent1 = _sceneMgr->createEntity("lWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(90));
			break;
		case 10:
			ent1 = _sceneMgr->createEntity("parallelwall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(0));
			break;
		case 11:
			ent1 = _sceneMgr->createEntity("onewall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(180));
			break;
		case 12:
			ent1 = _sceneMgr->createEntity("lWall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(0));
			break;
		case 13:
			ent1 = _sceneMgr->createEntity("onewall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(90));
			break;
		case 14:
			ent1 = _sceneMgr->createEntity("onewall.mesh");
			node1 = _sceneMgr->createSceneNode();
			node1->roll(Ogre::Degree(0));
			break;
		case 15:
			//ent1 = _sceneMgr->createEntity("");
			break;
		default:
			break;
		
	}

	node1->attachObject(ent1);

	return node1;
}

void PlayState::eatBall(int posX, int posY)
{
    _boardInfo[(int)posX][(int)posY] = EMPTY;
    _gameMgr->playWaka();
    _arrayNodeBalls[posX][posY]->detachObject((unsigned short)0);
    //delete _arrayNodeBalls[posX][posY];
    _arrayNodeBalls[posX][posY] = 0;
    _score++;
    _scoreOverlay = _overlayMgr->getOverlayElement("scoreValue");
    _scoreOverlay->setCaption(Ogre::StringConverter::toString(_score));
}

void
PlayState::updatePlayer()
{
	_nodePlayer->setOrientation(_nodePlayer->getInitialOrientation());

	switch (_nodePlayer->getDirection())
	{
		case Actor::_up:
				_nodePlayer->roll(Ogre::Degree(90));
				break;
		case Actor::_down:
				_nodePlayer->roll(Ogre::Degree(-90));
				break;
		case Actor::_right:
				_nodePlayer->roll(Ogre::Degree(0));
				break;
		case Actor::_left:
				_nodePlayer->yaw(Ogre::Degree(180));
				break;
	}
}

void PlayState::movingGhosts()
{
    std::list<Actor*>::iterator pos;
    pos = _nodesGhost.begin();
    while( pos != _nodesGhost.end())
    {
        pos++;
    }
}
void PlayState::checkGhost()
{
    double ghostPositionX = 0.0;
	double ghostPositionY = 0.0;
    double playerPositionX = 0.0;
    double playerPositionY = 0.0;
    //Solo chequeamos hasta que nos chocamos la primera vez
	bool exit = false;
	std::list<Actor*>::iterator pos;
    pos = _nodesGhost.begin();
    while( pos != _nodesGhost.end() && !exit )
    {
		ghostPositionX = (*pos)->getPosition().x;
		ghostPositionY = (*pos)->getPosition().y;
        playerPositionX = _nodePlayer->getPosition().x;
        playerPositionY = _nodePlayer->getPosition().y;

        pos++;
		if ( ( ( abs(playerPositionX - ghostPositionX) < _epsilonghost) &&
			 ( abs(playerPositionY - (ghostPositionY-2) ) < _epsilonghost)) ||
              ( ( abs(playerPositionX - ghostPositionX) < _epsilonghost) &&
			 ( abs(playerPositionY - (ghostPositionY+2) ) < _epsilonghost)) ||
              ( ( abs(playerPositionX - (ghostPositionX-2)) < _epsilonghost) &&
			 ( abs(playerPositionY - ghostPositionY ) < _epsilonghost)) ||
             ( ( abs(playerPositionX - (ghostPositionX+2)) < _epsilonghost) &&
			 ( abs(playerPositionY - ghostPositionY ) < _epsilonghost)))
		{	 
            exit = true;
			_gameMgr->updateScore(_score);
            changeState(ReplayState::getSingletonPtr());
		}
	}
}
void PlayState::movingGhostsInitial()
{
    
    std::list<Actor*>::iterator pos;
    pos = _nodesGhost.begin();
    while( pos != _nodesGhost.end())
    {
     bool isMoving = false;
    double newPosX = (*pos)->getPosition().x;
    double newPosY = (*pos)->getPosition().y;
    double newPosZ = (*pos)->getPosition().z;
    Actor::direction actorDir = (*pos)->getDirection();
    Actor::direction storeDir = (*pos)->getStoreDir();

    int boardPosX = newPosX/2-1;
    int boardPosY = abs(newPosY/2)-1;   

    bool isPosible = false;

    if( storeDir!=Actor::_stop )
    {
        switch (storeDir)
	    {	
		    case Actor::_up:
                if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			    {
			        newPosY +=(*pos)->getSpeed();
                    boardPosY = abs(newPosY/2)-1; 
                    isPosible = true;
			    }
                break;
		    case Actor::_down:
                if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			    {
			        newPosY -=(*pos)->getSpeed();
                    boardPosY = abs(newPosY/2); 
                    isPosible = true;
                }
                 break;
		    case Actor::_right:
                if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			    {
                    newPosX += (*pos)->getSpeed();
                    boardPosX=newPosX/2;
                    isPosible = true;
                }
			    break;
		    case Actor::_left:
                if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			    {
                    newPosX -=(*pos)->getSpeed();
                    boardPosX=newPosX/2-1;
                    isPosible = true;
                }
			    break;
		    default:
			    break;
	    }
	    if( isPosible )
        {
	        if( _boardInfo[(int)boardPosY][(int)boardPosX] != WALL) 
            {
                //En la nueva posición no encontraremos una pared, actualizamos posición.
                //Si hubiera pared, paramos hasta nueva pulsación
                (*pos)->setDirection( storeDir );
                (*pos)->setStoreDir( Actor::_stop );
               
                if( isInitialMove )
                {
                    if( _boardInfo[(int)boardPosY][(int)boardPosX] == SPAWN )
                    {
                        (*pos)->setPosition(newPosX,newPosY,newPosZ);    
                    }
                    else
                    {
                         (*pos)->setDirection(Actor::direction::_random);  
                    }
                }
                else
                {
                    //En la nueva posición no encontraremos una pared, actualizamos posición.
                    //Si hubiera pared, paramos hasta nueva pulsación
                    (*pos)->setPosition(newPosX,newPosY,newPosZ);    
                }
                isMoving = true;  
            }
        }
    }
    if( !isMoving )
    {
        newPosX = (*pos)->getPosition().x;
            newPosY = (*pos)->getPosition().y;
            newPosZ = (*pos)->getPosition().z;
    
            boardPosX = newPosX/2-1;
            boardPosY = abs(newPosY/2)-1;   
        switch ((*pos)->getDirection())
	    {	
		    case Actor::_up:
			    newPosY +=(*pos)->getSpeed();
                boardPosY = abs(newPosY/2)-1; 
			    break;
		    case Actor::_down:
			    newPosY -=(*pos)->getSpeed();
                boardPosY = abs(newPosY/2); 
			    break;
		    case Actor::_right:
                newPosX += (*pos)->getSpeed();
                boardPosX=newPosX/2;
			    break;
		    case Actor::_left:
                newPosX -=(*pos)->getSpeed();
                boardPosX=newPosX/2-1;
			    break;
		    default:
			    break;
	    }

	    if( (*pos)->getDirection()!=_stop && 
            _boardInfo[(int)boardPosY][(int)boardPosX] != WALL) 
        {
            if( isInitialMove )
            {
                if( _boardInfo[(int)boardPosY][(int)boardPosX] == SPAWN )
                {
                    (*pos)->setPosition(newPosX,newPosY,newPosZ);    
                }
                else
                {
                        (*pos)->setDirection(Actor::direction::_random);  
                }
            }
            else
            {
                //En la nueva posición no encontraremos una pared, actualizamos posición.
                //Si hubiera pared, paramos hasta nueva pulsación
                (*pos)->setPosition(newPosX,newPosY,newPosZ);    
            }
               
            isMoving = true;  
        }
        else
        {
                switch ((*pos)->getDirection())
	        {	
		        case _up:
                    (*pos)->setPosition((boardPosX+1)*2,((boardPosY+2)*-2),newPosZ);
			        break;
		        case _down:
                    (*pos)->setPosition((boardPosX+1)*2,((boardPosY)*-2),newPosZ);
			        break;
		        case _right:
			        (*pos)->setPosition((boardPosX)*2,((boardPosY+1)*-2),newPosZ);
			        break;
		        case _left:
                    (*pos)->setPosition((boardPosX+2)*2,((boardPosY+1)*-2),newPosZ);
			        break;
		        default:
			        break;
	        } 
                (*pos)->setDirection(Actor::direction::_random);  
        }    
    }
        pos++;
    }
}
void PlayState::createRedGhost(int posX, int posY, int posZ)
{
   createGhost("redGhost.mesh", posX, posY, posZ);
}
void PlayState::createWhiteGhost(int posX, int posY, int posZ)
{
   createGhost("whiteGhost.mesh", posX, posY, posZ);
}
void PlayState::createBlueGhost(int posX, int posY, int posZ)
{
   createGhost("blueGhost.mesh", posX, posY, posZ);
}
void PlayState::createOrangeGhost(int posX, int posY, int posZ)
{
   createGhost("orangeGhost.mesh", posX, posY, posZ);
}
void PlayState::createGreenGhost(int posX, int posY, int posZ)
{
    createGhost("greenGhost.mesh", posX, posY, posZ);
}
void PlayState::createPinkGhost(int posX, int posY, int posZ)
{
   createGhost("pinkGhost.mesh", posX, posY, posZ);
}
void PlayState::createCyanGhost(int posX, int posY, int posZ)
{
   createGhost("CyanGhost.mesh", posX, posY, posZ);
}

void PlayState::createGhost(std::string ghost, int posX, int posY, int posZ)
{
    Ogre::Entity* ent1;
    ent1 = _sceneMgr->createEntity(ghost);
    Actor* actorNode = reinterpret_cast<Actor *>(_sceneMgr->createSceneNode());
    actorNode->init( Actor::direction(_up ) );
    actorNode->attachObject(ent1);
    actorNode->setPosition(posX,posY,0);
    _sceneMgr->getRootSceneNode()->addChild(actorNode);
    _nodesGhost.push_back(actorNode);
}
bool PlayState::movingNewPos( Actor* &actor )
{
    bool isMoving = false;
    double newPosX = actor->getPosition().x;
    double newPosY = actor->getPosition().y;
    double newPosZ = actor->getPosition().z;
    Actor::direction actorDir = actor->getDirection();
    Actor::direction storeDir = actor->getStoreDir();

    int boardPosX = newPosX/2-1;
    int boardPosY = abs(newPosY/2)-1;   
    int ballPosX = boardPosX;
    int ballPosY = boardPosY; 

    bool isPosible = false;

    if( storeDir!=Actor::_stop )
    {
        switch (storeDir)
	    {	
		    case Actor::_up:
                if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			    {
                    ballPosY =  abs((newPosY-2))/2 -1;
			        newPosY +=actor->getSpeed();
                    boardPosY = abs(newPosY/2)-1; 
                    isPosible = true;
			    }
                break;
		    case Actor::_down:
                if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			    {
			        newPosY -=actor->getSpeed();
                    boardPosY = abs(newPosY/2); 
                    isPosible = true;
                }
                 break;
		    case Actor::_right:
                if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			    {
                    newPosX += actor->getSpeed();
                    boardPosX=newPosX/2;
                    isPosible = true;
                }
			    break;
		    case Actor::_left:
                if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			    {
                    ballPosX =  (newPosX+2)/2 -1;
                    newPosX -=actor->getSpeed();
                    boardPosX=newPosX/2-1;
                    isPosible = true;
                }
			    break;
		    default:
			    break;
	    }
	    if( isPosible )
        {
            if( _boardInfo[(int)ballPosY][(int)ballPosX] == BALL )
            {
                //Nos comemos la bola
                eatBall( (int)ballPosY, (int)ballPosX );
            }

	        if( _boardInfo[(int)boardPosY][(int)boardPosX] != WALL) 
            {
                //En la nueva posición no encontraremos una pared, actualizamos posición.
                //Si hubiera pared, paramos hasta nueva pulsación
                actor->setDirection( storeDir );
                actor->setStoreDir( Actor::_stop );
                actor->setPosition(newPosX,newPosY,newPosZ);  
                updatePlayer();
                if( isInitialMove )
                {
                    //Pacman ya se ha movido, por lo que los fantasmas ya pueden salir de la zona
                    //de SPAWN
                    isInitialMove = false;
                }
                isMoving = true;  
            }
        }
    }
        if( !isMoving )
        {
            newPosX = actor->getPosition().x;
             newPosY = actor->getPosition().y;
             newPosZ = actor->getPosition().z;
    
             boardPosX = newPosX/2-1;
             boardPosY = abs(newPosY/2)-1;   
             ballPosX = boardPosX;
             ballPosY = boardPosY; 
            switch (actor->getDirection())
	        {	
		        case Actor::_up:
                    ballPosY =  abs((newPosY-2))/2 -1;
			        newPosY +=actor->getSpeed();
                    boardPosY = abs(newPosY/2)-1; 
			        break;
		        case Actor::_down:
			        newPosY -=actor->getSpeed();
                    boardPosY = abs(newPosY/2); 
			        break;
		        case Actor::_right:
                    newPosX += actor->getSpeed();
                    boardPosX=newPosX/2;
			        break;
		        case Actor::_left:
                    ballPosX =  (newPosX+2)/2 -1;
                    newPosX -=actor->getSpeed();
                    boardPosX=newPosX/2-1;
			        break;
		        default:
			        break;
	        }
	
            if( _boardInfo[(int)ballPosY][(int)ballPosX] == BALL )
            {
                //Nos comemos la bola
                eatBall( (int)ballPosY, (int)ballPosX );
            }

	        if( actor->getDirection()!=_stop && 
                _boardInfo[(int)boardPosY][(int)boardPosX] != WALL) 
            {
                //En la nueva posición no encontraremos una pared, actualizamos posición.
                //Si hubiera pared, paramos hasta nueva pulsación
                actor->setPosition(newPosX,newPosY,newPosZ);  
                updatePlayer();
                if( isInitialMove )
                {
                    //Pacman ya se ha movido, por lo que los fantasmas ya pueden salir de la zona
                    //de SPAWN
                    isInitialMove = false;
                }
                isMoving = true;  
            }
            else
            {
                 switch (actor->getDirection())
	            {	
		            case _up:
                        actor->setPosition((boardPosX+1)*2,((boardPosY+2)*-2),newPosZ);
			            break;
		            case _down:
                        actor->setPosition((boardPosX+1)*2,((boardPosY)*-2),newPosZ);
			            break;
		            case _right:
			           actor->setPosition((boardPosX)*2,((boardPosY+1)*-2),newPosZ);
			            break;
		            case _left:
                       actor->setPosition((boardPosX+2)*2,((boardPosY+1)*-2),newPosZ);
			            break;
		            default:
			            break;
	            } 
                    actor->setDirection(actorDir);  
                    updatePlayer();
                
            }    
        }
    return isMoving;
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