#include "PauseState.h"

template<> PauseState* Ogre::Singleton<PauseState>::msSingleton = 0;

void
PauseState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  // Nuevo background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));

  _overlayMgr = Ogre::OverlayManager::getSingletonPtr();
  overlay = _overlayMgr->getByName("Pause");
  _pauseOverlay = _overlayMgr->getOverlayElement("pauseLabel");
  _pauseOverlay->setCaption("PAUSE");
  overlay->show();
  _pauseOverlay->show();
  _pauseCounter=0;
  _exitGame = false;
}

void
PauseState::exit ()
{
  overlay->hide();
  _pauseOverlay->hide();

}

void
PauseState::pause ()
{
}

void
PauseState::resume ()
{
}

bool
PauseState::frameStarted
(const Ogre::FrameEvent& evt)
{
	Ogre::Real deltaT = evt.timeSinceLastFrame;
	_pauseCounter = _pauseCounter + evt.timeSinceLastFrame;
	if (_pauseCounter>0.5)
	{
		_pauseCounter = 0.0;
		if (_pauseOverlay->isVisible())
			_pauseOverlay->hide();
		else
			_pauseOverlay->show();
	}

  return true;
}

bool
PauseState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
PauseState::keyPressed
(const OIS::KeyEvent &e) {
  // Tecla p --> Estado anterior.
  if (e.key == OIS::KC_P) {
    popState();
  }
}

void
PauseState::keyReleased
(const OIS::KeyEvent &e)
{
}

void
PauseState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
PauseState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
PauseState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

bool PauseState::axisMoved( const OIS::JoyStickEvent &e, int axis ){
    return true;
}
bool PauseState::buttonPressed( const OIS::JoyStickEvent &e, int button ){
    return true;
}
bool PauseState::buttonReleased( const OIS::JoyStickEvent &e, int button ){
    return true;
}

PauseState*
PauseState::getSingletonPtr ()
{
return msSingleton;
}

PauseState&
PauseState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}
