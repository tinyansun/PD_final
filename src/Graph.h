#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>

using namespace std;

class Graph;
class Node;
class Edge;

class Graph {
    public:
        Graph() {}
        void push_v(Node* v) {
            V.push_back(v);
        }
        void push_e(Edge* e) {
            E.push_back(e);
            e->get_s()->push_e(e);
            e->get_e()->push_e(e);
        }
        void complete_g() {
            for (size_t i = 0; i < V.size(); i++) {
                for (size_t j = i + 1; j < V.size(); j++) {
                    push_e(new Edge(V[i], V[j]));
                }
            }
        }
        vector<Edge*> MST();
        Node* DS_Find (Node* v);
        bool DS_Union (Node* u, Node* v);
    private:
        vector<Node*> V;
        vector<Edge*> E;

};

class Edge {
    friend class Graph;
    public:
        Edge(Node* s, Node* e) {
            _start_n = s;
            _end_n = e;
            set_w();
        }
        void print();
        void set_w() {
            if (is_MST) {
                _weight = dist_Mah(_start_n->get_coord(), _end_n->get_coord());
            }
        }
        Node* get_s() { return _start_n; }
        Node* get_e() { return _end_n; }
        float get_w() { return _weight; }
    private:
        bool is_MST = true;
        Node* _start_n;
        Node* _end_n;
        float _weight;
};

class Node {
    friend class Graph;
    public:
        Node(float x, float y) {
            // _prev_node = 0;
            // _flag = 0;
            _x = x;
            _y = y;
            _ptDS = this;
        }
        void print();
        void push_e(Edge* e) {
            if (getNeighbor(e) != nullptr) e_list.push_back(e);
        }
        Node* getNeighbor(Edge* e) {
            if (e->get_s() == this) return e->get_e();
            else if (e->get_e() == this) return e->get_s();
            else return 0;
        }
        pair<float, float> get_coord() {
            return {_x, _y};
        }

        Node* DS_Find();
        bool DS_Union(Node* v);
        // bool reachable(Node* v, bool set);
        // static void setGlobalFlag() { _global_flag++; }
        // void resetForDFS();
    private:
        float _x;
        float _y;
        vector<Edge*> e_list;

        // // for DFS
        // Node* _prev_node;
        // char _color = 'W'; // W,B,G
        // int _found_time = -1;
        // int _death_time = -1;
        // size_t _grp = 0;
        // int _flag;
        // static int _global_flag;

        // for disjoint set
        Node* _ptDS;
        int _rank = 1;

        // // for SCC
        // vector<Edge*> e_t_list;
        
};