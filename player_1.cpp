#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <vector>
#include <queue>
#include <map>
#include <cstdlib>
#include <ctime>

const int SIZE = 8;
const int ALL = 64;
const int VALUE_MAX = 20000;
const int VALUE_MIN = -20000;

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
    int cur_player;
    int level;
    int value;

    State(std::array<std::array<int, SIZE>, SIZE> o): cur_player(player), level(0), value(0) {
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                othello[i][j] = o[i][j];
    }
    State(const State& s) {
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                othello[i][j] = s.othello[i][j];
        valid_spots = s.valid_spots;
        cur_player = s.cur_player;
        level = s.level;
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
                        value += 150;
                    else
                        value -= 150;
                }
                else if((i == 1 && j == 1) || (i == 1 && j == 6) ||
                        (i == 6 && j == 1) || (i == 6 && j == 6)) {
                    if(othello[i][j] == player)
                        value -= 20;
                }
                else if((i == 0 && j == 1) || (i == 1 && j == 0) ||
                        (i == 0 && j == 6) || (i == 1 && j == 7) ||
                        (i == 6 && j == 0) || (i == 7 && j == 1) ||
                        (i == 6 && j == 7) || (i == 7 && j == 6)) {
                    if(othello[i][j] == player)
                        value -= 10;
                }
                else if(i == 0 || i == 7 || j == 0 || j == 7) {
                    if(othello[i][j] == player)
                        value += 20;
                }
            }
        // difference
        value += (me - op) * 5 * (me + op) / ALL;
        // mobility
        value += valid_spots.size() * 4 * ALL / (me + op);
    }
    void update(Point center) {
        // flip
        for (Point d: dir) {
            Point p = center + d;
            if(p.pt_in_board() && (othello[p.x][p.y] == (3 - cur_player))) {
                do {
                    if(othello[p.x][p.y] == cur_player) {
                        while(p != center) {
                            p = p - d;
                            othello[p.x][p.y] = cur_player;
                        }
                        break;
                    }
                    p = p + d;
                } while(p.pt_in_board() && othello[p.x][p.y]);
            }
        }
        cur_player = 3 - cur_player;
        // change valid_spots
        valid_spots.clear();
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++) {
                Point p(i, j);
                if(is_valid_spot(p))
                    valid_spots.push_back(p);
            }

        level++;
    }
    bool is_valid_spot(Point center) {
        if(othello[center.x][center.y])
            return false;
        for (Point d: dir) {
            Point p = center + d;
            if(p.pt_in_board() && (othello[p.x][p.y] == (3 - cur_player))) {
                do {
                    if(othello[p.x][p.y] == cur_player)
                        return true;
                    p = p + d;
                } while(p.pt_in_board() && othello[p.x][p.y]);
            }
        }
        return false;
    }
};

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            fin >> board[i][j];
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

std::map<int, Point> val_move;

int minimax(State s, int depth, int alpha, int beta) {
    if(depth == 0) {
        s.setValue();
        return s.value;
    }
    else if(s.cur_player == player) {
        int val = VALUE_MIN;

        int n = s.valid_spots.size();
        for(int i = 0; i < n; i++) {
            State next = s;
            next.update(next_valid_spots[i]);
            int m = minimax(next, depth - 1, alpha, beta);
            val = std::max(val, m);
            alpha = std::max(alpha, val);
            if(s.level == 0)
                val_move[m] = next_valid_spots[i];
            if(beta <= alpha)
                break;
        return val;
        }
    }
    else if(s.cur_player == (3 - player)) {
        int val = VALUE_MAX;

        int n = s.valid_spots.size();
        for(int i = 0; i < n; i++) {
            State next = s;
            next.update(next_valid_spots[i]);
            int m = minimax(next, depth - 1, alpha, beta);
            val = std::min(val, m);
            beta = std::min(beta, val);
            if(s.level == 0)
                val_move[m] = next_valid_spots[i];
            if(beta <= alpha)
                break;
            return val;
        }
    }
}

void write_valid_spot(std::ofstream& fout) {
    std::vector<State> old_level, new_level;

    State s(board);
    s.valid_spots = next_valid_spots;

    int val = minimax(s, 5, VALUE_MIN, VALUE_MAX);
    Point best = val_move[val];
    // Remember to flush the output to ensure the last action is written to file.
    fout << best.x << " " << best.y << std::endl;
    fout.flush();

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
