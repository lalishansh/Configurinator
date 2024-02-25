#pragma once

#include "common.h"
#include <type_traits>

template<typename FunctionToCallOnDestruct_t>
struct ExitFunctionConstructor {
	FunctionToCallOnDestruct_t fn;

	constexpr ExitFunctionConstructor(FunctionToCallOnDestruct_t&& in)
		: fn(in)
	{}
	constexpr ~ExitFunctionConstructor(){
		fn();
	}
};

enum class PipeExitFunctionToConstructor {};

template<typename ExitFunction_t>
constexpr auto operator | (PipeExitFunctionToConstructor _, ExitFunction_t&& fn){
	return ExitFunctionConstructor<ExitFunction_t>(std::forward<ExitFunction_t>(fn));
}

#define CONCAT_IMPL(a,b) a##b
#define CONCAT(a,b) CONCAT_IMPL(a,b)

#define DEFER \
    auto CONCAT(_defer_, __LINE__) = PipeExitFunctionToConstructor() | [&]() -> void