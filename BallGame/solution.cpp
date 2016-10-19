#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <queue>
#include <cassert>
#include <string>

#include "solution.h"

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::queue;
using std::list;

struct ObjectRowList {
public:
    std::vector<int> ids;
    std::vector<int> counts;
    int nValue;

    ObjectRowList(int nValue)
        : nValue(nValue) {
        counts.resize(nValue);
        ids.resize(nValue * nValue);
        clear();
    }

    void clear() {
        std::fill(counts.begin(), counts.end(), 0);
    }

    void add(int row, int id) {
        ids[row * nValue + counts[row]] = id;
        ++counts[row];
    }
    void pop(int row) {
        --counts[row];
        assert(counts[row] >= 0);
    }

    int id(int row, int index) {
        return ids[row * nValue + index];
    }
};

char Direction::toSymbol(Direction::Value dir) {
    switch (dir) {
    case South: return 'S';
    case West: return 'W';
    case East: return 'E';
    case North: return 'N';
    }

    throw std::runtime_error("undefined value");
}

Direction::Value Direction::opposite(Direction::Value dir) {
    switch (dir) {
    case South: return North;
    case West: return East;
    case East: return West;
    case North: return South;
    }

    throw std::runtime_error("undefined direction");
}

Direction::Value Direction::left(Direction::Value dir) {
    switch (dir) {
    case South: return East;
    case West: return South;
    case East: return North;
    case North: return West;
    }

    throw std::runtime_error("undefined direction");
}

Direction::Value Direction::right(Direction::Value dir) {
    switch (dir) {
    case South: return West;
    case West: return North;
    case East: return South;
    case North: return East;
    }

    throw std::runtime_error("undefined direction");
}

CellObject::CellObject() {
}
CellObject::CellObject(int id, Point pos, Type type)
    : id(id), pos(pos), type(type) {
}

Board::Board(int nValue)
    : m_nValue(nValue) {
    m_cell2Wall.resize(nValue * nValue * 4);
    for (int x = 0; x < nValue; ++x) {
        for (int y = 0; y < nValue; ++y) {
            m_cell2Wall[index(x, y, Direction::North)] = -1;
            m_cell2Wall[index(x, y, Direction::South)] = nValue;
            m_cell2Wall[index(x, y, Direction::West)] = -1;
            m_cell2Wall[index(x, y, Direction::East)] = nValue;
        }
    }
}

int Board::nearestWallPos(const Point& cell, Direction::Value dir) {
    return m_cell2Wall[index(cell.x, cell.y, dir)];
}

void Board::addWall(const Wall& wall) {
    bool isVertical = wall.first_point.y == wall.second_point.y;
    int staticCoord = isVertical == true ? wall.first_point.y : wall.first_point.x;
    int otherCoord = isVertical == true ? wall.first_point.x : wall.first_point.y;
    for (int coord = 0; coord < m_nValue; ++coord) {
        if (coord <= otherCoord) {
            if (isVertical == true) {
                m_cell2Wall[index(coord, staticCoord, Direction::East)] = 
                    std::min(m_cell2Wall[index(coord, staticCoord, Direction::East)], otherCoord + 1);
            } else {
                m_cell2Wall[index(staticCoord, coord, Direction::South)] = 
                    std::min(m_cell2Wall[index(staticCoord, coord, Direction::South)], otherCoord + 1);
            }
        } else {
            if (isVertical == true) {
                m_cell2Wall[index(coord, staticCoord, Direction::West)] = 
                    std::max(m_cell2Wall[index(coord, staticCoord, Direction::West)], otherCoord);
            } else {
                m_cell2Wall[index(staticCoord, coord, Direction::North)] = 
                    std::max(m_cell2Wall[index(staticCoord, coord, Direction::North)], otherCoord);
            }
        }

    }
}

bool State::isSolution() {
    return firstObject(Direction::North) < 0;
}

std::string State::allSteps() {
    std::string result(m_steps.begin(), m_steps.end());
    return result;
}

int State::depth() {
    return m_depth;
}
void State::setDepth(int value) {
    m_depth = value;
}

