#include <include.h>

import <string>;
import <vector>;
import <filesystem>;
#include <unordered_set>;
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

	struct Hash {
		usize operator()(const FunctionNode &node) const {
			return std::hash<decltype(node.Name)>()(node.Name);
		}
	};
	b8 operator==(const FunctionNode &oth) const {
		return Name == oth.Name && RequiresElevation == oth.RequiresElevation;
	}
};

using FunctionNodes = std::unordered_set<FunctionNode, FunctionNode::Hash>;
using NodeDB = std::unordered_map<std::string, FunctionNodes>;

struct NodeReference
{
	NodeDB::const_iterator Node;
	FunctionNodes::const_iterator Function;
	std::vector<std::pair<std::string_view, std::string>> ParameterOverrides;
};

struct Playlist
{
	std::string Name;
	std::vector<NodeReference> Nodes;
};

export struct DirectoryNode
{
	struct Hash {
		usize operator()(const DirectoryNode &node) const {
			return std::hash<decltype(node.Name)>()(node.Name);
		}
	};

	b8 operator==(const DirectoryNode &oth) const {
		return Name == oth.Name && ScriptNode == oth.ScriptNode && Children == oth.Children;
	}

	DirectoryNode(std::string_view name) : Name(name) {}

	std::string Name;
	std::vector<NodeDB::const_iterator> ScriptNode;
	std::unordered_set<DirectoryNode, Hash> Children;
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
		DirectoryNode Root { "All" };
	} NodeTree;

	std::vector<Playlist> ImportedPlaylists;
	// TODO: Local Playlists on current machine.

	std::vector<StagedNodeReference> Staged;
};

END_NAMESPACE
