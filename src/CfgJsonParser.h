#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;

class CfgJsonParser {
public:
    struct BlockInfo {
        string blockName;
        int throughBlockNetNum;
        bool isFeedthroughable;

        BlockInfo(const string& name, int tbn, bool ift)
            : blockName(name), throughBlockNetNum(tbn), isFeedthroughable(ift) {}
    };

private:
    string filePath;
    vector<BlockInfo> blocks;

public:
    CfgJsonParser(const string& path) : filePath(path) {}

    bool parse() {
        ifstream jsonFile(filePath);
        if (!jsonFile.is_open()) {
            cerr << "Error opening file: " << filePath << endl;
            return false;
        }

        nlohmann::json j;
        jsonFile >> j;

        for (const auto& block : j) {
            BlockInfo bi(
                block["block_name"],
                block["through_block_net_num"],
                block["is_feedthroughable"] == "True"
            );
            blocks.push_back(bi);
        }

        jsonFile.close();
        return true;
    }

    const vector<BlockInfo>& getBlocks() const {
        return blocks;
    }
};
