#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <map>
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
void astar_search(int origin_grid_x, int origin_grid_y, int stop_grid_x, int stop_grid_y);

int main(){
    // step 1: net ordering
    // TODO
    
    // step 2: routing
    vector<int> net;
    for (int i = 0; i < net.size(); i++){
        // net[i][j]: i -> which net, j -> which pair
        for (int j = 0; j < net[i].size(); j++){
            Block cur_blk_1, cur_blk_2;
            cur_blk_1 = net[i][j].first;
            cur_blk_2 = net[i][j].second; 

            int cur_blk_x, cur_blk_y;     
            // start blk
            if (cur_blk_1 == Tx){
                cur_blk_x = net[i][j].first->block_port_region_x + tx_coord_x;
                cur_blk_y = net[i][j].first->block_port_region_y + tx_coord_y;
            }
            // end blk
            else if (cur_blk_1 == Rx){
                cur_blk_x = net[i][j].first->block_port_region_x + rx_coord_x;
                cur_blk_y = net[i][j].first->block_port_region_y + rx_coord_y;
            }
            // middle blk
            else{
                // port random?
            }

            
        }
    }
    return 0;
}

void astar_search(int origin_grid_x, int origin_grid_y, int stop_grid_x, int stop_grid_y){
    vector<Grid*> Grid_list;
    // initial cur_grid
    Grid* cur_grid = new Grid(0, 0, origin_grid_x, origin_grid_y);
    Grid* nxt_grid = nullptr;
    map<pair<int, int>, bool> coord_2_obstacle;

    /*
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 7; j++) {
            coord_2_obstacle[make_pair(i, j)] = false;
        }
    }
    coord_2_obstacle[make_pair(4, 1)] = true;
    coord_2_obstacle[make_pair(4, 2)] = true;
    coord_2_obstacle[make_pair(5, 2)] = true;
    coord_2_obstacle[make_pair(2, 3)] = true;
    coord_2_obstacle[make_pair(3, 3)] = true;
    coord_2_obstacle[make_pair(4, 3)] = true;
    coord_2_obstacle[make_pair(2, 4)] = true;
    coord_2_obstacle[make_pair(3, 4)] = true;
    coord_2_obstacle[make_pair(4, 4)] = true;
    */
    // test (block A -> block B)
    // int A_x = A->getx() + tx;
    // int A_y = A->gety() + ty;
    // int B_x = B->getx() + rx;
    // int B_Y = B->gety() + ry;



    int right_cost, left_cost, up_cost, down_cost;
    Grid* right_grid;
    Grid* left_grid;
    Grid* up_grid;
    Grid* down_grid;

    // initial
    // TODO: obstacle?
    if (coord_2_obstacle[make_pair(origin_grid_x + 1, origin_grid_y)] == false){
        right_cost = 1 + cal_h(origin_grid_x + 1, origin_grid_y, stop_grid_x, stop_grid_y);
        right_grid = new Grid(1, right_cost, origin_grid_x + 1, origin_grid_y);
        right_grid->set_prev(cur_grid);
        Grid_list.push_back(right_grid);
    }

    if (coord_2_obstacle[make_pair(origin_grid_x - 1, origin_grid_y)] == false){
        left_cost = 1 + cal_h(origin_grid_x - 1, origin_grid_y, stop_grid_x, stop_grid_y);
        left_grid = new Grid(1, left_cost, origin_grid_x - 1, origin_grid_y);
        left_grid->set_prev(cur_grid);
        Grid_list.push_back(left_grid);
    }

    if (coord_2_obstacle[make_pair(origin_grid_x, origin_grid_y + 1)] == false){
        up_cost = 1 + cal_h(origin_grid_x, origin_grid_y + 1, stop_grid_x, stop_grid_y);
        up_grid = new Grid(1, up_cost, origin_grid_x, origin_grid_y + 1);
        up_grid->set_prev(cur_grid);
        Grid_list.push_back(up_grid);
    }

    if (coord_2_obstacle[make_pair(origin_grid_x, origin_grid_y - 1)] == false){
        down_cost = 1 + cal_h(origin_grid_x, origin_grid_y - 1, stop_grid_x, stop_grid_y);
        down_grid = new Grid(1, down_cost, origin_grid_x, origin_grid_y - 1);
        down_grid->set_prev(cur_grid);
        Grid_list.push_back(down_grid);
    }

    // Grid_list: all the grid that can possibly be chose, pop out if chosen
    int min_cost = 10000000;
    int min_h = 10000000;
    for (int i = 0; i < Grid_list.size(); i++){
        if ((Grid_list[i]->get_cost() <= min_cost) && (Grid_list[i]->get_cost() - Grid_list[i]->get_G() < min_h)){
            min_cost = Grid_list[i]->get_cost();
            min_h = Grid_list[i]->get_cost() - Grid_list[i]->get_G();
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
        if ( (!((cur_x + 1 == prev_x) && (cur_y == prev_y))) && (coord_2_obstacle[make_pair(cur_x + 1, cur_y)] == false) ){
            right_cost = cur_grid->get_G() + 1 + cal_h(cur_x + 1, cur_y, stop_grid_x, stop_grid_y);
            cout << "right: " << right_cost << endl;
            right_grid = new Grid(cur_grid->get_G() + 1, right_cost, cur_x + 1, cur_y);
            right_grid->set_prev(cur_grid);
            Grid_list.push_back(right_grid);
        }

        if ( (!((cur_x - 1 == prev_x) && (cur_y == prev_y))) && (coord_2_obstacle[make_pair(cur_x - 1, cur_y)] == false) ){
            left_cost = cur_grid->get_G() + 1 + cal_h(cur_x - 1, cur_y, stop_grid_x, stop_grid_y);
            cout << "left: " << left_cost << endl;
            left_grid = new Grid(cur_grid->get_G() + 1, left_cost, cur_x - 1, cur_y);
            left_grid->set_prev(cur_grid);
            Grid_list.push_back(left_grid);
        }
        
        if ( (!((cur_x == prev_x) && (cur_y + 1 == prev_y))) && (coord_2_obstacle[make_pair(cur_x, cur_y + 1)] == false) ){
            up_cost = cur_grid->get_G() + 1 + cal_h(cur_x, cur_y + 1, stop_grid_x, stop_grid_y);
            cout << "up: " << up_cost << endl;
            up_grid = new Grid(cur_grid->get_G() + 1, up_cost, cur_x, cur_y + 1);
            up_grid->set_prev(cur_grid);
            Grid_list.push_back(up_grid);
        }

        if ( (!((cur_x == prev_x) && (cur_y - 1 == prev_y))) && (coord_2_obstacle[make_pair(cur_x, cur_y - 1)] == false) ){
            down_cost = cur_grid->get_G() + 1 + cal_h(cur_x, cur_y - 1, stop_grid_x, stop_grid_y);
            cout << "down: " << down_cost << endl;
            down_grid = new Grid(cur_grid->get_G() + 1, down_cost, cur_x, cur_y - 1);
            down_grid->set_prev(cur_grid);
            Grid_list.push_back(down_grid);
        }

        min_cost = 10000000;
        min_h = 10000000;
        for (int i = 0; i < Grid_list.size(); i++){
            if ((Grid_list[i]->get_cost() <= min_cost) && (Grid_list[i]->get_cost() - Grid_list[i]->get_G() < min_h)){
                min_cost = Grid_list[i]->get_cost();
                min_h = Grid_list[i]->get_cost() - Grid_list[i]->get_G();
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

    return;
}

int cal_h(int x, int y, int stop_x, int stop_y){
    int x_out = abs(stop_x - x);
    int y_out = abs(stop_y - y);
    return x_out + y_out;
    /*
    if (x_out >= y_out) return x_out;
    else return y_out;
    */
}