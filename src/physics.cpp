#include "common_header.h"
#include "object_class.h"
#include <iostream>

double G_para = 5.0; // Gravity parameter
double dt = 0.01;    // Time step

extern vector<Object> objects;

// Physics simulation on objects range [start_index ~ end_index).
// (Objects with index out of the range don't participate in physics simulation.)
void physics(unsigned start_index, unsigned end_index) {
    vector<Vector3d> acceleration = vector<Vector3d>(end_index, Vector3d::Zero());
    vector<Vector3d> pos = vector<Vector3d>(end_index);
    vector<Vector3d> pos_last = vector<Vector3d>(end_index);
    // preparing data
    for (unsigned i = start_index; i < end_index; ++i) {
        pos[i] = Vector3d(objects[i].translateX, 
                          objects[i].translateY, 
                          objects[i].translateZ);
        pos_last[i] = Vector3d(objects[i].translateX_last, 
                               objects[i].translateY_last, 
                               objects[i].translateZ_last);
    }

    // collision calculation
    for (unsigned i = start_index; i < end_index; ++i) {
        for (unsigned j = i + 1; j < end_index; ++j) {
            Vector3d distance = pos[j] - pos[i];
            Vector3d direction = distance.normalized(); 
                                 // Positive direction unit is defined as from i to j
            double vi = (pos[i] - pos_last[i]).dot(direction);
            double vj = (pos[j] - pos_last[j]).dot(direction);
            if (vi - vj > 0
                && distance.squaredNorm() < square(objects[i].collision_radius + objects[j].collision_radius)) {
                double vi_n = (vi * (objects[i].mass - objects[j].mass) + vj * (2 * objects[j].mass)) 
                                / (objects[i].mass + objects[j].mass);
                double vj_n = (vj * (objects[j].mass - objects[i].mass) + vi * (2 * objects[i].mass)) 
                                / (objects[i].mass + objects[j].mass);
                // calculate collision change of state
                Vector3d original_last_displacement_i = pos_last[i] - pos[i];
                pos[i] = vi_n * vi > 0 ? pos[i] : pos_last[i];
                pos_last[i] = pos[i] + direction * (vi - vi_n) + original_last_displacement_i; 
                Vector3d original_last_displacement_j = pos_last[j] - pos[j];
                pos[j] = vj_n * vj > 0 ? pos[j] : pos_last[j];
                pos_last[j] = pos[j] + direction * (vj - vj_n) + original_last_displacement_j; 
                // break; // Adding this break will limit every object to only collide with one other
                          // object at the same time (in the same frame); this will greatly improve
                          // performance when there are a LOT of objects in the simulation, at the cost
                          // of missing collisions.
            }
        }

        // calculate the acceleration of object i
        for (unsigned j = start_index; j < end_index; ++j) {
            if (j == i) continue;
            Vector3d distance = pos[j] - pos[i];
            acceleration[i] += distance.normalized() * (G_para * objects[j].mass / distance.squaredNorm());
        }
    }
    
    // calculate the next positions for each object
    for(unsigned i = start_index; i < end_index; ++i) {
        Vector3d next_pos;
        next_pos = pos[i] * 2 - pos_last[i] + acceleration[i] * (dt * dt); // Verlet Algorithm

        // write result
        objects[i].translateX_last = pos[i].x();
        objects[i].translateY_last = pos[i].y();
        objects[i].translateZ_last = pos[i].z();
        objects[i].translateX = next_pos.x();
        objects[i].translateY = next_pos.y();
        objects[i].translateZ = next_pos.z();
    }
}