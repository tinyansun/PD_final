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

using namespace std;

int cal_h(int x, int y, int stop_x, int stop_y);
vector<pair<int, int>> astar_search(Router router, int origin_grid_x, int origin_grid_y, int stop_grid_x, int stop_grid_y);
vector<pair<int, int>> Z_shape(Router& router, int origin_grid_x, int origin_grid_y, int stop_grid_x, int stop_grid_y);
bool check_overlap(int x01, int x02, int x11, int x12) {
    return (x01 < x11 && x11 < x02) || (x01 < x12 && x12 < x02);
}
int comp_sec_cost(Grid** global_g, int origin_grid_x, int origin_grid_y, int stop_grid_x, int stop_grid_y) {
    int cost = 0;
    if (origin_grid_x == stop_grid_x) {
        int fix = origin_grid_x;
        int small = min(origin_grid_y, stop_grid_y);
        int big = max(origin_grid_y, stop_grid_y);
        for (int k = small; k <= big; k++){
            if (global_g[fix][k].get_throughable() == 0) return -1;
            else cost += global_g[fix][k].get_cost();
        }
    } else if (origin_grid_y == stop_grid_y){
        int fix = origin_grid_y;
        int small = min(origin_grid_x, stop_grid_x);
        int big = max(origin_grid_x, stop_grid_x);
        for (int k = small; k <= big; k++){
            if (global_g[k][fix].get_throughable() == 0) return -1;
            else cost += global_g[k][fix].get_cost();
        }
    }
    return cost;
}
int comp_cost(Grid** global_g, vector<pair<int, int>>& pt) {
    int cost = 0;
    for (int i = 1; i < pt.size(); i++) {
        int c_sec = comp_sec_cost(global_g, pt[i - 1].first, pt[i - 1].second, pt[i].first, pt[i].second);
        if (c_sec < 0) return -1;
        else cost += c_sec;
    } 
    return cost;
}

