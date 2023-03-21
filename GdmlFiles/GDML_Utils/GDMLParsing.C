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

int GDMLParsing()
{

    // Open the file
    TString FileNameIn  = "LEM-worldVol-17032023.gdml";
    TString FileNameOut = "LEM-worldVol-17032023-MODD.gdml";


    ifstream FileIn(FileNameIn);
    // Rewrite the Out File if it exists
    ofstream FileOut(FileNameOut, ios::trunc);
    string Line;

    string StringTrigger[12];

    StringTrigger[0] = "<volume name=\"LV_AL";            // Aluminium    Grey      
    StringTrigger[1] = "<volume name=\"LV_SiPM";          // Silicon      White
    StringTrigger[2] = "<volume name=\"LV_B";             // Bakelite     Green
    StringTrigger[3] = "<volume name=\"LV_P";             // Plastic      Light Blue   SensDet "PlasticScintillator"
    StringTrigger[4] = "<volume name=\"LV_V";             // Plastic      Light Blue   SensDet "PlasticScintillator"
    StringTrigger[5] = "<volume name=\"LV_Temp";          // Silicon      Black
    StringTrigger[6] = "<volume name=\"LV_Diss";          // Aluminium    Grey
    StringTrigger[7] = "<volume name=\"LV_THIN";          // Silicon      White        SensDet "Silicon"
    StringTrigger[8] = "<volume name=\"LV_THICK";         // Silicon      White        SensDet "Silicon"
    StringTrigger[9] = "<volume name=\"LV_Si_Thin";       // Aluminium    Purple
    StringTrigger[10] = "<volume name=\"LV_Si_Thick";      // Aluminium    Purple
    StringTrigger[11] = "<volume name=\"LV_Viti_Lunghe";   // Aluminium    Grey


    string StringTriggerVolRef[2];

    StringTriggerVolRef[0] = "<volumeref ref=\"LV_THIN";
    StringTriggerVolRef[1] = "<volumeref ref=\"LV_THICK";


    string StringMaterial[12];

    StringMaterial[0] = "G4_Al";
    StringMaterial[1] = "G4_Si";
    StringMaterial[2] = "G4_BAKELITE";
    StringMaterial[3] = "G4_PLASTIC_SC_VINYLTOLUENE";
    StringMaterial[4] = "G4_PLASTIC_SC_VINYLTOLUENE";
    StringMaterial[5] = "G4_Si";
    StringMaterial[6] = "G4_Al";
    StringMaterial[7] = "G4_Si";
    StringMaterial[8] = "G4_Si";
    StringMaterial[9] = "G4_Al";
    StringMaterial[10] = "G4_Al";
    StringMaterial[11] = "G4_Al";


    string StringColor[12];

    StringColor[0] = "#c8c8c8e6";
    StringColor[1] = "#ffffffff";
    StringColor[2] = "#00ff00ff";
    StringColor[3] = "#00ffffff";
    StringColor[4] = "#00ffffff";
    StringColor[5] = "#000000ff";
    StringColor[6] = "#c8c8c8e6";
    StringColor[7] = "#ffffffff";
    StringColor[8] = "#ffffffff";
    StringColor[9] = "#c8c8c8e6";
    StringColor[10] = "#c8c8c8e6";
    StringColor[11] = "#c8c8c8e6";


    string StringSensDet[12];

    StringSensDet[0] = "NONE";
    StringSensDet[1] = "NONE";
    StringSensDet[2] = "NONE";
    StringSensDet[3] = "PlasticScintillator";
    StringSensDet[4] = "PlasticScintillator";
    StringSensDet[5] = "NONE";
    StringSensDet[6] = "NONE";
    StringSensDet[7] = "Silicon";
    StringSensDet[8] = "Silicon";
    StringSensDet[9] = "NONE";
    StringSensDet[10] = "NONE";
    StringSensDet[11] = "NONE";


    int TriggerCounter = 0;
    bool Trigger = false;

    int TriggerVolRefCounter = 0;
    bool TriggerVolRef = false;







    // Loop until the "</solids>" section

    do
    {
        getline(FileIn, Line);
        FileOut << Line << endl;
    } while (Line.find("</solids>") == Line.npos);


    // Open a do while construct empty untill the file has the EOF
    while(getline(FileIn, Line))
    {
        TriggerCounter = 0;
        TriggerVolRefCounter = 0;
        Trigger = false;
        TriggerVolRef = false;
        // Scan if the line contains a trigger


        for (int i = 0; i < 12; i++)
        {
            cout << "i = " << i << endl;
            if (Line.find(StringTrigger[i]) != Line.npos)
            {
                Trigger = true;
                TriggerCounter = i;
            }
        }

        for(int i = 0; i < 2; i++)
        {
            if (Line.find(StringTriggerVolRef[i]) != Line.npos)
            {
                TriggerVolRef = true;
                TriggerVolRefCounter = i;
            }
        }


        if(Trigger == true)
        {
            if(TriggerCounter == 7 || TriggerCounter == 8)
            {
                if(Line.find("LV_THICK__") != Line.npos)
                {
                    Line.replace(Line.find("LV_THICK__"),10, "LV_THICK00");
                    Line.erase(Line.end()-3);
                }

                if(Line.find("LV_THIN__") != Line.npos)
                {
                    Line.replace(Line.find("LV_THIN__"), 9,"LV_THIN00");
                    Line.erase(Line.end()-3);
                }

                FileOut << Line << endl;
            }
            else
            {
                FileOut << Line << endl;
            }

            FileOut << "      <materialref ref=\"" << StringMaterial[TriggerCounter] << "\"/>" << endl;
            getline(FileIn, Line);
            getline(FileIn, Line);
            FileOut << Line << endl;
            if(StringSensDet[TriggerCounter].find("NONE") == Line.npos)
                FileOut << "      <auxiliary auxtype=\"SensDet\" auxvalue=\"" << StringSensDet[TriggerCounter] << "\"/>" << endl;
            
            getline(FileIn, Line);
            FileOut << "      <auxiliary auxtype=\"Color\" auxvalue=\"" << StringColor[TriggerCounter] << "\"/>" << endl;
        }
        else if(TriggerVolRef == true)
        {
            if(Line.find("LV_THICK__") != Line.npos)
                {
                    Line.replace(Line.find("LV_THICK__"), 10, "LV_THICK00");
                    Line.erase(Line.end()-4);
                }

                if(Line.find("LV_THIN__") != Line.npos)
                {
                    Line.replace(Line.find("LV_THIN__"), 9, "LV_THIN00");
                    Line.erase(Line.end()-4);
                }

                FileOut << Line << endl;


        } 
        else
        {
            FileOut << Line << endl;
        }

    }
    


    FileOut.close();
    FileIn.close();
    




    return 0;
}
