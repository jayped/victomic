#include "GameTimer.h"
#include <OgreTimer.h>
GameTimer::~GameTimer()
{
	isStarted = false;
	theInitialTime = 0;
	theFinalTime = 0;
}
void GameTimer::start()
{
	isStarted = true;
	Ogre::Timer aTimer;
	theInitialTime = aTimer.getMilliseconds(); 
}
void GameTimer::stop()
{
	Ogre::Timer aTimer;
	theFinalTime = aTimer.getMilliseconds(); 
}
std::string GameTimer::getActualTime()
{
	std::string aStrTime;
	Ogre::Timer aTimer;
	unsigned long aULTime = aTimer.getMilliseconds(); 
	return aStrTime;
}
std::string GameTimer::getTotalTime()
{
	std::string aStrTime;
	
	return aStrTime;
}
bool GameTimer::getStarted()
{
	return isStarted;
}