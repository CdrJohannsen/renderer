#include <fstream>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

vector<float> split(string text,string at){
    vector<float> splitted;
    size_t pos = 0;
    string segment;
    while ((pos = text.find(at)) != string::npos) {
        segment = text.substr(0, pos);
        text.erase(0, pos + at.length());
        splitted.push_back(stof(segment));
    }
    splitted.push_back(stof(text));
    //string* d = splitted.data();
    //cout << d[0] << endl;
    return splitted;
}

void parse(){
    string line;
    string n;
    ifstream model ("cube.obj");
    vector<vector<float>> vert;
    vector<vector<float>> face;
    while(!model.eof()){
        getline(model,line);
        if (line[0]=='v'){
            if (line[1]=='n'){continue;}
            if (line[1]=='t'){continue;}
            line.erase(0,2);
            vert.push_back(split(line," "));
        }
        else if (line[0]=='f'){
            line.erase(0,1);
            face.push_back(split(line," "));
        }
    }
    //vector<string> values;
    //values=*v.data();
    vector<Vect> verticies;
    for (vector<float> i:vert){
        Vect a = {i[0],i[1],i[2]};
        cout << a;
        verticies.push_back(a);
    }
    return;
}