vector<pair<int, int>> astar_search(Router router, int origin_grid_x, int origin_grid_y, int stop_grid_x, int stop_grid_y){
    // test
    cout << "start " << "x: " << origin_grid_x << " y: " << origin_grid_y << endl;
    cout << "end " << "x: " << stop_grid_x << " y: " << stop_grid_y << endl;
    vector<Grid*> Grid_list;
    // initial cur_grid
    Grid* cur_grid = new Grid(0, 0, origin_grid_x, origin_grid_y, 0);
    cur_grid->set_explored(1);

    Grid* nxt_grid = nullptr;

    // record path for output
    vector<pair<int, int>> path_grid_list;

    /*
    map<pair<int, int>, bool> coord_2_obstacle;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 7; j++) {
            coord_2_obstacle[make_pair(i, j)] = true;
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
    if (router.grid_graph[origin_grid_x + 1][origin_grid_y].get_throughable() == true) cout << "a" << endl;
    if (((origin_grid_x + 1) < router.grid_index(router.getBoundingbox()).first+1) && (router.grid_graph[origin_grid_x + 1][origin_grid_y].get_throughable() == true)){
        right_cost = 1 + cal_h(origin_grid_x + 1, origin_grid_y, stop_grid_x, stop_grid_y) + router.grid_graph[origin_grid_x + 1][origin_grid_y].get_wirenum();
        right_grid = new Grid(1, right_cost, origin_grid_x + 1, origin_grid_y, 0);
        right_grid->set_prev(cur_grid);
        // set_explored
        router.grid_graph[origin_grid_x + 1][origin_grid_y].set_explored(1);
        Grid_list.push_back(right_grid);
        // cout << "a" << endl;
    }

    if (router.grid_graph[origin_grid_x - 1][origin_grid_y].get_throughable() == true) cout << "b" << endl;
    if (((origin_grid_x - 1) > -1) && (router.grid_graph[origin_grid_x - 1][origin_grid_y].get_throughable() == true)){
        left_cost = 1 + cal_h(origin_grid_x - 1, origin_grid_y, stop_grid_x, stop_grid_y) + router.grid_graph[origin_grid_x - 1][origin_grid_y].get_wirenum();
        left_grid = new Grid(1, left_cost, origin_grid_x - 1, origin_grid_y, 0);
        left_grid->set_prev(cur_grid);
        // set_explored
        router.grid_graph[origin_grid_x - 1][origin_grid_y].set_explored(1);
        Grid_list.push_back(left_grid);
        // cout << "b" << endl;
    }

    if (router.grid_graph[origin_grid_x][origin_grid_y + 1].get_throughable() == true) cout << "c" << endl;
    if (((origin_grid_y + 1) < router.grid_index(router.getBoundingbox()).second+1) && (router.grid_graph[origin_grid_x][origin_grid_y + 1].get_throughable() == true)){
        up_cost = 1 + cal_h(origin_grid_x, origin_grid_y + 1, stop_grid_x, stop_grid_y) + router.grid_graph[origin_grid_x][origin_grid_y + 1].get_wirenum();
        up_grid = new Grid(1, up_cost, origin_grid_x, origin_grid_y + 1, 0);
        up_grid->set_prev(cur_grid);
        // set_explored
        router.grid_graph[origin_grid_x][origin_grid_y + 1].set_explored(1);
        Grid_list.push_back(up_grid);
        // cout << "c" << endl;
    }

    if (router.grid_graph[origin_grid_x][origin_grid_y - 1].get_throughable() == true) cout << "d" << endl;
    if (((origin_grid_y - 1) > -1) && (router.grid_graph[origin_grid_x][origin_grid_y - 1].get_throughable() == true)){
        down_cost = 1 + cal_h(origin_grid_x, origin_grid_y - 1, stop_grid_x, stop_grid_y) + router.grid_graph[origin_grid_x][origin_grid_y - 1].get_wirenum();
        down_grid = new Grid(1, down_cost, origin_grid_x, origin_grid_y - 1, 0);
        down_grid->set_prev(cur_grid);
        // set_explored
        router.grid_graph[origin_grid_x][origin_grid_y - 1].set_explored(1);
        Grid_list.push_back(down_grid);
        // cout << "d" << endl;
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

    // test segmentation
    if (nxt_grid == nullptr){
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ there's no way out! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
        return path_grid_list;
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
    int count = 0;
    while(!((cur_grid->get_x() == stop_grid_x) && (cur_grid->get_y() == stop_grid_y))){
    // for (int i = 0; i < 5; i++){
        cout << "-------------iteration "<<count++<<"----------------" << endl;
        if (cur_grid->get_prev() == nullptr) {
            cout << "No previous grid, exiting loop." << endl;
            break;
        }
        int prev_x = cur_grid->get_prev()->get_x();
        int prev_y = cur_grid->get_prev()->get_y();
        
        int cur_x = cur_grid->get_x();
        int cur_y = cur_grid->get_y();

        // avoid backtrack (dont cal when obstacle or it is prev block)
        if ( (!((cur_x + 1 == prev_x) && (cur_y == prev_y))) && ((cur_x + 1) < router.grid_index(router.getBoundingbox()).first+1) && (router.grid_graph[cur_x + 1][cur_y].get_throughable() == true) && (router.grid_graph[cur_x + 1][cur_y].get_explored() == false)){
            right_cost = cur_grid->get_G() + 1 + cal_h(cur_x + 1, cur_y, stop_grid_x, stop_grid_y) + router.grid_graph[cur_x + 1][cur_y].get_wirenum();
            //cout << "right: " << right_cost << endl;
            right_grid = new Grid(cur_grid->get_G() + 1, right_cost, cur_x + 1, cur_y, 0);
            right_grid->set_prev(cur_grid);
            // set_explored
            router.grid_graph[cur_x + 1][cur_y].set_explored(1);
            Grid_list.push_back(right_grid);
        }

        if ( (!((cur_x - 1 == prev_x) && (cur_y == prev_y))) && ((cur_x - 1) > -1) && (router.grid_graph[cur_x - 1][cur_y].get_throughable() == true)  && (router.grid_graph[cur_x - 1][cur_y].get_explored() == false) ){
            left_cost = cur_grid->get_G() + 1 + cal_h(cur_x - 1, cur_y, stop_grid_x, stop_grid_y) + router.grid_graph[cur_x - 1][cur_y].get_wirenum();
            //cout << "left: " << left_cost << endl;
            left_grid = new Grid(cur_grid->get_G() + 1, left_cost, cur_x - 1, cur_y, 0);
            left_grid->set_prev(cur_grid);
            // set_explored
            router.grid_graph[cur_x - 1][cur_y].set_explored(1);
            Grid_list.push_back(left_grid);
        }
        
        if ( (!((cur_x == prev_x) && (cur_y + 1 == prev_y))) && ((cur_y + 1) < router.grid_index(router.getBoundingbox()).second+1) && (router.grid_graph[cur_x][cur_y + 1].get_throughable() == true) && (router.grid_graph[cur_x][cur_y + 1].get_explored() == false)){
            up_cost = cur_grid->get_G() + 1 + cal_h(cur_x, cur_y + 1, stop_grid_x, stop_grid_y) + router.grid_graph[cur_x][cur_y + 1].get_wirenum();
            //cout << "up: " << up_cost << endl;
            up_grid = new Grid(cur_grid->get_G() + 1, up_cost, cur_x, cur_y + 1, 0);
            up_grid->set_prev(cur_grid);
            // set_explored
            router.grid_graph[cur_x][cur_y + 1].set_explored(1);
            Grid_list.push_back(up_grid);
        }

        if ( (!((cur_x == prev_x) && (cur_y - 1 == prev_y))) && ((cur_y - 1) > -1) && (router.grid_graph[cur_x][cur_y - 1].get_throughable() == true) && (router.grid_graph[cur_x][cur_y - 1].get_explored() == false)){
            down_cost = cur_grid->get_G() + 1 + cal_h(cur_x, cur_y - 1, stop_grid_x, stop_grid_y) + router.grid_graph[cur_x][cur_y - 1].get_wirenum();
            //cout << "down: " << down_cost << endl;
            down_grid = new Grid(cur_grid->get_G() + 1, down_cost, cur_x, cur_y - 1, 0);
            down_grid->set_prev(cur_grid);
            // set_explored
            router.grid_graph[cur_x][cur_y - 1].set_explored(1);
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
       
        // cout << "mincost: " << nxt_grid->get_cost() << endl;
        cout << "x: " << nxt_grid->get_x() << endl;
        cout << "y: " << nxt_grid->get_y() << endl;

        // update cur_grid
        cur_grid = nxt_grid;

        // cout << "before" << endl;
        // for (int i = 0; i < Grid_list.size(); i++){
        //     cout << Grid_list[i]->get_cost() << " ";
        // }
        // cout << endl;

        // pop out
        vector<Grid*>::iterator it = find(Grid_list.begin(), Grid_list.end(), nxt_grid);
        if (it != Grid_list.end()) {
            Grid_list.erase(it);
        }

        // cout << "after" << endl;
        // for (int i = 0; i < Grid_list.size(); i++){
        //     cout << Grid_list[i]->get_cost() << " ";
        // }
        // cout << endl;
        
        // test
        if (Grid_list.size() == 0){
            cout << "fail routing" << endl;
            break;
        }
    }

    // cout << "final cost: " << cur_grid->get_cost() << endl;
    // cout << "x: " << cur_grid->get_x() << " y: " << cur_grid->get_y() << endl;
    
    while(1){
        if ((cur_grid->get_x() == origin_grid_x) && (cur_grid->get_y() == origin_grid_y)) break;
        // cout << "x: " << cur_grid->get_prev()->get_x() << " y: " << cur_grid->get_prev()->get_y() << endl;
        path_grid_list.push_back({cur_grid->get_x(), cur_grid->get_y()});
        // record wirenum
        cur_grid->inc_wirenum();
        cur_grid = cur_grid->get_prev();
    }
    
    // free Grid_list
    for (int i = 0; i < Grid_list.size(); i++){
        delete Grid_list[i];
    }
    Grid_list.clear();

    // reset grid_graph
    for (int i = 0; i < router.grid_index(router.getBoundingbox()).first+1; i++){
        for(int j = 0; j < router.grid_index(router.getBoundingbox()).second+1; j++){
            router.grid_graph[i][j].reset();
        }
    }

    return path_grid_list;
}

vector<pair<int, int>> Z_shape(Router& router, int origin_grid_x, int origin_grid_y, int stop_grid_x, int stop_grid_y) {
    Grid** global_g = router.grid_graph;
    int x_span = abs(origin_grid_x - stop_grid_x);
    int y_span = abs(origin_grid_y - stop_grid_y);
    int x_min = min(origin_grid_x, stop_grid_x);
    int y_min = min(origin_grid_y, stop_grid_y);

    vector<vector<pair<int, int>>> result;
    int cost;
    int min_cost = -1;
    int best_id = -1;
    const int trial = 2;

    result.push_back({{origin_grid_x, origin_grid_y}, {stop_grid_x, origin_grid_y}, {stop_grid_x, stop_grid_y}});
    cost = comp_cost(global_g, result.back());
    if (min_cost == -1 || (min_cost > cost && cost > 0) ) {
        min_cost = cost;
        best_id = result.size() - 1;
    }
    // |-
    result.push_back({{origin_grid_x, origin_grid_y}, {origin_grid_x, stop_grid_y}, {stop_grid_x, stop_grid_y}});
    cost = comp_cost(global_g, result.back());
    if (min_cost == -1 || (min_cost > cost && cost > 0)) {
        min_cost = cost;
        best_id = result.size() - 1;
    }
    // Z
    for (int i = 1; i < trial + 1; i++) {
        int trial_x = x_min + rand() % x_span;
        result.push_back({{origin_grid_x, origin_grid_y}, {trial_x, origin_grid_y}, {trial_x, stop_grid_y}, {stop_grid_x, stop_grid_y}});
        cost = comp_cost(global_g, result.back());
        if (min_cost == -1 || (min_cost > cost && cost > 0)) {
            min_cost = cost;
            best_id = result.size() - 1;
        }
        int trial_y = y_min + rand() % y_span;
        result.push_back({{origin_grid_x, origin_grid_y}, {origin_grid_x, trial_y}, {stop_grid_x, trial_y}, {stop_grid_x, stop_grid_y}});
        cost = comp_cost(global_g, result.back());
        if (min_cost == -1 || (min_cost > cost && cost > 0)) {
            min_cost = cost;
            best_id = result.size() - 1;
        }
    }

    if (best_id == -1) return vector<pair<int, int>> ();
    vector<pair<int, int>> best_path = result[best_id];
    vector<pair<int, int>> output;
    for (int i = 1; i < best_path.size(); i++) {
        int x1 = best_path[i - 1].first;
        int x2 = best_path[i - 1].second;
        int y1 = best_path[i].first;
        int y2 = best_path[i].second;
        if (x1 == x2) {
            int fix = x1;
            int cur = y1;
            int end = y2;
            while (cur != end) {
                output.push_back({fix, cur});
                global_g[fix][cur].inc_wirenum();
                cur = cur < end ? cur + 1 : cur - 1;
            }
        } else if (y1 == y2){
            int fix = y1;
            int cur = x1;
            int end = x2;
            while (cur != end) {
                output.push_back({fix, cur});
                global_g[fix][cur].inc_wirenum();
                cur = cur < end ? cur + 1 : cur - 1;
            }
        }
    }
    





    
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