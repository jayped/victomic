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
  _sceneMgr->clearScene();
  _overlay->hide();
  //_root->getAutoCreatedWindow()->removeAllViewports();
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
		movingGhostsInitial();
		movingNewPos( _nodePlayer );
		updatePlayer();
		checkBalls();
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
    _nodePlayer->init(Actor::direction(_stop), 0.05F);
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
}

void
PlayState::checkBalls()
{
    if( !isStop )
    {
	    double _ballPositionX = 0;
	    double _ballPositionY = 0;
        //Solo chequeamos hasta que nos comemos una bola
	    bool exit = false;
	    std::list<Ogre::SceneNode*>::iterator list_iter = _arrayNodeBalls.begin();
	    while (list_iter != _arrayNodeBalls.end() && !exit && !isStop )
	    {
		    _playerPosition = _nodePlayer->getPosition();
		    _ballPositionX = (*list_iter)->getPosition().x;
		    _ballPositionY = (*list_iter)->getPosition().y;

		    if ( ( abs(_playerPosition.x - _ballPositionX) < _epsilon) &&
			     ( abs(_playerPosition.y - _ballPositionY) < _epsilon))
		    {	 
			    _gameMgr->playWaka();
			    (*list_iter)->detachObject((unsigned short)0);
			    list_iter = _arrayNodeBalls.erase(list_iter);
			    _score++;
			    _scoreOverlay = _overlayMgr->getOverlayElement("scoreValue");
			    _scoreOverlay->setCaption(Ogre::StringConverter::toString(_score));
                exit = true;
		    }
		    list_iter++;
	    }

	    if (_arrayNodeBalls.empty())
	    {
		    _gameMgr->updateScore(_score);
		    _arrayNodeBalls.clear();
		    changeState(ReplayState::getSingletonPtr());

		    //_exitGame = true;
	    }
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
	
    playerPositionX = _nodePlayer->getPosition().x;
    playerPositionY = _nodePlayer->getPosition().y;
    std::list<Actor*>::iterator pos;
    pos = _nodesGhost.begin();
    switch (_nodePlayer->getDirection())
	{	
		case Actor::_up:
			playerPositionY +=1;
			break;
		case Actor::_down:
			playerPositionY -=1;
			break;
		case Actor::_right:
            playerPositionX +=1;
			break;
		case Actor::_left:
           playerPositionX -=1;
			break;
		default:
			break;
	}

    while( pos != _nodesGhost.end() && !exit )
    {
		ghostPositionX = (*pos)->getPosition().x;
		ghostPositionY = (*pos)->getPosition().y;

        pos++;
		if ( ( abs(playerPositionX - ghostPositionX) < _epsilon) &&
			 ( abs(playerPositionY - ghostPositionY) < _epsilon))
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
	    double newPosX = (*pos)->getPosition().x;
        double newPosY = (*pos)->getPosition().y;
        double newPosZ = (*pos)->getPosition().z;
        Actor::direction actorDir = (*pos)->getDirection();
        Actor::direction storeDir = (*pos)->getStoreDir();

        int boardPosX = newPosX/2-1;
        int boardPosY = abs(newPosY/2)-1;   

	    switch (storeDir)
	    {	
		    case _up:
			    if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			    {
				    (*pos)->setDirection(Actor::_up);
				    (*pos)->setStoreDir(Actor::_stop);
			    }
			    break;
		    case _down:
			    if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			    {
				    (*pos)->setDirection(Actor::_down);
				    (*pos)->setStoreDir(Actor::_stop);
			    }
			    break;
		    case _right:
			    if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			    {
				
                    (*pos)->setDirection(Actor::_right);
				    (*pos)->setStoreDir(Actor::_stop);
                }
			    break;
		    case _left:
			    if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			    {
				    (*pos)->setDirection(Actor::_left);
				    (*pos)->setStoreDir(Actor::_stop);
			    }
			    break;
		    default:
			    break;
	    }

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
        }
        else
        {
            if( (*pos)->getDirection()!=_stop )
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
               // _storeDir = storeDir;
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

	switch (storeDir)
	{	
		case _up:
			if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			{
				actor->setDirection(Actor::_up);
				actor->setStoreDir(Actor::_stop);
			}
			break;
		case _down:
			if ((fmod(newPosX, 2)>-_epsilon) && (fmod(newPosX, 2)<_epsilon))
			{
				actor->setDirection(Actor::_down);
				actor->setStoreDir(Actor::_stop);
			}
			break;
		case _right:
			if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			{
				
                actor->setDirection(Actor::_right);
				actor->setStoreDir(Actor::_stop);
            }
			break;
		case _left:
			if ((fmod(newPosY, 2)>-_epsilon) && (fmod(newPosY, 2)<_epsilon))
			{
				actor->setDirection(Actor::_left);
				actor->setStoreDir(Actor::_stop);
			}
			break;
		default:
			break;
	}

	switch (actor->getDirection())
	{	
		case Actor::_up:
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
            newPosX -=actor->getSpeed();
            boardPosX=newPosX/2-1;
			break;
		default:
			break;
	}
	
	if( actor->getDirection()!=_stop && 
        _boardInfo[(int)boardPosY][(int)boardPosX] != WALL) 
    {
        //En la nueva posición no encontraremos una pared, actualizamos posición.
        //Si hubiera pared, paramos hasta nueva pulsación
        actor->setPosition(newPosX,newPosY,newPosZ);  
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
        if( actor->getDirection()!=_stop )
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
            // _storeDir = storeDir;
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

