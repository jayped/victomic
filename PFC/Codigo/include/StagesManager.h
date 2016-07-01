#ifndef STAGESMANAGER_H
#define STAGESMANAGER_H
#include <Ogre.h>
#include <string>
class Stage;
#define NUM_STAGES 20
class StagesManager : public Ogre::Singleton<StagesManager>
{
public:
    StagesManager(void);
    ~StagesManager(void);
    Stage getStage(int stage);
    static StagesManager& getSingleton ();
    static StagesManager* getSingletonPtr ();

private:
    void createStages();
    std::string numberToString ( int num );
    int stringToNumber(std::string s);
    std::string getNextItem( std::stringstream &ss);

private:
    Stage * _allStages;
};
#endif
