#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

#include "TString.h"

using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>


#include "TString.h"

using namespace std;

int GDMLGetSolidNames()
{

    // Open the file
    TString FileNameIn  = "LEM-19032023-worldVol.gdml";
    TString FileNameOut = "SolidNames_LEM-19032023-worldVol.txt";
    TString FileNameOut = "SolidNames_LEM-19032023-worldVol.txt";
    
    ifstream FileIn(FileNameIn);


    ifstream inFile(FileNameOut);
    if(inFile.good())
    {
        cout << "Il file esiste gia`" << endl;
        return 1;
    }



    ofstream FileOut(FileNameOut, ios::trunc);
    string Line;

    // Open a do while construct empty untill the file has the EOF
    while(getline(FileIn, Line))
    {
        if(Line.find("<volume name=") != string::npos)
        {
            string SolidName = Line.substr(Line.find("name=\"")+6, Line.find_last_of("\"")-Line.find("name=\"")-6);
            FileOut << SolidName << endl;
            cout << SolidName << endl;
        }
    }
    


    FileOut.close();
    FileIn.close();
    




    return 0;
}
