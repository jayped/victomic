#include "Can.h"
#include "ReplayState.h"
#include "PlayState.h"
#include "PauseState.h"
#include "MyMotionState.h"
#include <iostream>
#include <conio.h>
#include <math.h>
#include <cmath>
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

using namespace std;

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

PlayState::PlayState ()
{
    _root = 0;
    isInitialMove = true;
    isStop = false;

}

void
PlayState::enter ()
{
	if (!_root)
	{
		_root = Ogre::Root::getSingletonPtr();
		_gameMgr = GameManager::getSingletonPtr();

		// Se recupera el gestor de escena y la cámara.
		_sceneMgr = _root->getSceneManager("SceneManager");
		_sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
		//_sceneMgr->addRenderQueueListener(new Ogre::OverlaySystem());


		_camera = _sceneMgr->getCamera("IntroCamera");
		_camera->setPosition(Ogre::Vector3(_posCamX,_posCamY,_posCamZ));
		_camera->lookAt(Ogre::Vector3(_posCamX,_posCamY,0));
		_camera->setNearClipDistance(5);
		_camera->setFarClipDistance(10000);
  
		_viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
		_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 100.0));
		_win = _root->getAutoCreatedWindow();

		double width = _viewport->getActualWidth();
		double height = _viewport->getActualHeight();
		_camera->setAspectRatio(width / height);

	}
     isStop = false;
	_exitGame = false;
	 isInitialMove = true;

	
	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
  
	//_overlay = _overlayMgr->getByName("Score");

	// Bullet -------------------------------------------------------------------------------
	_broadphase = new btDbvtBroadphase();
	_collisionConf = new btDefaultCollisionConfiguration();
	_dispatcher = new btCollisionDispatcher(_collisionConf);
	_solver = new btSequentialImpulseConstraintSolver;
	_world = new btDiscreteDynamicsWorld(_dispatcher,_broadphase,_solver,_collisionConf);
  
	// Establecimiento propiedades del mundo
	_world->setGravity(btVector3(0,-3,0));

	// Creacion de los elementos iniciales del mundo
	CreateInitialWorld();
	// End Bullet -------------------------------------------------------------------------------
	_lives = 0;
}

void
PlayState::exit ()
{

    _sceneMgr->clearScene();
    //_overlay->hide();
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{
  // Se restaura el background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
	Ogre::Vector3 vt(0,0,0);     Ogre::Real tSpeed = 20.0;  
	Ogre::Real deltaT = evt.timeSinceLastFrame;
	int fps = 1.0 / deltaT;
	bool mbleft, mbmiddle, mbright; // Botones del raton pulsados
	
	_world->stepSimulation(deltaT, 1); // Actualizar simulacion Bullet

	// mira siempre al centro del escenario
	_camera->lookAt(Ogre::Vector3(0,0,0));

	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_ESCAPE)) return false;
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP))    vt += Ogre::Vector3(0, 0, -1);
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN))  vt += Ogre::Vector3(0, 0, 1);
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT))
	{
		Ogre::Vector3 localpos = _camera->getPosition();
		localpos.x-=.1;
		_camera->setPosition(localpos);
	}
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
	{
		Ogre::Vector3 localpos = _camera->getPosition();
		localpos.x+=.4;
		_camera->setPosition(localpos);
	}

  btVector3 impulse(0,0,0);
  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_I)) {_fallRigidBody->activate(true); impulse = btVector3(0,0,-.5);}
  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_J)) { _fallRigidBody->activate(true); impulse = btVector3(-.5,0,0);}
  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_K)) { _fallRigidBody->activate(true); impulse = btVector3(0,0,.5);}
  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_L)) { _fallRigidBody->activate(true); impulse = btVector3(.5,0,0);}

  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_SPACE)) { _fallRigidBody->activate(true); impulse = btVector3(0,3,0);}

  _fallRigidBody->applyCentralImpulse(impulse);

	return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  Real deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(deltaT, 1); // Actualizar simulacion Bullet
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  // Tecla p --> PauseState.
  if (e.key == OIS::KC_P) {
    pushState(PauseState::getSingletonPtr());
  }
  
  if (e.key == OIS::KC_UP) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.y+=1;
	  _camera->setPosition(localpos);
	  _lives++;
  }
  if (e.key == OIS::KC_DOWN) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.y-=1;
	  _camera->setPosition(localpos);
  }
  if (e.key == OIS::KC_LEFT) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.x-=1;
	  _camera->setPosition(localpos);
  }
  if (e.key == OIS::KC_RIGHT) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.x+=1;
	  _camera->setPosition(localpos);
  }
  if (e.key == OIS::KC_A) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.z-=1;
	  _camera->setPosition(localpos);
  }
  if (e.key == OIS::KC_Z) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.z+=1;
	  _camera->setPosition(localpos);
  }

  btVector3 impulse(0,0,0);
  if (e.key==OIS::KC_I) {_fallRigidBody->activate(true); impulse = btVector3(0,0,-.5);}
  if (e.key==OIS::KC_J) { _fallRigidBody->activate(true); impulse = btVector3(-.5,0,0);}
  if (e.key==OIS::KC_K){ _fallRigidBody->activate(true); impulse = btVector3(0,0,.5);}
  if (e.key==OIS::KC_L) { _fallRigidBody->activate(true); impulse = btVector3(.5,0,0);}

  if (e.key==OIS::KC_SPACE) { _fallRigidBody->activate(true); impulse = btVector3(0,3,0);}

  _fallRigidBody->applyCentralImpulse(impulse);


  
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  if (e.key == OIS::KC_ESCAPE) {
    _root->getAutoCreatedWindow()->removeAllViewports();
	_exitGame = true;
  }

}

