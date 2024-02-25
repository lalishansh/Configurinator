export module Configurinator.ParseArguments;

export import Configurinator.Model;
export import <span>;

import <boost/program_options.hpp>;
import <nlohmann/json.hpp>;

import <filesystem>;
import <iostream>;

#include <include.h>

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

BEGIN_NAMESPACE

	[[nodiscard]]
export b8 ParseArguments(std::span<const char* const> args, Configurinator::Model& model_data);

boost::program_options::options_description GetProgramOptionsDescription(std::filesystem::path exec_path) {
	namespace po = boost::program_options;
	namespace fs = std::filesystem;

	po::options_description desc("Options");

	desc.add_options()
		("help,h", "Print help message")
		//("command,c", po::value<std::vector<std::string>>()->composing()->multitoken(), "Executes the specified commands")
		("searchpaths,s", po::value<std::vector<std::string>>()->composing()->multitoken()->default_value([exec_path] {
			ON_DEBUG(return std::vector{ (fs::path(__FILE__) / "../../../~scripts/").string()  });
			return std::vector{ exec_path.parent_path().string() };
		}()), "Search paths for python scripts")
		("playlistsfiles,p", po::value<std::vector<std::string>>()->composing()->multitoken(), "YAML file withs playlists")
		("debugfile,d", po::value<std::string>()->default_value(std::string(std::tmpnam(nullptr))), "Internal procesing debug file, defaults to temporary file")
		("logfile,l", po::value<std::string>()->default_value(std::string(std::tmpnam(nullptr))), "Log file, defaults to temporary file")
		("pythonexec,e", po::value<std::string>()->default_value("python"), "Python executable")
		//("noexit", "Don't exit and open UI after running commands")
	;
	return desc;
}

std::string_view PythonScript_ParseScriptFiles_To_SelfFile();

b8 ParseArguments(std::span<const char* const> args, Model& model_data)
{
	return false;
}

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