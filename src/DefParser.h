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
    struct Component {
        string name;
        string type;
        pair<int, int> position;
        string orientation;

        Component(const string& n, const string& t, int x, int y, const string& o)
            : name(n), type(t), position(x, y), orientation(o) {}
    };

    struct Block {
        string name;
        vector<Component> components;
        
        Block() = default;
        Block(const string& n) : name(n) {}
    };

private:
    string defDirectory;
    vector<Component> chipComponents;
    unordered_map<string, Block> blocks;

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

            if (keyword == "COMPONENTS") {
                parseComponents(defFile, chipComponents);
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
        Block block(blockName);

        string line;
        while (getline(defFile, line)) {
            istringstream iss(line);
            string keyword;
            iss >> keyword;

            if (keyword == "COMPONENTS") {
                parseComponents(defFile, block.components);
            }
            // Add other parsing sections as needed
        }

        blocks[blockName] = block;
        defFile.close();
        return true;
    }

    void parseComponents(ifstream& defFile, vector<Component>& components) {
        string line;
        while (getline(defFile, line)) {
            if (line.find("END COMPONENTS") != string::npos) {
                break;
            }

            istringstream iss(line);
            string name, type, posX, posY, orientation;
            if (!(iss >> name >> type >> posX >> posY >> orientation)) {
                continue; // Ignore malformed lines
            }
            cerr<<"X : "<<posX<<" Y : "<<posY<<endl;
            components.emplace_back(name, type, stoi(posX), stoi(posY), orientation);
        }
    }

    const vector<Component>& getChipComponents() const {
        return chipComponents;
    }

    const unordered_map<string, Block>& getBlocks() const {
        return blocks;
    }
};
