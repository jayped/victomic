#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <string>

class GameTimer
{
public:
	GameTimer():theInitialTime( 0 ), theFinalTime( 0 ), isStarted( false ){}
	~GameTimer();
	void start();
	void stop();
	std::string getActualTime();
	std::string getTotalTime();
	bool getStarted();

private:
	
	unsigned long	theInitialTime;
	unsigned long	theFinalTime;
	bool			isStarted;
};

#endif