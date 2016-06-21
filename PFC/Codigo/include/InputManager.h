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

#ifndef InputManager_H
#define InputManager_H

#include <Ogre.h>
#include <OIS/OIS.h>

// Gestor para los eventos de entrada (teclado y ratón).
class InputManager : public Ogre::Singleton<InputManager>, public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener {
 public:
  InputManager ();
  virtual ~InputManager ();
  
  void initialise (Ogre::RenderWindow *renderWindow);
  void capture ();

  // Gestión de listeners.
  void addKeyListener (OIS::KeyListener *keyListener,
			 const std::string& instanceName);
  void addMouseListener (OIS::MouseListener *mouseListener,
			   const std::string& instanceName );
  void addJoystickListener( OIS::JoyStickListener *joystickListener, 
                const std::string& instanceName );

  void removeKeyListener (const std::string& instanceName);
  void removeMouseListener (const std::string& instanceName);
  void removeJoystickListener( const std::string& instanceName );

  void removeKeyListener (OIS::KeyListener *keyListener);
  void removeMouseListener (OIS::MouseListener *mouseListener);
  void removeJoystickListener( OIS::JoyStickListener *joystickListener );


  
  void removeAllListeners ();
  void removeAllKeyListeners ();
  void removeAllMouseListeners ();
  void removeAllJoystickListeners( void );

  
  void setWindowExtents (int width, int height);

  OIS::Keyboard* getKeyboard ();
  OIS::Mouse* getMouse ();
  OIS::JoyStick* getJoystick( unsigned int index );

  int getNumOfJoysticks( void );



  // Heredados de Ogre::Singleton.
  static InputManager& getSingleton ();
  static InputManager* getSingletonPtr ();

private:
    bool keyPressed (const OIS::KeyEvent &e);
    bool keyReleased (const OIS::KeyEvent &e);
  
    bool mouseMoved (const OIS::MouseEvent &e);
    bool mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

    bool povMoved( const OIS::JoyStickEvent &e, int pov );
    bool axisMoved( const OIS::JoyStickEvent &e, int axis );
    bool sliderMoved( const OIS::JoyStickEvent &e, int sliderID );
    bool buttonPressed( const OIS::JoyStickEvent &e, int button );
    bool buttonReleased( const OIS::JoyStickEvent &e, int button );
  
    OIS::InputManager *_inputSystem;
    OIS::Keyboard *_keyboard;
    OIS::Mouse *_mouse;

    std::vector<OIS::JoyStick*> mJoysticks;
    std::vector<OIS::JoyStick*>::iterator itJoystick;
    std::vector<OIS::JoyStick*>::iterator itJoystickEnd;

    std::map<std::string, OIS::KeyListener*> _keyListeners;
    std::map<std::string, OIS::MouseListener*> _mouseListeners;
    std::map<std::string, OIS::JoyStickListener*> mJoystickListeners;

    std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;
    std::map<std::string, OIS::JoyStickListener*>::iterator itJoystickListener;

    std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;
    std::map<std::string, OIS::JoyStickListener*>::iterator itJoystickListenerEnd;

};
#endif
