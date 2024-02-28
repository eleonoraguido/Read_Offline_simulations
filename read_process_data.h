#ifndef READ_PROCESS_DATA_H
#define READ_PROCESS_DATA_H


struct EventDataStations {
    std::vector<double> T0, Dist, Stot, azimuths;
    std::vector<std::vector<double>> traces_vec;
};

bool intersects(double min_range, double max_range, double bin_min, double bin_max); 

void ReadRootFiles(const std::string& commonPath, const std::string& output_dir, const std::string& particleName, double ene_min, double ene_max);
EventDataStations ProcessEventData(RecEvent* theRecEvent);

#endif