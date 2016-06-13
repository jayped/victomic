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
		_makeCamera = reinterpret_cast<MakeCamera*>(_camera);
		_makeCamera->init();
		
		/*
		_camera->setPosition(Ogre::Vector3(_posCamX,_posCamY,_posCamZ));
		_camera->lookAt(Ogre::Vector3(_posCamX,_posCamY,0));
		_camera->setNearClipDistance(5);
		_camera->setFarClipDistance(10000);
		*/

		_viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
		_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 100.0));
		_win = _root->getAutoCreatedWindow();

		//Create the Background ///////////////////////////////////////////////////////////////////////////
		_sceneMgr->setSkyBox(true, "skyMat", 100.0F, true);
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		double width = _viewport->getActualWidth();
		double height = _viewport->getActualHeight();
		_makeCamera->setAspectRatio(width / height);
		
		//_movingCamera= false;
		//_cameraAngle = 0;
		//_cameraDirection = _stop;
	}

     isStop = false;
	_exitGame = false;
	 //_currentCamera = 0;
	
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
	//initCameras();
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
	
	/// Movimiento de Nori
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_I)) {
		_fallRigidBody->activate(true);_fallRigidBody->translate(btVector3(0,0,-.002));
	
	btQuaternion btq = _fallRigidBody->getOrientation();
	  btTransform transform = _fallRigidBody->getWorldTransform();
	  btq.setRotation(btVector3(0,1,0),Math::PI);
	  transform.setRotation(btq);
	  _fallRigidBody->activate(true);
	  _fallRigidBody->setWorldTransform(transform);

  }
  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_J)) {
	_fallRigidBody->activate(true);_fallRigidBody->translate(btVector3(-.002,0,0));
		  btQuaternion btq = _fallRigidBody->getOrientation();
	  btTransform transform = _fallRigidBody->getWorldTransform();
	  btq.setRotation(btVector3(0,1,0),(3.0/2.0) * Math::PI);
	  transform.setRotation(btq);
	  _fallRigidBody->activate(true);
	  _fallRigidBody->setWorldTransform(transform);

  }
  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_K)) {
	_fallRigidBody->activate(true);_fallRigidBody->translate(btVector3(0,0,.002));
		  btQuaternion btq = _fallRigidBody->getOrientation();
	  btTransform transform = _fallRigidBody->getWorldTransform();
	  btq.setRotation(btVector3(0,1,0),0);
	  transform.setRotation(btq);
	  _fallRigidBody->activate(true);
	  _fallRigidBody->setWorldTransform(transform);

  }
  if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_L)) {
	_fallRigidBody->activate(true);_fallRigidBody->translate(btVector3(.002,0,0));
		  btQuaternion btq = _fallRigidBody->getOrientation();
	  btTransform transform = _fallRigidBody->getWorldTransform();
	  btq.setRotation(btVector3(0,1,0),Math::PI/2);
	  transform.setRotation(btq);
	  _fallRigidBody->activate(true);
	  _fallRigidBody->setWorldTransform(transform);

  }
  // fin movimiento nori.
  
  // nori no se cae
  _fallRigidBody->setAngularVelocity(btVector3(0,0,0));
  // fin nori no se cae

  // [!] prueba movimiento de camara
	
  if (_makeCamera->isMoving())
	{
		_makeCamera->move();
	}
  // fin prueba movimiento de camara

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
  
  if (e.key == OIS::KC_LEFT) {
	  _makeCamera->setMoving(_left);
	  //moveCamera(_left);
	  //_makeCamera->_cameraDirection=_left;
  }
  
  if (e.key == OIS::KC_RIGHT) {
	  _makeCamera->setMoving(_right);
  	  //moveCamera(_right);
	  //_cameraDirection=_right;
	}
  
  if (e.key == OIS::KC_SPACE) {
	    btVector3 impulse(0,5,0);
	  _fallRigidBody->activate(true);
	  _fallRigidBody->applyCentralImpulse(impulse);
  }

  /////////////////////////
  ///// Teclas para pruebas -------------------------
  /////////////////////////

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

  if (e.key == OIS::KC_T) {
  }
    if (e.key == OIS::KC_G) {
  }
  if (e.key == OIS::KC_F) {
  }
  if (e.key == OIS::KC_H) {
  }

  ///// Fin Teclas para pruebas ----------------------

  
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

// Adding methods -------------------------------------

