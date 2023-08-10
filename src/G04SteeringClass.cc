#include "G04SteeringClass.hh"


G04SteeringClass::G04SteeringClass()
{}

G04SteeringClass::~G04SteeringClass()
{}


void G04SteeringClass::UploadSteeringFile(G4String filename)
{
  std::ifstream steeringFile(filename);
  std::string line;

  // Loop until end of file is reached
    while (std::getline(steeringFile, line))
    {
        // Ignore empty lines and lines starting with #
        if (line.empty() || line[0] == '#')
            continue;

        // Split line into words separated by whitespace
        std::stringstream ss(line);
        std::vector<std::string> words;
        std::string word;

        while(std::getline(ss, word, ' '))
        {
            words.push_back(word);
        }

        if(words[0] == OutputTextFolder_KEY)
        {
            this->SetOutputTextFolder(words[1]);
            G4cout << "Output text folder set to: " << this->GetOutputTextFolder() << G4endl;
        }

        if(words[0] == OutputMonitoringFolder_KEY)
        {
            this->SetOutputMonitoringFolder(words[1]);
            G4cout << "Output monitoring folder set to: " << this->GetOutputMonitoringFolder() << G4endl;
        }

        if(words[0] == UpdateFrequency_KEY)
        {
            this->SetUpdateFrequency(std::stoi(words[1]));
            G4cout << "Update frequency set to: " << this->GetUpdateFrequency() << G4endl;
        }

        if(words[0] == JobNumber_KEY)
        {
            this->SetJobNumber(std::stoi(words[1]));
            G4cout << "Job number set to: " << this->GetJobNumber() << G4endl;
        }


        if(words[0] == FileNumber_KEY)
        {
            this -> SetFileNumber(std::stoi(words[1]));
        }
    }

}

