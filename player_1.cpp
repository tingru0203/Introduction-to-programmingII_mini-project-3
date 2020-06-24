#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include <map>
#include <cstdlib>
#include <ctime>

const int SIZE = 8;

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
	bool pt_in_board() {
        if(x >= 0 && x < SIZE && y >= 0 && y < SIZE)
            return true;
        return false;
	}
};

int player;
const std::array<Point, 8> dir{{Point(-1, -1), Point(-1, 0), Point(-1, 1), Point(0, -1),
                                Point(0, 1), Point(1, -1), Point(1, 0), Point(1, 1)}};
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

class State {
public:
    std::array<std::array<int, SIZE>, SIZE> othello;
    std::vector<Point> valid_spots;
    Point next_spot;
    int value;

    State(std::array<std::array<int, SIZE>, SIZE> o) {
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                othello[i][j] = o[i][j];
        setValue();
    }
    State(const State& s) {
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                othello[i][j] = s.othello[i][j];
        valid_spots = s.valid_spots;
        next_spot = s.next_spot;
        value = s.value;
    }
    void setValue() {
        value = 0;
        int me = 0, op = 0;
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++) {
                if(!othello[i][j]) // empty
                    continue;
                if(othello[i][j] == player)
                    me++;
                else
                    op++;

                if((i == 0 && j == 0) || (i == 0 && j == 7) ||
                   (i == 7 && j == 0) || (i == 7 && j == 7)) {
                    if(othello[i][j] == player)
                        value += 100;
                    else
                        value -= 100;
                }
                else if((i == 1 && j == 1) || (i == 1 && j == 6) ||
                        (i == 6 && j == 1) || (i == 6 && j == 6)) {
                    if(othello[i][j] == player)
                        value -= 50;
                    else
                        value += 50;
                }
                else if((i == 0 && j == 1) || (i == 1 && j == 0) ||
                        (i == 0 && j == 6) || (i == 1 && j == 7) ||
                        (i == 6 && j == 0) || (i == 7 && j == 1) ||
                        (i == 6 && j == 7) || (i == 7 && j == 6)) {
                    if(othello[i][j] == player)
                        value -= 30;
                    else
                        value += 30;
                }
            }
        if(me + op > 30)
            value += (me - op) * 2;
    }
    void update(Point center) {
        valid_spots.clear();

        // flip
        for (Point d: dir) {
            Point p = center + d;
            if(!p.pt_in_board())
                continue;
            if(othello[p.x][p.y] != (3 - player))
                continue;
            p = p + d;
            while(p.pt_in_board() && othello[p.x][p.y] != 0) {
                if(othello[p.x][p.y] == player) {
                    while(p != center) {
                        p = p - d;
                        othello[p.x][p.y] = player;
                    }
                    break;
                }
                p = p + d;
            }
        }
        // push valid_spots
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++) {
                if(othello[i][j] != 0)
                    continue;
                Point p(i, j);
                if(is_valid_spot(p))
                    valid_spots.push_back(p);
            }

        setValue();
    }
    bool is_valid_spot(Point center) {
        for (Point d: dir) {
            Point p = center + d;
            if(!p.pt_in_board())
                continue;
            if(othello[p.x][p.y] != (3 - player))
                continue;
            p = p + d;
            while(p.pt_in_board() && othello[p.x][p.y] != 0) {
                if(othello[p.x][p.y] == player)
                    return true;
                p = p + d;
            }
        }
        return false;
    }
    bool operator<(const State& rhs) const {
        return value < rhs.value;
    }
};

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    State s(board);
    std::priority_queue<State> pq;

    for (auto c : next_valid_spots) {
        // Remember to flush the output to ensure the last action is written to file.
        fout << c.x << " " << c.y << std::endl;
        fout.flush();
        State next = s;
        next.update(c);
        next.next_spot = c;
        pq.push(next);
    }

    while(!pq.empty()) {
        State cur = pq.top();
        // Remember to flush the output to ensure the last action is written to file.
        fout << cur.next_spot.x << " " << cur.next_spot.y << std::endl;
        fout.flush();

        pq.pop();

        for(auto c : cur.valid_spots) {
            State next = cur;
            next.update(c);
            if(next.value > cur.value - 200)
                pq.push(next);
        }
    }
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

