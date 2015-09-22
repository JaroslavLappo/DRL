static const int SCENT_THRESHOLD = 20;

struct Tile
{
  bool explored; // has the player already seen this tile ?
  unsigned int scent; // amount of player scent on this cell
  Tile() : explored(false), scent(0) {}
};
 
class Map
{
public :
  int width,height;

  Map(int width, int height);
  ~Map();
  void addItem(int x, int y);
  bool isWall(int x, int y) const;
  bool canWalk(int x, int y) const;
  bool isInFov(int x, int y) const;
  bool isExplored(int x, int y) const;
  void computeFov();  
  void render() const;
  void addMonster(int x, int y);
  unsigned int currentScentValue;
  unsigned int getScent(int x, int y) const;
protected :
  Tile *tiles;
  TCODMap *map;
  friend class BspListener;
  void dig(int x1, int y1, int x2, int y2);
  void createRoom(bool first, int x1, int y1, int x2, int y2);
};