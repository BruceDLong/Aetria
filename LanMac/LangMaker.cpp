// Build with: g++ -g  LangMaker.cpp -o LangMaker

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <map>

using namespace std;
int main(int argc, char **argv){
    string CultureName;
    printf("Enter Culture Name:");
    cin >> CultureName;
    printf("CultureName:%s\n\n", CultureName.data());

    int count=0;
    multimap<string, string> soundSeqs;
    string soundSeq;
    do{
        cin >> soundSeq;
        if(soundSeq!="" && soundSeq!="end"){
            string key=soundSeq.substr(0,1);
            string value=soundSeq.substr(2, string::npos);
            soundSeqs.insert(std::pair<string, string>(key,value));
            count++;
            printf("[%s]=%s.\n", key.data(), value.data());
        }
    } while(soundSeq!="end");

    printf("\nRead %i items.\n", count);

    ///////////////////////////
    // Now Read in each word to translate

    fstream words("gameWords");
    while(words){
        char Word[256];
        words.getline(Word, 256);

        ////
        string newWord;
        newWord+=soundSeqs["1"];
        newWord+=soundSeqs["2"];
        newWord+=soundSeqs["3"];

        std::cout << Word << ":" << newWord <<"\n";

    }
}
