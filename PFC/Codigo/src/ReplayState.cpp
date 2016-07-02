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
	_mapWidth = 0.0;

	_camera->setPosition(Ogre::Vector3(0,0,10));
	_camera->lookAt(Ogre::Vector3(0,0,0));
	_camera->setNearClipDistance(5);
	_camera->setFarClipDistance(10000);

	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
	overlay = _overlayMgr->getByName("Avance");
	Ogre::OverlayElement *mapOverlay = _overlayMgr->getOverlayElement("avance");
	_noriOverlay = _overlayMgr->getOverlayElement("noriState");
	_noriOverlay->setPosition(-(_noriOverlay->getWidth()/2),-(_noriOverlay->getHeight()/2));

    StagesManager * stagesMgr = StagesManager::getSingletonPtr();
	Stage * s = stagesMgr->getStage(_gameMgr->_currentStage);
	
	avancePantalla = _overlayMgr->getByName("AvanceStage");
	_avanceTest = _overlayMgr->getOverlayElement("AvanceTextLabel");
	_avanceTest->setCaption(Ogre::StringConverter::toString(s->world()) +"-"+ Ogre::StringConverter::toString(_gameMgr->_currentStage));
	avancePantalla->show();
	if (mapFit==false)
	{
		mapFit=true;
		double width = _viewport->getActualWidth();
		double height = _viewport->getActualHeight();
		double aspecRatioPanel = mapOverlay->getWidth()/mapOverlay->getHeight();
		double reduction = 0.8;

		mapOverlay->setDimensions(height*aspecRatioPanel*reduction,height*reduction);
		mapOverlay->setPosition(-(mapOverlay->getWidth()/2),-mapOverlay->getHeight()/2);

		_mapWidth = height*aspecRatioPanel*reduction;
		_mapHeight = height*reduction;

		double aspecRatioNori = _noriOverlay->getWidth()/_noriOverlay->getHeight();
		_noriOverlay->setDimensions(_mapHeight*aspecRatioNori*0.1,_mapHeight*0.1);
		_avanceTest->setPosition(0,(height/2)-(height/10));

	}
	_up=true;

	noriToWorld(s->world());
	
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
  	_sceneMgr->setSkyBox(true, "avancepapelmat", 100.0F, true);

	_gameMgr->playMapMusic();
  _exitGame = false;
}

void
ReplayState::exit ()
{
  overlay->hide();
  avancePantalla->hide();
  //_ReplayOverlay->hide();

  	_gameMgr->playMenu();

  	_gameMgr->stopMapMusic();
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
	Ogre::Real deltaT = evt.timeSinceLastFrame;
	_camera->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(deltaT*1.5)));
	
	/* nori moviendose
	if ((_acumulateY < _noriY + 10) && _up)
	{
		_acumulateY+=deltaT*50;
		_noriOverlay->setPosition(_noriX,_acumulateY);
	}
	if ((_acumulateY >= _noriY + 10) && _up)
	{
		_up=false;
		_acumulateY-=deltaT*50;
		_noriOverlay->setPosition(_noriX,_acumulateY);
	}
	if ((_acumulateY > _noriY - 10) && !_up)
	{
		_acumulateY-=deltaT*50;
		_noriOverlay->setPosition(_noriX,_acumulateY);
	}
	if ((_acumulateY <= _noriY - 10) && !_up)
	{
		_up=true;
		_acumulateY+=deltaT*50;
		_noriOverlay->setPosition(_noriX,_acumulateY);
	}
	*/

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
}

void
ReplayState::keyReleased
(const OIS::KeyEvent &e)
{
    if (e.key == OIS::KC_ESCAPE) {
        _exitGame = true;
    }
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

void ReplayState::noriToWorld(int aWorld)
{
	double block = _mapWidth/4;
	double x = (-block*2)+(block/2);

	_noriX = x+((aWorld-1)*block);
	_noriY = -_mapHeight/8;
	_acumulateY = _noriY;

	_noriOverlay->setPosition(_noriX,_noriY);
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
