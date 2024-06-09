using namespace std;

class Router {
private:
    int tracks_per_um;
    vector<DefParser::Component> chipComponents;
    unordered_map<string, DefParser::Block> blocks;
    vector<CfgJsonParser::BlockInfo> blockInfos;
    vector<ConnectionJsonParser::NetInfo> nets;

public:
    Router(int tpu, 
           const vector<DefParser::Component>& comps, 
           const unordered_map<string, DefParser::Block>& blks,
           const vector<CfgJsonParser::BlockInfo>& blkInfos, 
           const vector<ConnectionJsonParser::NetInfo>& nts)
        : tracks_per_um(tpu), chipComponents(comps), blocks(blks), blockInfos(blkInfos), nets(nts) {}

    // Function to access chip components
    const vector<DefParser::Component>& getChipComponents() const {
        return chipComponents;
    }

    // Function to access blocks
    const unordered_map<string, DefParser::Block>& getBlocks() const {
        return blocks;
    }

    // Function to access blockInfos
    const vector<CfgJsonParser::BlockInfo>& getBlockInfos() const {
        return blockInfos;
    }

    // Function to access nets
    const vector<ConnectionJsonParser::NetInfo>& getNets() const {
        return nets;
    }

    // Function to get tracks per um
    int getTracksPerUm() const {
        return tracks_per_um;
    }
};


