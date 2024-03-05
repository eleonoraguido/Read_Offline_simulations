#include <numeric>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <dirent.h>
#include <chrono>
#include <thread>

#include <vector>  
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

double begin_t;

// Function to periodically save data
void PeriodicSave(TTree* tree, TFile* file, const std::chrono::seconds& interval) {
    while (true) {
        // Write the tree to the output file
        file->cd();  // Make sure we are writing to the file
        tree->Write(0, TObject::kOverwrite);  // Overwrite the existing tree

        // Sleep for the specified interval
        std::this_thread::sleep_for(interval);
    }
}



// Function to check if an energy bin intersects with the given range
bool intersects(double min_range, double max_range, double bin_min, double bin_max) {
    return (bin_max > min_range) && (bin_min < max_range);
}

// Function to extract file name from full path
string GetFileName(const string& path) {
    size_t found = path.find_last_of("/\\");
    size_t second_last = path.substr(0, found).find_last_of("/\\");
    size_t third_last = path.substr(0, second_last).find_last_of("/\\");
    string outerDir = path.substr(third_last + 1, second_last - third_last - 1);
    string innerDir = path.substr(second_last + 1, found - second_last - 1);
    return innerDir + "_" + outerDir;
}


// Function to process an input file and write data to an output file
void ProcessInputFile(const string& inputFile, const string& output_dir, const string& particleName, double ene_min, double ene_max, int counterFile) {
    // Construct the output file name based on the input file name
    string outputFileName = output_dir + GetFileName(inputFile) + "_output_" + to_string(counterFile) + ".root";
    cout<<"Input file is "<<inputFile<<endl;
    cout<<"--> create output file : "<<outputFileName<<endl;
    
    TFile* outputFile = TFile::Open(outputFileName.c_str(), "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        cerr << "Error creating output file: " << outputFileName << endl;
        return;
    }

    // Create a TTree and set up branches as before
    TTree* mergedTree = new TTree("tree", "tree");
    // Define and set branches
    // Define variables to store event data
    int SDId, Nstat;
    double Theta, ESd, EMC, Shsize, Nstat_candidate, Theta_MC, X1, Phi, Phi_MC, delta_axis;
    TVector3 axis_MC, axis_SD;
    vector<double> T0, Dist, Stot, azimuthSP;
    vector<vector<double>> traces_vec;

    // Set branches in the mergedTree
    mergedTree->Branch("Nstat", &Nstat_candidate, "Nstat_candidate/D");
    mergedTree->Branch("X1", &X1, "X1/D");
    mergedTree->Branch("E_MC", &EMC, "EMC/D");
    mergedTree->Branch("E_SD", &ESd, "ESd/D");
    mergedTree->Branch("ID_SD", &SDId, "SDId/I");
    mergedTree->Branch("theta", &Theta, "Theta/D");
    mergedTree->Branch("theta_MC", &Theta_MC, "Theta_MC/D");
    mergedTree->Branch("phi", &Phi, "Phi/D");
    mergedTree->Branch("phi_MC", &Phi_MC, "Phi_MC/D");
    mergedTree->Branch("X1", &X1, "X1/D");
    mergedTree->Branch("axis_MC", &axis_MC);
    mergedTree->Branch("axis_SD", &axis_SD);
    mergedTree->Branch("delta_axis", &delta_axis, "delta_axis/D");
    mergedTree->Branch("Shsize", &Shsize);
    mergedTree->Branch("t0", &T0);
    mergedTree->Branch("Stot", &Stot);
    mergedTree->Branch("Dist", &Dist);
    mergedTree->Branch("azimuthSP", &azimuthSP);
    mergedTree->Branch("traces_vec", &traces_vec);

     // Open the data file
    RecEventFile dataFile(inputFile);
    RecEvent* theRecEvent = new RecEvent();
    DetectorGeometry theGeo;
    dataFile.SetBuffers(&theRecEvent);
    dataFile.ReadDetectorGeometry(theGeo);
    int counter = 0;   

    while (dataFile.ReadNextEvent() == RecEventFile::eSuccess){        
            counter++;


            ESd = theRecEvent->GetSDEvent().GetSdRecShower().GetEnergy();
            EMC = theRecEvent->GetGenShower().GetEnergy();
            Nstat_candidate = theRecEvent->GetSDEvent().GetNumberOfCandidates();
            // Check energy range condition
            
            if (log10(EMC) < ene_min || log10(EMC) > ene_max) continue;

            cout << endl << "=====================" << endl;
            cout<<"Reading from "<<inputFile<<endl;
            cout << endl << "Reading event number: " << counter << " of " << dataFile.GetNEvents() << endl;
            nstat = nstat_max;
            if(Nstat_candidate < 3){
                cout<<"Not enough candidate stations (< 3) -> event rejected"<<endl;
                continue;
            }
            if(ESd == 0){
                cout<<"No energy reconstruction -> event rejected"<<endl;
                continue;
            }
            if(Nstat_candidate < nstat_max) nstat = Nstat_candidate;
            
            //if(counter > 10) break;
            
            SDId = theRecEvent->GetSDEvent().GetEventId();
            cout << "  SDId:         " << SDId << endl;  
            cout << "  Nstat_cand    " << Nstat_candidate << endl;

            Theta = theRecEvent->GetSDEvent().GetSdRecShower().GetZenith();
            Theta_MC = theRecEvent->GetGenShower().GetZenith();
            cout << "  Theta:        " << Theta*180./TMath::Pi() << endl; 
            
            Phi = theRecEvent->GetSDEvent().GetSdRecShower().GetAzimuth();
            Phi_MC = theRecEvent->GetGenShower().GetAzimuth();

            X1 = theRecEvent->GetGenShower().GetX1();
            cout<<"Depth of first interaction (g cm-2):   "<< X1<<endl;

            axis_MC = theRecEvent->GetGenShower().GetAxisCoreCS(); 
            axis_SD = theRecEvent->GetSDEvent().GetSdRecShower().GetAxisCoreCS(); 
            delta_axis = axis_MC.Angle(axis_SD); 
            cout<<"Deviation in recostruncted axis: "<<delta_axis<<endl;
                
            cout << "  ESd:          " << ESd << "  ( "<<log10(ESd)<<" )" endl;      
            cout << "  EMC:          " << EMC << "  ( "<<log10(EMC)<<" )" endl; 
            Shsize = theRecEvent->GetSDEvent().GetSdRecShower().GetLDF().GetShowerSize();
            cout<< "  S1000:         " << Shsize << endl;

            // Call the function to process event data
            EventDataStations eventDataStations = ProcessEventData(theRecEvent);

            if(eventDataStations.isEmpty){
                cout<<"Problems with empty PMTs traces -> event rejected"<<endl;
                continue;   //If there were problems with filling the stations information, skip the event
            } 

            // Use the processed eventDataStations
            T0 = eventDataStations.T0;
            Stot = eventDataStations.Stot;
            traces_vec = eventDataStations.traces_vec;
            Dist = eventDataStations.Dist;
            azimuthSP = eventDataStations.azimuths;
            

            /// Fill the tree with the data from the current event
            mergedTree->Fill();
            
    }
    
    // Write the tree to the output file
    mergedTree->Write();
    //outputFile->Close();
    delete mergedTree;
    delete outputFile;
}




