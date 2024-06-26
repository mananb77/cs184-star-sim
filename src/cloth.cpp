#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"
#include "pointMass.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
  double x, y, z;
  double dx = width / ((double) num_width_points - 1.0); // # of segments
  double dy = height / ((double) num_height_points - 1.0);
  // grid of masses
  for (int h = 0; h < num_height_points; h++) {
    for (int w = 0; w < num_width_points; w++) {
      if (orientation == HORIZONTAL) {
        x = w * dx;
        y = 1.0;
        z = h * dy;
      } else {
        x = w * dx;
        y = h * dy;
        z = (((double) rand() / RAND_MAX) * 2.0 - 1.0) / 1000.0;
      }

      Vector3D pos(x, y, z);
      point_masses.emplace_back(pos, false);
    }
  }
  // pinned masses
  for (int i = 0; i < pinned.size(); i++) {
    int j = pinned[i][0];
    int k = pinned[i][1];
    PointMass* pinnedPm = &point_masses[num_width_points * k + j];
    pinnedPm->pinned = true;
  }

  // structural springs
  for (int h = 0; h < num_height_points; h++) {
    for (int w = 0; w < num_width_points; w++) {
      PointMass* pm = &point_masses[num_width_points * h + w]; // current pm
      if (h == 0 && w == 0) { // top-left corner

      } else if (h == 0) { // first row
        PointMass* leftPm = &point_masses[num_width_points * h + (w - 1)]; 
        springs.emplace_back(pm, leftPm, STRUCTURAL);
      } else if (w == 0) { // not first row
        PointMass* topPm = &point_masses[num_width_points * (h - 1) + w]; 
        springs.emplace_back(pm, topPm, STRUCTURAL);
      } else {
        PointMass* leftPm = &point_masses[num_width_points * h + (w - 1)];
        springs.emplace_back(pm, leftPm, STRUCTURAL);
        PointMass* topPm = &point_masses[num_width_points * (h - 1) + w]; 
        springs.emplace_back(pm, topPm, STRUCTURAL);
      }
    }
  }
    
  // shearing springs using 4x4 grid
  for (int h = 1; h < num_height_points; h++) {
    for (int w = 1; w < num_width_points; w++) {
      PointMass* bottomRightPm = &point_masses[num_width_points * h + w]; // current pm
      PointMass* bottomLeftPm = &point_masses[num_width_points * h + (w - 1)]; 
      PointMass* topRightPm = &point_masses[num_width_points * (h - 1) + w]; 
      PointMass* topLeftPm = &point_masses[num_width_points * (h - 1) + (w - 1)]; 
      springs.emplace_back(bottomRightPm, topLeftPm, SHEARING);
      springs.emplace_back(bottomLeftPm, topRightPm, SHEARING);
    }
  }

  // Add bending springs
  for (int h = 0; h < num_height_points; h++) {
    for (int w = 0; w < num_width_points; w++) {
      PointMass* pm = &point_masses[num_width_points * h + w]; // current pm
      if (h == 0 && w == 0) { // top-left corner

      } else if (h - 2 < 0) { 
        if (w - 2 >= 0) {
          PointMass* leftPm = &point_masses[num_width_points * h + (w - 2)]; // two left pm
          springs.emplace_back(pm, leftPm, BENDING);
        }
      } else if (w - 2 < 0) { 
        if (h - 2 >= 0) {
          PointMass* topPm = &point_masses[num_width_points * (h - 2) + w]; // two left pm
          springs.emplace_back(pm, topPm, BENDING);
        }
      } else {
        PointMass* leftPm = &point_masses[num_width_points * h + (w - 2)]; // two left pm
        springs.emplace_back(pm, leftPm, BENDING);
        PointMass* topPm = &point_masses[num_width_points * (h - 2) + w]; // two left pm
        springs.emplace_back(pm, topPm, BENDING);
      }
    }
  }
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.
  for (int i = 0; i < point_masses.size(); i++) {
    PointMass* pm = &point_masses[i];
    for (int j = 0; j < external_accelerations.size(); j++) {
      pm->forces += mass * external_accelerations[j];
    }
  }

  for (int s = 0; s < springs.size(); s++) {
    Spring* sp = &springs[s];
    Vector3D pa = sp->pm_a->position;
    Vector3D pb = sp->pm_b->position;
    double l = sp->rest_length;

    // Hooke's Law on springs with spring constraint type
    if (cp->enable_structural_constraints) {
      sp->pm_a->forces += cp->ks * ((pa - pb).norm() - l) * (pb - pa).unit();
      sp->pm_b->forces += cp->ks * ((pa - pb).norm() - l) * (pa - pb).unit();
    } 
    if (cp->enable_shearing_constraints) {
      sp->pm_a->forces += cp->ks * ((pa - pb).norm() - l) * (pb - pa).unit();
      sp->pm_b->forces += cp->ks * ((pa - pb).norm() - l) * (pa - pb).unit();
    } 
    if (cp->enable_bending_constraints) {
      sp->pm_a->forces += cp->ks * ((pa - pb).norm() - l) * (pb - pa).unit();
      sp->pm_b->forces += cp->ks * ((pa - pb).norm() - l) * (pa - pb).unit();
    } 
  }

  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for (int i = 0; i < point_masses.size(); i++) {
    PointMass* pm = &point_masses[i];

    // only for non-pinned point masses
    if (!pm->pinned) {
      Vector3D currPos = pm->position;
      pm->position = pm->position + (1.0 - cp->damping / 100) * (pm->position - pm->last_position) + (pm->forces / mass) * (delta_t * delta_t);
      pm->last_position = currPos;
    }
    pm->forces = Vector3D(0); // reset to no forces

  }


  // TODO (Part 4): Handle self-collisions.
  build_spatial_map();
  for (int i = 0; i < point_masses.size(); i++) {
    PointMass* pm = &point_masses[i];
    self_collide(*pm, simulation_steps);
  }


  // TODO (Part 3): Handle collisions with other primitives.
  for (int i = 0; i < point_masses.size(); i++) {
    PointMass* pm = &point_masses[i];
    for (int k = 0; k < (*collision_objects).size(); k++) {
      CollisionObject* o = (*collision_objects)[k];
      o->collide(*pm);
    }
  }

  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
  for (int i = 0; i < springs.size(); i++) {
    Spring* s = &springs[i];
    PointMass* pm_a = s->pm_a;
    PointMass* pm_b = s->pm_b;
    
    double springL = (pm_a->position - pm_b->position).norm();
    double overstretch = springL - (s->rest_length * 1.1); // check for greater than 10% of rest length

    if (overstretch > 0) {
      if (pm_a->pinned && pm_b->pinned) {

      } else if (pm_a->pinned) {
        pm_b->position += overstretch * (pm_a->position - pm_b->position).unit();
      } else if (pm_b->pinned) {
        pm_a->position += overstretch * (pm_b->position - pm_a->position).unit();
      } else {
        pm_b->position += (overstretch / 2) * (pm_a->position - pm_b->position).unit();
        pm_a->position += (overstretch / 2) * (pm_b->position - pm_a->position).unit();
      }
    }
  }
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
  for (int i = 0; i < point_masses.size(); i++) {
    PointMass* pm = &point_masses[i];
    float hash = hash_position(pm->position);
    if (map.find(hash) == map.end()) { // no bucket
      map[hash] = new vector<PointMass*>(); // new bucket
    }
    map[hash]->push_back(pm); // add pm into bucket
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
  float hash = hash_position(pm.position);
  Vector3D correct(0);
  int count = 0;

  for (PointMass* pM : *(map[hash])) {
    if (&pm == pM) {

    } else {
      Vector3D dis = (pM->position - pm.position);
      if (dis.norm() < 2 * thickness) {
        count++;
        correct += dis - 2 * thickness * dis.unit();
      }
    }
  }

  if (count != 0) {
    correct = correct / simulation_steps;
    pm.position += correct;
  }
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
  double w = 3 * width / num_width_points;
  double h = 3 * height / num_height_points;
  double t = max(w, h);

  int x = floor(pos.x / w);
  int y = floor(pos.y / h);
  int z = floor(pos.z / t);

  return x + y * y + z * z * z; 
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
