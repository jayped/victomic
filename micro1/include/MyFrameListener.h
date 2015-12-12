#include <Ogre.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OIS/OIS.h>

class MyFrameListener : public Ogre::FrameListener {
private:
	OIS::InputManager* _inputManager;
	OIS::Keyboard* _keyboard;
	OIS::Mouse* _mouse;
	Ogre::Camera* _camera;
	Ogre::SceneNode* _node;
	Ogre::OverlayManager* _overlayManager;
	Ogre::RenderWindow* _win;
	Ogre::RaySceneQuery *_raySceneQuery;
	Ogre::SceneManager* _sceneManager;
	int _gameState;

public:
	MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneNode* node,Ogre::OverlayManager* oveMan,Ogre::SceneManager* sceMan);
	~MyFrameListener();
	bool frameStarted(const Ogre::FrameEvent& evt);
	Ogre::Ray setRayQuery(int posx, int posy, int mask);
	bool MyFrameListener::createBoard();
};
