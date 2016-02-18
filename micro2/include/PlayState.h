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
#include "Actor.h"
#define _posCamX 21
#define _posCamY -21
#define _posCamZ 55
#define _epsilon 0.1

#define BOARDSIZE 20
#define WALL 1
#define BALL 2
#define SPAWN 3
#define EMPTY 0

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
  double truncPosition(double aPosition);
 

  // Heredados de Ogre::Singleton.
  static PlayState& getSingleton ();
  static PlayState* getSingletonPtr ();

private:
  void checkBalls();
  void updatePlayer();
  void movingGhosts();
  void checkGhost();
  //devolverá true si puede hacer el movimiento.
  bool movingNewPos( Actor* &actor );
  //Los fantasmas se moverán por la zona de spawn, sin salir.
  void movingGhostsInitial();
  void updateGhost();
  void createRedGhost(int posX, int posY, int posZ);
  void createWhiteGhost(int posX, int posY, int posZ);
  void createBlueGhost(int posX, int posY, int posZ);
  void createOrangeGhost(int posX, int posY, int posZ);
  void createGreenGhost(int posX, int posY, int posZ);
  void createPinkGhost(int posX, int posY, int posZ);
  void createCyanGhost(int posX, int posY, int posZ);
  void createGhost(std::string, int posX, int posY, int posZ);


 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;
  Ogre::OverlayManager* _overlayMgr;

  Actor* _nodePlayer;
  Ogre::Vector3 _playerPosition;
  std::list<Ogre::SceneNode*> _arrayNodeBalls;
  std::list<Actor*> _nodesGhost;
  int _score;
  int _hiscore;
  Ogre::OverlayElement *_scoreOverlay;
  GameManager *_gameMgr;
  Ogre::Overlay *_overlay;
  Ogre::Real _introCounter;
  
  bool _exitGame;

	private:
    //Array que contendrá la información del tablero:
    // WALL -- bloque
    // BALL -- bola
    // EMPTY -- nada
    int _boardInfo[BOARDSIZE][BOARDSIZE];
    bool isInitialMove;
    bool isStop;

};

#endif
