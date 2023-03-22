#include <fstream>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

void split(string text,string at){
    vector<string> splitted;
    size_t pos = 0;
    string segment;
    while ((pos = text.find(at)) != string::npos) {
        segment = text.substr(0, pos);
        text.erase(0, pos + at.length());
        splitted.push_back(segment);
    }
    splitted.push_back(text);
    string* d = splitted.data();
    cout << d[1];
    return;
}

void parse(){
    string line;
    string n;
    ifstream model ("cube.obj");
    vector<string*> v;
    while(!model.eof()){
        getline(model,line);
        if (line[0]=='v'){
            if (line[1]=='n'){continue;}
            line.erase(0,2);
            //v.push_back(split(line," "));
            split(line," ");
        }
    }
    //vector<string> values;
    //values=*v.data();
    //for (string i:values){
    //    cout << i;
    //}
    return;
}

