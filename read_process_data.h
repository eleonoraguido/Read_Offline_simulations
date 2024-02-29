#ifndef READ_PROCESS_DATA_H
#define READ_PROCESS_DATA_H

//Struch to store the information related to the three stations, for each event
struct EventDataStations {
    std::vector<double> T0, Dist, Stot, azimuths;
    std::vector<std::vector<double>> traces_vec;
};

//Function to periodically save data
void PeriodicSave(TTree* tree, TFile* file, const std::chrono::seconds& interval) {

//Function to check if an energy bin intersects with the given range
//It is used to check which directory will be read, depending on the energy range written in their name
bool intersects(double min_range, double max_range, double bin_min, double bin_max); 

//Read all the root files in the commonPath, selecting according to particleName and energy range.
//Create an output file in the given directory
void ReadRootFiles(const std::string& commonPath, const std::string& output_dir, const std::string& particleName, double ene_min, double ene_max);
EventDataStations ProcessEventData(RecEvent* theRecEvent);

#endif