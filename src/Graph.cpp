#include <Router.h>
#include <Graph.h>
#include <assert.h>

// some useful function

void Graph::test() {
}

vector<E_blk> Graph::MST() {
    vector<Edge*> E_buf(E);
    vector<Edge*> mst;
    // cout << "in" << endl;
    // cout << V.size() << endl;
    // for (auto e: E_buf) {
    //     e->print();
    // }
    // for (auto v: b2v) {
    //     cout << v.second << endl;
    // }
    sort(E_buf.begin(), E_buf.end(), [](Edge* & a, Edge* & b) { 
        return a->get_w() < b->get_w(); 
    });
    for (size_t i = 0; i < E_buf.size() && mst.size() < V.size() - 1; i++){
        Edge* edge = E_buf[i];
        if (DS_Union(edge->_start_n, edge->_end_n)) {
            mst.push_back(edge);
        }
    }
    reset();
    // assert(mst.size() == V.size() - 1);
    vector<E_blk> ans;
    for (auto e: mst) {
        ans.push_back(e->get_egde_blk());
    }
    return ans;
}

Node* Graph::DS_Find (Node* v) {
    if (v->_ptDS != v) v->_ptDS = DS_Find(v->_ptDS);
    return v->_ptDS;
}
bool Graph::DS_Union (Node* u, Node* v) {
    Node* u_lead = DS_Find(u);
    Node* v_lead = DS_Find(v);
    if (u_lead != v_lead) {
        if (u_lead->_rank < v_lead->_rank) u_lead->_ptDS = v_lead;
        else if (u_lead->_rank > v_lead->_rank) v_lead->_ptDS = u_lead;
        else {
            u_lead->_ptDS = v_lead;
            v_lead->_rank++;
        }
        return true;
    } else {
        return false;
    }
}

void Graph::make_complete_g() {
    make_complete_g(V);
}

void Graph::make_complete_g(const vector<Node*>& vs) {
    // cout << "in" << endl;
    // cout << vs.size() << endl;
    for (size_t i = 0; i < vs.size(); i++) {
        for (size_t j = i + 1; j < vs.size(); j++) {
            Edge* e = new Edge(vs[i], vs[j]);
            // cout << e << endl;
            if (!push_e(e)) delete e;
        }
    }
    // cout << E.size() << endl;
}

void Graph::make_bi_g(const vector<Node*>& vs1, const vector<Node*>& vs2) {
    for (size_t i = 0; i < vs1.size(); i++) {
        for (size_t j = 0; j < vs2.size(); j++) {
            Edge* e = new Edge(vs1[i], vs2[j]);
            if (!push_e(e)) delete e;
        }
    }
}

void Graph::make_supernode_g(const vector<Node*>& vs) {
    for (size_t i = 0; i < vs.size(); i++) {
        for (size_t j = i + 1; j < vs.size(); j++) {
            Edge* e = new Edge(vs[i], vs[j]);
            e->set_len(0);
            if (!push_e(e)) delete e;
        }
    }
}

void Graph::make_global_g(Grid** placement, pair<int, int> bounding) {
    // bounding = max x, max y of grid id
    Grid* upper_g = nullptr;
    vector<Grid*> left_g(bounding.second, nullptr);
    for (int i = 0; i < bounding.first; i++) {
        for (int j = 0; j < bounding.second; j++) {
            Grid& g = placement[i][j];
            assert(g.get_blocks().size() != 0);
            make_supernode_g(block2node(g.get_blocks()));
            if (upper_g != nullptr) make_bi_g(block2node(g.get_blocks()), block2node(upper_g->get_blocks()));
            if (left_g[j] != nullptr) make_bi_g(block2node(g.get_blocks()), block2node(left_g[j]->get_blocks()));
            upper_g = &g;
            left_g[j] = &g;
        }
    }
}

