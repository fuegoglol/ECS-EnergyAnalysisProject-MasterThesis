#pragma once

struct FSpaceship
{
	int32 Id;
	float BaseTranslationSpeed;
	float BaseRotationSpeed;
};

struct FSpaceshipTransform
{
	FVector_NetQuantize10 Location;
	FRotator Rotation;
};

struct FSpaceshipAI
{
	double Seed;
};

struct FWeapon
{
	float FireDelay;
	float FireCooldown = 5;
};

struct FBullet
{
	FVector_NetQuantize10 Location;
	FVector ForwardVector;
	double Lifetime = 10;
};
