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
            _G(G), _cost(cost), _x(x), _y(y) { }
        // get
        int get_G() {return _G;}
        int get_cost() {return _cost;}
        int get_x() {return _x;}
        int get_y() {return _y;}
        // set
        void set_G(int G) {_G = G;}
        void set_cost(int cost) {_cost = cost;}
        void set_x(int x) {_x = x;}
        void set_y(int y) {_y = y;}
        // destructor
        ~Grid(){ }
    private:
        int _G;
        int _cost;
        int _x;
        int _y;
        Grid* prev_grid;
};

int cal_h(int x, int y, int stop_x, int stop_y);

int main(){
    int origin_grid_x, origin_grid_y;
    int stop_grid_x, stop_grid_y;

    cin >> origin_grid_x >> origin_grid_y;
    cin >> stop_grid_x >> stop_grid_y;

    vector<Grid*> Grid_list;
    Grid* cur_grid;

    // priority_queue<int, vector<int>, greater<int> > cost_list;
    unordered_map<int, int> coordinate_2_cost;

    int right_cost, left_cost, up_cost, down_cost;

    // initial
    // TODO: obstacle?
    right_cost = 1 + cal_h(origin_grid_x + 1, origin_grid_y, stop_grid_x, stop_grid_y);
    Grid* right_grid = new Grid(1, right_cost, origin_grid_x + 1, origin_grid_y);
    Grid_list.push_back(right_grid);

    left_cost = 1 + cal_h(origin_grid_x - 1, origin_grid_y, stop_grid_x, stop_grid_y);
    Grid* left_grid = new Grid(1, left_cost, origin_grid_x - 1, origin_grid_y);
    Grid_list.push_back(left_grid);

    up_cost = 1 + cal_h(origin_grid_x, origin_grid_y + 1, stop_grid_x, stop_grid_y);
    Grid* up_grid = new Grid(1, up_cost, origin_grid_x, origin_grid_y + 1);
    Grid_list.push_back(up_grid);

    down_cost = 1 + cal_h(origin_grid_x, origin_grid_y - 1, stop_grid_x, stop_grid_y);
    Grid* down_grid = new Grid(1, down_cost, origin_grid_x, origin_grid_y - 1);
    Grid_list.push_back(down_grid);

    // Grid_list: all the grid that can possibly be chose, pop out if chosen
    int min_cost = 10000000;
    for (int i = 0; i < Grid_list.size(); i++){
        // TODO: tie?
        if (Grid_list[i]->get_cost() < min_cost){
            min_cost = Grid_list[i]->get_cost();
            cur_grid = Grid_list[i];
        }
    }
    cout << cur_grid->get_cost() << endl;

    cout << "before" << endl;
    for (int i = 0; i < Grid_list.size(); i++){
        cout << Grid_list[i]->get_cost() << " ";
    }
    cout << endl;

    // pop out
    if (cur_grid) {
        vector<Grid*>::iterator it = find(Grid_list.begin(), Grid_list.end(), cur_grid);
        if (it != Grid_list.end()) {
            Grid_list.erase(it);
            delete cur_grid;
        }
    }

    cout << "after" << endl;
    for (int i = 0; i < Grid_list.size(); i++){
        cout << Grid_list[i]->get_cost() << " ";
    }
    cout << endl;

    // loop
    /*
    while(!((cur_grid->get_x() == stop_grid_x) && (cur_grid->get_y() == stop_grid_y))){
        right_cost = 1 + cal_h(cur_grid->get_x() + 1, cur_grid->get_y(), stop_grid_x, stop_grid_y);
        left_cost = 1 + cal_h(cur_grid->get_x() - 1, cur_grid->get_y(), stop_grid_x, stop_grid_y);
        up_cost = 1 + cal_h(cur_grid->get_x(), cur_grid->get_y() + 1, stop_grid_x, stop_grid_y);
        down_cost = 1 + cal_h(cur_grid->get_x(), cur_grid->get_y() - 1, stop_grid_x, stop_grid_y);
    }
    */

    return 0;
}

int cal_h(int x, int y, int stop_x, int stop_y){
    int x_out = abs(stop_x - x);
    int y_out = abs(stop_y - y);
    if (x_out >= y_out) return x_out;
    else return y_out;
}
