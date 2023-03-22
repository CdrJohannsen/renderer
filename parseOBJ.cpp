#include <fstream>
#include <string>
#include <iostream>
using namespace std;

void parse(){
    string line;
    string n;
    ifstream model ("cube.obj");
    while(!model.eof()){
        getline(model,line);
        if (line[0]=='v'){
            if (line[1]=='n'){continue;}
            line.erase(0,2);
            size_t pos = 0;
            string token;
            string space=" ";
            while ((pos = line.find(space)) != string::npos) {
                token = line.substr(0, pos);
                cout << token << endl;
                line.erase(0, pos + space.length());
            }
            cout << line << endl;
        }
    }
}
