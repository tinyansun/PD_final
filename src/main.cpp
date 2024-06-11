#include <DefParser.h>
#include <CfgJsonParser.h>
#include <ConnectionJsonParser.h>
#include <Router.h>
#include <Graph.h>

using namespace std;

int main(int argc, char* argv[]) {

    //start timer
    auto start = chrono::high_resolution_clock::now();

    //parser
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

    Router router(tracks_per_um, connectionParser.getmaxTrack(), defParser.getBoundingbox(), defParser.getBlocks(), cfgParser.getBlocks(), connectionParser.getNets());
    cerr << "parse finish, time = " << (chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count()) / 1000.0 << endl;
    
    // Now, the router object contains all the necessary data for further processing.


    // usage: 
    // Graph G;
    // for (block in net) G.push_v(&block)
    // G.make_complete_g()
    // global_wire = G.MST()


    vector<ConnectionJsonParser::NetInfo> net = router.getNets();
    unordered_map<string, DefParser::Block> block = router.getBlocks();

    for(int i = 0; i < net.size(); i++){
        // new graph
        Graph cur_graph = new Graph();

        // push_v all blks of this net
        Block* cur_blk_tx, cur_blk_rx; // how to decalre Block?
        cur_blk_tx = blocks[net[i].tx]; // map string to block?
        cur_graph.push_v(cur_blk_tx);

        vector<string> blk_rx_list = net[i].rx;
        for (int j = 0; j < blk_rx_list.size(); j++){
            cur_blk_rx = blocks[blk_rx_list[j]];
            cur_graph.push_v(cur_blk_rx);
        }

        // implement MST: get vector of pair of blks
        cur_graph.make_complete_g();
        vector<Edge*> MST_out = cur_graph.MST();
        
        // Find coordinates of each pair
        for (int j = 0; j < MST_out.size(); j++){
            // Block*
            Block* cur_blk_1 = MST_out[j].first;
            Block* cur_blk_2 = MST_out[j].second;

            double cur_blk_1_x, cur_blk_1_y;
            double cur_blk_2_x, cur_blk_2_y;

            // start blk: blk_1, end_blk: blk_2
            if (cur_blk_1 == blocks[net[i].tx]){
                cur_blk_1_x = cur_blk_1.position.first + net[i].txCoord.first;
                cur_blk_1_y = cur_blk_1.position.second + net[i].txCoord.second;
                cur_blk_2_x = cur_blk_2.position.first + net[i].rxCoord.first;
                cur_blk_2_y = cur_blk_2.position.second + net[i].rxCoord.second;
                // A*-search
                astar_search(cur_blk_1_x, cur_blk_1_y, cur_blk_2_x, cur_blk_2_y);
            }
            // start blk: blk_2, end_blk: blk_1
            else if (cur_blk_1 == blocks[net[i].rx]){
                cur_blk_2_x = cur_blk_2.position.first + net[i].txCoord.first;
                cur_blk_2_y = cur_blk_2.position.second + net[i].txCoord.second;
                cur_blk_1_x = cur_blk_1.position.first + net[i].rxCoord.first;
                cur_blk_1_y = cur_blk_1.position.second + net[i].rxCoord.second;
                // A*-search
                astar_search(cur_blk_2_x, cur_blk_2_y, cur_blk_1_x, cur_blk_1_y);
            }
            else{
                cout << "Impossible! there's no middle blks!" << endl;
            }
        }
    }

}