// Function to read root files from a directory and process them
void ReadRootFiles(const string& commonPath, const string& output_dir, const string& particleName, double ene_min, double ene_max) {
    // Iterate over all directories within commonPath
    DIR* dir = opendir(commonPath.c_str());
    if (!dir) {
        cerr << "Error opening directory: " << commonPath << endl;
        return;
    }

    struct dirent* entry;
    int counterFile = 0;
    while ((entry = readdir(dir)) != nullptr) {
        string energyBinPath = commonPath + "/" + entry->d_name;

        // Check if entry is a directory and skip "." and ".."
        if (entry->d_type == DT_DIR && string(entry->d_name) != "." && string(entry->d_name) != "..") {
            // Parse the energy bin boundaries only for non-special directories
            string energy_bin = entry->d_name;
            std::istringstream bin_iss(energy_bin);
            double bin_min, bin_max;
            char separator;
            if (bin_iss >> bin_min >> separator >> bin_max) {
                // Check if the energy bin intersects with the range
                if (!intersects(ene_min, ene_max, bin_min, bin_max)) {
                    // Skip this bin if it doesn't intersect
                    continue;
                }
                // Process the bin here
                std::cout << "Processing energy bin: " << energy_bin << std::endl;
                counterFile = 0;
            } else {
                cerr << "Warning: Skipping invalid energy bin format: " << energy_bin << std::endl;
                continue;
            }
            
            // Construct the path to the subdirectory corresponding to the particle name
            string subDirPath = energyBinPath + "/" + particleName;
            // Check if the subdirectory exists
            DIR* particleDir = opendir(subDirPath.c_str());

            if (particleDir) {
                struct dirent* fileEntry;
                while ((fileEntry = readdir(particleDir)) != nullptr) {
                    string fileName = subDirPath + "/" + fileEntry->d_name;
                    // Check if fileEntry is a regular file and process it
                    if (fileEntry->d_type == DT_REG) {
                        // Process the input file and write data to an output file
                        counterFile++;
                        ProcessInputFile(fileName, output_dir, particleName, ene_min, ene_max, counterFile);
                    }
                }
            }
            closedir(particleDir);
        }
    }
    closedir(dir);
}


