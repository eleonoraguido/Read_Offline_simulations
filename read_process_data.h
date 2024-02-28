#ifndef READ_PROCESS_DATA_H
#define READ_PROCESS_DATA_H

struct EventDataStations {
    std::vector<double> T0, Dist, Stot, azimuths;
    std::vector<std::vector<double>> traces_vec;
};

void ReadRootFiles(const std::string& commonPath, const std::string& particleName);
EventDataStations ProcessEventData(RecEvent* theRecEvent);

#endif