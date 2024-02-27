import Configurinator.Model;

import <boost/program_options/options_description.hpp>;
import <filesystem>;
import <iostream>;

export import <span>;

#include <include.h>

export module Configurinator.ParseArguments;
import :ParseAndProcessArgs;

BEGIN_NAMESPACE

export [[nodiscard]]
b8 ParseArguments(std::span<const char* const> args, Configurinator::Model& model_data);

#pragma region parse & process args
struct ParseAndProcessArgsDesc_t {
	std::filesystem::path debugfile;
	std::filesystem::path logfile;
	std::filesystem::path settingsfile;
	std::filesystem::path pythonexec;
	std::vector<std::filesystem::path> searchpaths;
	std::vector<std::filesystem::path> playlistsfile;

	ParseAndProcessArgsDesc_t(std::filesystem::path exec_file);
	boost::program_options::options_description GetOptionsDescription();
	void Parse(std::span<const char * const> args);
};
#pragma endregion

#pragma region parse node database
void ParseNodeDatabase(const std::filesystem::path& intermidiatefile, decltype(Configurinator::Model::NodeDB)& db);
#pragma endregion

#pragma region build node tree & playlists from parsed database
void BuildNodeTree(const decltype(Configurinator::Model::NodeDB)& db, decltype(Configurinator::Model::NodeTree.Root)& node_tree);
void BuildPlaylist(const std::filesystem::path& playlistfile, const decltype(Configurinator::Model::NodeDB) &db, decltype(Configurinator::Model::ImportedPlaylists)::iterator playlist);
#pragma endregion

#pragma region parse settings
void ParseSettings(const std::filesystem::path& settingsfile, decltype(Configurinator::Model::settings)& settings);
#pragma endregion

/*
Parse Commandline argument [searchpaths(directories of .py files), playlistsfiles(json file), debugfile(raw txt), logfile(raw txt), pythonexec]
1. [ ] Parse and process program arguments
1. [ ] Write and run py file to search all 'searchpaths' for runnable script functions, and overwrite output in itself, parse the output to build 'NodeDB'
1. [ ] Build 'NodeTree' from 'NodeDB'
1. [ ] Parse 'playlistsfiles' to build 'ImportedPlaylists' (Level-1 type NodeTree, more like collection of lists) structures with nodes from NodeDB
1. [ ] Create Read-Only file handle to 'debugfile' and 'logfile' to prevent deletion
1. [ ] Load up settings file (if exists) to override settings
*/

b8 ParseArguments(std::span<const char* const> args, Configurinator::Model& model_data)
{
	ParseAndProcessArgsDesc_t desc(std::filesystem::path(args.front()));
	desc.Parse(args);

	ParseNodeDatabase(desc.debugfile, model_data.NodeDB);

	BuildNodeTree(model_data.NodeDB, model_data.NodeTree.Root);
	for (auto &playlistfile: desc.playlistsfile) {
		model_data.ImportedPlaylists.emplace_back();
		BuildPlaylist(playlistfile, model_data.NodeDB, model_data.ImportedPlaylists.end() - 1);
	}

	namespace fs = std::filesystem;
	if (fs::exists(desc.settingsfile) && fs::is_regular_file(desc.settingsfile)) {
		ParseSettings(desc.settingsfile, model_data.settings);
	}
	return false;
}

#pragma region parse & process args
std::ostream &operator<<(std::ostream &os, const std::vector<std::filesystem::path> &paths) {
	for (auto &p: paths)
		os << p << ' ';
	return os;
}

ParseAndProcessArgsDesc_t::ParseAndProcessArgsDesc_t(std::filesystem::path exec_file)
	: debugfile(exec_file.replace_extension(".debug.txt"))
	  , logfile(exec_file.replace_extension(".log.txt"))
	  , settingsfile(exec_file.parent_path() / "settings.json")
	  , pythonexec("python")
	  , searchpaths{
		  [exec_file] {
			  ON_DEBUG(
				  return std::vector{ (std::filesystem::path(__FILE__) / "../../../../examples/scripts-dir") });
			  return std::vector{exec_file.parent_path()};
		  }()
	  }
	  , playlistsfile([exec_file] {
		  ON_DEBUG(
			  return std::vector{ (std::filesystem::path(__FILE__) /
				  "../../../../examples/example.playlist.json") });
		  for (auto &p: std::filesystem::directory_iterator(exec_file.parent_path())) {
			  if (p.is_regular_file() && p.path().string().ends_with(".playlist.json"))
				  return std::vector{p.path()};
		  }
	  }())
{}

boost::program_options::options_description ParseAndProcessArgsDesc_t::GetOptionsDescription() {
	namespace po = boost::program_options;

	po::options_description desc("Options");
	desc.add_options()
		("help", "produce help message")
		("debugfile,d", po::value<std::string>()->default_value(debugfile.string()), "set debugfile")
		("logfile,l", po::value<std::string>()->default_value(logfile.string()), "set logfile")
		("settingsfile", po::value<std::string>()->default_value(settingsfile.string()), "set settingsfile")
		("pythonexec", po::value<std::string>()->default_value(pythonexec.string()), "set pythonexec")
		("searchpath,s", po::value<std::vector<std::string> >()->multitoken()->default_value(
			 [searchpaths = searchpaths] {
				 std::vector<std::string> paths;
				 for (auto &p: searchpaths)
					 paths.push_back(p.string());
				 return paths;
			}()), "set searchpaths")
		("playlistfile,p", po::value<std::vector<std::string> >()->multitoken()->default_value(
			 [playlistsfile = playlistsfile] {
				 std::vector<std::string> paths;
				 for (auto &p: playlistsfile)
					 paths.push_back(p.string());
				 return paths;
			}()), "set playlistsfile");
	return desc;
}

