#pragma once

#include <MetaStuff/Meta.h>

#include "Utilities/Vector.h"

namespace meta
{
	template <>
	inline auto registerMembers<SVector2f>()
	{
		return members(
			member("X", &SVector2f::X),
			member("Y", &SVector2f::Y)
		);
	}

	template <>
	inline auto registerMembers<SVector2i>()
	{
		return members(
			member("X", &SVector2i::X),
			member("Y", &SVector2i::Y)
		);
	}

	template <>
	inline auto registerMembers<SVector2u>()
	{
		return members(
			member("X", &SVector2u::X),
			member("Y", &SVector2u::Y)
		);
	}

	template <>
	inline auto registerMembers<SVector3f>()
	{
		return members(
			member("X", &SVector3f::X),
			member("Y", &SVector3f::Y),
			member("Z", &SVector3f::Z)
		);
	}

	template <>
	inline auto registerMembers<SVector3i>()
	{
		return members(
			member("X", &SVector3i::X),
			member("Y", &SVector3i::Y),
			member("Z", &SVector3i::Z)
		);
	}

	template <>
	inline auto registerMembers<SVector3u>()
	{
		return members(
			member("X", &SVector3u::X),
			member("Y", &SVector3u::Y),
			member("Z", &SVector3u::Z)
		);
	}

	template <>
	inline auto registerMembers<SVector4f>()
	{
		return members(
			member("X", &SVector4f::X),
			member("Y", &SVector4f::Y),
			member("Z", &SVector4f::Z),
			member("W", &SVector4f::W)
		);
	}

	template <>
	inline auto registerMembers<SVector4i>()
	{
		return members(
			member("X", &SVector4i::X),
			member("Y", &SVector4i::Y),
			member("Z", &SVector4i::Z),
			member("W", &SVector4i::W)
		);
	}

	template <>
	inline auto registerMembers<SVector4u>()
	{
		return members(
			member("X", &SVector4u::X),
			member("Y", &SVector4u::Y),
			member("Z", &SVector4u::Z),
			member("W", &SVector4u::W)
		);
	}
}
