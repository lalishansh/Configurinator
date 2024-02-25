export module Configurinator.Frontend;

export import Configurinator.Model;

export import <thread>;

#include <include.h>

BEGIN_NAMESPACE

	[[ nodiscard ]]
export std::thread SpinFrontendProcess(Model& model);

/*
Run Frontend UI in new thread.
1.  [ ] create 1 page UI with
	```
	+---------------------------------+
	|  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]   |
	+---------------------------------+
	|  [ NOD ]  |  [              ]   |
	|  [ E T ]  |  [              ]   |
	|  [ REE ]  |  [    LOGGING   ]   |
	+-----------+  [     PANEL    ]   |
	|  [ RUN ]  |  [              ]   |
	|  [ NIN ]  |  [              ]   |
	|  [ G   ]  |  [              ]   |
	+-----------+  [              ]   |
	|  [ NOD ]  |  [              ]   |
	|  [ E O ]  |  [              ]   |
	|  [ PTS ]  |  [              ]   |
	+-----------+---------------------+
	```
1.  [ ] use 'ModelData.NodeTree', 'ModelData.Playlists' to populate 'NODE TREE', 'RUNNING' will have queued scripts, 'LOGGING PANEL' will have log messages (synced to 'logfile')
1.  etc...
*/
std::thread SpinFrontendProcess(Model& model)
{
	return std::thread([](const auto& model_data) {}, model);
}

END_NAMESPACE