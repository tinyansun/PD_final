#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cmath>
using namespace std;

class Polygon {
public:

    Polygon(vector<pair<int, int>> input) :
    vertices(input) { }

    void clear() {
        vertices.clear();
    }

    void addVertex(int x, int y) {
        vertices.push_back(make_pair(x, y));
    }

    void printVertices() const {
        for (const auto& vertex : vertices) {
            cout << "(" << vertex.first << ", " << vertex.second << ") ";
        }
        cout << endl;
    }

    bool isPointInside(int x, int y) const {
        int n = vertices.size();
        if (n < 3) return false;
        bool inside = false;


        for (int i = 0, j = n - 1; i < n; j = i++) {
            int xi = vertices[i].first, yi = vertices[i].second;
            int xj = vertices[j].first, yj = vertices[j].second;

            // 检查点是否在边上
            if (((yi > y) != (yj > y)) &&
                (x < (xj - xi) * (y - yi) / (yj - yi) + xi)) {
                inside = !inside;
            }

            // 如果点在多边形的边上，认为不在多边形内
            if ((y - yi) * (xj - xi) == (x - xi) * (yj - yi) &&
                min(xi, xj) <= x && x <= max(xi, xj) &&
                min(yi, yj) <= y && y <= max(yi, yj)) {
                return false;
            }
        }
        return inside;
    }

    void getBounds(int &minX, int &maxX, int &minY, int &maxY) const {
        minX = maxX = vertices[0].first;
        minY = maxY = vertices[0].second;

        for (const auto& vertex : vertices) {
            if (vertex.first < minX) minX = vertex.first;
            if (vertex.first > maxX) maxX = vertex.first;
            if (vertex.second < minY) minY = vertex.second;
            if (vertex.second > maxY) maxY = vertex.second;
        }
    }

private:
    vector<pair<int, int>> vertices;
};

class Grid{
    public:
        // constuctor
        Grid() { }
        Grid(int G, int cost, int x, int y, bool throughable) :
            _G(G), _cost(cost), _x(x), _y(y), _throughable(throughable), _prevgrid(nullptr) { }
        // get
        int get_G() {return _G;}
        int get_cost() {return _cost;}
        int get_x() {return _x;}
        int get_y() {return _y;}
        int get_id() {return _id;}
        Grid* get_prev() { return _prevgrid;}
        bool get_throughable() { return _throughable;}
        vector<DefParser::Block*> get_blocks() { return _blocks;}
        // set
        void set_G(int G) {_G = G;}
        void set_cost(int cost) {_cost = cost;}
        void set_x(int x) {_x = x;}
        void set_y(int y) {_y = y;}
        void set_id(int id) {_id = id;}
        void set_prev(Grid* prev) {_prevgrid = prev;}
        void set_throughable(bool throughable) {_throughable = throughable;}
        void add_block(DefParser::Block* b) {_blocks.push_back(b);}
        // destructor
        ~Grid(){ }
    private:
        int _id;
        int _G;
        int _cost;
        int _x;
        int _y;
        Grid* _prevgrid;
        bool _throughable;
        vector<DefParser::Block*> _blocks;
};

class Router {
public:
    Grid** grid_graph;
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
            //grid_width = ceil((double)maxTrack / (double)tracks_per_um);
            grid_width = 2000;

            //init blocks
            for (auto it = blocks.begin(); it != blocks.end(); ++it) {
                string blockName = "BLOCK_" + it->second.type.substr(it->second.type.find('_') + 1);
                it->second.shape = blocks[blockName].shape;

                it->second.throughBlockNetNum = blockInfos[it->second.name].throughBlockNetNum;
                it->second.isFeedthroughable = blockInfos[it->second.name].isFeedthroughable;
            }

            //init grid_graph
            grid_graph = new Grid*[grid_index(boundingbox).first+1];
            for (int i = 0; i < grid_index(boundingbox).first; i++) {
                grid_graph[i] = new Grid[grid_index(boundingbox).second+1];
                for (int j = 0; j < grid_index(boundingbox).second; ++j) {
                    grid_graph[i][j] = Grid(0, 0, i, j, 1);
                }
            }

