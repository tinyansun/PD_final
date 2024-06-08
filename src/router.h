#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include "module.h"

using namespace std;

class Router {
    public:
        Router();
    private:
        vector<Net*> nets;
        vector<Block*> blks;
};