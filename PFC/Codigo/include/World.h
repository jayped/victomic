#ifndef WORLD_H
#define WORLD_H
#include <Ogre.h>
#include <btBulletDynamicsCommon.h>

// Clase que define las pantallas del juego.
class World //: public Ogre::SceneNode
{

public:
    World();
    ~World();
    
	// métodos inline
    int worldID() const { return _worldID; } 
    void worldID(int aWorldID) { _worldID = aWorldID; } 
	Ogre::ShadowTechnique shadowTechnique() const { return _shadowTechnique; }
	void shadowTechnique(Ogre::ShadowTechnique aShadowTechnique) { _shadowTechnique = aShadowTechnique; } 
	Ogre::Light::LightTypes lightType() const { return _lightType; }
	void lightType(Ogre::Light::LightTypes aLightType) { _lightType = aLightType; } 
	Ogre::Vector3 lightDirection() const { return _lightDirection; }
	void lightDirection(Ogre::Vector3 aLightDirection) { _lightDirection = aLightDirection; } 
	Ogre::Vector3 lightPosition() const { return _lightPosition; }
	void lightPosition(Ogre::Vector3 aLightPosition) { _lightPosition = aLightPosition; } 
	std::string skyBox() const { return _skyBox; }
	void skyBox(std::string aSkyBox) { _skyBox = aSkyBox; } 

private:
	int _worldID;
	Ogre::ShadowTechnique _shadowTechnique;
	Ogre::Light::LightTypes _lightType;
	Ogre::Vector3 _lightDirection;
	Ogre::Vector3 _lightPosition;
	std::string _skyBox;
};
#endif