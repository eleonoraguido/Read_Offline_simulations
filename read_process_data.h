#ifndef READ_PROCESS_DATA_H
#define READ_PROCESS_DATA_H

#include <chrono>
#include <thread>

//Struch to store the information related to the three stations, for each event
struct EventDataStations {
    std::vector<double> T0, Dist, Stot, azimuths;
    std::vector<std::vector<double>> traces_vec;

    bool isEmpty; // Flag to indicate if the object is empty

    // Constructor to initialize the object as empty
    EventDataStations() : isEmpty(true) {}
};

//Function to periodically save data
void PeriodicSave(TTree* tree, TFile* file, const std::chrono::seconds& interval);

//Function to check if an energy bin intersects with the given range
//It is used to check which directory will be read, depending on the energy range written in their name
bool intersects(double min_range, double max_range, double bin_min, double bin_max); 

// Function to extract file name from full path string (used to create output file from input path)
std::string GetFileName(const std::string& path);

//Look for all the root files in the commonPath, selecting according to particleName and energy range.
void ReadRootFiles(const std::string& commonPath, const std::string& output_dir, const std::string& particleName, double ene_min, double ene_max);

// Read each input root file and create output file with a simplified tree in the directory chosen in config.txt
void ProcessInputFile(const std::string& inputFile, const std::string& output_dir, const std::string& particleName, double ene_min, double ene_max, int counterFile);

//Create an object EventDataStations with the info about 3 stations, to be stored in the tree
EventDataStations ProcessEventData(RecEvent* theRecEvent);

#endif