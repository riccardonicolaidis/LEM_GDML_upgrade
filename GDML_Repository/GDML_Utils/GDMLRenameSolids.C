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

int GDMLRenameSolids()
{

    // Open the file
    TString FileNameIn  = "LEM-19032023-worldVol.gdml";
    TString FileNameOut = "LEM-19032023-worldVol-modified.gdml";
    TString FileNameInRename = "rn.SolidNames_LEM-19032023-worldVol.txt";

    ifstream FileIn(FileNameIn);
    ifstream FileInRename(FileNameInRename);

    // Open new Out File but if it exists give error


    ofstream FileOut(FileNameOut, ios::trunc);
    
    
    string Line;
    string Line2;
    string Line3;
    // Open a do while construct empty untill the file has the EOF
    while(getline(FileIn, Line))
    {
        if(Line.find("<volume name=") != string::npos)
        {
            string SolidName = Line.substr(Line.find("name=\"")+6, Line.find_last_of("\"")-Line.find("name=\"")-6);
            
            getline(FileInRename, Line2);

            // Replace Solidname with Line in Line
            size_t pos = Line.find(SolidName);
            if (pos != string::npos)
            {
                Line.replace(pos, SolidName.length(), Line2);
            }
            else
            {
                cout << "Error: SolidName not found" << endl;
            }

            
            // Print the Solid Name only if SolidName is different from Line2
            if(SolidName != Line2)
            {
                cout << SolidName << " -> " << Line2 << endl;
            }
        }
        else
        {
            FileOut << Line;
        }
    }
    


    FileOut.close();
    FileIn.close();
    




    return 0;
}
