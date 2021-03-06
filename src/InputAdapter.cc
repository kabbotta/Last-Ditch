#include "InputAdapter.h"

#include <iostream>
#include "systems/MapSystem.h"

using namespace ld;
using namespace osg;


bool InputAdapter::handle(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  switch(ea.getEventType())
  {
  case osgGA::GUIEventAdapter::PUSH:
    return handle_mouse_click(ea, aa);
  case osgGA::GUIEventAdapter::MOVE:
    return handle_mouse_move(ea, aa);
  case osgGA::GUIEventAdapter::KEYDOWN:
    return handle_key_down(ea, aa);
  case osgGA::GUIEventAdapter::KEYUP:
    return handle_key_up(ea, aa);
  default:
    return false;
  }
}


bool InputAdapter::handle_mouse_click(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  switch (ea.getButtonMask())
  {
  case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
  {
    if (camera_system.has_active_cursor())
    {
      camera_system.show_cursor(false);
      center_mouse(ea, aa);
    }
    else
      camera_system.show_cursor(true);

    return false;
  }
  default:
    return false;
  }
}


bool InputAdapter::handle_mouse_move(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  if (camera_system.has_active_cursor())
    return false;
  else
    return handle_mouse_delta(ea, aa);
}


bool InputAdapter::handle_key_down(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  switch (ea.getKey())
  {
  case 'w':
    input.forward = true; return false;
  case 'a':
    input.left = true; return false;
  case 's':
    input.backward = true; return false;
  case 'd':
    input.right = true; return false;
  case 'f':
    input.use = true; return false;
  case 'e':
  {
    auto& user = entity_system.get_user("kadijah");

    user.inactive_time = 1.0;
    user.start = user.position;
    user.target = user.start + Vec3(0, 0, 1);

    return false;
  }
  case 'q':
  {
    auto& user = entity_system.get_user("kadijah");

    user.inactive_time = 1.0;
    user.start = user.position;
    user.target = user.start - Vec3(0, 0, 1);

    return false;
  }
  default:
    return false;
  }
}


bool InputAdapter::handle_key_up(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  switch (ea.getKey())
  {
  case 'w':
    input.forward = false; return false;
  case 'a':
    input.left = false; return false;
  case 's':
    input.backward = false; return false;
  case 'd':
    input.right = false; return false;
  default:
    return false;
  }
}


bool InputAdapter::handle_mouse_delta(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  auto dx = ea.getX() - mouse_center.x();
  auto dy = ea.getY() - mouse_center.y();

  if (dx == 0 && dy == 0) return false;

  center_mouse(ea, aa);

  auto& user = entity_system.get_user("kadijah");
  user.heading -= user.x_rot_speed * dx;
  user.pitch -= user.y_rot_speed * dy;

  const double max_pitch = inDegrees(89.f);

  if (user.pitch > max_pitch) user.pitch = max_pitch;
  else if (user.pitch < -max_pitch) user.pitch = -max_pitch;

  return false;
}


void InputAdapter::center_mouse(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  mouse_center.set(
    (ea.getXmin() + ea.getXmax()) / 2.f,
    (ea.getYmin() + ea.getYmax()) / 2.f);

  aa.requestWarpPointer(mouse_center.x(), mouse_center.y());
}
