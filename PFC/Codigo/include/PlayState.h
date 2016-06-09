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

#ifndef PlayState_H
#define PlayState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include "GameState.h"
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <btBulletDynamicsCommon.h>
#include "Actor.h"
#define _posCamX 0 //-15
#define _posCamY 24
#define _posCamZ 50
#define _epsilon 0.1
#define _epsilonghost 0.2
#define _impulse_x -1
#define _impulse_y 10
#define _init_lives 3
#define _block1 18
#define _block2 10
#define _bonusItemDelay 15
#define _canLaunch 5
#define _cameraMax 3
#define _cameraMin 0

using namespace Ogre;

class PlayState : public Ogre::Singleton<PlayState>, public GameState
{
 enum direction {_stop, _up, _down, _right, _left};
  
 public:
  PlayState ();

  void enter ();
  void exit ();
  void pause ();
  void resume ();

  void keyPressed (const OIS::KeyEvent &e);
  void keyReleased (const OIS::KeyEvent &e);

  void mouseMoved (const OIS::MouseEvent &e);
  void mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
  void mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);
  void createScene();
  void createStage();
  void moveCamera(direction dir);
  void initCameras();

  // Heredados de Ogre::Singleton.
  static PlayState& getSingleton ();
  static PlayState* getSingletonPtr ();

private:
  Ogre::SceneNode* getWallEntity(int aType);
  
  // bullet
  btBroadphaseInterface* _broadphase;
  btDefaultCollisionConfiguration* _collisionConf;
  btCollisionDispatcher* _dispatcher;
  btSequentialImpulseConstraintSolver* _solver;
  btDiscreteDynamicsWorld* _world;

  btCollisionShape* _groundShape;  btCollisionShape* _fallShape;
  btRigidBody* _groundRigidBody;  btRigidBody* _fallRigidBody;
  
  void CreateInitialWorld();
  // end bullet

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;
  Ogre::OverlayManager* _overlayMgr;
  OIS::Mouse* _mouse;

  int _score;
  int _hiscore;
  Ogre::OverlayElement *_scoreOverlay;
  Ogre::OverlayElement *_roundOverlay;
  Ogre::OverlayElement *_livesOverlay;
  GameManager *_gameMgr;
  Ogre::Overlay *_overlay;
  Ogre::Real _introCounter;
  int _bottleCounter;
  Ogre::RaySceneQuery *_raySceneQuery;
  Ogre::RenderWindow* _win;
  int _lives;
  int _round;
  Ogre::Overlay *_overlayMouse;
  Ogre::Real _playCounter;
  Ogre::Real _canCounter;
  Ogre::Real _flashCounter;
  bool _exitGame;
  bool _movingCamera; // true: la camara se esta moviendo
  int _currentCamera; // punto actual de la camara 0..3
  Actor *_player;
  std::vector<Ogre::Vector3> _cameras;

	private:
    bool isInitialMove;
    bool isStop;

};

#endif
