#include <stdio.h>
#include <string.h>
#include <math.h>
#include "main.h"

Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &col) : x(x), y(y), ch(ch), col(col), blocks(true), attacker(NULL), destructible(NULL), ai(NULL), pickable(NULL), container(NULL)
{
  strcpy(this->name, name);
}

Actor::~Actor()
{
  if (attacker) delete attacker;
  if (destructible) delete destructible;
  if (ai) delete ai;
  if (pickable) delete pickable;
  if (container) delete container;
}
 
void Actor::render() const
{
    TCODConsole::root->setChar(x,y,ch);
    TCODConsole::root->setCharForeground(x,y,col);
}

void Actor::update()
{
  if (ai)
    ai->update(this);
}

float Actor::getDistance(int cx, int cy) const
{
  int dx = x - cx;
  int dy = y - cy;
  return sqrtf(dx*dx + dy*dy);
}