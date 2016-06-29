#include "IntroState.h"
#include "PlayState.h"
#include "ReplayState.h"


template<> IntroState* Ogre::Singleton<IntroState>::msSingleton = 0;

void
IntroState::enter ()
{
	
	if (!_root)
	{
		// Carga inicial.
		_root = Ogre::Root::getSingletonPtr();

		_sceneMgr = _root->getSceneManager("SceneManager");
		_sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

		_camera = _sceneMgr->createCamera("IntroCamera");
		_gameMgr = GameManager::getSingletonPtr();
	}
		//_gameMgr->playWhiteNoise();

		_camera->setPosition(Ogre::Vector3(0,0,90));
		_camera->lookAt(Ogre::Vector3(0,0,0));
		_camera->setNearClipDistance(5);
		_camera->setFarClipDistance(10000);

		_viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
		
		
		_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
		//Ogre::Overlay *overlaytv = _overlayMgr->getByName("aTV");
		overlay = _overlayMgr->getByName("Splash");

		_startOverlay = _overlayMgr->getOverlayElement("startLabel");
		_startOverlay->setCaption("PRESS SPACE TO START");
		_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
		_pressStartCounter = 0.0;
		_onConsole = 0.0;

		_exitGame = false;
		_isOn = false;


		// white noise inicial
		double width = _viewport->getActualWidth();
		double height = _viewport->getActualHeight();
		_camera->setAspectRatio(width / height);

	
		// plano pantalla
		// Creacion del plano
		Ogre::Plane pl1(Ogre::Vector3::UNIT_Z,30);
		Ogre::MeshManager::getSingleton().createPlane("pl1",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			pl1,7.1f,7.1f,1,1,true,1,1,1,Ogre::Vector3::UNIT_Y);
		
		Ogre::Plane pl2(Ogre::Vector3::UNIT_Z,30);
		Ogre::MeshManager::getSingleton().createPlane("pl2",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			pl2,5.1f,5.1f,1,1,true,1,1,1,Ogre::Vector3::UNIT_Y);

		// Añadir el plano a la escena
		nodeG = _sceneMgr->createSceneNode("nodeG");
		noriplane = _sceneMgr->createSceneNode("noriplane");
		noriplane->setPosition(Ogre::Vector3(0,0,-1));

		Ogre::Entity* grEnt = _sceneMgr->createEntity("pEnt", "pl1");
		Ogre::Entity* grEnt2 = _sceneMgr->createEntity("pEnt2", "pl2");
		grEnt->setMaterialName("intromat");
		grEnt2->setMaterialName("norimat");
		
		nodeG->attachObject(grEnt);
		noriplane->attachObject(grEnt2);
		_sceneMgr->getRootSceneNode()->addChild(nodeG);
		_sceneMgr->getRootSceneNode()->addChild(noriplane);

		// carga de Pantalla por la que va el jugador. en caso de no haber progreso se inicia en la 1
		_gameMgr->_currentStage=1;
		_gameMgr->loadStage();


}

void
IntroState::exit()
{
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
  Ogre::Overlay *overlay = _overlayMgr->getByName("Splash");
  overlay->hide();
  //_gameMgr->stopCoin(); // [!] Lanzar el sonido de logo.
  _gameMgr->stopMusicStart();
	_gameMgr->playMenu();

}

void
IntroState::pause ()
{
}

void
IntroState::resume ()
{
}

bool
IntroState::frameStarted
(const Ogre::FrameEvent& evt) 
{
	Ogre::Real deltaT = evt.timeSinceLastFrame;
	_pressStartCounter = _pressStartCounter + evt.timeSinceLastFrame;
	_onConsole = _onConsole + evt.timeSinceLastFrame;
	
	if (_pressStartCounter>0.5)
	{
		_pressStartCounter = 0.0;
		if (_startOverlay->isVisible())
			_startOverlay->hide();
		else
			_startOverlay->show();
	}
	
	if ((_onConsole>1) && !_isOn && noriplane->getPosition().y<3.5)
	{
		noriplane->setPosition(Ogre::Vector3(0,noriplane->getPosition().y+0.01,-1));
	}
	
	if ((_onConsole>2) && !_isOn)
	{
		//_gameMgr->playCoin();
	}
	if ((_onConsole>3) && !_isOn)
	{
		//_gameMgr->stopCoin();
		_isOn=true;
		nodeG->detachObject((unsigned short)0);
		overlay->show();
		_startOverlay->show();
		_gameMgr->playMusicStart();
	}
	
	if ((_onConsole>5) && !_UHFOFF)
	{
		_UHFOFF=true;
	}

	
	return true;
}

bool
IntroState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
IntroState::keyPressed
(const OIS::KeyEvent &e)
{
  // Transición al siguiente estado.
  // Espacio --> PlayState
  if (e.key == OIS::KC_SPACE) {
    changeState(ReplayState::getSingletonPtr());
  }
}

void
IntroState::keyReleased
(const OIS::KeyEvent &e )
{
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }
}

void
IntroState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
IntroState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
IntroState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

bool IntroState::axisMoved( const OIS::JoyStickEvent &e, int axis ){
    return true;
}
bool IntroState::buttonPressed( const OIS::JoyStickEvent &e, int button ){
    return true;
}
bool IntroState::buttonReleased( const OIS::JoyStickEvent &e, int button ){
    return true;
}


IntroState*
IntroState::getSingletonPtr ()
{
return msSingleton;
}

IntroState&
IntroState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}
