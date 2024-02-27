import Configurinator.Model;

export import <thread>;

#include <include.h>

export module Configurinator.Backend;

BEGIN_NAMESPACE
export [[ nodiscard ]]
std::thread SpinBackendProcess(Model& model);

/*
Run Backend in new thread.
1. [ ] watch 'ModelData.Staged' for changes, like forwarding from nodes status from queued to running to finished or failed.
1. [ ] write write current running node to 'debugfile' waiting/watching for node status to change in 'debugfile' and update 'ModelData.Staged' accordingly.
1. [ ] on first run, clear 'debugfile' and 'logfile', spin up 'pythonexec' to run script that watches 'debugfile' for node to run, that process will spin a thread to run the node and write log to 'logfile', updates its status (on completion/failed in 'debugfile'). This process will continue to run until 'debugfile' is deleted and will wait if 'debugfile' empty.
*/
std::thread SpinBackendProcess(Model& model)
{
	return std::thread([](const auto& model_data) {}, model);
}

END_NAMESPACE
