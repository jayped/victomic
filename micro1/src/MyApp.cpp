#include "MyApp.h" 

MyApp::MyApp() {
  _sceneManager = NULL;
  _framelistener = NULL;
}

MyApp::~MyApp() {
  delete _root;
  delete _framelistener;
}

int MyApp::start() {
  
	// root creation
	_root = new Ogre::Root();

	// config restore
	if(!_root->restoreConfig()) {
		_root->showConfigDialog();
		_root->saveConfig();
	}
  
	// scene manager
	Ogre::RenderWindow* window = _root->initialise(true,"MyApp Example"); // creamos la ventana de forma automatica y guardamos el puntero a un objeto de vuelta en window.
	_sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
	_sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	_sceneManager->addRenderQueueListener(new Ogre::OverlaySystem());

	// camera
	Ogre::Camera* cam = _sceneManager->createCamera("MainCamera");
	cam->setPosition(Ogre::Vector3(10,10,2));
	cam->lookAt(Ogre::Vector3(0,0,0));
	cam->setDirection(Ogre::Vector3(-0.006,-0.165,-0.9621));
	cam->setNearClipDistance(5);
	cam->setFarClipDistance(10000);

	Ogre::Viewport* viewport = window->addViewport(cam);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));
	double width = viewport->getActualWidth();
	double height = viewport->getActualHeight();
	cam->setAspectRatio(width / height);
  
	// starting
	loadResources();
	createScene();
	createOverlay();

	_framelistener = new MyFrameListener(window,cam,_sceneManager->getRootSceneNode(),_overlayManager,_sceneManager);
	_root->addFrameListener(_framelistener);
  
	_root->startRendering();
	return 0;
}

void MyApp::loadResources() {
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");
  
  Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
  Ogre::String sectionstr, typestr, datastr;
  while (sI.hasMoreElements()) {
    sectionstr = sI.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) {
      typestr = i->first;    datastr = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation
            (datastr, typestr, sectionstr);	
    }
  }
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void MyApp::createScene() {
	
	/*
	// Creacion del tablero.
	for (int i=0; i<10; i++)
	{
		for (int j=0; j<10; j++)
		{
			int cuborand = rand()%2;
			Ogre::Entity* ent1;
			if (cuborand == 1)
				ent1 = _sceneManager->createEntity("cube.mesh");
			else
				ent1 = _sceneManager->createEntity("cuboejes.mesh");
			ent1->setQueryFlags(cuborand?0:1);

			Ogre::SceneNode* node1 = _sceneManager->createSceneNode();
			node1->attachObject(ent1);
			node1->translate(i*2.1,j*2.1,-50);
			
			_sceneManager->getRootSceneNode()->addChild(node1);
		}
	}
	*/

	// Creacion del plano
	Ogre::Plane pl1(Ogre::Vector3::UNIT_Y,-5);
	Ogre::MeshManager::getSingleton().createPlane("pl1",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		pl1,200,200,1,1,true,1,20,20,Ogre::Vector3::UNIT_Z);

	// Añadir el plano a la escena
	Ogre::SceneNode* nodeG = _sceneManager->createSceneNode("nodeG");
	Ogre::Entity* grEnt = _sceneManager->createEntity("pEnt", "pl1");
	grEnt->setMaterialName("Ground");
	nodeG->attachObject(grEnt);

	// Sombras
	_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Light* light = _sceneManager->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));
	nodeG->attachObject(light);
	_sceneManager->getRootSceneNode()->addChild(nodeG);
	
}

void MyApp::createOverlay() {
	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay = _overlayManager->getByName("Info");
	Ogre::Overlay *overlayTitle = _overlayManager->getByName("Title");
	Ogre::Overlay *overlayWin = _overlayManager->getByName("Win");
	Ogre::Overlay *overlayLose = _overlayManager->getByName("Lose");
	overlay->show();
	overlayTitle->show();
}
