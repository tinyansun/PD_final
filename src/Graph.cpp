#include "Graph.h"
#include <assert.h>

// some useful function

void Graph::test() {
    V = vector<Node*> ();
    E = vector<Edge*> ();
    push_v(new Node(0, 1));
    push_v(new Node(2, 1));
    push_v(new Node(3, 0));
    push_v(new Node(4, 3));
    push_v(new Node(0, 0));
    make_complete_g();
    for (auto e: MST()) {
        e->print();
    }
}

vector<Edge*> Graph::MST() {
    vector<Edge*> E_buf(E);
    vector<Edge*> mst;
    sort(E_buf.begin(), E_buf.end(), [](Edge* & a, Edge* & b) { 
        return a->get_w() < b->get_w(); 
    });
    for (size_t i = 0; i < E_buf.size() && mst.size() < V.size() - 1; i++){
        Edge* edge = E_buf[i];
        if (DS_Union(edge->_start_n, edge->_end_n)) {
            mst.push_back(edge);
        }
    }
    assert(mst.size() == V.size() - 1);
    return mst;
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

// edge
void Edge::print() {
    cout << "(" << _start_n->get_coord().first << "," << _start_n->get_coord().second << ") ~ "
        << "(" << _end_n->get_coord().first << "," << _end_n->get_coord().second << ") --> "
        << get_w() << endl;
}
double Edge::get_dist() {
    if (_is_MST) {
        return dist_Mah(_start_n->get_coord(), _end_n->get_coord());
    }
}
// Node
// int Node::_global_flag = 0;
void Node::print() {
    // cout << _id << ":" << endl;
    // cout << "new_found" << (_global_flag == _flag) << endl;
    // cout << "--color : " << _color;
    // cout << "  --f_t : " << _found_time;
    // cout << "  --d_t : " << _death_time;
    // cout << endl;
    // if (_prev_node != 0) {
    //     cout << "--prev : " << _prev_node->_id << endl;
    // } else {
    //     cout << "--prev : None" << endl; 
    // }
    // for (auto e: e_list) {
    //     cout << getNeighbor(e)->_id << " ";
    // }
    // cout << endl;
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