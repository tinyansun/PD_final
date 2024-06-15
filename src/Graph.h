#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>

using namespace std;

using Blk = DefParser::Block;
using Net = ConnectionJsonParser::NetInfo;
using E_blk = pair<Blk*, Blk*>;

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
            _dir = esti_dir();

            _dense = 0;
            _max_dense = 20; // to modify
        }
        void print();
        void set_len(double len) { _len = len; }
        void set_len() { _len = get_dist(); }
        void set_dense(double d) { _dense = d; }
        void add_dense(double d) { _dense += d; }
        double get_dist();
        bool esti_dir();
        Node* get_s() { return _start_n; }
        Node* get_e() { return _end_n; }
        float get_w() { 
            if (_is_MST) return _len;
            else return _len; // need consider dense
        }
        E_blk get_egde_blk();
    private:
        bool _is_MST = true;
        Node* _start_n;
        Node* _end_n;
        float _len;
        bool _dir; // 0 for _ , 1 for |
        float _dense;
        float _max_dense;
        // bool _is_add_dij;
        void reset() {
            _dense = 0;
            // _is_add_dij = false;
        }
};

class Node {
    friend class Graph;
    public:
        Node(Blk* blk) {
            // _prev_node = 0;
            _blk = blk;
            _x = blk->position.first;
            _y = blk->position.second;

            _ptDS = this;
            _rank = 1;

            _prev_e = nullptr;
            _val_dij = -1; // -1 = inf
            _is_add_dij = false;
            _heap_id = -1;
        }

        Node(float x, float y) {
            _x = x;
            _y = y;

            _ptDS = this;
            _rank = 1;

            _prev_e = nullptr;
            _val_dij = -1; // -1 = inf
            _is_add_dij = false;
            _heap_id = -1;
        }
        void print();
        Node* getNeighbor(Edge* e);
        pair<float, float> get_coord() {
            return {_x, _y};
        }
        bool operator > (const Node v) const {
            if (_val_dij == v._val_dij) return false;
            else if (_val_dij == -1) return true;
            else if (v._val_dij == -1) return false;
            else return _val_dij > v._val_dij;
        }
        Blk* get_blk() { return _blk; }
    private:
        float _x;
        float _y;
        vector<Edge*> _e_list;
        unordered_set<Node*> _neighbor;
        bool push_e(Edge* e) {
            Node* n = getNeighbor(e);
            if (n != nullptr && n != this
                && _neighbor.count(n) == 0
                && this->get_blk()->isFeedthroughable
                && n->get_blk()->isFeedthroughable) {
                _e_list.push_back(e);
                _neighbor.insert(n);
                return true;
            } else {
                // if (!n->get_blk()->isFeedthroughable) {
                //     cout << "in" << endl;
                //     cout << n->get_blk()->name << endl;
                // }
                return false;
            }
        }
        void reset() {
            _ptDS = this;
            _rank = 1;
            _prev_e = nullptr;
            _val_dij = -1;
            _is_add_dij = false;
        }

        // for disjoint set
        // modified in Graph
        Node* _ptDS;
        int _rank;

        // for Dijkstra
        Edge* _prev_e;
        float _val_dij;
        bool _is_add_dij;
        int _heap_id;
        void Dijk_addv() {
            _val_dij = 0;
            _is_add_dij = true;
        }
        Blk* _blk;

};

class Graph {
    public:
        Graph() {}
        ~Graph() {
            for (auto& n: V) delete n;
            for (auto& e: E) delete e;
        }
        void push_v(Blk* blk) {
            if (b2v.find(blk) == b2v.end()) {
                // cout << "in" << endl;
                // cout << blk << endl;
                Node* v = new Node(blk);
                // cout << v->get_blk() << endl;
                b2v[blk] = v;
                push_v(v);
            }
        }
        void push_v(Node* v) {
            // cout << "in" << endl;
            V.push_back(v);
            // cout << V.size() << endl;
        }
        bool push_e(Edge* e) {
            if (e->get_s()->push_e(e) && e->get_e()->push_e(e)) {
                E.push_back(e);
                return true;
            } else return false;
        }
        void make_complete_g();
        void make_complete_g(const vector<Node*>&);
        void make_bi_g(const vector<Node*>& ,const vector<Node*>&);
        void make_supernode_g(const vector<Node*>&);
        void make_global_g(Grid** ,pair<int, int>);
        void reset() {
            reset_V();
            reset_E();
        }
        void set_Dijk() { 
            reset_V();
            v_remain.clear();
        }
        void reset_V() {
            for (auto& v: V) v->reset();
        }
        void reset_E() {
            for (auto& e: E) e->reset();
        }

        vector<E_blk> MST();
        vector<E_blk> Dijk(vector<Blk*>);

        void test();
    private:
        Node* DS_Find (Node*);
        bool DS_Union (Node*, Node*);

        // for Dijk
        vector<Node*> v_remain;
        Node* HP_ExtractMin();
        void HP_DecreaseKey(Node*, float);
        void HP_Insert(Node*);
        void HP_Heapify(Node*);
        void HP_swap(Node*, Node*);
        Node* HP_par(Node* v) { 
            if (v->_heap_id <= 0) return 0;
            else return v_remain[(v->_heap_id - 1)/2];
        }
        Node* HP_left(Node* v) { 
            if ((2 * v->_heap_id + 1) >= v_remain.size()) return 0;
            return v_remain[(2 * v->_heap_id + 1)];
        }
        Node* HP_right(Node* v) {
            if ((2 * v->_heap_id + 2) >= v_remain.size()) return 0;
            return v_remain[(2 * v->_heap_id + 2)];
        }
        void Dijk_Relax(Node*);

        vector<Node*> V;
        unordered_map<Blk*, Node*> b2v;
        vector<Node*> block2node(vector<Blk*> blks) {
            vector<Node*> ns;
            for (auto& blk: blks) {
                ns.push_back(b2v[blk]);
            }
            return ns;
        }
        vector<Edge*> E;
};