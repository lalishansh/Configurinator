import Configurinator.Model;

export import <thread>;

#include <include.h>

import <ftxui/component/component.hpp>;
import <ftxui/component/screen_interactive.hpp>;
import <ftxui/component/loop.hpp>;

export module Configurinator.Frontend;
import :CustomComponents;

BEGIN_NAMESPACE

export [[ nodiscard ]]
std::thread SpinFrontendProcess(Model& model);

ftxui::Component GetTopBarComponent(const Model& model)
{
	using namespace ftxui;
	return Renderer([] {
		return hbox({
			text("Configurinator") | center
		});
	});
}

ftxui::Component GetNodeTreeComponent(const Model& model)
{
	using namespace ftxui;
	return Renderer([] {
		return vbox({
			text("+-----------+"),
			text("| NODE TREE |"),
			text("+-----------+"),
		});
	});
}

ftxui::Component GetStagedNodesComponent(const Model& model)
{
	using namespace ftxui;
	return Renderer([] {
		return vbox({
			text("+-----------+"),
			text("| STAGED ND |"),
			text("+-----------+"),
		});
	});
}

ftxui::Component GetNodeOptionsComponent(const Model& model)
{
	using namespace ftxui;
	return Renderer([] {
		return vbox({
			text("+-----------+"),
			text("| NODE OPTS |"),
			text("+-----------+"),
		});
	});
}

ftxui::Component GetLogPanelComponent(const Model& model)
{
	using namespace ftxui;
	return Renderer([] {
		return vbox({
			text("+-----------+"),
			text("| LOG PANEL |"),
			text("+-----------+"),
		});
	});
}

/*
Run Frontend UI in new thread.
1.  [ ] create 1 page UI with
	```
	+---------------------------------+
	|  [ ]  [ ]   [ ]  [ ]  [ ]  [ ]  |
	+---------------------------------+
	|  [ NOD ]  |  [               ]  |
	|  [ E T ]  |  [               ]  |
	|  [ REE ]  |  [               ]  |
	+-----------+  [               ]  |
	|  [ STA ]  |  [    LOGGING    ]  |
	|  [ GED ]  |  [     PANEL     ]  |
	|  [  ND ]  |  [               ]  |
	+-----------+  [               ]  |
	|  [ NOD ]  |  [               ]  |
	|  [ E O ]  |  [               ]  |
	|  [ PTS ]  |  [               ]  |
	+-----------+---------------------+
	```
1.  [ ] use 'ModelData.NodeTree', 'ModelData.Playlists' to populate 'NODE TREE', 'RUNNING' will have queued scripts, 'LOGGING PANEL' will have log messages (synced to 'logfile')
1.  etc...
*/
std::thread SpinFrontendProcess(Model& model)
{
	return std::thread([](const auto& model_data) {
		auto c = GetLogPanelComponent(model_data);
		auto screen = ftxui::ScreenInteractive::FitComponent();
		screen.Loop(c);
	}, model);
}

END_NAMESPACE
