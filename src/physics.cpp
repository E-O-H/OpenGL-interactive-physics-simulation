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
    vector<Vector3d> temp_pos = vector<Vector3d>(end_index);      // For temporarily storing the result of collision calculation,
    vector<Vector3d> temp_pos_last = vector<Vector3d>(end_index); // a.k.a. new states to be updated after collision
    // preparing data
    for (unsigned i = start_index; i < end_index; ++i) {
        pos[i] = Vector3d(objects[i].translateX, 
                          objects[i].translateY, 
                          objects[i].translateZ);
        pos_last[i] = Vector3d(objects[i].translateX_last, 
                               objects[i].translateY_last, 
                               objects[i].translateZ_last);
        temp_pos[i] = pos[i];            // Used as the default update value if there is no collision.
        temp_pos_last[i] = pos_last[i];  // Both pos and pos_last are updated after collision in this implementation
                                         // in order to preserve the total energy (kinetic + potential energy)
    }

    // collision calculation
    for (unsigned i = start_index; i < end_index; ++i) {
        for (unsigned j = start_index; j < end_index; ++j) {
            if (j == i) continue;
            Vector3d distance = pos[j] - pos[i];
            Vector3d direction = distance.normalized();
            double vi = (pos[i] - pos_last[i]).dot(direction);
            double vj = (pos[j] - pos_last[j]).dot(direction);
            if (vi - vj > 0
                && distance.squaredNorm() < square(objects[i].collision_radius + objects[j].collision_radius)) {
                double vi_n = (vi * (objects[i].mass - objects[j].mass) + vj * (2 * objects[j].mass)) 
                                / (objects[i].mass + objects[j].mass);
                // calculate collision change of state
                temp_pos[i] = vi_n * vi > 0 ? pos[i] : pos_last[i];
                temp_pos_last[i] = temp_pos[i] + direction * (vi - vi_n)
                                    + (pos_last[i] - pos[i]); 
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
        pos[i] = temp_pos[i];
        pos_last[i] = temp_pos_last[i];

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