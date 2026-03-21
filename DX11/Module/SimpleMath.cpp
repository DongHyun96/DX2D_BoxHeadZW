#include "pch.h"
#include "SimpleMath.h"

const SimpleMath::Vector2 SimpleMath::Vector2::Zero  = {0.f, 0.f};
const SimpleMath::Vector2 SimpleMath::Vector2::One   = {1.f, 1.f};
const SimpleMath::Vector2 SimpleMath::Vector2::UnitX = {1.f, 0.f};
const SimpleMath::Vector2 SimpleMath::Vector2::UnitY = {0.f, 1.f};

const SimpleMath::Vector3 SimpleMath::Vector3::Zero    = {0.f, 0.f, 0.f};           
const SimpleMath::Vector3 SimpleMath::Vector3::One     = {1.f, 1.f, 1.f};
const SimpleMath::Vector3 SimpleMath::Vector3::UnitX   = {1.f, 0.f, 0.f};
const SimpleMath::Vector3 SimpleMath::Vector3::UnitY   = {0.f, 1.f, 0.f};
const SimpleMath::Vector3 SimpleMath::Vector3::UnitZ   = {0.f, 0.f, 1.f};
const SimpleMath::Vector3 SimpleMath::Vector3::Up      = {0.f, 1.f, 0.f};
const SimpleMath::Vector3 SimpleMath::Vector3::Down    = {0.f, -1.f, 0.f};
const SimpleMath::Vector3 SimpleMath::Vector3::Right   = {1.f, 0.f, 0.f};
const SimpleMath::Vector3 SimpleMath::Vector3::Left    = {-1.f, 0.f, 0.f};
const SimpleMath::Vector3 SimpleMath::Vector3::Forward = {0.f, 0.f, 1.f};
const SimpleMath::Vector3 SimpleMath::Vector3::Front   = {0.f, 0.f, 1.f};
const SimpleMath::Vector3 SimpleMath::Vector3::Backward= {0.f, 0.f, -1.f};

