class Engine
{
public :
  enum GameStatus
  {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT
  } gameStatus;
  TCOD_key_t lastKey;
  TCOD_mouse_t mouse;
  TCODList<Actor *> actors;
  Actor *player;
  Map *map;
  int fovRadius;
  int screenWidth;
  int screenHeight;
  Gui *gui;
  Actor *getClosestMonster(int x, int y, float range) const;
  bool pickATile(int *x, int *y, float maxRange = 0.0f);
  
  Engine(int screenWidth, int screenHeight);
  ~Engine();
  void update();
  void render();
  void sendToBack(Actor *actor);  
};
 
extern Engine engine;