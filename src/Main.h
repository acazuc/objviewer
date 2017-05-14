#ifndef MAIN_H
# define MAIN_H

# include <librender/Window/Window.h>

using librender::Window;

namespace objviewer
{

	class Main
	{

	private:
		static Window *window;
		static float posX;
		static float posY;
		static float posZ;

	public:
		static void main(int ac, char **av);
		void mouseMoveCallback();

	};

}

#endif
