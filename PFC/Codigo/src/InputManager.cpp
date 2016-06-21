#include "InputManager.h"

template<> InputManager* Ogre::Singleton<InputManager>::msSingleton = 0;

InputManager::InputManager ():
  _inputSystem(0),
  _keyboard(0),
  _mouse(0)
{
}

InputManager::~InputManager ()
{
    if (_inputSystem) {

        if (_keyboard) {
          _inputSystem->destroyInputObject(_keyboard);
          _keyboard = 0;
        }

        if (_mouse) {
          _inputSystem->destroyInputObject(_mouse);
          _mouse = 0;
        }

        if( mJoysticks.size() > 0 ) {
            itJoystick    = mJoysticks.begin();
            itJoystickEnd = mJoysticks.end();
            for(; itJoystick != itJoystickEnd; ++itJoystick ) {
                _inputSystem->destroyInputObject( *itJoystick );
            }
 
            mJoysticks.clear();
        }

        OIS::InputManager::destroyInputSystem(_inputSystem);

        _inputSystem = 0;

        // Limpiar todos los listeners.
        _keyListeners.clear();
        _mouseListeners.clear();
        mJoystickListeners.clear();
    }
}

void InputManager::initialise(Ogre::RenderWindow *renderWindow){
    if(!_inputSystem) {
        // Setup basic variables
        OIS::ParamList paramList;    
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        renderWindow->getCustomAttribute("WINDOW", &windowHnd);
        // Fill parameter list
        windowHndStr << windowHnd;
        paramList.insert(std::make_pair(std::string( "WINDOW"),
				        windowHndStr.str()));

        // Create inputsystem
        _inputSystem = OIS::InputManager::
            createInputSystem(paramList);

        _keyboard = static_cast<OIS::Keyboard*>
            (_inputSystem->createInputObject(OIS::OISKeyboard, true));
        _keyboard->setEventCallback(this);

        _mouse = static_cast<OIS::Mouse*>
            (_inputSystem->createInputObject(OIS::OISMouse, true));
        _mouse->setEventCallback(this);

        // Get window size
        unsigned int width, height, depth;
        int left, top;
        renderWindow->getMetrics(width, height, depth, left, top);

        // Set mouse region
        this->setWindowExtents( width, height );
        
        if (_inputSystem->getNumberOfDevices(OIS::OISJoyStick) > 0) {
            //mJoysticks.resize( mInputSystem->numJoySticks() );
            mJoysticks.resize( _inputSystem->getNumberOfDevices(OIS::OISJoyStick) );
 
            itJoystick    = mJoysticks.begin();
            itJoystickEnd = mJoysticks.end();
            for(; itJoystick != itJoystickEnd; ++itJoystick ) {
                (*itJoystick) = static_cast<OIS::JoyStick*>( _inputSystem->createInputObject( OIS::OISJoyStick, true ) );
                (*itJoystick)->setEventCallback( this );
            }
        }
    }
}

void
InputManager::capture (){
    // Capturar y actualizar cada frame.
    if (_mouse)
        _mouse->capture();
  
    if (_keyboard)
        _keyboard->capture();

    if( mJoysticks.size() > 0 ) {
        itJoystick    = mJoysticks.begin();
        itJoystickEnd = mJoysticks.end();
        for(; itJoystick != itJoystickEnd; ++itJoystick ) {
            (*itJoystick)->capture();
        }
    }
}

void
InputManager::addKeyListener
(OIS::KeyListener *keyListener, const std::string& instanceName)
{
  if (_keyboard) {
    // Comprobar si el listener existe.
    itKeyListener = _keyListeners.find(instanceName);
    if (itKeyListener == _keyListeners.end()) {
      _keyListeners[instanceName] = keyListener;
    }
    else {
      // Elemento duplicado; no hacer nada.
    }
  }
}

void
InputManager::addMouseListener
(OIS::MouseListener *mouseListener, const std::string& instanceName)
{
  if (_mouse) {
    // Comprobar si el listener existe.
    itMouseListener = _mouseListeners.find(instanceName);
    if (itMouseListener == _mouseListeners.end()) {
      _mouseListeners[instanceName] = mouseListener;
    }
    else {
      // Elemento duplicado; no hacer nada.
    }
  }
}

void InputManager::addJoystickListener( OIS::JoyStickListener *joystickListener, const std::string& instanceName ) {
    if( mJoysticks.size() > 0 ) {
        // Check for duplicate items
        itJoystickListener = mJoystickListeners.find( instanceName );
        if( itJoystickListener == mJoystickListeners.end() ) {
            mJoystickListeners[ instanceName ] = joystickListener;
        }
        else {
            // Duplicate Item
        }
    }
}

void
InputManager::removeKeyListener
(const std::string& instanceName)
{
  // Comprobar si el listener existe.
  itKeyListener = _keyListeners.find(instanceName);
  if (itKeyListener != _keyListeners.end()) {
    _keyListeners.erase(itKeyListener);
  }
  else {
    // No hacer nada.
  }
}