void ParseAndProcessArgsDesc_t::Parse(std::span<const char * const> args) {
	namespace po = boost::program_options;
	po::variables_map vm;
	po::store(po::parse_command_line(args.size(), args.data(), GetOptionsDescription()), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << GetOptionsDescription() << "\n";
		exit(0);
	}

	debugfile = vm["debugfile"].as<std::string>();
	logfile = vm["logfile"].as<std::string>();
	pythonexec = vm["pythonexec"].as<std::string>();
	searchpaths.clear();
	for (auto &p: vm["searchpaths"].as<std::vector<std::string> >()) {
		searchpaths.push_back(p);
	}
	playlistsfile.clear();
	for (auto &p: vm["playlistsfile"].as<std::vector<std::string> >()) {
		playlistsfile.push_back(p);
	}
}
#pragma endregion
#pragma region parse node database
void ParseNodeDatabase(const std::filesystem::path& intermidiatefile, decltype(Configurinator::Model::NodeDB)& db)
{
	// Parse the debugfile and write to db
}
#pragma endregion
#pragma region build node tree & playlists from parsed database
void BuildNodeTree(const decltype(Configurinator::Model::NodeDB)& db, decltype(Configurinator::Model::NodeTree.Root)& node_tree)
{
	// Build the node tree from db
}
void BuildPlaylist(const std::filesystem::path& playlistfile, const decltype(Configurinator::Model::NodeDB) &db, decltype(Configurinator::Model::ImportedPlaylists)::iterator playlist)
{
	// Build the playlist from db
}
#pragma endregion
#pragma region parse settings
void ParseSettings(const std::filesystem::path& settingsfile, decltype(Configurinator::Model::settings)& settings)
{
	// Parse the settings file and write to settings
}
#pragma endregion

/* // EXAMPLE PLAYLIST FILE TO PARSE
[
	{
		"script": "/path/to/file.py",
		"function": "FunctionName",
		"overrides": {
			"ParameterName": "value"
		}
	}
]
*/
/* # EXAMPLE SCRIPT FILE TO PARSE:
import script_defines

@run_elevated
def ElevatedFunctionEvecuted():
	print("ElevatedFunctionEvecuted")
	return 0

@run_non_elevated
def NonElevatedFunctionEvecuted():
	print("NonElevatedFunctionEvecuted")
	return 0
*/
/* // Dumped Output from SCRIPT FILE
{
	"script": "/path/to/file.py": {
		"FunctionName": {
			"params": [
				{ "name": "a", "type": "<class 'type'>", "default": "value" }
			]
			"elevated": true
			"return": "<class 'type'>"
		}
	}
}
*/
std::string_view PythonScript_ParseScriptFiles_To_SelfFile()
{
	return R"(# coding=utf-8
import sys, os

output_file = sys.argv[0]
scripts_path = sys.argv[1:]

# Recursively search for all .py files in the scripts_path
script_files = []
for path in scripts_path:
	for root, dirs, files in os.walk(path):
		for file in files:
			if file.endswith(".py"):
				script_files.append(os.path.join(root, file))

# Write the script functions of files to the output file
from importlib import util, machinery
from inspect import signature
def write_script_functions_to_file(script_file, dict_obj):
	loader = machinery.SourceFileLoader("script", script_file)
	spec = util.spec_from_loader(loader.name, loader)
	module = util.module_from_spec(spec)
	loader.exec_module(module)

	def check_function_is_script_type(func_qualname):
		return func_qualname.startswith("run_") and func_qualname.endswith(".<locals>.wrapper")
	def check_function_requires_elevation(func_qualname):
		return func_qualname.startswith("run_elevated")

	functions = {}
	for name, func in module.__dict__.items():
		if callable(func) and check_function_is_script_type(func.__qualname__):
			sig = signature(func)
			params = []
			for p in sig.parameters.values():
				params.append({'name': p.name})
				if p.default is not p.empty:
					params[-1]['default'] = str(p.default)
					params[-1]['type'] = str(type(p.default))
				if p.annotation is not p.empty:
					params[-1]['type'] = str(p.annotation)
			functions[name] = {'params': params, 'elevated': check_function_requires_elevation(func.__qualname__)}
			if sig.return_annotation is not sig.empty:
				functions[name]['return'] = str(sig.return_annotation)

	if len(functions) > 0:
		dict_obj[str(script_file)] = functions

import json
with open(output_file, "w") as file:
	dict_obj = {}
	for script_file in script_files:
		write_script_functions_to_file(script_file, dict_obj)
	file.write(json.dumps(dict_obj, sort_keys=True, indent=4))
)";
}

END_NAMESPACE
