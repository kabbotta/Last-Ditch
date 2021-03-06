#include "PhysicsSystem.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <algorithm>
#include "../Debug.h"

using namespace std;
using namespace ld;
using namespace osg;

PhysicsSystem::PhysicsSystem(
  Input& input_,
  EntitySystem& entity_system_,
  MapSystem& map_system_
)
  : tile_radius(TILE_SIZE / 4),
    input(input_),
    entity_system(entity_system_),
    map_system(map_system_)
{
  printf("Physics System ready\n");
}


void PhysicsSystem::update(double dt)
{
  auto& user = entity_system.get_user("kadijah");

  simulate(user, dt);
}


void PhysicsSystem::simulate(DynamicEntity& user, double dt)
{
  Quat user_heading(user.heading, Vec3(0, 0, 1));

  if (user.inactive_time > 0.0)
  {
    user.inactive_time -= .05;

    if (user.inactive_time < 0.0)
    {
      user.inactive_time = 0.0;
      user.position = cosine_interp(user.start, user.target, 1);
    }
    else
      user.position = cosine_interp(user.start, user.target, 1 - user.inactive_time);
  }
  else
  {
    Vec3 direction;
    if (input.forward) direction += Vec3(0, -1, 0);
    if (input.backward) direction += Vec3(0, 1, 0);
    if (input.left) direction += Vec3(1, 0, 0);
    if (input.right) direction += Vec3(-1, 0, 0);

    direction.normalize();

    Vec3 velocity(user_heading * direction * user.speed);

    user.position += velocity * dt;
  }

  if (user.collision_active) scan_collisions(user);

  Matrix r, t;
  r.makeRotate(user_heading);
  t.makeTranslate(
    Vec3(
      user.position.x() * TILE_SIZE,
      user.position.y() * TILE_SIZE,
      user.position.z() * FLOOR_HEIGHT));

  user.matrix = r * t;
}


void PhysicsSystem::scan_collisions(DynamicEntity& user)
{
  auto floor = (int)std::floor(user.position.z());

  if (floor < 0 || floor >= NUM_FLOORS) return;

  auto px = (int)std::round(user.position.x());
  auto py = (int)std::round(user.position.y());

  for (auto x = px - 1; x <= px + 1; ++x)
    for (auto y = py - 1; y <= py + 1; ++y)
      if (map_system.get_tile(x, y, floor).solid)
	resolve_collision(user, x, y);
}


void PhysicsSystem::resolve_collision(DynamicEntity& user, int x, int y)
{
  Vec2d tile_pos(x, y);
  Vec2d user_pos(user.position.x(), user.position.y());
  Vec2d min(x - tile_radius, y - tile_radius);
  Vec2d max(x + tile_radius, y + tile_radius);
  Vec2d nearest(user_pos);

  if (nearest.x() < min.x()) nearest.x() = min.x();
  else if (nearest.x() > max.x()) nearest.x() = max.x();

  if (nearest.y() < min.y()) nearest.y() = min.y();
  else if (nearest.y() > max.y()) nearest.y() = max.y();

  Vec2d norm(user_pos - nearest);
  auto dist = norm.normalize();
  auto depth = USER_RADIUS - dist;

  if (depth > 0) user.position += Vec3d(norm.x(), norm.y(), 0) * depth;
}


double PhysicsSystem::cosine_interp(double v1, double v2, double t_)
{
  auto t = (1 - cos(M_PI * t_)) / 2;

  return (v1 * (1 - t) + v2 * t);
}


Vec3d PhysicsSystem::cosine_interp(Vec3 v1, Vec3 v2, double t)
{
  auto x = cosine_interp(v1.x(), v2.x(), t);
  auto y = cosine_interp(v1.y(), v2.y(), t);
  auto z = cosine_interp(v1.z(), v2.z(), t);

  return {x, y, z};
}
