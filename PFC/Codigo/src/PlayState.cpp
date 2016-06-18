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
		_sceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));
		
		_sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		Ogre::Light* light = _sceneMgr->createLight("light1");
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(Ogre::Vector3(0.80,-1,0));
		light->setPosition(Ogre::Vector3(0,0,0));
		_sceneMgr->getRootSceneNode()->attachObject(light);
		
		_camera = _sceneMgr->getCamera("IntroCamera");
		_makeCamera = reinterpret_cast<MakeCamera*>(_camera);
		_makeCamera->init();
		
		_viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
		_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 100.0));
		_win = _root->getAutoCreatedWindow();

		//Create the Background
		_sceneMgr->setSkyBox(true, "skyMat", 100.0F, true);

		double width = _viewport->getActualWidth();
		double height = _viewport->getActualHeight();
		_makeCamera->setAspectRatio(width / height);
		
	}

     isStop = false;
	_exitGame = false;
	
	// inicializacion de movimientos de nori.
	for (int i =0; i<4; i++) _storeMove[i] = false;	

	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
  
	//_overlay = _overlayMgr->getByName("Score");

	// Bullet
	_broadphase = new btDbvtBroadphase();
	_collisionConf = new btDefaultCollisionConfiguration();
	_dispatcher = new btCollisionDispatcher(_collisionConf);
	_solver = new btSequentialImpulseConstraintSolver;
	_world = new btDiscreteDynamicsWorld(_dispatcher,_broadphase,_solver,_collisionConf);
  
	// Establecimiento propiedades del mundo
	_world->setGravity(btVector3(0,-9,0));

	// Creacion de los elementos iniciales del mundo
	CreateInitialWorld();
	// End Bullet

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
	// Sincronización
	Ogre::Vector3 vt(0,0,0);
	Ogre::Real tSpeed = 20.0;  
	Ogre::Real deltaT = evt.timeSinceLastFrame;
	int fps = 1.0 / deltaT;

	_world->stepSimulation(deltaT, 1); // Actualizar simulacion Bullet

	_player->setSpeedRelative(deltaT);

	// mira siempre al centro del escenario
	_makeCamera->lookAt(Ogre::Vector3(0,0,0));
	
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_ESCAPE)) return false;

	/// Movimiento de Nori
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_K)) {
		_player->move((int)_down, _makeCamera->getCameraPosition());
		_storeMove[0]=true;
	}
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_L)) {
		_player->move((int)_right, _makeCamera->getCameraPosition());
		_storeMove[1]=true;
	}
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_I)) {
		_player->move((int)_up, _makeCamera->getCameraPosition());
		_storeMove[2]=true;
	}
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_J)) {
		_player->move((int)_left, _makeCamera->getCameraPosition());
		_storeMove[3]=true;
	}
	// fin movimiento nori.
  
	// [!] Key Test
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_V)) {
	}
	
	// orienta a nori segun movimiento.
	if ( (_storeMove[0]==true) || (_storeMove[1]==true) || (_storeMove[2]==true) || (_storeMove[3]==true) )
		_player->orientate(_storeMove, _makeCamera->getCameraPosition());

	// nori se para
	if ( (_storeMove[0]==false) && (_storeMove[1]==false) && (_storeMove[2]==false) && (_storeMove[3]==false) )
		_player->stop();
  
	// nori no se cae
	_player->getRigitBody()->setAngularVelocity(btVector3(0,0,0));
	
	// Mueve la camara, mientras no se alcance un punto clave
	if (_makeCamera->isMoving())
	{
		_makeCamera->move(deltaT);
	}

	// Actualiza el estado del personaje
	_player->getState();

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
  }
  
  if (e.key == OIS::KC_RIGHT) {
	  _makeCamera->setMoving(_right);
	}
  
  if (e.key == OIS::KC_SPACE) {
	  _player->jump();
	  
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
	  _makeCamera->setCameraDistance(localpos.z);

	  _camera->setPosition(localpos);
  }
  if (e.key == OIS::KC_Z) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.z+=1;
	  _makeCamera->setCameraDistance(localpos.z);
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

  	/// Movimiento de Nori
	if (e.key == OIS::KC_K) {
		_storeMove[0]=false;
	}
	if (e.key == OIS::KC_L) {
		_storeMove[1]=false;
	}
	if (e.key == OIS::KC_I) {
		_storeMove[2]=false;
	}
	if (e.key == OIS::KC_J) {
		_storeMove[3]=false;
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
	_player = addActor(1.0f,1.75f,1.0f,"nori","Nori.mesh",0.0f,10.0f,0.0f,1);
	
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

Actor *
PlayState::addActor(double aShapeX,double aShapeY,double aShapeZ, string nameEntity, string meshName,
					double aMotionPosX,double aMotionPosY,double aMotionPosZ, btScalar aMass)
{

	btCollisionShape *newFallShape = new btBoxShape(btVector3(aShapeX, aShapeY, aShapeZ));
	Entity *entity = _sceneMgr->createEntity(nameEntity, meshName);
	//SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	Actor *newActor = reinterpret_cast<Actor *>(_sceneMgr->getRootSceneNode()->createChildSceneNode(nameEntity));
	newActor->init();
	newActor->attachObject(entity);
  
	// cuerpo rigido para bullet
	MyMotionState* newFallMotionState = new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(aMotionPosX,aMotionPosY,aMotionPosZ)), newActor);
	btVector3 newFallInertia(-0,0,0);
	newFallShape->calculateLocalInertia(aMass,newFallInertia);
	btRigidBody::btRigidBodyConstructionInfo newFallRigidBodyCI(aMass,newFallMotionState,newFallShape,newFallInertia);
	btRigidBody *newFallRigidBody = new btRigidBody(newFallRigidBodyCI);
	_world->addRigidBody(newFallRigidBody);
	newActor->setPosition(aMotionPosX,aMotionPosY,aMotionPosZ);
	// add rigid body to nori
	newActor->setRigitBody(newFallRigidBody);
	
	return newActor;

}

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

