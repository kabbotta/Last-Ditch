#include "CameraSystem.h"

#include <iostream>
#include <osgGA/TrackballManipulator>
#include "../InputAdapter.h"
#include "../ThirdPersonManipulator.h"

using namespace ld;

CameraSystem::CameraSystem(osg::ref_ptr<osg::Group> root, Input& input_)
  : running(true),
    active_cursor(true),
    input(input_),
    viewer()
{
  using namespace osg;

  viewer.setSceneData(root);
  viewer.setCameraManipulator(new ThirdPersonManipulator(*this));
  viewer.addEventHandler(new InputAdapter(input, *this));
  viewer.getCamera()->setProjectionMatrixAsPerspective(40, 1, .1, 100);
  viewer.realize();
}


void CameraSystem::update()
{
  using namespace osg;

  if (viewer.done())
  {
    running = false;
    return;
  }

  viewer.frame();
}


void CameraSystem::toggle_cursor()
{
  show_cursor(!active_cursor);
}


void CameraSystem::show_cursor(bool show)
{
  osgViewer::Viewer::Windows windows;
  viewer.getWindows(windows);

  for (auto window : windows)
  {
    if (show)
    {
      window->setCursor(osgViewer::GraphicsWindow::InheritCursor);
      active_cursor = true;
    }
    else
    {
      window->setCursor(osgViewer::GraphicsWindow::NoCursor);
      active_cursor = false;
    }
  }
}
