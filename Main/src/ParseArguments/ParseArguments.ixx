import Configurinator.Model;

import <boost/program_options/options_description.hpp>;
import <filesystem>;
import <iostream>;
import <fstream>;
import <nlohmann/json.hpp>;

export import <span>;

#include <include.h>
#include <fmt/core.h>

export module Configurinator.ParseArguments;
import :ParseAndProcessArgs;

namespace std {
    using namespace std::filesystem;

    ostream &operator<<(ostream &os, const path &p) {
        return os << p.string();
    }

    ostream &operator<<(ostream &os, const vector<path> &p) {
        for (auto &i: p)
            os << i << " ";
        return os;
    }

    ostream &operator<<(ostream &os, const vector<string> &p) {
        for (auto &i: p)
            os << i << " ";
        return os;
    }

    ostream &operator<<(ostream &os, const vector<string_view> &p) {
        for (auto &i: p)
            os << i << " ";
        return os;
    }
}

BEGIN_NAMESPACE
    export [[nodiscard]]
    b8 ParseArguments(std::span<const char * const> args, Configurinator::Model &model_data);

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
    void ParseNodeDatabase(const std::filesystem::path &intermidiatefile, const ::std::filesystem::path &pythonexec,
                           const std::span<std::filesystem::path> searchpaths,
                           decltype(Configurinator::Model::NodeDB) &db);
#pragma endregion

#pragma region build node tree & playlists from parsed database
    void BuildNodeTree(const decltype(Configurinator::Model::NodeDB) &db,
                       decltype(Configurinator::Model::NodeTree.Root) &node_tree);

    void BuildPlaylist(const std::filesystem::path &playlistfile, const decltype(Configurinator::Model::NodeDB) &db,
                       decltype(Configurinator::Model::ImportedPlaylists)::iterator playlist);
#pragma endregion

#pragma region parse settings
    void ParseSettings(const std::filesystem::path &settingsfile, decltype(Configurinator::Model::settings) &settings);
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

    b8 ParseArguments(std::span<const char * const> args, Configurinator::Model &model_data) {
        ParseAndProcessArgsDesc_t desc(std::filesystem::path(args.front()));
        desc.Parse(args);

        ParseNodeDatabase(desc.debugfile, desc.pythonexec, desc.searchpaths, model_data.NodeDB);

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
    ParseAndProcessArgsDesc_t::ParseAndProcessArgsDesc_t(std::filesystem::path exec_file)
        : debugfile(exec_file.replace_extension(".debug.txt"))
          , logfile(exec_file.replace_extension(".log.txt"))
          , settingsfile(exec_file.parent_path() / "settings.json")
          , pythonexec("python")
          , searchpaths{
              [exec_file] {
                  namespace fs = std::filesystem;
                  ON_DEBUG(
                      return std::vector{ fs::absolute(fs::path(__FILE__) / "../../../../examples/scripts-dir") });
                  return std::vector{exec_file.parent_path()};
              }()
          }
          , playlistsfile{
              [exec_file] {
                  namespace fs = std::filesystem;
                  ON_DEBUG(
                      return std::vector{ fs::absolute(fs::path(__FILE__) / "../../../../examples/ex.playlist.json") });
                  std::vector<fs::path> paths;
                  for (auto &p: fs::directory_iterator(exec_file.parent_path())) {
                      if (p.is_regular_file() && p.path().string().ends_with(".playlist.json"))
                          paths.push_back(p.path());
                  }
                  return paths;
              }()
          } {
    }

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

        namespace fs = std::filesystem;
        debugfile = fs::absolute(fs::path(vm["debugfile"].as<std::string>())).native();
        logfile = fs::absolute(fs::path(vm["logfile"].as<std::string>())).native();
        pythonexec = fs::path(vm["pythonexec"].as<std::string>());

        searchpaths.clear();
        for (auto &p: vm["searchpath"].as<std::vector<std::string> >()) {
            searchpaths.emplace_back(fs::absolute(p).native());
        }
        playlistsfile.clear();
        for (auto &p: vm["playlistfile"].as<std::vector<std::string> >()) {
            playlistsfile.emplace_back(fs::absolute(p).native());
        }
    }
#pragma endregion

