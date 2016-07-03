/*********************************************************************
 * Módulo 1. Curso de Experto en Desarrollo de Videojuegos
 * Autor: David Vallejo Fernández    David.Vallejo@uclm.es
 *
 * Código modificado a partir de Managing Game States with OGRE
 * http://www.ogre3d.org/tikiwiki/Managing+Game+States+with+OGRE
 * Inspirado en Managing Game States in C++
 * http://gamedevgeek.com/tutorials/managing-game-states-in-c/
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#ifndef GameManager_H
#define GameManager_H

#include <stack>
#include <Ogre.h>
#include <OgreSingleton.h>
#include <OIS/OIS.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include "SoundFXManager.h"

#include "InputManager.h"
#include "StagesManager.h"
#define JOYSTICK_MAX_AXIS 32767

class GameState;

class GameManager : public Ogre::FrameListener, public Ogre::Singleton<GameManager>, public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
 public:
  GameManager ();
  ~GameManager (); // Limpieza de todos los estados.


  // avance en el juego.
  int _currentStage;
  int _currentWorld;
  
  //sonidos
  SoundFXPtr _jumpEffect;
  SoundFXPtr _menuEffect;
  SoundFXPtr _breakEffect;
  SoundFXPtr _switchOnEffect;
  SoundFXPtr _switchOffEffect;
  SoundFXPtr _goalEffect;
  SoundFXPtr _cameraEffect;
  SoundFXPtr _walkEffect;
  SoundFXPtr _endjumpEffect;

  SoundFXPtr _mapMusic;
  SoundFXPtr _musicStartEffect;
  SoundFXPtr _endEffect;
  
  // musicas [!] cambiar por la clase Track.cpp
	SoundFXPtr _world01Music;
	SoundFXPtr _world02Music;
	SoundFXPtr _world03Music;
	SoundFXPtr _world04Music;
	SoundFXPtr _endMusic;

  SoundFXManager* _pSoundFXManager;

  //SoundFXPtr getSoundFXPtr () { return _jumpEffect; }

  // Para el estado inicial.
  void start (GameState* state);
  
  // Funcionalidad para transiciones de estados.
  void changeState (GameState* state);
  void pushState (GameState* state);
  void popState ();
  
  // Heredados de Ogre::Singleton.
  static GameManager& getSingleton ();
  static GameManager* getSingletonPtr ();
  bool _initSDL ();
  void updateStage(int aCurrentStage);
  void loadStage();
  
  void playJump();
  void stopJump();
  void playMenu();
  void playBreak();
  void stopBreak();
  void playSwitchOn();
  void stopSwitchOn();
  void playSwitchOff();
  void stopSwitchOff();
  void playGoal();
  void playCamera();
  void playWalk();
  void stopWalk();
  void playEndJump();
  void stopEndJump();

  void playMusic(int aWorld);
  void stopMusic(int aWorld);
  void playMapMusic();
  void stopMapMusic();
  void playEndMusic();
  void stopEndMusic();


  void playMusicStart();
  void stopMusicStart();
  int world() const { return _currentWorld; } 
  void world(int aWorld) { _currentWorld = aWorld; } 

    bool isRightMov();
    bool isLeftMov();
    bool isUpMov();
    bool isDownMov();

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneManager;
  Ogre::RenderWindow* _renderWindow;

  // Funciones de configuración.
  void loadResources ();
  bool configure ();
  
  // Heredados de FrameListener.
  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

 private:
  // Funciones para delegar eventos de teclado
  // y ratón en el estado actual.
  bool keyPressed (const OIS::KeyEvent &e);
  bool keyReleased (const OIS::KeyEvent &e);

  bool mouseMoved (const OIS::MouseEvent &e);
  bool mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
  bool mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);
   
    bool axisMoved( const OIS::JoyStickEvent &e, int axis );
    bool buttonPressed( const OIS::JoyStickEvent &e, int button );
    bool buttonReleased( const OIS::JoyStickEvent &e, int button );

  // Gestor de eventos de entrada.
  InputManager *_inputMgr;
  StagesManager *_stagesMgr;
    
    bool _isRightMov;
    bool _isLeftMov;
    bool _isUpMov;
    bool _isDownMov;
  // Estados del juego.
  std::stack<GameState*> _states;

};

#endif
