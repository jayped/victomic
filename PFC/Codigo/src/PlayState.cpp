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
		
		_camera = _sceneMgr->getCamera("IntroCamera");
		_makeCamera = reinterpret_cast<MakeCamera*>(_camera);
		_makeCamera->init();
		
		_viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
		_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 100.0));
		_win = _root->getAutoCreatedWindow();

		double width = _viewport->getActualWidth();
		double height = _viewport->getActualHeight();
		_makeCamera->setAspectRatio(width / height);
		
	}

	// Luz del escenario.
	_sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Light* light = _sceneMgr->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(0.80,-1,0));
	light->setPosition(Ogre::Vector3(0,0,0));
	_sceneMgr->getRootSceneNode()->attachObject(light);
	
	//Create the Background
	_sceneMgr->setSkyBox(true, "skyMat", 100.0F, true);

	isStop = false;
	_exitGame = false;
	_counterActorsID = 0;
	_controlBlock = false;
	_stage = 1;

	// carga de stage de archivo
	// TO-DO

	// inicializacion de movimientos de nori.
	for (int i =0; i<4; i++) _storeMove[i] = false;	

	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
  
	_overlay = _overlayMgr->getByName("TestOverlay");
	_overlay->show();
	//_overlayMgr->getOverlayElement("TestValue")->setCaption("holas");

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

}

