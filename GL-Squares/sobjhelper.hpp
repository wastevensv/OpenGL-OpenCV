#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include "verts.hpp"

using namespace std;

void load_sobj(const string path, vector<Vert<float, 6>> &out_vert) {
    ifstream in_file;
    in_file.open(path);

    if(in_file.is_open()) {
      vector<Vert<float, 6>> tmp_vert;
      string line;

      while(!in_file.eof()) {
        getline(in_file, line);
        if(line[0] != '!' && line[0] != '#') {
          Vert<float, 6> data;
          sscanf(line.c_str(), "%f %f %f %f %f %f", &data[0], &data[1],
            &data[2], &data[3], &data[4], &data[5]);
          out_vert.push_back(data);
        }
      }
    }
}