// CreateInitialWorld
// se encargara de cargar los mundos segun la stage en la que nos encontremos.
// la idea es realizar varias stages que se cargen dinamicamente.
// cada stage se correspondera con una matriz 3d realizada en un archivo de texto o hardcodeada.
void PlayState::CreateInitialWorld() {
  
	// Suelo
	_fallShape = new btBoxShape(btVector3(15.0f, 5.0f, 15.0f));
	// bloques de choque
	Entity *entity2 = _sceneMgr->createEntity("suelo", "GrassFloor.mesh");
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
	btCollisionShape *_fallShape_nori = new btBoxShape(btVector3(1.0f, 1.75f, 1.0f));
	Entity *entity = _sceneMgr->createEntity("nori", "Nori.mesh");
	//SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	_player = reinterpret_cast<Actor *>(_sceneMgr->getRootSceneNode()->createChildSceneNode("nori"));
	_player->attachObject(entity);
  
	// cuerpo rigido para bullet
	MyMotionState* fallMotionState = new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,10,0)), _player);
	btScalar mass_nori = 1;
	btVector3 fallInertia_nori(-0,0,0);
	_fallShape_nori->calculateLocalInertia(mass_nori,fallInertia_nori);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass_nori,fallMotionState,_fallShape_nori,fallInertia_nori);
	_fallRigidBody = new btRigidBody(fallRigidBodyCI);
	_world->addRigidBody(_fallRigidBody);
	_player->setPosition(0.0f,10.0f,0.0f);
	// add rigid body to nori
	_player->setRigitBody(_fallRigidBody);
	
	
	// piedra cesped
	btCollisionShape *_blockShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	Entity *blockEntity = _sceneMgr->createEntity("block", "GrassCube.mesh");
	Actor *Actorblock = reinterpret_cast<Actor *>(_sceneMgr->getRootSceneNode()->createChildSceneNode("block"));
	Actorblock->attachObject(blockEntity);
  
	// cuerpo rigido para bullet
	MyMotionState* blockMotionState = new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-7,8,7)), Actorblock);
	btScalar mass_block = 0;
	btVector3 blockFallInertia(-0,0,0);
	_blockShape->calculateLocalInertia(mass_block,blockFallInertia);
	btRigidBody::btRigidBodyConstructionInfo blockFallRigidBodyCI(mass_block,blockMotionState,_blockShape,blockFallInertia);
	btRigidBody *blockRigidBody = new btRigidBody(blockFallRigidBodyCI);
	_world->addRigidBody(blockRigidBody);
	Actorblock->setPosition(-7.0f,8.0f,7.0f);
	// add rigid body to nori
	Actorblock->setRigitBody(blockRigidBody);

	// piedra roca
	btCollisionShape *_rockShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	Entity *rockEntity = _sceneMgr->createEntity("rock", "RockCube.mesh");
	Actor *ActorRock = reinterpret_cast<Actor *>(_sceneMgr->getRootSceneNode()->createChildSceneNode("rock"));
	ActorRock->attachObject(rockEntity);
  
	// cuerpo rigido para bullet
	MyMotionState* rockMotionState = new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(7,5,7)), ActorRock);
	btScalar mass_rock = 1;
	btVector3 rockFallInertia(-0,0,0);
	_rockShape->calculateLocalInertia(mass_rock,rockFallInertia);
	btRigidBody::btRigidBodyConstructionInfo rockFallRigidBodyCI(mass_rock,rockMotionState,_rockShape,rockFallInertia);
	btRigidBody *rockRigidBody = new btRigidBody(rockFallRigidBodyCI);
	_world->addRigidBody(rockRigidBody);
	ActorRock->setPosition(7.0f,5.0f,7.0f);
	// add rigid body to nori
	ActorRock->setRigitBody(rockRigidBody);

}

void
PlayState::moveCamera(direction aDirection)
{/*
	if (_movingCamera==false)
	{
		_movingCamera=true;
	}

*/}

/*
void
PlayState::initCameras()
{
	//_cameras = {Ogre::Vector3(_posCamX,_posCamY,_posCamX)};
	std::vector<Ogre::Vector3>::iterator it;
	it=_cameras.begin();
	_cameras.insert(it,(Ogre::Vector3(-_posCamZ,_posCamY,_posCamX)));
	it=_cameras.begin();
	_cameras.insert(it,(Ogre::Vector3(_posCamX,_posCamY,-_posCamZ)));
	it=_cameras.begin();
	_cameras.insert(it,(Ogre::Vector3(_posCamZ,_posCamY,_posCamX)));
	it=_cameras.begin();
	_cameras.insert(it,(Ogre::Vector3(_posCamX,_posCamY,_posCamZ)));
}
*/

// End Adding methods -------------------------------------

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