void
InputManager::removeMouseListener 
(const std::string& instanceName)
{
  // Comprobar si el listener existe.
  itMouseListener = _mouseListeners.find(instanceName);
  if (itMouseListener != _mouseListeners.end()) {
    _mouseListeners.erase(itMouseListener);
  }
  else {
    // No hacer nada.
  }
}

void InputManager::removeJoystickListener( const std::string& instanceName ) {
    // Check if item exists
    itJoystickListener = mJoystickListeners.find( instanceName );
    if( itJoystickListener != mJoystickListeners.end() ) {
        mJoystickListeners.erase( itJoystickListener );
    }
    else {
        // Doesn't Exist
    }
}

void
InputManager::removeKeyListener
(OIS::KeyListener *keyListener)
{
  itKeyListener = _keyListeners.begin();
  itKeyListenerEnd = _keyListeners.end();
  for (; itKeyListener != itKeyListenerEnd; ++itKeyListener) {
    if (itKeyListener->second == keyListener) {
      _keyListeners.erase(itKeyListener);
      break;
    }
  }
}

void
InputManager::removeMouseListener
(OIS::MouseListener *mouseListener)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) {
    if (itMouseListener->second == mouseListener) {
      _mouseListeners.erase(itMouseListener);
      break;
    }
  }
}

void InputManager::removeJoystickListener( OIS::JoyStickListener *joystickListener ) {
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if( itJoystickListener->second == joystickListener ) {
            mJoystickListeners.erase( itJoystickListener );
            break;
        }
    }
}

void InputManager::removeAllListeners(){
    _keyListeners.clear();
    _mouseListeners.clear();
    mJoystickListeners.clear();
}

void
InputManager::removeAllKeyListeners ()
{
  _keyListeners.clear();
}

void
InputManager::removeAllMouseListeners ()
{
  _mouseListeners.clear();
}

void InputManager::removeAllJoystickListeners( void ) {
    mJoystickListeners.clear();
}

void
InputManager::setWindowExtents 
(int width, int height)
{
  // Establecer la región del ratón.
  // Llamar al hacer un resize.
  const OIS::MouseState &mouseState = _mouse->getMouseState();
  mouseState.width = width;
  mouseState.height = height;
}

OIS::Keyboard*
InputManager::getKeyboard ()
{
    return _keyboard;
}

OIS::Mouse*
InputManager::getMouse ()
{
    return _mouse;
}

OIS::JoyStick* InputManager::getJoystick( unsigned int index ) {
    // Make sure it's a valid index
    if( index < mJoysticks.size() ) {
        return mJoysticks[ index ];
    }
 
    return 0;
}

int InputManager::getNumOfJoysticks( void ) {
    // Cast to keep compiler happy ^^
    return (int) mJoysticks.size();
}

bool InputManager::keyPressed (const OIS::KeyEvent &e){
  itKeyListener = _keyListeners.begin();
  itKeyListenerEnd = _keyListeners.end();
  // Delega en los KeyListener añadidos.
  for (; itKeyListener != itKeyListenerEnd; ++itKeyListener) {
    itKeyListener->second->keyPressed(e);
  }

  return true;
}

bool
InputManager::keyReleased
(const OIS::KeyEvent &e)
{
  itKeyListener = _keyListeners.begin();
  itKeyListenerEnd = _keyListeners.end();
  // Delega en los KeyListener añadidos.
  for (; itKeyListener != itKeyListenerEnd; ++itKeyListener) {
    itKeyListener->second->keyReleased( e );
  }

  return true;
}

bool
InputManager::mouseMoved
(const OIS::MouseEvent &e)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
 // Delega en los MouseListener añadidos.
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) {
    itMouseListener->second->mouseMoved( e );
  }

  return true;
}

bool
InputManager::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
  // Delega en los MouseListener añadidos.
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) {
    itMouseListener->second->mousePressed( e, id );
  }

    return true;
}

bool
InputManager::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
  // Delega en los MouseListener añadidos.
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) {
    itMouseListener->second->mouseReleased( e, id );
  }

  return true;
}

bool InputManager::povMoved( const OIS::JoyStickEvent &e, int pov ) {
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->povMoved( e, pov ))
            break;
    }
 
    return true;
}

bool InputManager::axisMoved( const OIS::JoyStickEvent &e, int axis ) {
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->axisMoved( e, axis ))
            break;
    }
 
    return true;
}
 
bool InputManager::sliderMoved( const OIS::JoyStickEvent &e, int sliderID ) {
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->sliderMoved( e, sliderID ))
            break;
    }
 
    return true;
}
 
bool InputManager::buttonPressed( const OIS::JoyStickEvent &e, int button ) {
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->buttonPressed( e, button ))
            break;
    }
 
    return true;
}
 
bool InputManager::buttonReleased( const OIS::JoyStickEvent &e, int button ) {
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->buttonReleased( e, button ))
            break;
    }
 
    return true;
}

InputManager*
InputManager::getSingletonPtr ()
{
  return msSingleton;
}

InputManager&
InputManager::getSingleton ()
{  
  assert(msSingleton);
  return *msSingleton;
}
