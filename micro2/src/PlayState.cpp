#include "PlayState.h"
#include "PauseState.h"

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

  _camera = _sceneMgr->getCamera("IntroCamera");
  _camera->setPosition(Ogre::Vector3(10,10,2));
  _camera->lookAt(Ogre::Vector3(0,0,0));
  _camera->setDirection(Ogre::Vector3(-0.006,-0.165,-0.9621));
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

	_playerDirection = _stop;
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
	switch (_playerDirection)
	{	
		case _up:
			_nodePlayer->setPosition(_playerPosition.x,_playerPosition.y+0.005,_playerPosition.z);
			break;
		case _down:
			_nodePlayer->setPosition(_playerPosition.x,_playerPosition.y-0.005,_playerPosition.z);
			break;
		case _right:
			_nodePlayer->setPosition(_playerPosition.x+0.005,_playerPosition.y,_playerPosition.z);
			break;
		case _left:
			_nodePlayer->setPosition(_playerPosition.x-0.005,_playerPosition.y,_playerPosition.z);
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
	_playerDirection = _up;
  }
  if (e.key == OIS::KC_DOWN) {
	//_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 1.0));
	//Ogre::Vector3 pos = _nodePlayer->getPosition();
	//_nodePlayer->setPosition(pos.x,pos.y-0.1,pos.z);
	_playerDirection = _down;
  }
  if (e.key == OIS::KC_LEFT) {
	//_viewport->setBackgroundColour(Ogre::ColourValue(1.0, 0.0, 1.0));
	//Ogre::Vector3 pos = _nodePlayer->getPosition();
	//_nodePlayer->setPosition(pos.x-0.1,pos.y,pos.z);
	_playerDirection = _left;
  }
  if (e.key == OIS::KC_RIGHT) {
	//_viewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 1.0));
	//Ogre::Vector3 pos = _nodePlayer->getPosition();
	//_nodePlayer->setPosition(pos.x+0.1,pos.y,pos.z);
	_playerDirection = _right;
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
	_sceneMgr->setSkyDome(true, "skyMat", 5, 1);

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

	// Sombras
	_sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Light* light = _sceneMgr->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));
	nodeG->attachObject(light);
	_sceneMgr->getRootSceneNode()->addChild(nodeG);

	// personaje
	Ogre::Entity* ent1;
	ent1 = _sceneMgr->createEntity("CuboAgua.mesh");
	_nodePlayer = _sceneMgr->createSceneNode();
	_nodePlayer->attachObject(ent1);
	_nodePlayer->translate(2.2,2.2,-50);
	_sceneMgr->getRootSceneNode()->addChild(_nodePlayer);

}


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