void
PlayState::exit ()
{
    _sceneMgr->clearScene();
	_listOfActors.clear();
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
	if (_gameMgr->isDownMov() && !_controlBlock) {
		_player->move((int)_down, _makeCamera->getCameraPosition());
		_storeMove[0]=true;
	}
    else
    {
        _storeMove[0]=false;
    }
	if (_gameMgr->isRightMov() && !_controlBlock) {
		_player->move((int)_right, _makeCamera->getCameraPosition());
		_storeMove[1]=true;
	}
    else
    {
        _storeMove[1]=false;
    }
	if (_gameMgr->isUpMov() && !_controlBlock) {
		_player->move((int)_up, _makeCamera->getCameraPosition());
		_storeMove[2]=true;
	}
    else
    {
        _storeMove[2]=false;
    }
	if (_gameMgr->isLeftMov() && !_controlBlock) {
		_player->move((int)_left, _makeCamera->getCameraPosition());
		_storeMove[3]=true;
	}
    else
    {
        _storeMove[3]=false;
    }
	// fin movimiento nori.
  
	// [!] Key Test
	if (InputManager::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_V)) {
	}
	
	// orienta a nori segun movimiento.
	if ( (_storeMove[0]==true) || (_storeMove[1]==true) || (_storeMove[2]==true) || (_storeMove[3]==true) )
    {
		_player->orientate(_storeMove, _makeCamera->getCameraPosition());
    }
	// nori se para
	if ( (_storeMove[0]==false) && (_storeMove[1]==false) && (_storeMove[2]==false) && (_storeMove[3]==false) )
    {
		_player->stop();
    }
	// nori no se cae
	if (!_controlBlock)
		_player->getRigitBody()->setAngularVelocity(btVector3(0,0,0));
	else
		_player->getRigitBody()->setAngularVelocity(btVector3(0,_player->getRigitBody()->getAngularVelocity().getY(),0));
	
	// Mueve la camara, mientras no se alcance un punto clave
	if (_makeCamera->isMoving())
	{
		_makeCamera->move(deltaT);
	}

	// Actualiza el estado del personaje
    _player->updateState(deltaT);

	// test de colisiones
	//_player->colision(_TESTRIGIDBODY);
	//btCollisionAlgorithm* pAlgorithm = _world->getDispatcher()->findAlgorithm( _player->getRigitBody()->getCollisionShape(), pBulletObj2 );
	//btManifoldResult oManifoldResult( pBulletObj1, pBulletObj2 );
	//pAlgorithm->processCollision( pBulletObj1, pBulletObj2, pBtWorld->getDispatchInfo(), &oManifoldResult );
	//btPersistentManifold* pManifold = oManifoldResult.getPersistentManifold();
	// colisiones
	int ncolisiones = colision(_player->getRigitBody());
	
	processActors(deltaT);
	//_overlayMgr->getOverlayElement("TestValue")->setCaption(Ogre::StringConverter::toString(ncolisiones));

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
  if (e.key == OIS::KC_P && !_controlBlock) {
    pushState(PauseState::getSingletonPtr());
  }

  if (e.key == OIS::KC_LEFT && !_controlBlock) {
	  _makeCamera->setMoving(_left);
  }
  
  if (e.key == OIS::KC_RIGHT && !_controlBlock) {
	  _makeCamera->setMoving(_right);
	}
  
  if (e.key == OIS::KC_SPACE && !_controlBlock) {
	  _player->jump();
	  
  }

  /////////////////////////
  ///// Teclas para pruebas -------------------------
  /////////////////////////

  if (e.key == OIS::KC_UP) {
	  Ogre::Vector3 localpos = _camera->getPosition();
	  localpos.y+=1;
	  _camera->setPosition(localpos);
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

  // Tecla r --> endgame.
  if (e.key == OIS::KC_R) {
    changeState(ReplayState::getSingletonPtr());
  }

  if (e.key == OIS::KC_T) {
	  _controlBlock = !_controlBlock;
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


bool PlayState::axisMoved( const OIS::JoyStickEvent &e, int axis ){
    return true;
}
bool PlayState::buttonPressed( const OIS::JoyStickEvent &e, int button ){
    return true;
}
bool PlayState::buttonReleased( const OIS::JoyStickEvent &e, int button ){
    return true;
}
// Adding methods -------------------------------------

// CreateInitialWorld
// se encargara de cargar los mundos segun la stage en la que nos encontremos.
// la idea es realizar varias stages que se cargen dinamicamente.
// cada stage se correspondera con una matriz 3d realizada en un archivo de texto o hardcodeada.
void PlayState::CreateInitialWorld() {
	//Nori
	_player = addActor(1.0f,1.75f,1.0f,"nori","Nori.mesh","walking",0.0f,10.0f,0.0f,1,0);
	//_player->initNori();
	// ---
	/*
	string name="";
	int numname=0;
	for (int yy = 0; yy<4; yy++)
	{
		for (int xx = 0; xx<15; xx++)
		{
			name = "actor"+ to_string(numname);
			numname++;
			addActor(1.0f, 1.0f, 1.0f,name, "box01.mesh","", -14+(xx*2), 6 +(yy*2), -10.0f+(yy*2),0, 10);
		}
	}
	*/

	addActor(15.0f, 5.0f, 15.0f,"suelo", "World01.mesh","",		0.0f,	0.0f,	0.0f,	0,	1);
	addActor(1.0f, 1.0f, 1.0f,"block", "box01.mesh","",			-7.0f,	8.0f,	7.0f,	0,	2);
	addActor(1.0f, 1.0f, 1.0f,"rock6", "RockBox.mesh","",		-7.0f,	12.0f,	7.0f,	1,	3);
	//addActor(1.0f, 1.0f, 1.0f,"rockc6", "RockBox.mesh","",		-7.0f,	14.0f,	7.0f,	1,	3);
	//addActor(1.0f, 1.0f, 1.0f,"rockd6", "RockBox.mesh","",		14.0f,	6.0f,	4.0f,	1,	3);
	addActor(1.0f, .33f, 1.0f,"rock3", "SwitchBaseBox.mesh","",	4.0f,	6.0f-.66,-14.0f,0,	8);
	
	//addActor(1.0f, 1.0f, 1.0f,"dww", "RockBox.mesh","",			4.0f,	10.0f,		0.0f,	4.0f,	10.0f,		0.0f,1,3);
	//addActor(1.0f, 1.0f, 1.0f,"d1", "FragileRockBox.mesh","",	-5.0f,	8.0f,	7.0f,	0,	4);
	addActor(1.0f, 1.0f, 1.0f,"dd2", "FragileRockBox.mesh","",	-3.0f,	8.0f,	7.0f,	0,	4);
	addActor(1.0f, .33f, 1.0f,"e", "SwitchBox.mesh","",		    -5.0f,	6.0f-.66,7.0f,	0,	5);
	
	addActor(.0f, .0f, .0f,"g1", "TransparentBox.mesh","",		-6.0f,	8.0f,	0.0f,	0,	6);
	addActor(.0f, .0f, .0f,"g2", "TransparentBox.mesh","",		-2.0f,	8.0f,	0.0f,	0,	6);
	addActor(.0f, .0f, .0f,"g3", "TransparentBox.mesh","",		2.0f,	8.0f,	0.0f,	0,	6);
	addActor(.0f, .0f, .0f,"g4", "TransparentBox.mesh","",		6.0f,	8.0f,	0.0f,	0,	6);
	addActor(.0f, .0f, .0f,"g5", "TransparentBox.mesh","",		10.0f,	8.0f,	0.0f,	0,	6);
	//addActor(1.0f, 1.0f, 1.0f,"h", "SolidBox.mesh","",			-2.0f,	8.0f,	-14.0f,	0,	7);
	
	//addActor(1.0f, 1.0f, 1.0f,"a", "box01.mesh","",			-7.0f,10.0f,7.0f,-7.0f,10.0f,7.0f,0,2);
	//addActor(1.0f, 1.0f, 1.0f,"b", "RockBox.mesh","",		0.0f,14.0f,0.0f,0.0f,14.0f,0.0f,1,3);
	//addActor(1.0f, 1.0f, 1.0f,"c", "RockBox.mesh","",		0.0f,16.0f,0.0f,0.0f,16.0f,0.0f,1,3);
	//addActor(1.0f, .33f, 1.0f,"f", "SwitchBaseBox.mesh","",		4.0f,6.0f-.66,-10.0f,4.0f,6.0f-.66,-10.0f,0,8);
	
	// 1: // Bloque Suelo de Escenario
	// 2: // Bloque Construccion de Escenario
	// 3: // Piedra movil
	// 4: // Bloque fragil
	// 5: // Interruptor
	// 6: // Bloque transparente
	// 7: // Bloque solido
	// 8: // Meta de escenario

}

Actor *
PlayState::addActor(double aShapeX,double aShapeY,double aShapeZ, string nameEntity, string meshName, string nameAnimation,
					double aMotionPosX,double aMotionPosY,double aMotionPosZ, btScalar aMass, int aFlags)
{

	btCollisionShape *newFallShape = new btBoxShape(btVector3(aShapeX, aShapeY, aShapeZ));
	Entity *entity = _sceneMgr->createEntity(nameEntity, meshName);
	
	//SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	Actor *newActor = reinterpret_cast<Actor *>(_sceneMgr->getRootSceneNode()->createChildSceneNode(nameEntity));
	newActor->attachObject(entity);
	newActor->init(aFlags,_counterActorsID);
	_counterActorsID++;
	
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
	newFallRigidBody->setFlags(aFlags);

	// añadido el actor a la lista general de actores.
	_actorsIt = _listOfActors.end();
	_listOfActors.insert(_actorsIt,newActor);

	return newActor;

}

int
PlayState::colision(btCollisionObject *aObject)
{

	btRigidBody* rigidCollision = NULL;
	int colisionesNori=0;
	int numManifolds = _world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = _world->getDispatcher()->getManifoldByIndexInternal(i);
		btRigidBody* obA = (btRigidBody*)contactManifold->getBody0();
		btRigidBody* obB = (btRigidBody*)contactManifold->getBody1();
		
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				int flagA = obA->getFlags();
				int flagB = obB->getFlags();
				int x = 0;
				
				// 3: // Piedra movil
				// 4: // Bloque fragil
				// 5: // Interruptor
				// 6: // Bloque transparente
				// 7: // Bloque solido
				// 8: // Meta de escenario

				// 3 Piedra movil
				if ((obA->getFlags() == 0) && (obB->getFlags() == 3))
				{
					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.76) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 2.04))
					{
						int x=0;
					}
				}				

				// 4 Bloque fragil
				// 4.1 Player
				if ((obA->getFlags() == 0) && (obB->getFlags() == 4))
				{
					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.76) &&
						(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 2.74))
					{
						Actor *iteratorDelete;
						for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
						{
							iteratorDelete = *it;
							if (iteratorDelete->getRigitBody()==obB)
							{
								iteratorDelete->setActivated(true); // marcado para eliminar en unos segundos.
							}
						}
					}
				}
				// 4.2 Piedra movil
				if ((obA->getFlags() == 3) && (obB->getFlags() == 4))
				{
 					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (centroDeMasasA-centroDeMasasB < 2.10) &&
						(centroDeMasasA-centroDeMasasB > 1.10))
					{
						Actor *iteratorDelete;
						for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
						{
							iteratorDelete = *it;
							if (iteratorDelete->getRigitBody()==obB)
							{
								iteratorDelete->setActivated(true); // marcado para eliminar en unos segundos.
							}
						}
					}
				}			

				// 5 Interruptor
				// 5.1 nori
				if ((obA->getFlags() == 0) && (obB->getFlags() == 5))
				{
					Actor *delActor;
					Actor *iterActor;
					std::list<Actor *> delList;
					std::list<Actor *>::iterator _delIt;

					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.1) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 1.9))
					{
						Actor *iteratorDelete;
						for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
						{
							iteratorDelete = *it;
							if (iteratorDelete->getRigitBody() == obB)
							{
								// Es un interruptor
								iteratorDelete->setActivated(true);
							}
						}
					}
				}				

				// 5.2 piedra
				if ((obA->getFlags() == 3) && (obB->getFlags() == 5))
				{
					Actor *delActor;
					Actor *iterActor;
					std::list<Actor *> delList;
					std::list<Actor *>::iterator _delIt;

					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 1.4) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 1.2))
					{
						Actor *iteratorDelete;
						for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
						{
							iteratorDelete = *it;
							if (iteratorDelete->getRigitBody() == obB)
							{
								// Es un interruptor
								iteratorDelete->setActivated(true);
							}
						}
					}
				}				


				// 8 Meta de escenario
				if ((obA->getFlags() == 0) && (obB->getFlags() == 8))
				{
					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.07) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 2.04))
					{
						// Codigo final fase.
						//changeState(ReplayState::getSingletonPtr());
						if (!_controlBlock)
						{
							Actor *iteratorDelete;
							for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
							{
								iteratorDelete = *it;
								if (iteratorDelete->getRigitBody()==obB)
								{
									iteratorDelete->goal();
								}
							}

							_player->goal();
							_controlBlock=true;
						}
					}
				}				
			}
		}
	}

	return colisionesNori;//numManifolds;
}

