#include <set>
#include <gmsh.h>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

auto Tor (float R, float r, int N = 100, float lc = 1e-1){
    std::vector <int> Center_of_circle;;
    std::vector <int> First_point_of_circle;
    std::vector <int> Second_point_of_circle;

    for (auto i = 0; i < N; ++i){
        float z = r * sin(i * 2 * M_PI / N);
        float y = r * cos(i * 2 * M_PI / N);
        Center_of_circle.push_back(gmsh::model::geo::addPoint(0, 0, z, lc));
        First_point_of_circle.push_back(gmsh::model::geo::addPoint(0, R + y, z, lc));
        Second_point_of_circle.push_back(gmsh::model::geo::addPoint(0, - R - y, z, lc));
      }
    std::vector <int> mass_of_circles_first;
    std::vector <int> mass_of_circles_second;
    std::vector <int> Arc1;
    std::vector <int> Arc2;
    int CenterPoint1 = gmsh::model::geo::addPoint(0, R, 0);
    int CenterPoint2 = gmsh::model::geo::addPoint(0, - R, 0);

    for (auto i = 0; i < N; ++ i){
        mass_of_circles_first.push_back(gmsh::model::geo::addCircleArc(First_point_of_circle[i], Center_of_circle[i], Second_point_of_circle[i]));
        mass_of_circles_second.push_back(gmsh::model::geo::addCircleArc(Second_point_of_circle[i], Center_of_circle[i], First_point_of_circle[i]));
        Arc1.push_back(gmsh::model::geo::addCircleArc(First_point_of_circle[i % N], CenterPoint1, First_point_of_circle[(i + 1) % N]));
        Arc2.push_back(gmsh::model::geo::addCircleArc(Second_point_of_circle[i % N], CenterPoint2, Second_point_of_circle[(i + 1) % N]));
      }
    std::vector <int> Pattern_first;
    std::vector <int> Pattern_second;
    for (auto i = 0; i < N; ++i){
        Pattern_first.push_back(
            gmsh::model::geo::addCurveLoop({mass_of_circles_first[i % N], Arc2[i % N], - mass_of_circles_first[(i + 1) % N], - Arc1[i % N]}));
        Pattern_second.push_back(
            gmsh::model::geo::addCurveLoop({mass_of_circles_second[i % N], Arc1[i % N], - mass_of_circles_second[(i + 1) % N], - Arc2[i % N]}));
        }
    std::vector <int> Planes;
    for (auto i: Pattern_first){
        Planes.push_back(gmsh::model::geo::addPlaneSurface({i}));
      }
    for (auto i: Pattern_second){
        Planes.push_back(gmsh::model::geo::addPlaneSurface({i}));
      }
    return Planes;
}

int main(int argc, char **argv){

  gmsh::initialize();

  gmsh::model::add("tor");

  std::vector <int> Inside = Tor(5, 0.9);
  std::vector <int> Outside = Tor(5, 1);

  gmsh::model::geo::addSurfaceLoop(Inside, 1);
  gmsh::model::geo::addSurfaceLoop(Outside, 2);
  gmsh::model::geo::addVolume({1, -2});

  gmsh::model::geo::synchronize();

  gmsh::model::mesh::generate(3);

  gmsh::write("tor.msh");

  std::set<std::string> args(argv, argv + argc);
  if(!args.count("-nopopup")) gmsh::fltk::run();

  gmsh::finalize();

  return 0;
}
