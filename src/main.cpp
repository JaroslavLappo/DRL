#include "libtcod.hpp"
#include "main.h"

 
Engine engine(80, 40);
 
int main()
{
  while (!TCODConsole::isWindowClosed())
  {
    engine.update();
    engine.render();
    TCODConsole::flush();
  }
  return 0;
}