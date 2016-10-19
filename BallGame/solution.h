#ifndef SOLUTION_H
#define SOLUTION_H

struct Point {
    int x, y;

    inline int& first(bool isVertical) {
        return isVertical == true ? y : x;
    }

    int& second(bool isVertical) {
        return isVertical == true ? x : y;
    }
};

struct Wall {
    Point first_point, second_point;
};

struct CellObject {
    enum Type {
        Hole,
        Ball
    };

    CellObject();
    CellObject(int id, Point pos, Type type);

    int id;
    Point pos;
    Type type;
};

class Direction {
public:
    static const int DirectionCount = 4;
    enum Value {
        North = 0,
        East,
        South,
        West
    };

    static char toSymbol(Direction::Value dir);
    static Direction::Value opposite(Direction::Value dir);
    static Direction::Value left(Direction::Value dir);
    static Direction::Value right(Direction::Value dir);
};

class Board {
public:
    Board(int nValue);
    int nearestWallPos(const Point& cell, Direction::Value dir);
    void addWall(const Wall& wall);

private:
    inline int index(int x, int y, int dir) {
        return dir * m_nValue * m_nValue + x * m_nValue + y;
    }

    int m_nValue;
    std::vector<int> m_cell2Wall;
};

class State {
public:
    struct CellObjectPosition {
        CellObject data;
        int next[Direction::DirectionCount];

    };

    State() {}
    struct iterator {
        iterator& operator++();
        bool operator!=(const iterator& other);
        CellObjectPosition& operator*();

        int id;
        Direction::Value dir;
        State* parent;
    };

    bool isSolution();
    void addStep(char value);
    std::string allSteps();

    int depth();
    void setDepth(int depth);

    void setObjectCount(int count);
    CellObjectPosition& operator[](int id);
    void setObject(int id, const CellObject& obj);

    int firstObject(Direction::Value dir);
    void setFirstObject(Direction::Value dir, int id);

    iterator begin(Direction::Value dir);
    iterator end(Direction::Value dir);
private:
    int m_depth;
    std::list<char> m_steps;
    Direction::Value m_dir;

    std::vector<CellObjectPosition> m_objects;
    int m_firstObjectIds[4];
};

bool solveProblem(int& n_value, int& m_value, int& k_value,
    std::vector<Point>& balls, std::vector<Point>& holes,
    std::vector<Wall>& walls, std::list<std::string>& solutions);

#endif
