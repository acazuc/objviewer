#ifndef FPS_MANAGER_H
# define FPS_MANAGER_H

# include <cstdint>

namespace objviewer
{

	class FpsManager
	{

	private:
		static uint64_t lastFrame;
		static uint32_t fpsCount;
		static uint32_t fps;

	public:
		static void init();
		static void update();
		static uint32_t getFps() {return (FpsManager::fps);};
	};

}

#endif
