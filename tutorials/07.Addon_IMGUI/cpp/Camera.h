#pragma once
//#include <glm/glm.hpp>
#include "vec.hpp"

class Camera
{
public:
  typedef typename  kmuvcl::math::vec3f     vec3;
  typedef typename  kmuvcl::math::vec4f     vec4;
  typedef typename  kmuvcl::math::mat4x4f   mat4;

public:
  enum Mode { kOrtho, kPerspective };

public:
  Camera()
    : position_(0,0,0), 
      front_dir_(0,0,-1), 
      up_dir_(0,1,0), 
      right_dir_(1,0,0), 
      near_(-1),
      far_(1),
      fovy_(45),
      mode_(kOrtho)
  {}
  Camera(const vec3& _position, const vec3& _front_dir, const vec3& _up_dir, float _fovy)
    : position_(_position), front_dir_(_front_dir), up_dir_(_up_dir), fovy_(_fovy)
  {
    right_dir_ = kmuvcl::math::cross(front_dir_, up_dir_);
  }
  
  void move_forward(float delta);
  void move_backward(float delta);
  void move_left(float delta);
  void move_right(float delta);
  void move_up(float delta);
  void move_down(float delta);
  void rotate_left(float delta);
  void rotate_right(float delta);
	
  const vec3  position() const          { return  position_; }
  const vec3  front_direction() const   { return  front_dir_; } 
  const vec3  up_direction() const      { return  up_dir_; }
  const vec3  right_direction() const   { return  right_dir_; }	
	const vec3  center_position() const;
  

  const float				near() const { return near_; }
  const float       far() const { return far_; }
  void							set_near(float _near) { near_ = _near; }
  void							set_far(float _far) { far_ = _far; }

	const float				fovy() const							{ return fovy_; }
	void							set_fovy(float _fovy)			{ fovy_ = _fovy; }

  Camera::Mode      mode() const { return mode_; }
  void              set_mode(Camera::Mode _mode) { mode_ = _mode; }

private:
	vec3  position_;    // position of the camera  
  vec3  front_dir_;   // front direction of the camera    (it should be a unit vector whose length is 1)
  vec3  up_dir_;      // up direction of the camera       (it should be a unit vector whose length is 1)
  vec3  right_dir_;   // right direction of the camera    (it should be a unit vector whose length is 1)

  float near_;        // near clipping plane 
  float far_;         // far clipping plane 

	float fovy_;

  Mode  mode_;
};