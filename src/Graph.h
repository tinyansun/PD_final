#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <set>

using namespace std;

class Graph;
class Node;
class Edge;

template<typename T>
T dist_Mah(const pair<T, T>& a, const pair<T, T>& b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

class Edge {
    friend class Graph;
    public:
        Edge(Node* s, Node* e) {
            _start_n = s;
            _end_n = e;
            _len = get_dist();

            _dense = 0;
            _max_dense = 20; // to modify
            _is_add_dij = false;
        }
        void print();
        double get_dist();
        void set_len(double len) { _len = len; }
        void set_len() { _len = get_dist(); }
        void set_dense(double d) { _dense = d; }
        void add_dense(double d) { _dense += d; }
        void reset() {
            _dense = 0;
            _is_add_dij = false;
        }
        Node* get_s() { return _start_n; }
        Node* get_e() { return _end_n; }
        float get_w() { 
            if (_is_MST) return _len;
            else {
                if (!_is_add_dij) return _len; // need to consider density
                else return 0;
            }
        }
    private:
        bool _is_MST = true;
        Node* _start_n;
        Node* _end_n;
        float _len;
        float _dense;
        float _max_dense;
        bool _is_add_dij;
};

class Node {
    friend class Graph;
    public:
        Node(float x, float y) {
            // _prev_node = 0;
            _x = x;
            _y = y;
            _ptDS = this;
            _rank = 1;
            _prev_e = nullptr;
            _val_dij = -1; // -1 = inf
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
        void reset() {
            _ptDS = this;
            _rank = 1;
            _prev_e = nullptr;
            _val_dij = -1;
        }
        Node* DS_Find();
        bool DS_Union(Node* v);
        struct cmp_dij {
            bool operator()(const Node *lhs, const Node *rhs) const {
              if (rhs->_val_dij == -1) return true;
              else if (lhs->_val_dij == -1) return false;
              else return lhs->_val_dij < rhs->_val_dij;
            }
        };
    private:
        float _x;
        float _y;
        vector<Edge*> e_list;

        // for disjoint set
        // modified in Graph
        Node* _ptDS;
        int _rank;

        // for Dijkstra
        Edge* _prev_e;
        float _val_dij;

};

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
        void make_complete_g() {
            for (size_t i = 0; i < V.size(); i++) {
                for (size_t j = i + 1; j < V.size(); j++) {
                    push_e(new Edge(V[i], V[j]));
                }
            }
        }
        void reset_g() {
            reset_V();
            reset_E();
        }
        void reset_V() {
            for (auto& v: V) v->reset();
        }
        void reset_E() {
            for (auto& e: E) e->reset();
        }

        vector<Edge*> MST();
        Node* DS_Find (Node*);
        bool DS_Union (Node*, Node*);

        vector<Edge*> Dijk();
        Node* Dijk_ExtractMin(set<Node*, Node::cmp_dij>&);
        void Dijk_DecreaseKey(set<Node*, Node::cmp_dij>&, Node*);
        void Dijk_Insert(set<Node*, Node::cmp_dij>&, Node*);

        void test();
    private:
        vector<Node*> V;
        vector<Edge*> E;
};