void State::addStep(char value) {
    m_steps.push_back(value);
}

State::CellObjectPosition& State::operator[](int id) {
    return m_objects[id];
}

void State::setObjectCount(int count) {
    assert(count > 0);
    m_objects.resize(count);
}

void State::setObject(int id, const CellObject& obj) {
    m_objects[id].data = obj;
}

int State::firstObject(Direction::Value dir) {
    return m_firstObjectIds[dir];
}
void State::setFirstObject(Direction::Value dir, int id) {
    m_firstObjectIds[dir] = id;
}

State::iterator& State::iterator::operator++() {
    id = parent->m_objects[id].next[Direction::opposite(dir)];
    return *this;
}
bool State::iterator::operator!=(const State::iterator& other) {
    return id != other.id;
}
State::CellObjectPosition& State::iterator::operator*() {
    return parent->m_objects[id];
}

State::iterator State::begin(Direction::Value dir) {
    State::iterator result;
    result.dir = dir;
    result.parent = this;
    result.id = m_firstObjectIds[dir];
    return result;
}
State::iterator State::end(Direction::Value dir) {
    iterator result;
    result.dir = dir;
    result.parent = this;
    result.id = -1;
    return result;
}

struct sort_cell_key {
    bool isVertically;
    sort_cell_key(bool isVertically)
        : isVertically(isVertically) {
    }

    inline bool operator() (const CellObject& first, const CellObject& second) {
        if (isVertically == true) {
            if (first.pos.x != second.pos.x) {
                return first.pos.x < second.pos.x;
            }
            return first.pos.y < second.pos.y;
        }

        if (first.pos.y != second.pos.y) {
            return first.pos.y < second.pos.y;
        }
        return first.pos.x < second.pos.x;
    }
};

