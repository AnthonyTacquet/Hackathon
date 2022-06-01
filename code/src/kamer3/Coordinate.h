#ifndef Coordinate_h
#define Coordinate_h

class Coordinate {
  public:
    Coordinate(int x, int y);
    int x;
    int y;

    bool operator==(Coordinate const & other) const {
      return this->x == other.x && this->y == other.y;
    }
};

#endif