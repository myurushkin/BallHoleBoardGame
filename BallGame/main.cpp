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

bool readPoint(ifstream& inf, Point& result, int max_value) {
    inf >> result.x >> result.y;
    --result.x;
    --result.y;
    return result.x >= 0 && result.x < max_value
        && result.y >= 0 && result.y < max_value;
}

bool initializeGame(ifstream& inf, int& n_value, int& m_value, int& k_value,
    std::vector<Point>& balls, std::vector<Point>& holes, std::vector<Wall>& walls) {
    inf >> n_value >> k_value >> m_value;
    if (n_value <= 0 || k_value <= 0 || m_value < 0)
        return false;

    for (int i = 0; i < k_value; ++i) {
        Point point;
        if (readPoint(inf, point, n_value) == false)
            return false;
        balls.push_back(point);
    }

    for (int i = 0; i < k_value; ++i) {
        Point point;
        if (readPoint(inf, point, n_value) == false)
            return false;
        holes.push_back(point);
    }

    for (int i = 0; i < m_value; ++i) {
        Wall wall;
        if (readPoint(inf, wall.first_point, n_value) == false
            || readPoint(inf, wall.second_point, n_value) == false)
            return false;
        if (wall.first_point.x == wall.second_point.x) {
            if (wall.first_point.y > wall.second_point.y) {
                std::swap(wall.first_point.y, wall.second_point.y);
            }
        } else {
            if (wall.first_point.x > wall.second_point.x) {
                std::swap(wall.first_point.x, wall.second_point.x);
            }
        }
        walls.push_back(wall);
    }

    return true;
}

void testmod() {
    vector<string> inputFiles = { "tests/input1.txt",
        "tests/input2.txt",
        "tests/input3.txt",
        "tests/input4.txt",
        "tests/input5.txt",
        "tests/input6.txt"
    };
    vector<string> outputFiles = { "tests/output1.txt",
        "tests/output2.txt",
        "tests/output3.txt",
        "tests/output4.txt",
        "tests/output5.txt",
        "tests/output6.txt"
    };

    for (int i = 0; i < inputFiles.size(); ++i) {
        std::cout << "TEST " << i << "... ";
        auto input = inputFiles[i];
        auto output = outputFiles[i];

        list<string> answers;
        ifstream outf(output);
        if (outf.is_open() == false) {
            continue;
        }
        while (!outf.eof()) {
            string answer;
            std::getline(outf, answer);
            if (answer.empty() == false) {
                answers.push_back(answer);
            }
        }

        ifstream inf(input);
        if (inf.is_open() == false) {
            continue;
        }
        std::list<std::string> solutions;
        int n_value, m_value, k_value;
        std::vector<Point> balls, holes;
        std::vector<Wall> walls;

        initializeGame(inf, n_value, m_value, k_value, balls, holes, walls);
        solveProblem(n_value, m_value, k_value, balls, holes, walls, solutions);

        bool success = answers.size() == solutions.size();
        if (success) {
            for (auto& answer : answers) {
                bool found = false;
                for (auto& sol : solutions) {
                    if (sol == answer) {
                        found = true;
                        break;
                    }
                }

                if (found == false) {
                    success = false;
                    break;
                }
            }
        }

        std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    }
}

void solve() {
    std::list<std::string> solutions;
    int n_value, m_value, k_value;
    std::vector<Point> balls, holes;
    std::vector<Wall> walls;

    ifstream inf("input.txt");
    ofstream outf("output.txt");
    if (initializeGame(inf, n_value, m_value, k_value, balls, holes, walls) == false) {
        outf << "Invalid input data";
    }
    if (solveProblem(n_value, m_value, k_value, balls, holes, walls, solutions) == true) {
        for (auto& sol : solutions) {
            outf << sol << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    testmod();
    // solve();
    return 0;
}
