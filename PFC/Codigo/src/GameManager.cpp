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
  _isRightMov = false;
  _isLeftMov = false;
  _isDownMov = false;
  _isUpMov = false;
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
  _inputMgr->addJoystickListener( this, "GameManager" );

  // El GameManager es un FrameListener.
  _root->addFrameListener(this);

  // CARGA DE SONIDOS
  
  _jumpEffect = _pSoundFXManager->load("sound_jump.wav");
  _menuEffect = _pSoundFXManager->load("sound_menu.wav");
  _breakEffect = _pSoundFXManager->load("sound_break.wav");
  _walkEffect = _pSoundFXManager->load("sound_walk.wav");
  //_switchEffect = _pSoundFXManager->load("sound_switch.wav");
  _goalEffect = _pSoundFXManager->load("sound_goal.wav");
  _musicStartEffect = _pSoundFXManager->load("musicstart.wav");
  _cameraEffect = _pSoundFXManager->load("sound_camera.wav");
  
  

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
GameManager::updateStage(int aCurrentStage)
{

	ofstream myWritefile;
 
	myWritefile.open("currentStage.txt");
	myWritefile << Ogre::StringConverter::toString(aCurrentStage);
	_currentStage = aCurrentStage;
	myWritefile.close();
}

void
GameManager::loadStage()
{
	string line;
	ofstream myWritefile;
	ifstream myReadFile;
	const char* linechar;
 
	myReadFile.open ("currentStage.txt");
	if (myReadFile.is_open())
	{
		std::getline(myReadFile,line);
		myReadFile.close();
 		linechar = line.c_str();
		_currentStage = atoi(linechar);
	}
	else
	{
		myWritefile.open("currentStage.txt");
		myWritefile << Ogre::StringConverter::toString(_currentStage);
	}

	
}

// Sonidos
void
GameManager::playJump()
{
	_jumpEffect->play();
}
void
GameManager::stopJump()
{
	_jumpEffect->stop();
}
void
GameManager::playMenu()
{
	_menuEffect->play();
}
void
GameManager::playBreak()
{
	_breakEffect->play();
}
void
GameManager::stopBreak()
{
	_breakEffect->stop();
}
void
GameManager::playSwitch()
{
	_switchEffect->play();
}
void
GameManager::stopSwitch()
{
	_switchEffect->stop();
}

void
GameManager::playGoal()
{
	_goalEffect->play();
}

void
GameManager::playMusicStart()
{
	_musicStartEffect->play();
}
void
GameManager::stopMusicStart()
{
	_musicStartEffect->stop();
}

void
GameManager::playCamera()
{
	_cameraEffect->play();
}

void
GameManager::playWalk()
{
	_walkEffect->play(-1);
}

void
GameManager::stopWalk()
{
	_walkEffect->stop();
}

// End sonidos

GameManager*
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

bool GameManager::keyPressed (const OIS::KeyEvent &e){
    
    _states.top()->keyPressed(e);

    if (e.key == OIS::KC_K) {
        _isDownMov = true;
    }
    if (e.key == OIS::KC_L) {
        _isRightMov = true;
    }
    if (e.key == OIS::KC_J) {
        _isLeftMov = true;
    }
    if (e.key == OIS::KC_I) {
        _isUpMov = true;
    }

    return true;
}

