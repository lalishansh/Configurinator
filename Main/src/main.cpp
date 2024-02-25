import Configurinator.Model;
import Configurinator.ParseArguments;
import Configurinator.Frontend;
import Configurinator.Backend;


int main(const int argc, const char* const argv[]) {
	Configurinator::Model model_data;
	if (! Configurinator::ParseArguments(std::span{argv, static_cast<size_t>(argc)}, model_data)) {
		return 1;
	}

	std::thread ui_thread = Configurinator::SpinFrontendProcess(model_data);
	std::thread exec_thread = Configurinator::SpinBackendProcess(model_data);

	ui_thread.join();
	exec_thread.join();

	return 0;
}

/*
# Configurinator.md
1. Parse Commandline argument [searchpaths(directories of .py files), playlistsfiles(json file), debugfile(raw txt), logfile(raw txt), pythonexec]
    1. [ ] create 'debugfile', 'logfile' and hold read-write handle to these files and schedule to delete them
    1. [x] Run py script to search all 'searchpaths' for runnable script functions, and write it to 'debugfile'
    1. [ ] parse 'debugfile' to NodeDB in 'ModelData'
    1. [ ] construct nested tree structure mirroring directory structure using NodeDB
    1. [ ] parse 'playlistsfiles' to construct additional tree (Level-1 type, more like collection of lists) structures with nodes from NodeDB
    1. [ ] search for settings.json in 'Configurinator.exe' directory, load if found.
*/

/*
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
namespace std
{
	ostream& operator<<(ostream& os, const vector<string>& vec) {
		if (! vec.empty()) {
			os << vec[0];
			for (size_t i = 1; i < vec.size(); ++i) {
				os << " " << vec[i];
			}
		}
		return os;
	}
}

b8 ParseArguments(std::span<const char* const> args, ModelData& model_data) {
	// 1. PARSE ARGUMENTS
	namespace po = boost::program_options;
	namespace fs = std::filesystem;
	po::options_description desc("Options");
	desc.add_options()
		("help,h", "Print help message")
		("searchpaths,s", po::value<std::vector<std::string>>()->composing()->multitoken()->default_value([args] {
			ON_DEBUG(return std::vector{ fs::path(__FILE__).parent_path().parent_path().string() });
			return std::vector{ fs::path(args[0]).parent_path().string() };
		}()), "Search paths for python scripts")
		("playlistsfiles,p", po::value<std::vector<std::string>>()->composing()->multitoken(), "YAML file withs playlists")
		("debugfile,d", po::value<std::string>()->default_value(std::string(std::tmpnam(nullptr))), "Internal procesing debug file, defaults to temporary file")
		("logfile,l", po::value<std::string>()->default_value(std::string(std::tmpnam(nullptr))), "Log file, defaults to temporary file")
		("pythonexec,e", po::value<std::string>()->default_value("python"), "Python executable")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(args.size(), args.data(), desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << "Usage:\n  " + fs::path(args[0]).filename().string() + " [Options...]" << std::endl;
		std::cout << desc << std::endl;
		return false;
	}

	model_data.settings.readonly.debugfile = std::move(vm["debugfile"].as<std::string>());
	model_data.settings.readonly.logfile = std::move(vm["logfile"].as<std::string>());
	model_data.settings.readonly.pythonexec = std::move(vm["pythonexec"].as<std::string>());

	// 2. Run py script to search all 'searchpaths' for runnable script functions, and write it to 'debugfile'
	std::ofstream debugfile(model_data.settings.readonly.debugfile);
	debugfile << PythonScript_ParseScriptFiles_To_SelfFile();
	debugfile.close();

	std::string command = model_data.settings.readonly.pythonexec + " " + model_data.settings.readonly.debugfile;
	const std::vector<std::string> searchpaths = std::move(vm["searchpaths"].as<std::vector<std::string>>());
	for (const std::string& searchpath : searchpaths) {
		command += " " + searchpath;
	}
	std::system(command.c_str());

	const std::vector<std::string> playlistsfiles = std::move(vm["playlistsfiles"].as<std::vector<std::string>>());

	// 3. parse NodeDB
	using json = nlohmann::json;
	std::ifstream debugfile_stream(model_data.settings.readonly.debugfile);
	json j = json::parse(debugfile_stream);
	debugfile_stream.close();

	for (auto&& [key, value] : j.items()) {
		model_data.NodeDB[std::move(key)].reserve(value.size());
		for (auto&& [name, function] : value.items()) {
			ModelData::FunctionNode node;
			node.Name = std::move(name);
			node.Parameters.reserve(function["params"].size());
			for (auto&& param : function["params"]) {
				ModelData::FunctionNode::Parameter parameter;
				parameter.Name = std::move(param["name"]);
				if (param.contains("type")) {
					parameter.Type = std::move(param["type"]);
				}
				if (param.contains("default")) {
					parameter.DefaultValue = std::move(param["default"]);
				}
				node.Parameters.push_back(std::move(parameter));
			}
			node.RequiresElevation = function["elevated"];
			model_data.NodeDB[key].push_back(std::move(node));
		}
	}


}

std::thread SpinFrontendProcess(ModelData& model_data) {
	return std::thread([](const ModelData& model_data) {}, model_data);
}

std::thread SpinBackendProcess(ModelData& model_data) {
	return std::thread([](const ModelData& model_data) {}, model_data);
}
*/