#include "FpsManager.h"
#include <iostream>
#include <chrono>

namespace objviewer
{

	uint64_t FpsManager::lastFrame;
	uint32_t FpsManager::fpsCount;
	uint32_t FpsManager::fps;

	static uint64_t microtime()
	{
		return (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch()).count());
	}

	void FpsManager::init()
	{
		lastFrame = microtime();
	}

	void FpsManager::update()
	{
		if (microtime() - lastFrame >= 1000000)
		{
			lastFrame += 1000000;
			fps = fpsCount;
			fpsCount = 0;
		}
		else
		{
			fpsCount++;
		}
	}

}
