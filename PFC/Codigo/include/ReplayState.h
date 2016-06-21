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

#ifndef ReplayState_H
#define ReplayState_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "GameState.h"

class ReplayState : public Ogre::Singleton<ReplayState>, public GameState
{
 public:
  ReplayState():_root(0) {}

  void enter ();
  void exit ();
  void pause ();
  void resume ();

  void keyPressed (const OIS::KeyEvent &e);
  void keyReleased (const OIS::KeyEvent &e);

  void mouseMoved (const OIS::MouseEvent &e);
  void mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
  void mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool axisMoved( const OIS::JoyStickEvent &e, int axis );
  bool buttonPressed( const OIS::JoyStickEvent &e, int button );
  bool buttonReleased( const OIS::JoyStickEvent &e, int button );

  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

  // Heredados de Ogre::Singleton.
  static ReplayState& getSingleton ();
  static ReplayState* getSingletonPtr ();

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;
  Ogre::OverlayManager* _overlayMgr;
  Ogre::Overlay *overlay;
  Ogre::OverlayElement *_ReplayOverlay;
  Ogre::Real _ReplayCounter;
  GameManager *_gameMgr;
  

  bool _exitGame;
};

#endif
