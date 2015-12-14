#ifndef ROOM_H
#define ROOM_H

struct Room
{
  Room(int x_, int y_, int w_, int h_, const Room* master_ = nullptr)
    : x(x_),
      y(y_),
      w(w_),
      h(h_),
      master(master_)
  {}

  bool operator==(const Room& room)
  {
    return x == room.x && y == room.y && w == room.w && h == room.h;
  }

  int x, y;
  int w, h;

  const Room* master;
};

#endif /* ROOM_H */