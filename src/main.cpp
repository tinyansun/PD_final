#include <DefParser.h>
#include <CfgJsonParser.h>
#include <ConnectionJsonParser.h>
#include <Router.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: ./CGR <tracks/um> <def_directory> <cfg_file> <connection_file>" << endl;
        return -1;
    }
    
    int tracks_per_um = stoi(argv[1]);
    string defDirectory = argv[2];
    string cfgFile = argv[3];
    string connectionFile = argv[4];

    DefParser defParser(defDirectory);
    if (!defParser.parse()) {
        cerr << "Failed to parse DEF files." << endl;
        return -1;
    }

    CfgJsonParser cfgParser(cfgFile);
    if (!cfgParser.parse()) {
        cerr << "Failed to parse CFG JSON file." << endl;
        return -1;
    }

    ConnectionJsonParser connectionParser(connectionFile);
    if (!connectionParser.parse()) {
        cerr << "Failed to parse Connection JSON file." << endl;
        return -1;
    }

    Router router(tracks_per_um, defParser.getChipComponents(), defParser.getBlocks(), cfgParser.getBlocks(), connectionParser.getNets());

    // Now, the router object contains all the necessary data for further processing.


    // usage: 
    // Graph G;
    // for (block in net) G.push_v(&block)
    // G.make_complete_g()
    // global_wire = G.MST()


    vector<ConnectionJsonParser::NetInfo> net = router.getNets();
    unordered_map<string, DefParser::Block> blocks = router.getBlocks();
    

    for(int i = 0; i < net.size(); i++){
        // new graph
        Graph cur_graph;

        // record path for each net, i: which block pair, j: grids on path
        vector<vector<pair<double, double>>> Astar_out;

        // push_v all blks of this net
        cur_graph.push_v(&blocks[net[i].tx]);

        vector<string> blk_rx_list = net[i].rx;
        for (int j = 0; j < blk_rx_list.size(); j++){
            cur_graph.push_v(&blocks[blk_rx_list[j]]);
        }

        // for test
        cout << "start MST" << endl;

        // implement MST: get vector of pair of blks
        cur_graph.make_complete_g();
        vector<pair<DefParser::Block*, DefParser::Block*>> MST_out = cur_graph.MST();
        
        cout << "start Astar" << endl;

        // Find coordinates of each pair
        for (int j = 0; j < MST_out.size(); j++){

            // Block*
            DefParser::Block* cur_blk_1 = MST_out[j].first;
            DefParser::Block* cur_blk_2 = MST_out[j].second;

            double cur_blk_1_x, cur_blk_1_y;
            double cur_blk_2_x, cur_blk_2_y;

            // start blk: blk_1, end_blk: blk_2
            if (MST_out[j].first == &blocks[net[i].tx]){
                // tx
                cur_blk_1_x = (MST_out[j].first)->position.first + net[i].txCoord.first;
                cur_blk_1_y = (MST_out[j].first)->position.second + net[i].txCoord.second;

                // multiple rx
                int index;

                for (int k = 0; k < net[i].rx.size(); k++){
                    if (MST_out[j].second->name == net[i].rx[k]) index = k;
                }

                cur_blk_2_x = (MST_out[j].second)->position.first + net[i].rxCoord[index].first;
                cur_blk_2_y = (MST_out[j].second)->position.second + net[i].rxCoord[index].second;
                
                // change to grid coord
                double grid_1_x, grid_1_y;
                double grid_2_x, grid_2_y;

                grid_1_x = router.grid_index(make_pair(cur_blk_1_x, cur_blk_1_y)).first;
                grid_1_y = router.grid_index(make_pair(cur_blk_1_x, cur_blk_1_y)).second;
                grid_2_x = router.grid_index(make_pair(cur_blk_2_x, cur_blk_2_y)).first;
                grid_2_y = router.grid_index(make_pair(cur_blk_2_x, cur_blk_2_y)).second;

                // A*-search
                Astar_out[j] = astar_search(router, grid_1_x, grid_1_y, grid_2_x, grid_2_y);
            }
            // start blk: blk_2, end_blk: blk_1
            else if (MST_out[j].second == &blocks[net[i].tx]){
                cur_blk_2_x = (MST_out[j].second)->position.first + net[i].txCoord.first;
                cur_blk_2_y = (MST_out[j].second)->position.second + net[i].txCoord.second;

                // multiple rx
                int index;

                for (int k = 0; k < net[i].rx.size(); k++){
                    if (MST_out[j].second->name == net[i].rx[k]) index = k;
                }
                cur_blk_1_x = (MST_out[j].first)->position.first + net[i].rxCoord[index].first;
                cur_blk_1_y = (MST_out[j].first)->position.second + net[i].rxCoord[index].second;

                // change to grid coord
                double grid_1_x, grid_1_y;
                double grid_2_x, grid_2_y;

                grid_1_x = router.grid_index(make_pair(cur_blk_1_x, cur_blk_1_y)).first;
                grid_1_y = router.grid_index(make_pair(cur_blk_1_x, cur_blk_1_y)).second;
                grid_2_x = router.grid_index(make_pair(cur_blk_2_x, cur_blk_2_y)).first;
                grid_2_y = router.grid_index(make_pair(cur_blk_2_x, cur_blk_2_y)).second;

                // A*-search
                Astar_out[j] = astar_search(router, grid_2_x, grid_2_y, grid_1_x, grid_1_y);
            }
            else{
                cout << "Impossible! there's no middle blks!" << endl;
            }
        }
        
        // store the result back to net-struct   
        // TODO:

        net[i]._Astar_out = Astar_out;

        /*
        for (int j = 0; j < Astar_out.size(); j++){
            vector<Grid> tmp;
            for (int k = 0; k < Astar_out[j].size(); k++){
                // net[i]._Astar_out[j][k] = Astar_out[j][k];
                tmp.push_back(Astar_out[j][k]);
                
            }
            //net[i]._Astar_out[j].push_back(Astar_out[j][k]);
            net[i]._Astar_out.push_back(tmp);
        }
        */
    }

    return 0;
}

