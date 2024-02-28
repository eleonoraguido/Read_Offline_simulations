
#include <numeric>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <dirent.h>


#include <RecEvent.h>
#include <RecEventFile.h>
#include <DetectorGeometry.h>
#include <SdRecStation.h>

#include <TMath.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TVector3.h>

#include "generic_tools.h"
#include "tools_traces.h"
#include "read_process_data.h"
#include "extract.h"

using namespace std;


//*******************************************************************
//Settings:
//*******************************************************************
int trace_length = 150;
int nstat_max = 3; //number of hottest stations to select 
int nstat = 3; //it will be changed for each event (maximum is nstat_max)



//*******************************************
//                   MAIN
//*******************************************

int main() {

    std::unordered_map<std::string, std::string> config = parse_config_file("config.txt");

    // Access configuration values
    std::string input_path = config["input_path"];
    std::string output_path = config["output_path"];
    std::string particleName = config["particle"];
    double ene_min = std::stod(config["Ene_min"]);
    double ene_max = std::stod(config["Ene_max"]);

    // Use the configuration values as needed
    std::cout << "Input path: " << input_path << std::endl;
    std::cout << "Output path: " << output_path << std::endl;
    std::cout << "Particle: " << particleName << std::endl;
    std::cout << "Minimum energy: " << ene_min << std::endl;
    std::cout << "Maximum energy: " << ene_max << std::endl;

    // Read root files for the specified particle and create merged tree
    ReadRootFiles(input_path, particleName, ene_min, ene_max);

    return 0;
}