#pragma region parse node database
    std::string_view PythonScript_ParseScriptFiles_To_SelfFile() {
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

    void ParseNodeDatabase(const std::filesystem::path &intermidiatefile, const std::filesystem::path &pythonexec,
                           const std::span<std::filesystem::path> searchpaths,
                           decltype(Configurinator::Model::NodeDB) &db) { {
            std::ofstream scriptfile(intermidiatefile);
            scriptfile << PythonScript_ParseScriptFiles_To_SelfFile();
            scriptfile.close();
        } {
            std::string command = pythonexec.string() + " \"" + intermidiatefile.string() + "\"";
            for (auto &searchpath: searchpaths) {
                command += " \"" + searchpath.string() + "\"";
            }
            std::system(command.c_str());
        } {
            using json = nlohmann::json;
            std::ifstream file(intermidiatefile);
            auto j = json::parse(file);
            file.close();

            for (auto &&[key, value]: j.items()) {
                db[std::move(key)].reserve(value.size());
                for (auto &&[name, function]: value.items()) {
                    db[key].emplace_back();
                    auto &node = db[key].back();

                    node.Name = std::move(name);
                    node.Parameters.reserve(function["params"].size());
                    for (auto &&param: function["params"]) {
                        node.Parameters.emplace_back();
                        auto &parameter = node.Parameters.back();

                        parameter.Name = std::move(param["name"]);
                        if (param.contains("type")) {
                            parameter.Type = std::move(param["type"]);
                        }
                        if (param.contains("default")) {
                            parameter.DefaultValue = std::move(param["default"]);
                        }
                    }
                    node.RequiresElevation = function["elevated"].get<bool>();
                }
            }
        }
    }
#pragma endregion

#pragma region build node tree & playlists from parsed database
    void BuildNodeTree(const decltype(Configurinator::Model::NodeDB) &db,
                       decltype(Configurinator::Model::NodeTree.Root) &node_tree) {
        // Build the node tree from db

        // TODO: we can speed this by using co-routine and not constructing a vector.
        // somthing like std::string_view get_next_split(str, delim, start, end)
        auto split = [](const std::string_view str, const char delim) -> std::vector<std::string_view> {
            // count deliminators
            u32 delim_count = 0;
            for (const char c: str) {
                delim_count += (c == delim);
            }
            delim_count++;

            usize start_split_from = 0;
            usize end_split_on = -1;
            std::vector<std::string_view> splits;
            splits.reserve(delim_count + 1);
            for (usize i = 0; i < str.size(); ++i) {
                if (str[i] == delim) {
                    start_split_from = end_split_on;
                    end_split_on = i;
                    const auto idx = start_split_from + 1;
                    const auto len = end_split_on - idx;
                    splits.emplace_back(str.data() + idx, len);
                }
            }
            const auto idx = end_split_on + 1;
            const auto len = str.size() - idx;
            splits.emplace_back(str.data() + idx, len);
            return splits;
        };

        for (auto script_node_ref = db.begin(); script_node_ref != db.end(); script_node_ref++) {
            const auto &script_node = *script_node_ref;
            namespace fs = std::filesystem;

            const auto splits = split(script_node.first, fs::path::preferred_separator);

            auto *dir_node = &node_tree;
            for (usize depth = 0; depth < splits.size() - 1; ++depth) {
                auto [itr, success] = dir_node->Children.emplace(splits[depth]);
                dir_node = const_cast<decltype(dir_node)>(&*itr);
            }
            dir_node->ScriptNode.push_back(script_node_ref);
        }

        namespace fs = std::filesystem;
        // collapse the tree for single children and no script nodes

        struct {
            void operator()(std::remove_cvref_t<decltype(node_tree)> &node) {
                while (node.Children.size() == 1 && node.ScriptNode.empty()) {
                    auto child = const_cast<decltype(node)>(*node.Children.begin());
                    node.Name += decltype(node.Name)::value_type(fs::path::preferred_separator) + child.Name;
                    node.Children = std::move(child.Children);
                    node.ScriptNode = std::move(child.ScriptNode);
                }
                for (auto itr = node.Children.begin(); itr != node.Children.end(); ++itr) {
                    this->operator()(const_cast<decltype(node)>(*itr));
                }
            }
        } collapse;
        collapse(node_tree);
    }

    void BuildPlaylist(const std::filesystem::path &playlistfile, const decltype(Configurinator::Model::NodeDB) &db,
                       decltype(Configurinator::Model::ImportedPlaylists)::iterator playlist) {
        // Build the playlist from db
    }
#pragma endregion

#pragma region parse settings
    void ParseSettings(const std::filesystem::path &settingsfile, decltype(Configurinator::Model::settings) &settings) {
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


END_NAMESPACE
