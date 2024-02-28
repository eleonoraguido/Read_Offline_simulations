// tools_traces.h

#ifndef TOOLS_TRACES_H
#define TOOLS_TRACES_H

extern ETraceType TypeTrace;

// Function DECLARATION
std::vector<double> arrival_times(std::vector<SdRecStation>);
std::vector<double> arrival_times_wrt_hottest(std::vector<SdRecStation> );
std::vector<double> total_signals(std::vector<SdRecStation>);


#endif