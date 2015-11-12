#include "CameraSystem.h"

#include <iostream>
#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventHandler>
#include <osgViewer/ViewerEventHandlers>
#include "../Constants.h"
#include "../InputAdapter.h"
#include "../components/DynamicEntity.h"

using namespace ld;
using namespace osg;

CameraSystem::CameraSystem(
  osg::ref_ptr<osg::Group> root,
  Input& input, EntitySystem& entity_system_
)
  : running(true),
    active_cursor(true),
    entity_system(entity_system_),
    viewer()
{
  viewer.setSceneData(root);
  viewer.addEventHandler(new InputAdapter(input, entity_system, *this));

  osgViewer::StatsHandler* stats_handler = new osgViewer::StatsHandler;
  stats_handler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_O);
  viewer.addEventHandler(stats_handler);

  viewer.getCamera()->setProjectionMatrixAsPerspective(40, 1, .1, 100);

  viewer.realize();
}


void CameraSystem::update()
{
  if (viewer.done())
  {
    running = false;
    return;
  }

  DynamicEntity& user = entity_system.get_user("kadijah");

  Vec3 pos(user.position + Vec3(0, 0, 1.4));
  Vec3 dir(-sin(user.heading), cos(user.heading), .2);

  viewer.getCamera()->setViewMatrixAsLookAt(pos + dir, pos, Vec3(0, 0, 1));

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
