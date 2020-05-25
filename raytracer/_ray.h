#ifndef _RAY_H_
#define _RAY_H_

#include <iostream>
using namespace std;

#include "_vectors.h"

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Ray () {}
  Ray (const Vec3f &origin, const Vec3f &direction) {
    _origin = origin; 
    _direction = direction; }
  Ray (const Ray& r) {*this=r;}

  // ACCESSORS
  const Vec3f& getOrigin() const { return _origin; }
  const Vec3f& getDirection() const { return _direction; }
  Vec3f pointAtParameter(float t) const {
    return _origin + _direction * t; }

private:
  // REPRESENTATION
  Vec3f _origin;
  Vec3f _direction;
};

inline ostream &operator<<(ostream &os, const Ray &r) {
  os << "Ray <o:" <<r.getOrigin()<<", d:"<<r.getDirection()<<">";
  return os;
}
#endif // _RAY_H_
