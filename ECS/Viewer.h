#pragma once
#include <tuple>

namespace ECS {

	template<typename... T>
	struct Viewer
	{
	public:


		std::tuple<T &...> item_;

	private:
	};
}