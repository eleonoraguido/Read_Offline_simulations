// tools_traces.cc

#include <vector>  
#include <RecEvent.h>
#include <RecEventFile.h>
#include <DetectorGeometry.h>
#include <SdRecStation.h>

#include "tools_traces.h"  


using namespace std; 

ETraceType TypeTrace = eTotalUnsaturatedTrace; // Define the variable

//return the total signal of each station (sum over the traces and over the 3 PMTs)
vector<double> total_signals(vector<SdRecStation> station){
  
  vector<double> total_signal;

  int Nstat = station.size();

  //Compute total signal only from StartSlot to EndSlot
  for(int i = 0; i < Nstat; i++){
    double start = station[i].GetSignalStartSlot();
    double length = station[i].GetSignalEndSlot() - station[i].GetSignalStartSlot();
    double total_signal_vem = 0;
    for(int k = 1; k <= 3; k++){  //loop over the 3 PMT
      float PeakOverCharge = station[i].GetPeak(k)/station[i].GetCharge(k);
      vector<float> trace_uncut = station[i].GetPMTTraces(TypeTrace,k).GetVEMComponent(); 
      vector<float> trace = vector<float>(trace_uncut.begin() + start,trace_uncut.begin() + start + length );
      for(int j=0; j< trace.size(); j++){
        total_signal_vem += (trace[j] * PeakOverCharge);
      }
    }
    total_signal.push_back(total_signal_vem/3.);
  }

  /*for(int i = 0; i < Nstat; i++){
    double total_signal_vem = 0;
    for(int k = 1; k <= 3; k++){  //loop over the 3 PMT
      float PeakOverCharge = station[i].GetPeak(k)/station[i].GetCharge(k);
      vector<float> trace = station[i].GetPMTTraces(TypeTrace,k).GetVEMComponent(); 
      for(int j=0; j< trace.size(); j++){
        total_signal_vem += (trace[j] * PeakOverCharge);
      }
    }
    total_signal.push_back(total_signal_vem/3.);
  }*/ 
  return total_signal;
}



//return the arrival time bin of all the stations with respect to the arrival time bin 
//of the first station (1 time bin = 25 ns)
vector<double> arrival_times(vector<SdRecStation> station){
  vector<double> seconds;
  vector<double> Nseconds;
  vector<double> t0;

  int Nstat = station.size();

  for(int i = 0; i < Nstat; i++){
    seconds.push_back(station[i].GetTimeSecond());
    Nseconds.push_back(station[i].GetTimeNSecond());
  }

  double minsec = *min_element(seconds.begin(),seconds.end());

  for(int i = 0; i < Nstat; i++){
    seconds[i] -= minsec;
    t0.push_back(seconds[i] * 10e9 + Nseconds[i]);
  }

  double minNsec = *min_element(t0.begin(),t0.end());

  for(int i = 0; i < Nstat; i++){
    t0[i] -= minNsec;
    t0[i] /= 25.;
  }
  
  return t0;
}


//return the arrival time bin of all the stations with respect to hottest station arrival time (1 time bin = 25 ns)
//only if the stations are sorted by total signal (first station the hottest)
vector<double> arrival_times_wrt_hottest(vector<SdRecStation> station){

  vector<double> seconds;
  vector<double> Nseconds;
  vector<double> t0;

  int Nstat = station.size();

  for(int i = 0; i < Nstat; i++){
    seconds.push_back(station[i].GetTimeSecond());
    Nseconds.push_back(station[i].GetTimeNSecond());
  }

  double minsec = seconds[0];

  for(int i = 0; i < Nstat; i++){
    seconds[i] -= minsec;
    t0.push_back(seconds[i] * 10e9 + Nseconds[i]);
  }

  double minNsec = Nseconds[0];

  for(int i = 0; i < Nstat; i++){
    t0[i] -= minNsec;
    t0[i] /= 25.;
  }
  
  return t0;
}
