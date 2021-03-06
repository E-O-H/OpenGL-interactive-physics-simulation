#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <utility>
// Linear Algebra Library
#include <Eigen/Core>
#include <Eigen/Geometry>

using std::string;
using std::vector;
using std::pair;
using std::sin;
using std::cos;
using std::atan;
using Eigen::Vector3f;
using Eigen::Vector3d;
using Eigen::Vector4f;
using Eigen::Matrix4f;

#define PI 3.1415926

#define RESOLUTION_X 1920
#define RESOLUTION_Y 1080

template <typename T>
inline T square(T a) {
    return a * a;
}

template <typename T>
inline T positiveMod(T a, T b) {
    return (a % b + b) % b;
}