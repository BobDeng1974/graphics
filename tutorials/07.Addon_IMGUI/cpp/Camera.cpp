#include "Camera.h"
//#include <glm/ext.hpp>
#include "transform.hpp"

const Camera::vec3 Camera::center_position() const
{
  return  vec3(position_[0] + front_dir_[0],
               position_[1] + front_dir_[1],
               position_[2] + front_dir_[2]);
}

// TODO: fill up the following functions properly 
void Camera::move_forward(float delta)
{
  position_ += delta * front_dir_;
}

void Camera::move_backward(float delta)
{
  move_forward(-delta);
}

void Camera::move_left(float delta)
{
  position_ -= delta * right_dir_;
}

void Camera::move_right(float delta)
{
  move_left(-delta);
}

void Camera::move_up(float delta)
{
  position_ += delta * up_dir_;
}

void Camera::move_down(float delta)
{
  move_up(-delta);
}

void Camera::rotate_left(float delta)
{
  mat4  Ry;
  vec4  f(front_dir_[0], front_dir_[1], front_dir_[2], 0.0f);

  f = Ry * f;

  front_dir_ = vec3(f[0], f[1], f[2]);
  right_dir_ = kmuvcl::math::cross(front_dir_, up_dir_);
}

void Camera::rotate_right(float delta)
{
  rotate_left(-delta);
}