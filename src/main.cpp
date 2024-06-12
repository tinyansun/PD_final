#include <DefParser.h>
#include <CfgJsonParser.h>
#include <ConnectionJsonParser.h>
#include <Router.h>

using namespace std;

int main(int argc, char* argv[]) {
    //start timer
    auto start = chrono::high_resolution_clock::now();

    bool draw = false;
    int tracks_per_um;
    string defDirectory;
    string cfgFile;
    string connectionFile;
    //parser
    if(argc == 6) {
        draw = true;
        tracks_per_um = stoi(argv[2]);
        defDirectory = argv[3];
        cfgFile = argv[4];
        connectionFile = argv[5];
    }
    else if (argc == 5) {
        tracks_per_um = stoi(argv[1]);
        defDirectory = argv[2];
        cfgFile = argv[3];
        connectionFile = argv[4];
    }
    else{
        cerr << "Usage: ./CGR <tracks/um> <def_directory> <cfg_file> <connection_file>" << endl;
        return -1;
    }
    
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
    if(draw){
        router.printGrid();
    }

    return 0;
    // Now, the router object contains all the necessary data for further processing.
    
}

