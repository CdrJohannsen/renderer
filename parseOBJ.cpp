#include <fstream>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

vector<string> split(string text,string at){
    vector<string> splitted;
    size_t pos = 0;
    string segment;
    while ((pos = text.find(at)) != string::npos) {
        segment = text.substr(0, pos);
        text.erase(0, pos + at.length());
        splitted.push_back(segment);
    }
    splitted.push_back(text);
    //string* d = splitted.data();
    //cout << d[0] << endl;
    return splitted;
}

void parse(){
    string line;
    string n;
    ifstream model ("cube.obj");
    vector<vector<string>> vert;
    vector<vector<string>> face;
    while(!model.eof()){
        getline(model,line);
        if (line.substr(0,2)=="v "){
            line.erase(0,2);
            vector<string> a = split(line," ");
            vert.push_back(a);
        }
        else if (line[0]=='f'){
            line.erase(0,2);
            face.push_back(split(line," "));
        }
    }
    vector<Vect> verticies;
    for (vector<string> i:vert){
        Vect a = {stof(i[0]),stof(i[1]),stof(i[2])};
        verticies.push_back(a);
    }
    for (vector<string> i:face){
        Face a;
        vector<vector<string>> faces;
        for (string j:i){
            faces.push_back(split(j,"/"));
        }
        *a.vert_a = verticies[stoi(faces[0][0])];
        *a.vert_b = verticies[stoi(faces[1][0])];
        //*a.vert_c = verticies[stoi(faces[2][0])];
        //cout << a <<endl;
    }
    return;
}

