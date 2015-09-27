#include "main.h"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight)
{

  TCODConsole::setCustomFont("terminal8x12_gs_ro.png", TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
  TCODConsole::initRoot(screenWidth, screenHeight, "RL", FULLSCREEN, TCOD_RENDERER_SDL);
  player = new Actor(40, 25, '@', "player", TCODColor::white);
  player->destructible = new PlayerDestructible(30, 2, "your cadaver");
  player->attacker = new Attacker(5);
  player->ai = new PlayerAi();
  player->container = new Container(26);
  actors.push(player);
  map = new Map(screenWidth, screenHeight - 7);
  gui = new Gui();
  gui->message(TCODColor::red, "Welcome stranger!\nPress ? for help.");
}

Engine::~Engine()
{
  actors.clearAndDelete();
  delete map;
  delete gui;
}

void Engine::update()
{
  if (gameStatus == STARTUP)
    map->computeFov();
  gameStatus = IDLE;
  TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);

  player->update();
  if (gameStatus == NEW_TURN)
  {
    map->currentScentValue++;

    for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++)
    {
      Actor *actor = *iterator;
      if ( actor != player )
      {
        actor->update();
      }
    }
  }
}

void Engine::render()
{
  TCODConsole::root->clear();
    // draw the map
  map->render();
  // draw the actors
  for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++)
  {
    Actor *actor = *iterator;
    if (map->isInFov(actor->x, actor->y))
    {
      actor->render();
    }
  }
  gui->render();
  player->render();
  // show the player's stats
}

void Engine::sendToBack(Actor *actor)
{
  actors.remove(actor);
  actors.insertBefore(actor,0);
}