// Function to process event data and return it in a struct
EventDataStations ProcessEventData(RecEvent* theRecEvent) {
    EventDataStations EventData;
    bool fill_event = true;

    std::vector<SdRecStation> SDstations = theRecEvent->GetSDEvent().GetStationVector();
    int Nstat = SDstations.size();
    std::cout << "Number of SD stations: " << Nstat << std::endl;

    
    vector<SdRecStation> statvec; //selected stations
    
    vector<double> total_signal;

    vector < vector<float> > traces_avg;
    vector<double> distances;
    vector<double> azimuths;

    for(int j=0 ; j < Nstat; j++)
    { 
         //cout<<"Station     "<<j+1<<" out of "<<Nstat<<endl;
        if(SDstations[j].IsCandidate() == true)
        {
            begin_t = SDstations[j].GetSignalStartSlot();

            vector<float> trace_pmt(trace_length, 0.0);
            vector< vector<float> > traces(3, trace_pmt);  //to store the three traces (3 PMT); 3xtrace_length
            vector<float> trace_pmt_avg(trace_length, 0.0);

            bool break_flag = false;

            for(int k = 1; k <= 3; k++){  //loop over the 3 PMT
                float PeakOverCharge = SDstations[j].GetPeak(k)/SDstations[j].GetCharge(k);
                vector<float> trace_uncut = SDstations[j].GetPMTTraces(TypeTrace,k).GetVEMComponent(); 
                if(trace_uncut.size() == 0){
                    break_flag = true;
                    break;
                }
                vector<float> trace = vector<float>(trace_uncut.begin() + begin_t,trace_uncut.begin() + begin_t + trace_length );
            
                for (size_t i = 0; i < trace_length && i < trace.size(); ++i) {
                    trace_pmt_avg[i] += trace[i];
                }
                traces[k-1] = trace;
            
            }
            if(break_flag){
                if(j < 2){
                    fill_event = false;  // do not fill the event if less than 3 stations has 3 working PMTs
                }
                 break;
            }
            // Calculate the average trace
            for (int i = 0; i < trace_length; ++i) {
                trace_pmt_avg[i] /= 3.0;
            }

            // Add the averaged trace to traces_avg
            statvec.push_back(SDstations[j]);
            traces_avg.push_back(trace_pmt_avg);
            distances.push_back(SDstations[j].GetSPDistance());
            azimuths.push_back(SDstations[j].GetAzimuthSP());
      }
      //cout<<"the rejection status is "<<SDstations[j].GetRejectionStatus()<<endl; //debug: status == 0 if candidate, status == 24 for rejected stations
    }


    if(fill_event){
        cout<<"Selected stations: "<<statvec.size()<<endl;
        int Nstat_sel = statvec.size();
    
        vector<double> t0 = arrival_times(statvec);
        vector<double> tot_sig = total_signals(statvec);

        /*
        for(int i=0; i < t0.size(); i++ ){
            cout<<"Station "<<statvec[i].GetId()<<" is a candidate"<<endl;
            cout<<"       Arrival time: "<<t0[i]<<endl;
            cout<<"       Total signal: "<<tot_sig[i]<<endl;
            cout<<"         ";
            for(int j=0; j < traces_avg[i].size(); j++){
            cout<<traces_avg[i][j]<<" ";
            }
            cout<<endl<<endl;
        }*/



        //select three hottest stations
        vector<pair<double, int > >vec;
        for(int i = 0; i < tot_sig.size(); i++){
        vec.push_back(make_pair(tot_sig[i],i));
        }

        sort(vec.begin(), vec.end());
    
        //int indices[nstat] = {vec[vec.size()-1].second, vec[vec.size()-2].second, vec[vec.size()-3].second, vec[vec.size()-4].second, vec[vec.size()-5].second};

        /*vector <double> t0_sel;
        vector <double> tot_sig_sel;
        vector <double> distance_sel;
        vector <double> azimuths_sel;
        vector < vector<double> > traces_sel;*/

        
        vector<double> x_axis;    //x axis value for the trace histograms
        for(int i = 0; i < trace_length; i++){
        x_axis.push_back(i);
        }
    

        for(int i = 0; i < nstat; i++){
            EventData.T0.push_back(t0[i]);
            EventData.Stot.push_back(tot_sig[i]);
            EventData.Dist.push_back(distances[i]);
            EventData.azimuths.push_back(azimuths[i]);

            vector<double> traces_avg_d(traces_avg[i].begin(), traces_avg[i].end());
            EventData.traces_vec.push_back(traces_avg_d);
        }
        cout<<endl<<endl<<endl<<endl;
        //TCanvas *c= new TCanvas();
        
        for(int i=0; i < EventData.T0.size(); i++ ){
            cout<<"       Arrival time: "<<EventData.T0[i]<<endl;
            cout<<"       Total signal: "<<EventData.Stot[i]<<endl;
            cout<<"       Distance from shower core: "<<EventData.Dist[i]<<endl;
            cout<<"       Azimuth in SP coord: "<<EventData.azimuths[i]<<endl;
            EventData.isEmpty = false; //it is not empty anymore
            /*cout<<"         ";
            cout<<traces_sel[i].size()<<endl;
            for(int j=0; j < traces_sel[i].size(); j++){
                cout<<traces_sel[i][j]<<" ";
            }*/

            /*TH1D *ht = new TH1D("histo trace","trace",trace_length,0.,trace_length);
            ht->FillN(trace_length,x_axis.data(),traces_sel[i].data());
            ht->Draw("same");*/
            

            cout<<endl<<endl;
        }
    }

    return EventData;
}




