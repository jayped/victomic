//#include "stdafx.h"

#include "StagesManager.h"
#include "Stage.h"
#include <sstream> 
#include <fstream>
#include <iostream>
template<> StagesManager* Ogre::Singleton<StagesManager>::msSingleton = 0;

StagesManager::StagesManager(void)
{
    //_allStages = new Stage[NUM_STAGES];
    for( int i = 0; i < NUM_STAGES; i++ ){
        _allStages[i] = 0;
    }
    createStages();
}


StagesManager::~StagesManager(void)
{
    for( int i = 0; i < NUM_STAGES; i++ ){
        if( _allStages[i]!=0 )
            delete _allStages[i];
    }
}

void StagesManager::createStages(){

    std::string stageName = "./stages/stage";
    std::string fileExt = ".txt";
    std::string stageNum = "0";
    char line[128];
    memset( line, 0, 128 );
    for( int i = 0; i < NUM_STAGES; i++ ){
        if( i < 9 ){
            stageNum = "0";
            stageNum += numberToString(i+1);
        }
        else
        {
            stageNum = numberToString(i+1);
        }

        std::ifstream fileStage(stageName+stageNum+fileExt); 
        //Si el fichero se ha abierto y está en buen estado
        if( fileStage.good() ){
            Stage * stage = new Stage();
            int x = 0;
            int y = 0;
            int z = 0;
            //hasta que lleguemos el final del fichero
            while(!fileStage.eof()){
                //leemos una línea hasta el siguiente retorno de carro de como mucho 128 caracteres
                fileStage.getline(line, 128);

                std::stringstream ssline(line);
                std::string nextItem = "";
                bool incY = true;
                if( stage->stageID() != 0 && stage->world() != 0 ){
                    do{
                        nextItem =  getNextItem(ssline);
                        if( nextItem != "" ){
                            if( nextItem !="-" ){
                                stage->setActors(x,y,z,stringToNumber(nextItem));
                                x=x<14?x+1:0;
                                incY = true;
                            }
                            else{
                                z=z<14?z+1:0;
                                y=0;
                                incY=false;
                            }
                        }
                    }while( nextItem!="" );
                    if(incY){
                        y=y<5?y+1:0;
                    }
                }
                if( stage->stageID() == 0 ){
                    nextItem = getNextItem(ssline);
                    if( nextItem == "Stage" ){
                        nextItem = getNextItem(ssline);
                        stage->stageID(stringToNumber( nextItem ) );
                    }
                }
                if( stage->world() == 0 ){
                    nextItem = getNextItem(ssline);
                    if( nextItem == "World" ){
                        nextItem = getNextItem(ssline);
                        stage->world(stringToNumber( nextItem ) );
                        _allStages[stage->stageID()-1] = stage;
                    }
                }
            }
        }
        else{
            Ogre::LogManager::getSingletonPtr()->logMessage( "Error abriendo el fichero " + stageName+stageNum+fileExt );
        }
    }
}

std::string StagesManager::numberToString ( int num ){
    std::ostringstream ss;
    ss << num;
    return ss.str();
}

int StagesManager::stringToNumber(std::string s){
    int num = 0;
    std::istringstream (s) >> num;
    return num;
}
std::string StagesManager::getNextItem( std::stringstream &ss){
    std::string next = "";
    char cad1[128];
    memset( cad1, 0, 128 );
    //De la línea leída, separamos por el caracter '.'
    ss.getline(cad1, 128, '.');
    next = cad1;   
    return next;
}
Stage * StagesManager::getStage(int stage){
    return _allStages[stage-1];
}

StagesManager* StagesManager::getSingletonPtr ()
{
  return msSingleton;
}

StagesManager& StagesManager::getSingleton ()
{  
  assert(msSingleton);
  return *msSingleton;
}
