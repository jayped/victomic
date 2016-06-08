#include <Ogre.h>

#include "GameManager.h"
#include "GameState.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

template<> GameManager* Ogre::Singleton<GameManager>::msSingleton = 0;

GameManager::GameManager ()
{
  _root = 0;
}

GameManager::~GameManager ()
{
  while (!_states.empty()) {
    _states.top()->exit();
    _states.pop();
  }
  
  if (_root)
    delete _root;
}

void
GameManager::start
(GameState* state)
{
  _initSDL();

  // Creación del objeto Ogre::Root.
  _root = new Ogre::Root();
  // se crea aqui el scene manager para introducir el overlay, porque si se hace en el introstate, no recoge los resources overlays en loadresources.
  _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
  _root->getSceneManager("SceneManager")->addRenderQueueListener(new Ogre::OverlaySystem());
  
  loadResources();
  
  _pSoundFXManager = OGRE_NEW SoundFXManager;
  
  if (!configure())
    return;    
  	
  _inputMgr = new InputManager;
  _inputMgr->initialise(_renderWindow);

  // Registro como key y mouse listener...
  _inputMgr->addKeyListener(this, "GameManager");
  _inputMgr->addMouseListener(this, "GameManager");

  // El GameManager es un FrameListener.
  _root->addFrameListener(this);

  // CARGA DE SONIDOS
  
  _shotEffect = _pSoundFXManager->load("shot.wav");
  _introEffect = _pSoundFXManager->load("musicgame.wav");
  _coinEffect = _pSoundFXManager->load("coin.wav");
  _deathEffect = _pSoundFXManager->load("death.wav");
  _scoreEffect = _pSoundFXManager->load("puntua.wav");
  _bonusEffect = _pSoundFXManager->load("bonus.wav");
  _musicStartEffect = _pSoundFXManager->load("musicstart.wav");
  _gunEffect = _pSoundFXManager->load("gun.wav");
  _endEffect = _pSoundFXManager->load("end.wav");

  

  // Transición al estado inicial.
  changeState(state);

  // Bucle de rendering.
  _root->startRendering();


}

void
GameManager::changeState
(GameState* state)
{
  // Limpieza del estado actual.
  if (!_states.empty()) {
    // exit() sobre el último estado.
    _states.top()->exit();
    // Elimina el último estado.
    _states.pop();
  }

  // Transición al nuevo estado.
  _states.push(state);
  // enter() sobre el nuevo estado.
  _states.top()->enter();
}

void
GameManager::pushState
(GameState* state)
{
  // Pausa del estado actual.
  if (!_states.empty())
    _states.top()->pause();
  
  // Transición al nuevo estado.
  _states.push(state);
  // enter() sobre el nuevo estado.
  _states.top()->enter();
}

void
GameManager::popState ()
{
  // Limpieza del estado actual.
  if (!_states.empty()) {
    _states.top()->exit();
    _states.pop();
  }
  
  // Vuelta al estado anterior.
  if (!_states.empty())
    _states.top()->resume();
}

void
GameManager::loadResources ()
{
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");
  
  Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
  Ogre::String sectionstr, typestr, datastr;
  while (sI.hasMoreElements()) {
    sectionstr = sI.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) {
      typestr = i->first;    datastr = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation
            (datastr, typestr, sectionstr);	
    }
  }
}

bool
GameManager::configure ()
{
  if (!_root->restoreConfig()) {
    if (!_root->showConfigDialog()) {
      return false;
    }
  }
  
  _renderWindow = _root->initialise(true, "Game State Example");
  
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  
  return true;
}

void
GameManager::updateScore(int aNewScore)
{

	string line;
	ofstream myWritefile;
	ifstream myReadFile;
	const char* linechar;
 
	// check score
	_score = aNewScore;
	if (aNewScore > _hiscore)
	{
		_hiscore = aNewScore;
		myWritefile.open("pacmanhiscore.txt");
		myWritefile << Ogre::StringConverter::toString(_hiscore);
	}

	myWritefile.close();
}

void
GameManager::loadHiScore()
{
	string line;
	ofstream myWritefile;
	ifstream myReadFile;
	const char* linechar;
 
	myReadFile.open ("pacmanhiscore.txt");
	if (myReadFile.is_open())
	{
		std::getline(myReadFile,line);
		myReadFile.close();
 		linechar = line.c_str();
		_hiscore = atoi(linechar);
	}
	else
	{
		myWritefile.open("pacmanhiscore.txt");
		myWritefile << Ogre::StringConverter::toString(_hiscore);
	}

	
}

// Sonidos
void
GameManager::playShot()
{
	_shotEffect->play();
}
void
GameManager::stopShot()
{
	_shotEffect->stop();
}
void
GameManager::playIntro()
{
	_introEffect->play();
}
void
GameManager::playCoin()
{
	_coinEffect->play();
}
void
GameManager::stopCoin()
{
	_coinEffect->stop();
}
void
GameManager::playDeath()
{
	_deathEffect->play();
}
void
GameManager::stopDeath()
{
	_deathEffect->stop();
}

void
GameManager::playScore()
{
	_scoreEffect->play();
}

void
GameManager::playBonus()
{
	_bonusEffect->play();
}
void
GameManager::playMusicStart()
{
	_musicStartEffect->play();
}

void
GameManager::playGun()
{
	_gunEffect->play();
}

void
GameManager::playEnd()
{
	_endEffect->play();
}

GameManager*
// End sonidos
GameManager::getSingletonPtr ()
{
  return msSingleton;
}

GameManager&
GameManager::getSingleton ()
{  
  assert(msSingleton);
  return *msSingleton;
}

// Las siguientes funciones miembro delegan
// el evento en el estado actual.
bool
GameManager::frameStarted
(const Ogre::FrameEvent& evt)
{
  _inputMgr->capture();
  return _states.top()->frameStarted(evt);
}

bool
GameManager::frameEnded
(const Ogre::FrameEvent& evt)
{
  return _states.top()->frameEnded(evt);
}

bool
GameManager::keyPressed 
(const OIS::KeyEvent &e)
{
  _states.top()->keyPressed(e);
  return true;
}

bool
GameManager::keyReleased
(const OIS::KeyEvent &e)
{
  _states.top()->keyReleased(e);
  return true;
}

bool
GameManager::mouseMoved 
(const OIS::MouseEvent &e)
{
  _states.top()->mouseMoved(e);
  return true;
}

bool
GameManager::mousePressed 
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  _states.top()->mousePressed(e, id);
  return true;
}

bool
GameManager::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  _states.top()->mouseReleased(e, id);
  return true;
}

bool
GameManager::_initSDL() {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    return false;
  }
  // Llamar a  SDL_Quit al terminar.
  atexit(SDL_Quit);
 
  // Inicializando SDL mixer...
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,MIX_DEFAULT_CHANNELS, 4096) < 0) {
    return false;
  }
 
  // Llamar a Mix_CloseAudio al terminar.
  atexit(Mix_CloseAudio);
 
  return true;    
}