// LLamada en cada frame, modifica el estado de cada elemento de escena, segun ocurran ciertos eventos.
void
PlayState::processActors(double aDeltaT)
{
	Actor *delActor;
	Actor *iterActor;
	std::list<Actor *> delList;
	std::list<Actor *>::iterator _delIt;
	int lType = 0;
	int lTypeToCreate = 0;
	Ogre::String lMeshName = "";
	std::list<Actor *> switchList;
	
	for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
	{
		Actor *lActor;
		lActor = *it;
		int actorType = lActor->getType();
		
		// 3: // Piedra movil
		// 4: // Bloque fragil
		// 5: // Interruptor
		// 6: // Bloque transparente
		// 7: // Bloque solido
		// 8: // Meta de escenario
		switch (actorType)
		{
			case 4: // Bloque Fragil
				if (lActor->getActivated())
				{
					lActor->updateCounter(aDeltaT);
					if ((int)Math::Ceil(lActor->getCounter()*100)  % 2 == 0)
						lActor->setVisible(true);			
					else
						lActor->setVisible(false);			

					if (lActor->getCounter()<0.5)
					{
						lActor->setVisible(true);
						lActor->generateParticles();
						//Ogre::SceneNode *_paraBorrar = dynamic_cast<Ogre::SceneNode*> (lActor);
						//_world->removeRigidBody(lActor->getRigitBody());
						//activateAllActors();
					}
					if (lActor->getCounter()<0)
					{
						Ogre::SceneNode *_paraBorrar = dynamic_cast<Ogre::SceneNode*> (lActor);
						int numeroHijos = _paraBorrar->numAttachedObjects();
						Entity *ent = static_cast<Entity*>(_paraBorrar->detachObject(unsigned short (0)));
						_sceneMgr->destroyEntity(ent);
						_sceneMgr->destroySceneNode(_paraBorrar);
						
						_world->removeRigidBody(lActor->getRigitBody());
						activateAllActors();

						_delIt = delList.end();
						delList.insert(_delIt,lActor);
					}
				}
				break;
			case 5: // 5: // Interruptor
				if (lActor->getActivated())
				{
					lType = 6; // Busca transparentes
					lMeshName = "SolidBox.mesh";
					lTypeToCreate=7;
				}

				else
				{
					lType = 7; // Busca Solidos
					lMeshName = "TransparentBox.mesh";
					lTypeToCreate=6;
				}
				lActor->setActivated(false);
				Actor *iteratorDelete;
				for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
				{
					iteratorDelete = *it;
					if (iteratorDelete->getType() == lType)
					{
						Ogre::SceneNode *_paraBorrar = dynamic_cast<Ogre::SceneNode*> (iteratorDelete);
						int numeroHijos = _paraBorrar->numAttachedObjects();
						Entity *ent = static_cast<Entity*>(_paraBorrar->detachObject(unsigned short (0)));
						_sceneMgr->destroyEntity(ent);
						
						_world->removeRigidBody(iteratorDelete->getRigitBody());
						activateAllActors();

						_delIt = switchList.end();
						switchList.insert(_delIt,iteratorDelete);

					}
				}
						
				for (std::list<Actor *>::iterator it=switchList.begin(); it != switchList.end(); ++it)
				{
					Actor *iteratorDelete=*it;
					int lActorID = iteratorDelete->getActorID();

					Ogre::SceneNode *lSceneNode = dynamic_cast<Ogre::SceneNode*> (iteratorDelete);
					_listOfActors.remove(iteratorDelete);
					Ogre::Vector3 lPosition = lSceneNode->getPosition();
					_sceneMgr->destroySceneNode(lSceneNode);
					addActor(1.0f, 1.0f, 1.0f,"actors"+to_string(lActorID), lMeshName,"", lPosition.x, lPosition.y, lPosition.z, 0, lTypeToCreate);
				}
				
				// Eliminar transparentes y añadir solidos.
				
				break;
			case 6:
				_world->removeRigidBody(lActor->getRigitBody()); // Elimina el cuerpo rigido de los transparentes si existe.
				activateAllActors(); // activa los elementos posibles que esten encima de el para que actue la física.
				break;
			case 8:
				break;
		}

	}

	// Eliminación de actores en la lista de eliminados.
	for (std::list<Actor *>::iterator it=delList.begin(); it != delList.end(); ++it)
	{
		Actor *iteratorDelete=*it;
		_listOfActors.remove(iteratorDelete);
	}
}

void
PlayState::activateAllActors()
{
	Actor *iterActor;
	std::list<Actor *> delList;
	std::list<Actor *>::iterator _delIt;

	for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
	{
		iterActor = *it;
		iterActor->getRigitBody()->activate(true);
	}				
}

void
PlayState::nextStage()
{
	_stage++;
	
	// guardado de progreso.
	// TO-DO
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

