#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <queue>

using namespace std;

class Grid{
    public:
        // constuctor
        Grid() { }
        Grid(int G, int cost, int x, int y) :
            _G(G), _cost(cost), _x(x), _y(y), _prevgrid(nullptr) { }
        // get
        int get_G() {return _G;}
        int get_cost() {return _cost;}
        int get_x() {return _x;}
        int get_y() {return _y;}
        Grid* get_prev() { return _prevgrid;}
        // set
        void set_G(int G) {_G = G;}
        void set_cost(int cost) {_cost = cost;}
        void set_x(int x) {_x = x;}
        void set_y(int y) {_y = y;}
        void set_prev(Grid* prev) {_prevgrid = prev;}
        // destructor
        ~Grid(){ }
    private:
        int _G;
        int _cost;
        int _x;
        int _y;
        Grid* _prevgrid;
};

int cal_h(int x, int y, int stop_x, int stop_y);

int main(){
    int origin_grid_x, origin_grid_y;
    int stop_grid_x, stop_grid_y;

    cin >> origin_grid_x >> origin_grid_y;
    cin >> stop_grid_x >> stop_grid_y;

    vector<Grid*> Grid_list;
    // initial cur_grid
    Grid* cur_grid = new Grid(0, 0, origin_grid_x, origin_grid_y);
    Grid* nxt_grid = nullptr;

    // priority_queue<int, vector<int>, greater<int> > cost_list;
    unordered_map<int, int> coordinate_2_cost;

    int right_cost, left_cost, up_cost, down_cost;

    // initial
    // TODO: obstacle?
    right_cost = 1 + cal_h(origin_grid_x + 1, origin_grid_y, stop_grid_x, stop_grid_y);
    Grid* right_grid = new Grid(1, right_cost, origin_grid_x + 1, origin_grid_y);
    right_grid->set_prev(cur_grid);
    Grid_list.push_back(right_grid);

    left_cost = 1 + cal_h(origin_grid_x - 1, origin_grid_y, stop_grid_x, stop_grid_y);
    Grid* left_grid = new Grid(1, left_cost, origin_grid_x - 1, origin_grid_y);
    left_grid->set_prev(cur_grid);
    Grid_list.push_back(left_grid);

    up_cost = 1 + cal_h(origin_grid_x, origin_grid_y + 1, stop_grid_x, stop_grid_y);
    Grid* up_grid = new Grid(1, up_cost, origin_grid_x, origin_grid_y + 1);
    up_grid->set_prev(cur_grid);
    Grid_list.push_back(up_grid);

    down_cost = 1 + cal_h(origin_grid_x, origin_grid_y - 1, stop_grid_x, stop_grid_y);
    Grid* down_grid = new Grid(1, down_cost, origin_grid_x, origin_grid_y - 1);
    down_grid->set_prev(cur_grid);
    Grid_list.push_back(down_grid);

    // Grid_list: all the grid that can possibly be chose, pop out if chosen
    int min_cost = 10000000;
    for (int i = 0; i < Grid_list.size(); i++){
        // TODO: tie?
        if (Grid_list[i]->get_cost() < min_cost){
            min_cost = Grid_list[i]->get_cost();
            nxt_grid = Grid_list[i];
        }
    }

    cout << nxt_grid->get_cost() << endl;

    cout << "-------------iteration----------------" << endl;
    cout << "mincost: " << nxt_grid->get_cost() << endl;
    cout << "x: " << nxt_grid->get_x() << endl;
    cout << "y: " << nxt_grid->get_y() << endl;

    cout << "before" << endl;
    for (int i = 0; i < Grid_list.size(); i++){
        cout << Grid_list[i]->get_cost() << " ";
    }
    cout << endl;

    // update cur_grid
    cur_grid = nxt_grid;

    // pop out
    vector<Grid*>::iterator it = find(Grid_list.begin(), Grid_list.end(), nxt_grid);
    if (it != Grid_list.end()) {
        Grid_list.erase(it);
    }


    cout << "after" << endl;
    for (int i = 0; i < Grid_list.size(); i++){
        cout << Grid_list[i]->get_cost() << " ";
    }
    cout << endl;

    // loop
    while(!((cur_grid->get_x() == stop_grid_x) && (cur_grid->get_y() == stop_grid_y))){
    // for (int i = 0; i < 5; i++){
        cout << "-------------iteration----------------" << endl;
        if (cur_grid->get_prev() == nullptr) {
            cout << "No previous grid, exiting loop." << endl;
            break;
        }
        int prev_x = cur_grid->get_prev()->get_x();
        int prev_y = cur_grid->get_prev()->get_y();
        
        int cur_x = cur_grid->get_x();
        int cur_y = cur_grid->get_y();

        bool is_obstacle = false;
        
        // avoid backtrack (dont cal when obstacle or it is prev block)
        if ( (!((cur_x + 1 == prev_x) && (cur_y == prev_y))) && (is_obstacle == false) ){
            right_cost = cur_grid->get_G() + 1 + cal_h(cur_x + 1, cur_y, stop_grid_x, stop_grid_y);
            cout << "right: " << right_cost << endl;
            right_grid = new Grid(cur_grid->get_G() + 1, right_cost, cur_x + 1, cur_y);
            right_grid->set_prev(cur_grid);
            Grid_list.push_back(right_grid);
        }

        if ( (!((cur_x - 1 == prev_x) && (cur_y == prev_y))) && (is_obstacle == false) ){
            left_cost = cur_grid->get_G() + 1 + cal_h(cur_x - 1, cur_y, stop_grid_x, stop_grid_y);
            cout << "left: " << left_cost << endl;
            left_grid = new Grid(cur_grid->get_G() + 1, left_cost, cur_x - 1, cur_y);
            left_grid->set_prev(cur_grid);
            Grid_list.push_back(left_grid);
        }
        
        if ( (!((cur_x == prev_x) && (cur_y + 1 == prev_y))) && (is_obstacle == false) ){
            up_cost = cur_grid->get_G() + 1 + cal_h(cur_x, cur_y + 1, stop_grid_x, stop_grid_y);
            cout << "up: " << up_cost << endl;
            up_grid = new Grid(cur_grid->get_G() + 1, up_cost, cur_x, cur_y + 1);
            up_grid->set_prev(cur_grid);
            Grid_list.push_back(up_grid);
        }

        if ( (!((cur_x == prev_x) && (cur_y - 1 == prev_y))) && (is_obstacle == false) ){
            down_cost = cur_grid->get_G() + 1 + cal_h(cur_x, cur_y - 1, stop_grid_x, stop_grid_y);
            cout << "down: " << down_cost << endl;
            down_grid = new Grid(cur_grid->get_G() + 1, down_cost, cur_x, cur_y - 1);
            down_grid->set_prev(cur_grid);
            Grid_list.push_back(down_grid);
        }

        min_cost = 10000000;
        for (int i = 0; i < Grid_list.size(); i++){
            // TODO: tie?
            if (Grid_list[i]->get_cost() < min_cost){
                min_cost = Grid_list[i]->get_cost();
                nxt_grid = Grid_list[i];
            }
        }

        cout << "mincost: " << nxt_grid->get_cost() << endl;
        cout << "x: " << nxt_grid->get_x() << endl;
        cout << "y: " << nxt_grid->get_y() << endl;

        // update cur_grid
        cur_grid = nxt_grid;

        cout << "before" << endl;
        for (int i = 0; i < Grid_list.size(); i++){
            cout << Grid_list[i]->get_cost() << " ";
        }
        cout << endl;

        // pop out
        vector<Grid*>::iterator it = find(Grid_list.begin(), Grid_list.end(), nxt_grid);
        if (it != Grid_list.end()) {
            Grid_list.erase(it);
        }

        cout << "after" << endl;
        for (int i = 0; i < Grid_list.size(); i++){
            cout << Grid_list[i]->get_cost() << " ";
        }
        cout << endl;
    }

    cout << "final cost: " << cur_grid->get_cost() << endl;
    cout << "x: " << cur_grid->get_x() << " y: " << cur_grid->get_y() << endl;
    
    // path
    while(1){
        if ((cur_grid->get_x() == origin_grid_x) && (cur_grid->get_y() == origin_grid_y)) break;
        cout << "x: " << cur_grid->get_prev()->get_x() << " y: " << cur_grid->get_prev()->get_y() << endl;
        cur_grid = cur_grid->get_prev();
    }

    return 0;
}

int cal_h(int x, int y, int stop_x, int stop_y){
    int x_out = abs(stop_x - x);
    int y_out = abs(stop_y - y);
    if (x_out >= y_out) return x_out;
    else return y_out;
}