bool GameManager::keyReleased(const OIS::KeyEvent &e){
    
    _states.top()->keyReleased(e);

    if (e.key == OIS::KC_K) {
        _isDownMov = false;
    }
    if (e.key == OIS::KC_L) {
        _isRightMov = false;
    }
    if (e.key == OIS::KC_J) {
        _isLeftMov = false;
    }
    if (e.key == OIS::KC_I) {
        _isUpMov = false;
    }
    
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

bool GameManager::axisMoved( const OIS::JoyStickEvent &e, int axis ){
    
    if(axis == 0 && e.state.mAxes[axis].abs < 0){
        //stick analógico hacia arriba
        if(e.state.mAxes[axis].abs <= -JOYSTICK_MAX_AXIS*0.6){
            if(_isUpMov == false)
                _isUpMov = true;
        }
    }
    if(axis == 0 && e.state.mAxes[axis].abs > 0){
        //stick analógico hacia abajo
        if(e.state.mAxes[axis].abs >= JOYSTICK_MAX_AXIS*0.6){
            if(_isDownMov == false)
                _isDownMov = true;
        }
    }
    if(axis == 1 && e.state.mAxes[axis].abs < 0){
        //stick analógico hacia la izquierda
        if(e.state.mAxes[axis].abs <= -JOYSTICK_MAX_AXIS*0.6){
            if(_isLeftMov == false)
                _isLeftMov = true;
        }
    }
    if(axis == 1 && e.state.mAxes[axis].abs > 0){
        //stick analógico hacia la derecha
        if(e.state.mAxes[axis].abs >= JOYSTICK_MAX_AXIS*0.6){
            if(_isRightMov == false)
                _isRightMov = true;
        }
    }

    if(axis == 0 && abs(e.state.mAxes[axis].abs) <= JOYSTICK_MAX_AXIS*0.3){
        //Ni arriba ni abajo
        _isUpMov = false;
        _isDownMov = false;
    }
    if(axis == 1 && abs(e.state.mAxes[axis].abs) <= JOYSTICK_MAX_AXIS*0.3){
        //Ni izquierda ni derecha
        _isRightMov = false;
        _isLeftMov = false;
    }

    return true;
}
bool GameManager::buttonPressed( const OIS::JoyStickEvent &e, int button ){
    OIS::KeyCode keyPressed = OIS::KC_UNASSIGNED;

    switch(button){
        case 0:
            //Se pulsa el botón 1
            //Si está en el estado intro, empezará a jugar, y si está en estado play, saltará
             keyPressed = OIS::KC_SPACE;
            break;
        case 1:
            //Se pulsa el botón 2
            //Si está jugando se irá al estado pausa
            keyPressed = OIS::KC_P;
            break;
        case 2:
            //Se pulsa el botón 3
            break;
        case 3:
            //Se pulsa el botón 4
			keyPressed = OIS::KC_R;
            break;
        case 4:
            //Se pulsa L1
            //Si está jugando la cámara girará a la izquierda
            keyPressed = OIS::KC_LEFT;
            break;
        case 5:
            //Se pulsa R1
            //Si está jugando la cámara girará a la derecha
            keyPressed = OIS::KC_RIGHT;
            break;
        case 6:
            //Se pulsa BACK
            keyPressed = OIS::KC_ESCAPE;
            break;
        case 7:
            //Se pulsa START
            keyPressed = OIS::KC_SPACE;
            break;            
        case 8:
            //Se pulsa el stick analógico de la izquierda
            break;
        case 9:
            //Se pulsa el stick analógico de la derecha
            break;
        default:
            break;
    }
    if( keyPressed !=  OIS::KC_UNASSIGNED && keyPressed !=  OIS::KC_ESCAPE){
        OIS::KeyEvent kevent(NULL, keyPressed, 1);
        _states.top()->keyPressed( kevent );
    }  
    else if(keyPressed ==  OIS::KC_ESCAPE){
        OIS::KeyEvent kevent(NULL, keyPressed, 1);
        _states.top()->keyReleased( kevent );
    }
    return true;
}
bool GameManager::buttonReleased( const OIS::JoyStickEvent &e, int button ){
    //_states.top()->buttonReleased(e, button);
    return true;
}

bool GameManager::isRightMov(){
    return _isRightMov;
}
bool GameManager::isLeftMov(){
    return _isLeftMov;
}
bool GameManager::isUpMov(){
    return _isUpMov;
}
bool GameManager::isDownMov(){
    return _isDownMov;
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
