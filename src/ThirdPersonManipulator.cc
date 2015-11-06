#include "ThirdPersonManipulator.h"

#include <iostream>
#include "systems/CameraSystem.h"

using namespace ld;

ThirdPersonManipulator::ThirdPersonManipulator(
  CameraSystem& camera_system_,
  osg::ref_ptr<osg::PositionAttitudeTransform> user_xform_
)
  : osgGA::CameraManipulator(),
    user_xform(user_xform_),
    matrix(),
    mouse_center(0, 0),
    camera_system(camera_system_)
{
  using namespace osg;

  base_rotation.makeRotate(M_PI / 2, Vec3(1, 0, 0));
  base_translate.makeTranslate(Vec3(0, -2.3, 1.4));

  Matrix t;
  t.makeTranslate(user_xform->getPosition());

  matrix = base_rotation * base_translate * t;
}


void ThirdPersonManipulator::setByMatrix(const osg::Matrixd& matrix_)
{
  matrix = matrix_;
}


void ThirdPersonManipulator::setByInverseMatrix(const osg::Matrixd& matrix_)
{
  matrix = osg::Matrix::inverse(matrix_);
}


osg::Matrixd ThirdPersonManipulator::getMatrix() const
{
  return matrix;
}


osg::Matrixd ThirdPersonManipulator::getInverseMatrix() const
{
  return osg::Matrix::inverse(matrix);
}


bool ThirdPersonManipulator::handle(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  switch (ea.getEventType())
  {
  case osgGA::GUIEventAdapter::FRAME:
    return handle_frame(ea, aa);
  case osgGA::GUIEventAdapter::MOVE:
    return handle_mouse_move(ea, aa);
  case osgGA::GUIEventAdapter::KEYDOWN:
    return handle_key_down(ea, aa);
  default:
    return false;
  }
}


bool ThirdPersonManipulator::handle_frame(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  using namespace osg;

  update_matrix();

  return false;
}


bool ThirdPersonManipulator::handle_mouse_move(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  if (camera_system.has_active_cursor())
    return false;
  else
    return handle_mouse_delta_movement(ea, aa);
}


bool ThirdPersonManipulator::handle_key_down(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  return false;
}


bool ThirdPersonManipulator::handle_mouse_delta_movement(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
  float dx = ea.getX() - mouse_center.x();
  float dy = ea.getY() - mouse_center.y();

  if (dx == 0 && dy == 0) return false;

  center_mouse_pointer(ea, aa);

  return perform_mouse_delta_movement(dx, dy);
}


bool ThirdPersonManipulator::perform_mouse_delta_movement(
  const float dx, const float dy)
{
  using namespace osg;

  Matrix m;
  m.makeRotate(dx * .001, Vec3(0, 0, 1));

  matrix *= m;

  return true;
}


void ThirdPersonManipulator::center_mouse_pointer(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
  mouse_center.set(
    (ea.getXmin() + ea.getXmax()) / 2.0f,
    (ea.getYmin() + ea.getYmax()) / 2.0f);

  aa.requestWarpPointer(mouse_center.x(), mouse_center.y());
}


void ThirdPersonManipulator::update_matrix()
{
  using namespace osg;

  Matrix t;
  t.makeTranslate(user_xform->getPosition());

  matrix = base_rotation * base_translate * t;
}


void ThirdPersonManipulator::setNode(osg::Node* node_)
{
  node = node_;
}


osg::Node* ThirdPersonManipulator::getNode()
{
  return node;
}


const osg::Node* ThirdPersonManipulator::getNode() const
{
  return node;
}
