#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
  // TODO (Part 3): Handle collisions with spheres.
  Vector3D l = pm.position - origin;
  if (l.norm() - radius < 0) { // distance between length from sphere's origin to point mass and surface (test within sphere)
    Vector3D tan = origin + radius * l.unit(); // tangent point on sphere
    Vector3D correct = tan - pm.last_position;
    pm.position = pm.last_position + (1 - friction) * correct;
  }
}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}