            //traverse blks and set throughable
            for (auto it = blocks.begin(); it != blocks.end(); ++it) {
                //calculate actual shape
                //it->second.actual_shape = calculateActualCoordinate(it->second.position, it->second.shape, it->second.orientation);
                
                //set throughable
                Polygon polygon(calculateActualCoordinate(it->second.position, it->second.shape, it->second.orientation));
                int minX, maxX, minY, maxY;
                polygon.getBounds(minX, maxX, minY, maxY);
                for (int i = minX; i <= maxX; ++i) {
                    for (int j = minY; j <= maxY; ++j) {
                        assert(i < (grid_index(boundingbox).first+1));
                        assert(j < (grid_index(boundingbox).second+1));
                        if (polygon.isPointInside(i, j)) {
                            grid_graph[i][j].set_throughable(false);
                            grid_graph[i][j].add_block(&it->second);
                        }
                    }
                }
            }
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
        double shift_y = (floor(coordinate.second/grid_width)+0.5)*grid_width;
        return make_pair(shift_x, shift_y);
    }

    pair<int, int> transformPoint(const pair<int, int>& point, const string& orientation, int width, int height) {
        if (orientation == "N") {
            // North:
            return point;
        } else if (orientation == "S") {
            // South:
            return pair<int, int>(width - point.first, height - point.second);
        } else if (orientation == "E") {
            // East:
            return pair<int, int>(height - point.second, point.first);
        } else if (orientation == "W") {
            // West:
            return pair<int, int>(point.second, width - point.first);
        } else if (orientation == "FN") {
            // Flipped North:
            return pair<int, int>(point.first, height - point.second);
        } else if (orientation == "FS") {
            // Flipped South:
            return pair<int, int>(width - point.first, point.second);
        } else if (orientation == "FW") {
            // Flipped West:
            return pair<int, int>(point.second, point.first);
        } else if (orientation == "FE") {
            // Flipped East:
            return pair<int, int>(height - point.second, width - point.first);
        } else {
            cerr << "Unknown orientation: " << orientation << endl;
            return pair<int, int>(0, 0);
        }
    }

    vector<pair<int, int>> calculateActualCoordinate(const pair<int, int>& origin, const vector<pair<int, int>>& relativePoints, const string& orientation) {
        //calculate width and height
        int minX = relativePoints[0].first, maxX = relativePoints[0].first;
        int minY = relativePoints[0].second, maxY = relativePoints[0].second;
        for (const auto& point : relativePoints) {
            if (point.first < minX) minX = point.first;
            if (point.first > maxX) maxX = point.first;
            if (point.second < minY) minY = point.second;
            if (point.second > maxY) maxY = point.second;
        }
        int width = maxX - minX;
        int height = maxY - minY;

        vector<pair<int, int>> actualPoints;
        for (const auto& relativePoint : relativePoints) {
            pair<int, int> transformedPoint = transformPoint(relativePoint, orientation, width, height);
            pair<int, int> actualPoint = grid_index(make_pair(origin.first + transformedPoint.first, origin.second + transformedPoint.second));
            // cerr << "Relative Coordinate: (" << origin.first + transformedPoint.first << ", " << origin.second + transformedPoint.second << ")" << endl;
            // cerr << "Actual Coordinate: (" << actualPoint.first << ", " << actualPoint.second << ")" << endl;
            actualPoints.push_back(actualPoint);
        }
        
        return actualPoints;
    }

    // Calculate the overflow cost
    double CalOverflowCost() {
        double cost = 0.0;
        double cap_gcell_edge = maxTrack;
        for (auto net : nets) {
            double hpwl = net.CalHPWL();
            double occupied_track = net.numTracks;
            vector<vector<Grid>> segmentList = net.routingSegment;
            double segemnt_cost = 0.0;
            for (int i=0; i<segmentList.size(); i++) {
                vector<Grid> twoPinSegment = segmentList[i];
                for (int j=0; j<twoPinSegment.size(); j++) {
                    Grid grid = twoPinSegment[j];
                    double trackCost = grid.get_cost();
                    segemnt_cost += trackCost;
                }
            }
            cost += (segemnt_cost / hpwl);
        }

        return cost;
    }

    // Calculate the overflow cost
    double CalNetTurnCost() {
        double cost = 0.0;
        for (auto net : nets) {
            unordered_map<int, bool> grid_record;
            vector<vector<Grid>> segmnetList = net.routingSegment;
            int turn_num = 0;
            for (int i=0; i<segmnetList.size(); i++) {
                vector<Grid> twoPinSegment = segmnetList[i];
                if (twoPinSegment.size() >= 3) {
                    for (int j=0; j<twoPinSegment.size()-2; j++) {
                        Grid grid1 = twoPinSegment[j];
                        Grid grid2 = twoPinSegment[j+1];
                        Grid grid3 = twoPinSegment[j+2];
                        bool sameAxis = 0;  // 0 for x-axis the same, 1 for y-axis the same
                        if (grid_record.find(grid1.get_id()) == grid_record.end() || grid_record.find(grid2.get_id()) == grid_record.end() || grid_record.find(grid3.get_id()) == grid_record.end()) {
                            if (grid1.get_x() == grid2.get_x()) {
                                sameAxis = 0;
                            }
                            else {
                                sameAxis = 1;
                            }
                            if (!sameAxis) {
                                turn_num = (grid3.get_x() != grid2.get_x()) ? turn_num + 1 : turn_num;
                            }
                            else {
                                turn_num = (grid3.get_y() != grid2.get_y()) ? turn_num + 1 : turn_num;
                            }
                            grid_record[grid1.get_id()] = 1;
                            grid_record[grid2.get_id()] = 1;
                            grid_record[grid3.get_id()] = 1;
                        } 
                    }
                }
            }
            turn_num = (turn_num > 1) ? turn_num : -INT_MAX;
            cost += exp(turn_num);
        }

        return cost;
    }

    // Calculate the evaluation score
    double EvaluateScore(size_t time) {
        double final_score = 0.0;

        // Calculate time cost
        double time_param = 0.1;
        double time_score = time_param * exp(time / (2 * 60 * 60));

        // Calculate overflow cost
        double overflow_param = 0.55;
        double overflow_cost = CalOverflowCost();
        double overflow_score = overflow_param * overflow_cost;

        // Calculate net turn cost
        double net_turn_param = 0.01;
        double net_turn_cost = CalNetTurnCost();
        double net_turn_score = net_turn_param * net_turn_cost;

        final_score = overflow_score + net_turn_score + time_score;

        return final_score;
    }

    //data member
};
