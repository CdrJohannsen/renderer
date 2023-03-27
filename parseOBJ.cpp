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

vector<Vect> normals;
vector<Vect> verticies;

vector<Face> parse(){
    string line;
    string n;
    ifstream model ("cube.obj");
    vector<vector<string>> vert;
    vector<vector<string>> face;
    vector<vector<string>> normal;
    while(!model.eof()){
        getline(model,line);
        if (line.substr(0,2)=="v "){
            line.erase(0,2);
            vert.push_back(split(line," "));
        }
        else if (line.substr(0,2)=="vn"){
            line.erase(0,3);
            normal.push_back(split(line," "));
        }
        else if (line[0]=='f'){
            line.erase(0,2);
            face.push_back(split(line," "));
        }
    }
    for (vector<string> i:vert){
        Vect a = {stof(i[0]),stof(i[1]),stof(i[2])};
        verticies.push_back(a);
    }
    for (vector<string> i:normal){
        Vect a = {stof(i[0]),stof(i[1]),stof(i[2])};
        normals.push_back(a);
    }
    vector<Face> faces;
    for (vector<string> i:face){
        vector<vector<string>> face_string;
        for (string j:i){
            face_string.push_back(split(j,"/"));
        }
        Face f;
        f.vert_a = stoi(face_string[0][0])-1;
        f.vert_b = stoi(face_string[1][0])-1;
        f.vert_c = stoi(face_string[2][0])-1;
        f.norm_a = stoi(face_string[0][2])-1;
        f.norm_b = stoi(face_string[1][2])-1;
        f.norm_c = stoi(face_string[2][2])-1;
        faces.push_back(f);
    }
    return faces;
}

