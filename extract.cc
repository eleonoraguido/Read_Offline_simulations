
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

int main(int argc, char* argv[]) {
    // Check if a single argument (mass name) is provided
    if (argc != 2) {
        cerr << "Error. Usage example: " << argv[0] << " <particle_name>" << endl;
        return 1;
    }

    string particleName = argv[1];
    string commonPath = "/data/auger8/SDPhoton_v4r0p2/CORSIKA77420/EPOS";

    // Read root files for the specified particle and create merged tree
    ReadRootFiles(commonPath, particleName);

    return 0;
}
