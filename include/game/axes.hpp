#pragma once

// Enum representing the available axes
enum class Axes {
	X,
	Y,
	L,
	R
};

constexpr Axes ALL_AXES[] = {Axes::X, Axes::Y, Axes::L, Axes::R};
#define NUM_AXES (sizeof(ALL_AXES) / sizeof(ALL_AXES[0]))



