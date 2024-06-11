#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

class DefParser {
public:
    struct Region {
        string name;
        pair<int, int> min, max;

        Region() = default;
        Region(const string& n, int minX, int minY, int maxX, int maxY)
            : name(n), min(make_pair(minX, minY)), max(make_pair(maxX, maxY)) {}
    };

    struct Block {
        pair<int, int> position;
        vector<pair<int, int>> shape;
        string type;
        string orientation;

        string name;
        vector<pair<int, int>> final_shape;

        int throughBlockNetNum;
        bool isFeedthroughable;

        Block() = default;
        Block(const string& n, string t, int x, int y, const string& o)
            : name(n), type(t), position(x, y), orientation(o) {}
    };

private:
    pair<int, int> Boundingbox;
    string defDirectory;
    unordered_map<string, Block> blocks;
    unordered_map<string, Region> regions;

public:
    DefParser(const string& dir)
        : defDirectory(dir) {}

    bool parse() {
        // Find and parse chip_top.def
        fs::path chipTopPath;
        for (const auto& entry : fs::directory_iterator(defDirectory)) {
            if (entry.path().filename() == "chip_top.def") {
                chipTopPath = entry.path();
                break;
            }
        }
        if (chipTopPath.empty() || !parseMainDefFile(chipTopPath)) {
            cerr << "Failed to parse chip_top.def file." << endl;
            return false;
        }

        // Parse all other block DEF files
        for (const auto& entry : fs::directory_iterator(defDirectory)) {
            if (entry.path().extension() == ".def" && entry.path().filename() != "chip_top.def") {
                if (!parseBlockDefFile(entry.path())) {
                    cerr << "Failed to parse DEF file: " << entry.path() << endl;
                    return false;
                }
            }
        }

        return true;
    }

    bool parseMainDefFile(const fs::path& filePath) {
        ifstream defFile(filePath);
        if (!defFile.is_open()) {
            cerr << "Error opening file: " << filePath << endl;
            return false;
        }

        string line;
        while (getline(defFile, line)) {
            istringstream iss(line);
            string keyword;
            iss >> keyword;

            if (keyword == "DIEAREA") {
                int maxX, maxY;
                iss>>keyword;
                iss>>keyword;
                iss>>keyword;
                iss>>keyword;
                iss>>keyword;

                iss>>keyword;
                maxX = stoi(keyword);
                iss>>keyword;
                maxY = stoi(keyword);
                Boundingbox = make_pair(maxX, maxY);
            }
            
            if (keyword == "COMPONENTS") {
                parseComponents(defFile, blocks);
            }
            
            if (keyword == "REGIONS") {
                parseRegions(defFile, regions);
            }
            // Add other parsing sections as needed
        }

        defFile.close();
        return true;
    }

    bool parseBlockDefFile(const fs::path& filePath) {
        ifstream defFile(filePath);
        if (!defFile.is_open()) {
            cerr << "Error opening file: " << filePath << endl;
            return false;
        }

        string blockName = filePath.stem();
        blockName = "BLOCK_" + blockName.substr(blockName.find('_') + 1);
        string line;

        while (getline(defFile, line)) {
            istringstream iss(line);
            string keyword;
            iss >> keyword;

            if (keyword == "DIEAREA") {
                iss >> keyword;
                int X, Y;
                while(keyword == "("){
                    iss >> keyword;
                    X = stoi(keyword);
                    iss >> keyword;
                    Y = stoi(keyword);
                    blocks[blockName].shape.push_back(make_pair(X, Y));

                    iss >> keyword;
                    iss >> keyword;
                }
            }
            // Add other parsing sections as needed
        }

        defFile.close();
        return true;
    }

    void parseComponents(ifstream& defFile, unordered_map<string, Block>& blocks) {
        string line;
        while (getline(defFile, line)) {
            if (line.find("END COMPONENTS") != string::npos) {
                break;
            }

            istringstream iss(line);
            string name, type, posX, posY, orientation, redundant;
            if (!(iss >> redundant >> name >> type >> redundant >> redundant >> redundant >> posX >> posY >> redundant >> orientation >> redundant)) {
                continue; // Ignore malformed lines
            }
            Block block(name, type, stoi(posX), stoi(posY), orientation);
            blocks.insert({name, block});
        }
    }

    void parseRegions(ifstream& defFile, unordered_map<string, Region>& regions) {
        string line;
        while (getline(defFile, line)) {
            if (line.find("END REGIONS") != string::npos) {
                break;
            }

            istringstream iss(line);
            string name, minX, minY, maxX, maxY, orientation, redundant;
            if (!(iss >> redundant >> name >> redundant >> minX >> minY >> redundant >> redundant >> maxX >> maxY)) {
                continue; // Ignore malformed lines
            }
            Region region(name, stoi(minX), stoi(minY), stoi(maxX), stoi(maxY));
            regions.insert({name, region});
        }
    }

    const pair<int, int>& getBoundingbox() const {
        return Boundingbox;
    }

    const unordered_map<string, Block>& getBlocks() const {
        return blocks;
    }

    const unordered_map<string, Region>& getRegions() const {
        return regions;
    }
};
