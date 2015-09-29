#ifdef _WIN32
#include <Windows.h>

#define main() WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

#endif

#define FULLSCREEN 0

#include "libtcod.hpp"

#define NAME_LENGTH 50

class Actor;
#include "Destructible.h"
#include "Attacker.h"
#include "Ai.h"
#include "Pickable.h"
#include "Container.h"
#include "Actor.h"
#include "Gui.h"
#include "Map.h"
#include "Engine.h"