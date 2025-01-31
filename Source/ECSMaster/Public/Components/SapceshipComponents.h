#pragma once

struct FSpaceship
{
	int32 Id;
	float BaseTranslationSpeed;
	float BaseRotationSpeed;
};

struct FSpaceshipTransform
{
	FVector_NetQuantize Location;
	FRotator Rotation;
};

struct FSpaceshipAI
{
	double Seed;
};
