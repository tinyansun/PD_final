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
    
}

