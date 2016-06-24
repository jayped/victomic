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
	if (_gameMgr->isDownMov()) {
		_player->move((int)_down, _makeCamera->getCameraPosition());
		_storeMove[0]=true;
	}
    else
    {
        _storeMove[0]=false;
    }
	if (_gameMgr->isRightMov()) {
		_player->move((int)_right, _makeCamera->getCameraPosition());
		_storeMove[1]=true;
	}
    else
    {
        _storeMove[1]=false;
    }
	if (_gameMgr->isUpMov()) {
		_player->move((int)_up, _makeCamera->getCameraPosition());
		_storeMove[2]=true;
	}
    else
    {
        _storeMove[2]=false;
    }
	if (_gameMgr->isLeftMov()) {
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
	_player->getRigitBody()->setAngularVelocity(btVector3(0,0,0));
	
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
	_player = addActor(1.0f,1.75f,1.0f,"nori","Nori.mesh","walking",0.0f,10.0f,0.0f,0.0f,10.0f,0.0f,1,0);
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
			addActor(1.0f, 1.0f, 1.0f,name, "box01.mesh","", -14+(xx*2), 6 +(yy*2), -10.0f+(yy*2), -14+(xx*2), 6 +(yy*2), -10.0f+(yy*2),0, 10);
		}
	}
	*/

	addActor(15.0f, 5.0f, 15.0f,"suelo", "World01.mesh","",		0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0,1);
	addActor(1.0f, 1.0f, 1.0f,"block", "box01.mesh","",			-7.0f,8.0f,7.0f,-7.0f,8.0f,7.0f,0,2);
	addActor(1.0f, 1.0f, 1.0f,"rock6", "RockBox.mesh","",		-7.0f,12.0f,7.0f,-7.0f,12.0f,7.0f,1,3);
	addActor(1.0f, 1.0f, 1.0f,"rockc6", "RockBox.mesh","",		-7.0f,14.0f,7.0f,-7.0f,14.0f,7.0f,1,3);
	addActor(1.0f, 1.0f, 1.0f,"rockd6", "RockBox.mesh","",		14.0f,6.0f,4.0f,14.0f,6.0f,4.0f,1,3);
	addActor(1.0f, .33f, 1.0f,"rock3", "SwitchBaseBox.mesh","",		4.0f,6.0f-.66,-14.0f,4.0f,6.0f-.66,-14.0f,0,8);
	
	addActor(1.0f, 1.0f, 1.0f,"dww", "RockBox.mesh","",	4.0f,	10.0f,		0.0f,	4.0f,	10.0f,		0.0f,1,3);
	addActor(1.0f, 1.0f, 1.0f,"d", "FragileRockBox.mesh","",	4.0f,	6.0f,		0.0f,	4.0f,	6.0f,		0.0f,0,4);
	addActor(1.0f, 1.0f, 1.0f,"dd", "FragileRockBox.mesh","",	6.0f,	6.0f,		0.0f,	6.0f,	6.0f,		0.0f,0,4);
	addActor(1.0f, .33f, 1.0f,"e", "SwitchBox.mesh","",		    -10.0f,	6.0f-.66,	-14.0f,	-10.0f,	6.0f-.66,	-14.0f-.66,0,5);
	
	addActor(.0f, .0f, .0f,"g", "TransparentBox.mesh","",	-6.0f,	6.0f,		-14.0f,	-6.0f,	6.0f,		-14.0f,0,6);
	addActor(1.0f, 1.0f, 1.0f,"h", "SolidBox.mesh","",			-2.0f,	6.0f,		-14.0f,	-2.0f,	6.0f,		-14.0f,0,7);
	
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
					double aMotionPosX,double aMotionPosY,double aMotionPosZ, double aObjectPosX,double aObjectPosY,double aObjectPosZ, btScalar aMass, int aFlags)
{

	btCollisionShape *newFallShape = new btBoxShape(btVector3(aShapeX, aShapeY, aShapeZ));
	Entity *entity = _sceneMgr->createEntity(nameEntity, meshName);
	//SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	Actor *newActor = reinterpret_cast<Actor *>(_sceneMgr->getRootSceneNode()->createChildSceneNode(nameEntity));
	newActor->attachObject(entity);
	newActor->init(aFlags);
  
	// cuerpo rigido para bullet
	MyMotionState* newFallMotionState = new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(aMotionPosX,aMotionPosY,aMotionPosZ)), newActor);
	btVector3 newFallInertia(-0,0,0);
	newFallShape->calculateLocalInertia(aMass,newFallInertia);
	btRigidBody::btRigidBodyConstructionInfo newFallRigidBodyCI(aMass,newFallMotionState,newFallShape,newFallInertia);
	btRigidBody *newFallRigidBody = new btRigidBody(newFallRigidBodyCI);
	_world->addRigidBody(newFallRigidBody);
	newActor->setPosition(aObjectPosX,aObjectPosY,aObjectPosZ);
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
				if ((obA->getFlags() == 0) && (obB->getFlags() == 4))
				{
					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.76) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 2.04))
					{
						colisionesNori = numContacts;
						double centroDeMasasA = obA->getCenterOfMassPosition().getY();
						double centroDeMasasB = obB->getCenterOfMassPosition().getY();
						if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.76) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 2.74))
							//obA->applyCentralImpulse(btVector3(0,0.5,0));
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
				}				

				// 5 Interruptor
				if ((obA->getFlags() == 0) && (obB->getFlags() == 5))
				{
					colisionesNori = numContacts;
					double centroDeMasasA = obA->getCenterOfMassPosition().getY();
					double centroDeMasasB = obB->getCenterOfMassPosition().getY();
					if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.07) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 2.04))
					{
						int x=0;
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
						changeState(ReplayState::getSingletonPtr());
					}
				}				

				/*
				if ((obA->getFlags() == 0) && (obB->getFlags() == 3))
				{
						colisionesNori = numContacts;
						double centroDeMasasA = obA->getCenterOfMassPosition().getY();
						double centroDeMasasB = obB->getCenterOfMassPosition().getY();
						if ( (Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) < 2.76) &&
							(Ogre::Math::Abs(centroDeMasasA-centroDeMasasB) > 2.74))
							//obA->applyCentralImpulse(btVector3(0,0.5,0));
						{
							Actor *iteratorDelete;
							for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
							{
								iteratorDelete = *it;
								if (iteratorDelete->getRigitBody()==obB)
								{
									Ogre::SceneNode *_paraBorrar = dynamic_cast<Ogre::SceneNode*> (iteratorDelete);
									//_sceneMgr->getRootSceneNode()->detachObject(_paraBorrar);
									int numeroHijos = _paraBorrar->numAttachedObjects();
									Entity *ent = static_cast<Entity*>(_paraBorrar->detachObject(unsigned short (0)));
									_sceneMgr->destroyEntity(ent);
									_sceneMgr->destroySceneNode(_paraBorrar);
								}
								 
							}
							_world->removeRigidBody(obB);
							delete obB;
						}
					}*/
				
				
				
				


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

	for (std::list<Actor *>::iterator it=_listOfActors.begin(); it != _listOfActors.end(); ++it)
	{
		Actor *lActor;
		lActor = *it;
		int actorType = lActor->getType();
		
		// 4: // Bloque fragil
		// 5: // Interruptor
		// 8: // Meta de escenario
		switch (actorType)
		{
			case 4:
				if (lActor->getActivated())
				{
					lActor->updateCounter(aDeltaT);
					if ((int)Math::Ceil(lActor->getCounter()*100)  % 2 == 0)
						lActor->setVisible(true);			
					else
						lActor->setVisible(false);			

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
			case 5:
				break;
			case 6:
				_world->removeRigidBody(lActor->getRigitBody());
				activateAllActors();
				break;
			case 8:
				break;
		}

	}

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

