#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "_vectors.h"

class Material {

public:

  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &diffuseColor) { _diffuseColor = diffuseColor; }
  virtual ~Material() {}

  // ACCESSORS
  virtual Vec3f getDiffuseColor() const { return _diffuseColor; }

protected:
  Vec3f _diffuseColor;

};

#endif //  _MATERIAL_H_
