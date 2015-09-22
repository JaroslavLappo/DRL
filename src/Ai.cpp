#include <stdio.h>
#include <math.h>
#include "main.h"

static const int TRACKING_TURNS = 3;

void PlayerAi::update(Actor *owner)
{
  int dx=0, dy=0;
  
  switch(engine.lastKey.vk)
  {
    case TCODK_UP : dy = -1; break;
    case TCODK_DOWN : dy = 1; break;
    case TCODK_LEFT : dx = -1; break;
    case TCODK_RIGHT : dx = 1; break;
    case TCODK_CHAR : handleActionKey(owner, engine.lastKey.c); break;
    case TCODK_ESCAPE :
      exit(0);
    default:break;
  }

  if (owner->destructible && owner->destructible->isDead())
  {
    return;
  }

  if (dx != 0 || dy != 0)
  {
    engine.gameStatus=Engine::NEW_TURN;
    if (moveOrAttack(owner, owner->x + dx,owner->y + dy))
    {
      engine.map->computeFov();
    }
  }
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety)
{
  if (engine.map->isWall(targetx, targety))
    return false;
  // look for living actors to attack
  for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++)
  {
    Actor *actor = *iterator;
    if (actor->destructible && !actor->destructible->isDead() && actor->x == targetx && actor->y == targety)
    {
      owner->attacker->attack(owner, actor);
      return false;
    }
  }
  // look for corpses && items
  for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++)
  {
    Actor *actor = *iterator;
    bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->pickable;
    if (corpseOrItem && actor->x == targetx && actor->y == targety)
      engine.gui->message(TCODColor::white, "There's a %s here.",actor->name);
  }
  owner->x = targetx;
  owner->y = targety;
  return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ch)
{
  switch (ch)
  {
  case 'g': // pickup item
  {
    bool found = false;
    for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++)
    {
      Actor *actor = *iterator;
      if (actor->pickable && actor->x == owner->x && actor->y == owner->y)
      {
        if (actor->pickable->pick(actor, owner))
        {
          found = true;
          engine.gui->message(TCODColor::lightGrey, "You pick the %s.",
            actor->name);
          break;
        }
        else if (!found)
        {
          found = true;
          engine.gui->message(TCODColor::red, "Your inventory is full.");
        }
      }
    }
    if (!found)
      engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick.");
    engine.gameStatus = Engine::NEW_TURN;
    break;
  }
  case 'i': // display inventory
  {
    Actor *actor = choseFromInventory(owner);
    if (actor)
    {
      actor->pickable->use(actor, owner);
      engine.gameStatus = Engine::NEW_TURN;
    }
  }
  break;
  case 'd':
  {
    if (engine.player->ch == '@')
    {
      engine.player->ch = 'D';
      engine.player->attacker->power = 20;
      engine.player->destructible->hp = engine.player->destructible->maxHp * 10;
      engine.gui->message(TCODColor::white, "You have turned into a dragon.");
    }
    else
    {
      engine.player->ch = '@';
      engine.player->attacker->power = 5;
      engine.player->destructible->hp = engine.player->destructible->maxHp;
      engine.gui->message(TCODColor::white, "You have turned into a human.");
    }
    break;
  }
  case '?':
  {
    engine.gui->message(TCODColor::white, "You can move hero by pressing arrows.");
    engine.gui->message(TCODColor::white, "You can attack monsters by walking on them.");
    engine.gui->message(TCODColor::white, "You can pick items by pressing 'g', while staying on them.");
    engine.gui->message(TCODColor::white, "You can open your inventory, by pressing 'i'.");
    engine.gui->message(TCODColor::white, "If there are some problems, just press 'd'.");
  }
  }
}

Actor *PlayerAi::choseFromInventory(Actor *owner)
{
  static const int INVENTORY_WIDTH = 50;
  static const int INVENTORY_HEIGHT = 28;
  static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
  // display the inventory frame
  con.setDefaultForeground(TCODColor(200, 180, 50));
  con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");
  // display the items with their keyboard shortcut
  con.setDefaultForeground(TCODColor::white);
  int shortcut = 'a';
  int y = 1;
  for (Actor **it = owner->container->inventory.begin(); it != owner->container->inventory.end(); it++)
  {
    Actor *actor = *it;
    con.print(2, y, "%c - %s", shortcut, actor->name);
    y++;
    shortcut++;
  }

  // blit the inventory console on the root console
  TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, engine.screenWidth / 2 - INVENTORY_WIDTH / 2, engine.screenHeight / 2 - INVENTORY_HEIGHT / 2);
  TCODConsole::flush();
  // wait for a key press
  TCOD_key_t key;
  TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);

  if (key.vk == TCODK_CHAR)
  {
    int actorIndex = key.c - 'a';

    if (actorIndex >= 0 && actorIndex < owner->container->inventory.size())
      return owner->container->inventory.get(actorIndex);
  }
  return NULL;
}

void MonsterAi::update(Actor *owner)
{
  if (owner->destructible && owner->destructible->isDead())
    return;

  moveOrAttack(owner, engine.player->x, engine.player->y);
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety)
{
  int dx = targetx - owner->x;
  int dy = targety - owner->y;
  int stepdx = dx > 0 ? 1 : -1;
  int stepdy = dy > 0 ? 1 : -1;

  float distance = sqrtf(dx * dx + dy * dy);
  
  if (distance < 2)
  {
    if (owner->attacker)
    {
      owner->attacker->attack(owner, engine.player);
    }

    return;
  }

  if (engine.map->isInFov(owner->x, owner->y))
  {
    dx = (int)(round(dx / distance));
    dy = (int)(round(dy / distance));
    if (engine.map->canWalk(owner->x + dx, owner->y + dy))
    {
      owner->x += dx;
      owner->y += dy;
      return;
    }
    if (engine.map->canWalk(owner->x + stepdx, owner->y))
    {
      owner->x += stepdx;
      return;
    }
    if (engine.map->canWalk(owner->x, owner->y + stepdy))
    {
      owner->y += stepdy;
      return;
    }
    return;
  }

  unsigned int bestLevel = 0;
  int bestCellIndex = -1;
  static int tdx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
  static int tdy[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

  for (int i = 0; i< 8; i++)
  {
    int cellx = owner->x + tdx[i];
    int celly = owner->y + tdy[i];
    if (engine.map->canWalk(cellx, celly))
    {
      unsigned int cellScent = engine.map->getScent(cellx, celly);

      if (cellScent > engine.map->currentScentValue - SCENT_THRESHOLD && cellScent > bestLevel)
      {
        bestLevel = cellScent;
        bestCellIndex = i;
      }
    }
  }

  if (bestCellIndex != -1)
  {
    // the monster smells the player. follow the scent
    owner->x += tdx[bestCellIndex];
    owner->y += tdy[bestCellIndex];
  }
}