vector<E_blk> Graph::Dijk(vector<Blk*> b_net) {
    unordered_set<Node*> v_net;
    for (auto v: block2node(b_net)) {
        if (v->get_blk()->isFeedthroughable == false) continue;
        v_net.insert(v);
    }
    set_Dijk();
    vector<Edge*> wiring;

    Node* v0 = *(v_net.begin());
    v0->_val_dij = 0;
    v0->_prev_e = 0;

    for (auto& v: V) {
        HP_Insert(v);
    }
    while (!v_net.empty() && !v_remain.empty()) {
        Node* cur_v = HP_ExtractMin();
        auto cur_v_iter = v_net.find(cur_v);
        if (cur_v_iter != v_net.end()) {
            // we found a new RX
            v_net.erase(cur_v_iter);
            while (!cur_v->_is_add_dij) {
                cur_v->Dijk_addv();
                Dijk_Relax(cur_v);
                if (cur_v->_prev_e != nullptr) { // v0 has no predecessor
                    //assert(cur_v->_prev_e != nullptr);
                    wiring.push_back(cur_v->_prev_e);
                    cur_v = cur_v->getNeighbor(cur_v->_prev_e);
                }
            }
        } else {
            // update neighbor
            Dijk_Relax(cur_v);
        }
    }
    assert(v_net.empty());
    vector<E_blk> output;
    for (auto& e: wiring) {
        output.push_back(e->get_egde_blk());
    }
    return output;
}
Node* Graph::HP_ExtractMin() {
    if (v_remain.empty()) return 0;
    if (v_remain.size() == 1) {
        Node* min_v = v_remain[0];
        min_v->_heap_id = -1;
        v_remain.clear();
        return min_v;
    }
    
    HP_swap(v_remain.front(), v_remain.back());
    Node* min_v = v_remain.back();
    min_v->_heap_id = -1;
    
    v_remain.pop_back();
    HP_Heapify(v_remain[0]);
    return min_v;
}
void Graph::HP_DecreaseKey(Node* v, float val) {
    if (v->_is_add_dij) return;
    assert(v->_val_dij >= val || v->_val_dij == -1);

    size_t id = v->_heap_id;
    v->_val_dij = val;
    if (v->_heap_id == -1) HP_Insert(v);
    else {
        while (HP_par(v) != 0 && *HP_par(v) > *v) { 
            HP_swap(v, HP_par(v));
        }
    }
}
void Graph::HP_Insert(Node* v) {
    v_remain.push_back(v);
    v->_heap_id = v_remain.size() - 1;
    while (HP_par(v) != 0 && *HP_par(v) > *v) { 
       HP_swap(v, HP_par(v));
    }
}
void Graph::HP_Heapify(Node* v) {
    Node* l = HP_left(v); 
    Node* r = HP_right(v); 
    Node* smallest = v; 
    if (l != 0 && *v > *l) smallest = l; 
    if (r != 0 && *smallest > *r) smallest = r; 
    if (smallest != v) { 
        HP_swap(v, smallest); 
        HP_Heapify(smallest); 
    } 
}
void Graph::HP_swap(Node* u, Node* v) {
    // swap(u, v);
    int temp = u->_heap_id;
    u->_heap_id = v->_heap_id;
    v->_heap_id = temp;
    v_remain[u->_heap_id] = u;
    v_remain[v->_heap_id] = v;
}
void Graph::Dijk_Relax(Node* v) {
    for (Edge* e: v->_e_list) {
        Node* u = v->getNeighbor(e);
        float new_dist = v->_val_dij + e->get_w();
        if (u->_val_dij == -1 || u->_val_dij > new_dist) {
            u->_prev_e = e;
            HP_DecreaseKey(u, new_dist);
        }
    }
}
// edge
void Edge::print() {
    cout << "(" << _start_n->get_coord().first << "," << _start_n->get_coord().second << ") ~ "
        << "(" << _end_n->get_coord().first << "," << _end_n->get_coord().second << ") --> "
        << get_w() << endl;
}
double Edge::get_dist() {
    if (_is_MST) {
        return dist_Mah(_start_n->get_coord(), _end_n->get_coord());
    } else {
        int min_x_s = _start_n->get_coord().first;
        int min_y_s = _start_n->get_coord().second;
        int max_x_s = min_x_s;
        int max_y_s = min_y_s;
        int min_x_e = _end_n->get_coord().first;
        int min_y_e = _end_n->get_coord().second;
        int max_x_e = min_x_s;
        int max_y_e = min_y_s;
        
        for (auto& sec: _start_n->get_blk()->actual_shape) {
            if (max_x_s < sec.first) max_x_s = sec.first;
            if (max_y_s < sec.second) max_y_s = sec.second;
        }
        for (auto& sec: _end_n->get_blk()->actual_shape) {
            if (max_x_e < sec.first) max_x_e = sec.first;
            if (max_y_e < sec.second) max_y_e = sec.second;
        }

        int max_x_span = max(max_x_s, max_x_e) - min(min_x_s, min_x_e);
        int max_y_span = max(max_y_s, max_y_e) - min(min_y_s, min_y_e);
        int min_x_span, min_y_span;
        if (min_x_s > max_x_e) min_x_span = min_x_s - max_x_e;
        else if(min_x_e > max_x_s) min_x_span = min_x_e - max_x_s;
        else min_x_span = 0;
        if (min_y_s > max_y_e) min_y_span = min_y_s - max_y_e;
        else if(min_y_e > max_y_s) min_y_span = min_y_e - max_y_s;
        else min_y_span = 0;


        return (max_x_span + max_y_span, min_x_span + min_y_span) / 2;
    }
}
bool Edge::esti_dir() {
    return (abs(_start_n->get_coord().first - _end_n->get_coord().first) < abs(_start_n->get_coord().second - _end_n->get_coord().second));
}
E_blk Edge::get_egde_blk() {
    return {_start_n->get_blk(), _end_n->get_blk()};
}
// Node
// int Node::_global_flag = 0;
void Node::print() {
}
Node* Node::getNeighbor(Edge* e) {
    assert(e != nullptr);
    if (e->get_s() == this) return e->get_e();
    else if (e->get_e() == this) return e->get_s();
    else return 0;
}

// bool Node::reachable(Node* v, bool set) {
//     if (v == this) return true;
//     if (set) setGlobalFlag();
//     _flag = _global_flag;
//     vector<Node*> neighbor;
//     for (auto& e: e_list) {
//         if (e->_removed) continue;
//         Node* n = getNeighbor(e);
//         if (n->_flag != _global_flag) neighbor.push_back(n);
//     }
//     for (auto& n: neighbor) {
//         if (n->reachable(v, false)) return true;
//     }
//     return false;
// }