#include "LastDitch.h"

#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>
#include "src/Constants.h"

using namespace ld;

LastDitch::LastDitch()
  : root(new osg::Group),
    input(),
    map_system(),
    render_system(root, map_system),
    physics_system(input, render_system),
    camera_system(root, render_system, input)
{
  while (camera_system.is_running())
  {
    physics_system.update();
    camera_system.update();
  }
}


int main()
{
  LastDitch app;
}