void
PlayState::mouseMoved
(const OIS::MouseEvent &e)
{
  	// Captura del raton
	int posx = e.state.X.abs;
	int posy = e.state.Y.abs;

	// Dibujado del cursor
	Ogre::OverlayElement *oe;
	oe = _overlayMgr->getOverlayElement("cursor");
	oe->setLeft(posx);
	oe->setTop(posy);
	
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{



}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{

}

// Adding methods
void PlayState::CreateInitialWorld() {
  
	// Suelo
	_fallShape = new btBoxShape(btVector3(15.0f, 5.0f, 15.0f));
	// bloques de choque
	Entity *entity2 = _sceneMgr->createEntity("suelo", "Floor.mesh");
	SceneNode *node2 = _sceneMgr->getRootSceneNode()->createChildSceneNode("suelo");
	node2->attachObject(entity2);

	btRigidBody* _wallRigidBody1;

	MyMotionState* fallMotionState1 = new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)), node2);
	btScalar mass = 0;
	btVector3 fallInertia(0,0,0);
	_fallShape->calculateLocalInertia(mass,fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI1(mass,fallMotionState1,_fallShape,fallInertia);
	_wallRigidBody1 = new btRigidBody(fallRigidBodyCI1);
	_world->addRigidBody(_wallRigidBody1);
	node2->setPosition(0.0f,0.0f,0.0f);
 
	// ---

	//Nori
	btCollisionShape *_fallShape_nori = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	Entity *entity = _sceneMgr->createEntity("nori", "Nori.mesh");
	//SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	Can *node = reinterpret_cast<Can *>(_sceneMgr->getRootSceneNode()->createChildSceneNode("nori"));
	node->attachObject(entity);
  
	// cuerpo rigido para bullet
	MyMotionState* fallMotionState = new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,30,0)), node);
	btScalar mass_nori = 1;
	btVector3 fallInertia_nori(-0,0,0);
	_fallShape_nori->calculateLocalInertia(mass_nori,fallInertia_nori);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass_nori,fallMotionState,_fallShape_nori,fallInertia_nori);
	_fallRigidBody = new btRigidBody(fallRigidBodyCI);
	_world->addRigidBody(_fallRigidBody);
	node->setPosition(0.0f,30.0f,0.0f);
	// add rigid body to nori
	node->setRigitBody(_fallRigidBody);
  
}

// End Adding methods
PlayState*
PlayState::getSingletonPtr ()
{
return msSingleton;
}

PlayState&
PlayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