bool solveProblem(int& n_value, int& m_value, int& k_value,
    std::vector<Point>& balls, std::vector<Point>& holes,
    std::vector<Wall>& walls, std::list<std::string>& solutions) {

    Board board(n_value);
    for (auto& wall : walls) {
        board.addWall(wall);
    }

    State initState;
    std::vector<CellObject> allObjects;
    int idTemp = 0;
    for (auto& ball : balls) {
        allObjects.push_back(CellObject(idTemp++, ball, CellObject::Ball));
    }
    for (auto& hole : holes) {
        allObjects.push_back(CellObject(idTemp++, hole, CellObject::Hole));
    }

    initState.setObjectCount(k_value * 2);
    for (unsigned int i = 0; i < allObjects.size(); ++i) {
        int id = allObjects[i].id;
        initState[id].data = allObjects[i];
    }

    for (bool sortVertically : { false, true }) {
        Direction::Value dir = sortVertically == true ? Direction::North : Direction::West;
        Direction::Value oppdir = sortVertically == true ? Direction::South : Direction::East;
        std::sort(allObjects.begin(), allObjects.end(), sort_cell_key(sortVertically));
        initState.setFirstObject(dir, allObjects.front().id);
        initState.setFirstObject(oppdir, allObjects.back().id);
        for (unsigned int i = 0; i < allObjects.size(); ++i) {
            int id = allObjects[i].id;
            initState[id].next[dir] = i > 0 ? allObjects[i - 1].id : -1;
            initState[id].next[oppdir] = i < allObjects.size() - 1 ? allObjects[i + 1].id : -1;
        }
    }

    std::queue<State> states;
    states.push(initState);

    ObjectRowList objRowList(n_value);
    while (states.empty() == false) {
        State currentState = states.front();
        states.pop();

        if (solutions.empty() == false && solutions.back().length() < static_cast<size_t>(currentState.depth()) + 1)
            break;

        for (auto dir : { Direction::West, Direction::North, Direction::East, Direction::South }) {
            objRowList.clear();
            bool isVertical = dir == Direction::South || dir == Direction::North;
            int step = dir == Direction::North || dir == Direction::West ? 1 : -1;

            State newState;
            newState = currentState;
            newState.setDepth(currentState.depth() + 1);
            newState.addStep(Direction::toSymbol(dir));

            bool isErrorState = false;
            for (auto it = newState.begin(dir); it != newState.end(dir); ++it) {
                State::CellObjectPosition& nextObj = *it;
                int prevCellObjectId = nextObj.next[dir];
                if (nextObj.data.type == CellObject::Hole) {
                    prevCellObjectId = nextObj.data.id;
                    objRowList.add(nextObj.data.pos.first(isVertical), nextObj.data.id);
                    continue;
                }
                int wallPos = board.nearestWallPos(nextObj.data.pos, dir);
                if (prevCellObjectId < 0 || newState[prevCellObjectId].data.pos.second(isVertical) != nextObj.data.pos.second(isVertical)) {
                    nextObj.data.pos.first(isVertical) = wallPos + step;
                    objRowList.add(nextObj.data.pos.first(isVertical), nextObj.data.id);
                    continue;
                } else {
                    auto& prevCellObject = newState[prevCellObjectId].data;
                    if (prevCellObject.type == CellObject::Ball
                        || (wallPos - prevCellObject.pos.first(isVertical))*step >= 0) {
                        if (step > 0) {
                            nextObj.data.pos.first(isVertical) = std::max(wallPos, prevCellObject.pos.first(isVertical)) + step;
                        } else {
                            nextObj.data.pos.first(isVertical) = std::min(wallPos, prevCellObject.pos.first(isVertical)) + step;
                        }
                        objRowList.add(nextObj.data.pos.first(isVertical), nextObj.data.id);
                        continue;
                    } else {
                        if (newState[prevCellObjectId].data.id % k_value == nextObj.data.id) {
                            Direction::Value odir = Direction::opposite(dir);

                            int nextFirst = newState[nextObj.data.id].next[odir];
                            int prevFirst = newState[prevCellObjectId].next[dir];

                            if (newState.firstObject(dir) == prevCellObjectId) {
                                newState.setFirstObject(dir, nextFirst);
                            }
                            if (newState.firstObject(odir) == nextObj.data.id) {
                                newState.setFirstObject(odir, prevFirst);
                            }
                            if (prevFirst >= 0) {
                                newState[prevFirst].next[odir] = nextFirst;
                            }
                            if (nextFirst >= 0) {
                                newState[nextFirst].next[dir] = prevFirst;
                            }

                            objRowList.pop(newState[prevCellObjectId].data.pos.first(isVertical));
                        } else {
                            isErrorState = true;
                            break;
                        }
                    }
                }
            }

            if (isErrorState == true) {
                continue;
            }

            Direction::Value prevdir = dir == Direction::North 
                || dir == Direction::East ? Direction::left(dir) : Direction::right(dir);
            Direction::Value nextdir = dir == Direction::North 
                || dir == Direction::East ? Direction::right(dir) : Direction::left(dir);
            bool ismaindir = dir == Direction::North || dir == Direction::West;

            int prevId = -1, nextId = -1;
            newState.setFirstObject(prevdir, -1);
            for (int i = 0; i < n_value; ++i) {
                int from = ismaindir ? 0 : objRowList.counts[i] - 1;
                int to = ismaindir ? objRowList.counts[i] - 1 : 0;
                int inc = ismaindir ? 1 : -1;

                for (int j = from; (to - j) * inc >= 0; j += inc) {
                    nextId = objRowList.id(i, j);
                    newState[nextId].next[prevdir] = prevId;
                    if (prevId >= 0) {
                        newState[prevId].next[nextdir] = nextId;
                    } else {
                        newState.setFirstObject(prevdir, nextId);
                    }
                    prevId = nextId;
                }
            }

            if (nextId >= 0) {
                newState[nextId].next[nextdir] = -1;
            }
            newState.setFirstObject(nextdir, nextId);
            if (newState.isSolution() == true) {
                string nextSolution = newState.allSteps();
                solutions.push_back(nextSolution);
                continue;
            } else {
                if (solutions.empty() == true) {
                    states.push(newState);
                }
            }
        }
    }
    return true;
}
