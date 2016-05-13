#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "verts.hpp"

using namespace std;

void convert(string path, vector<Vert<float, 6>> & out_vert) {
  cout << "Loading OBJ file " << path << "..." << endl;
  
  ifstream in_file;
  in_file.open (path);

  if (in_file.is_open()) {
    vector<Vert<float, 6>> tmp_vert;
    vector<Vert<int, 3>> tmp_index;
    string line;

    while(!in_file.eof()) {
      getline(in_file, line);

      istringstream line_s (line);
      string header;
      line_s >> header;

      if(header[0] != '#') {
        if(header.compare("v") == 0) {
          Vert<float, 6> data;
          sscanf(line.c_str(), "v %f %f %f", &data[0], &data[1], &data[2]);

          tmp_vert.push_back(data);

        } else if(header.compare("f") == 0) {
          Vert<int, 9> data;

          char skip = '\0';

          int ret = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &data[0], &data[1],
              &data[2],&data[3],&data[4],&data[5],&data[6],&data[7],&data[8]);
          if(ret!=9) {
            ret = sscanf(line.c_str(), "f %d %d %d\n",
              &data[0], &data[3], &data[6]);
            if(ret!=3) {
              cerr << "Parsing failed." << endl;
              return;
            }
          }

          Vert<int, 3> index;
          index[0] = data[0] - 1;
          index[1] = data[3] - 1;
          index[2] = data[6] - 1;
          tmp_index.push_back(index);
        }
      }
    }

    for(int i = 0; i < tmp_index.size(); i++) {
      out_vert.push_back(tmp_vert[tmp_index[i][0]]);
      out_vert.push_back(tmp_vert[tmp_index[i][1]]);
      out_vert.push_back(tmp_vert[tmp_index[i][2]]);
    }
    in_file.close();
  }
  else cerr << "Error opening the input or output file" << endl;
}

int main( int argc, char *argv[] ) {
  vector<Vert<float, 6>> verts;
  if(argc == 1) return -1;
  string path = argv[1];
  convert( path, verts );

  ofstream out_file;
  path += ".sobj";
  out_file.open (path);

  out_file << "! " << verts.size() << ' ' << '6' << endl;
  for(int i = 0; i < verts.size(); ++i) {
    out_file << verts[i] << endl;
    cout << verts[i] << endl;
  }
  out_file.close();
}
