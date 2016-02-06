#include "PlayState.h"
#include "PauseState.h"
#include <iostream>
#include <conio.h>
#include "libxl.h"
#include <math.h>
#include <cmath>


using namespace libxl;

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

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
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 1.0));
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
	_playerPosition = _nodePlayer->getPosition();
	
	double truncX = truncPosition(_playerPosition.x);
	double truncY = truncPosition(_playerPosition.y);

	switch (_storeDir)
	{	
		case _up:
			if ((fmod(truncX, 2)>-0.1) && (fmod(truncX, 2)<0.1))
			{
				_playerDirection=_up;
				_storeDir=_stop;
			}
			break;
		case _down:
			if ((fmod(truncX, 2)>-0.1) && (fmod(truncX, 2)<0.1))
			{
				_playerDirection=_down;
				_storeDir=_stop;
			}
			break;
		case _right:
			if ((fmod(truncY, 2)>-0.1) && (fmod(truncY, 2)<0.1))
			{
				_playerDirection=_right;
				_storeDir=_stop;
			}
			break;
		case _left:
			if ((fmod(truncY, 2)>-0.1) && (fmod(truncY, 2)<0.1))
			{
				_playerDirection=_left;
				_storeDir=_stop;
			}
			break;
		default:
			break;
	}


	switch (_playerDirection)
	{	
		case _up:
			_nodePlayer->setPosition(truncX,truncY+0.05,_playerPosition.z);
			break;
		case _down:
			_nodePlayer->setPosition(truncX,truncY-0.05,_playerPosition.z);
			break;
		case _right:
			_nodePlayer->setPosition(truncX+0.05,truncY,_playerPosition.z);
			break;
		case _left:
			_nodePlayer->setPosition(truncX-0.05,truncY,_playerPosition.z);
			break;
		default:
			break;
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

double
PlayState::truncPosition(double aPosition)
{
	int truncado = aPosition * 100;
	double devuelta = truncado/100.0;
	return devuelta;
	
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

