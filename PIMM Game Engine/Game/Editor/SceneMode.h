#pragma once
#include <PIMM/Math/Vec3.h>

namespace pimm
{
	enum class SceneMode
	{
		Edit = 0,
		Play,
		Pause
	};

	struct TransformSnapshot
	{
		Vec3 position;
		Vec3 rotation;
		Vec3 scale;
	};
}
