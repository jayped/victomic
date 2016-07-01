#include "ReplayState.h"
#include "PlayState.h"

template<> ReplayState* Ogre::Singleton<ReplayState>::msSingleton = 0;

void
ReplayState::enter ()
{
	bool mapFit=false;
	if (!_root)
	{
	  _root = Ogre::Root::getSingletonPtr();
	  _gameMgr = GameManager::getSingletonPtr();

	  // Se recupera el gestor de escena y la cámara.
	  _sceneMgr = _root->getSceneManager("SceneManager");
	  
	  _camera = _sceneMgr->getCamera("IntroCamera");
	  //_viewport = _root->getAutoCreatedWindow()->getViewport(0);
	  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
	  // Nuevo background colour.
	  //_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 1.0));
	  _overlayMgr = Ogre::OverlayManager::getSingletonPtr();

	  mapFit=false;
	}

	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
	overlay = _overlayMgr->getByName("Avance");
	Ogre::OverlayElement *mapOverlay = _overlayMgr->getOverlayElement("avance");

	if (mapFit==false)
	{
		mapFit=true;
		double width = _viewport->getActualWidth();
		double height = _viewport->getActualHeight();
		double aspecRatioPanel = mapOverlay->getWidth()/mapOverlay->getHeight();
		double reduction = 0.8;

		mapOverlay->setDimensions(height*aspecRatioPanel*reduction,height*reduction);
		mapOverlay->setPosition(-(mapOverlay->getWidth()/2),-mapOverlay->getHeight()/2);

	}
	
/*
  _ReplayOverlay = _overlayMgr->getOverlayElement("reTitleLabel");
  _ReplayOverlay->setCaption("Game results:");
  _ReplayOverlay = _overlayMgr->getOverlayElement("reScoreLabel");
  _ReplayOverlay->setCaption("Score");
  _ReplayOverlay = _overlayMgr->getOverlayElement("reScoreDataLabel");
  _ReplayOverlay->setCaption(Ogre::StringConverter::toString(_gameMgr->_currentStage));
  _ReplayOverlay = _overlayMgr->getOverlayElement("reHiscoreLabel");
  _ReplayOverlay->setCaption("HiScore");
  _ReplayOverlay = _overlayMgr->getOverlayElement("reHiscoreDataLabel");
  //_ReplayOverlay->setCaption(Ogre::StringConverter::toString(_gameMgr->_hiscore));
  _ReplayOverlay = _overlayMgr->getOverlayElement("reInfoLabel");
  _ReplayOverlay->setCaption("PRESS SPACE TO REPLAY...");
  */

  overlay->show();
  //_ReplayOverlay->show();
	//_gameMgr->playEnd();

  _exitGame = false;
}

void
ReplayState::exit ()
{
  overlay->hide();
  //_ReplayOverlay->hide();
  	_gameMgr->playMenu();

}

void
ReplayState::pause ()
{
}

void
ReplayState::resume ()
{
}

bool
ReplayState::frameStarted
(const Ogre::FrameEvent& evt)
{

  return true;
}

bool
ReplayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
ReplayState::keyPressed
(const OIS::KeyEvent &e) {
  if (e.key == OIS::KC_SPACE) {
	changeState(PlayState::getSingletonPtr());
  }
  if (e.key == OIS::KC_ESCAPE) {
	_exitGame = true;
  }
}

void
ReplayState::keyReleased
(const OIS::KeyEvent &e)
{
}

void
ReplayState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
ReplayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
ReplayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

bool ReplayState::axisMoved( const OIS::JoyStickEvent &e, int axis ){
    return true;
}
bool ReplayState::buttonPressed( const OIS::JoyStickEvent &e, int button ){
    return true;
}
bool ReplayState::buttonReleased( const OIS::JoyStickEvent &e, int button ){
    return true;
}


ReplayState*
ReplayState::getSingletonPtr ()
{
return msSingleton;
}

ReplayState&
ReplayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}
