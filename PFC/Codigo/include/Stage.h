#ifndef STAGE_H
#define STAGE_H
#include <Ogre.h>
//#include <btBulletDynamicsCommon.h>

// Clase que define las pantallas del juego.
class Stage //: public Ogre::SceneNode
{

public:
    Stage();
    ~Stage();
    
	// métodos inline
    int stageID() const { return _stageID; }
    void stageID(int aStageID) { _stageID = aStageID; }
    int world() const { return _world; } 
    void world(int aWorld) { _world = aWorld; } 

	int *** getActors();
    void setActors(int ***aActors);
    void setActors(int x, int y, int z, int value);

private:
	int _stageID;
	int _world;
	int *** _actors; // [15][6][15]
	int *** _textStage; // [!]
};
#endif