#include "EndGameState.h"
#include "PlayState.h"

template<> EndGameState* Ogre::Singleton<EndGameState>::msSingleton = 0;

void
EndGameState::enter ()
{
	bool mapFit=false;
	if (!_root)
	{
	  _root = Ogre::Root::getSingletonPtr();
	  _gameMgr = GameManager::getSingletonPtr();

	  // Se recupera el gestor de escena y la cámara.
	  _sceneMgr = _root->getSceneManager("SceneManager");
	  
	  _camera = _sceneMgr->getCamera("IntroCamera");
	  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
	  //_viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
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

  overlay->show();

  _exitGame = false;
}

void
EndGameState::exit ()
{
  overlay->hide();
  //_ReplayOverlay->hide();
  	_gameMgr->playMenu();

}

void
EndGameState::pause ()
{
}

void
EndGameState::resume ()
{
}

bool
EndGameState::frameStarted
(const Ogre::FrameEvent& evt)
{

  return true;
}

bool
EndGameState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
EndGameState::keyPressed
(const OIS::KeyEvent &e) {
 
}

void EndGameState::keyReleased(const OIS::KeyEvent &e){
    //Desde este estado no se podrá volver a PlayState
    if (e.key == OIS::KC_ESCAPE) {
        _exitGame = true;
    }
}

void
EndGameState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
EndGameState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
EndGameState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

bool EndGameState::axisMoved( const OIS::JoyStickEvent &e, int axis ){
    return true;
}
bool EndGameState::buttonPressed( const OIS::JoyStickEvent &e, int button ){
    return true;
}
bool EndGameState::buttonReleased( const OIS::JoyStickEvent &e, int button ){
    return true;
}


EndGameState*
EndGameState::getSingletonPtr ()
{
return msSingleton;
}

EndGameState&
EndGameState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}
