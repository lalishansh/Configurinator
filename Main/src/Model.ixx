#include <include.h>

import <string>;
import <vector>;
import <filesystem>;
#include <unordered_map>;

export module Configurinator.Model;

BEGIN_NAMESPACE

struct Settings
{
	using Path_t = std::filesystem::path;
	struct {
		Path_t debugfile;
		Path_t logfile;
		Path_t pythonexec;
	} paths;
};

struct FunctionNode
{
	std::string Name;
	struct Parameter
	{
		std::string Name;
		std::string Type;
		std::string CurrentValue;
		std::string DefaultValue;
	};
	std::vector<Parameter> Parameters;
	u64 RequiresElevation : 1;
};

using NodeDB = std::unordered_map<std::string, std::vector<FunctionNode>>;

struct NodeReference
{
	NodeDB::iterator Node;
	std::string_view FunctionName;
	std::vector<std::pair<std::string_view, std::string>> ParameterOverrides;
};

struct Playlist
{
	std::string Name;
	std::vector<NodeReference> Nodes;
};

struct DirectoryNode
{
	std::string Name;
	std::vector<DirectoryNode> Children;
	std::vector<std::string_view> Scripts;
};


struct StagedNodeReference
{
	NodeReference node;
	enum class Status
	{
		QUEUED,
		RUNNING,
		FINISHED,
		FAILED
	} status;
};

export struct Model
{
	Settings settings;

	NodeDB NodeDB;

	struct
	{
		// TODO: Implement flattened tree structure with spans instead of vectors
		DirectoryNode Root{
			.Name = "All"
		};
	} NodeTree;

	std::vector<Playlist> ImportedPlaylists;
	// TODO: Local Playlists on current machine.

	std::vector<StagedNodeReference> Staged;
};

END_NAMESPACE
