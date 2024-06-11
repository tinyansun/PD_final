#include <cassert>

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
        bool get_throughable() { return _throughable;}
        // set
        void set_G(int G) {_G = G;}
        void set_cost(int cost) {_cost = cost;}
        void set_x(int x) {_x = x;}
        void set_y(int y) {_y = y;}
        void set_prev(Grid* prev) {_prevgrid = prev;}
        void set_throughable(bool throughable) {_throughable = throughable;}
        // destructor
        ~Grid(){ }
    private:
        int _G;
        int _cost;
        int _x;
        int _y;
        Grid* _prevgrid;
        bool _throughable;
};

class Router {
public:
    vector<vector<Grid>> Grid_list;
    
private:
    int tracks_per_um;
    int maxTrack;
    pair<int, int> boundingbox;
    unordered_map<string, DefParser::Block> blocks;
    unordered_map<string, CfgJsonParser::BlockInfo> blockInfos;
    vector<ConnectionJsonParser::NetInfo> nets;
    double grid_width;

public:
    Router(int tpu, 
           int mT,
           const pair<int, int>& box,
           const unordered_map<string, DefParser::Block>& blks,
           const unordered_map<string, CfgJsonParser::BlockInfo>& blkInfos, 
           const vector<ConnectionJsonParser::NetInfo>& nts)
        : tracks_per_um(tpu), maxTrack(mT), boundingbox(box), blocks(blks), blockInfos(blkInfos), nets(nts) {
            grid_width = ceil((double)maxTrack / (double)tracks_per_um);

            for (auto it = blocks.begin(); it != blocks.end(); ++it) {
                string blockName = "BLOCK_" + it->second.type.substr(it->second.type.find('_') + 1);
                it->second.shape = blocks[blockName].shape;

                it->second.throughBlockNetNum = blockInfos[it->second.name].throughBlockNetNum;
                it->second.isFeedthroughable = blockInfos[it->second.name].isFeedthroughable;
            }
            cerr<<(boundingbox).first<<" "<<(boundingbox).second<<endl;
            cerr<<grid_coordinate(boundingbox).first<<" "<<grid_coordinate(boundingbox).second<<endl;
            //for test
            cerr<<"maxTrack: "<<maxTrack<<" tpu: "<<tracks_per_um<<" grid_width "<<grid_width<<endl;
        }

    // Function to get tracks per um
    const int& getTracksPerUm() const {
        return tracks_per_um;
    }

    // Function to get Boundingbox
    const pair<int, int>& getBoundingbox() const {
        return boundingbox;
    }

    // Function to access blocks
    const unordered_map<string, DefParser::Block>& getBlocks() const {
        return blocks;
    }

    // Function to access nets
    const vector<ConnectionJsonParser::NetInfo>& getNets() const {
        return nets;
    }

    // Function to calculate grid_index
    const pair<int, int> grid_index(pair<double, double> coordinate) const {
        int shift_x = floor(coordinate.first/grid_width);
        int shift_y = floor(coordinate.second/grid_width);
        return make_pair(shift_x, shift_y);
    }

    // Function to calculate grid_coordinate
    const pair<double, double> grid_coordinate(pair<double, double> coordinate) const {
        double shift_x = (floor(coordinate.first/grid_width)+0.5)*grid_width;
        double shift_y = floor(coordinate.second/grid_width)+0.5*grid_width;
        return make_pair(shift_x, shift_y);
    }

    //data member
};


