#ifndef VDISP_VDISP_OOP_H
#define VDISP_VDISP_OOP_H


// interface IPoint {
//   double get_x();
//   double get_y();
// }
typedef struct {
  double (*get_x)(void *self);
  double (*get_y)(void *self);
} voo_vtable_for_point;

typedef struct {
  // technically n of these, one per interface
  voo_vtable_for_point *vtable;
} voo_rtti;

// class Point implements IPoint {
//   double x;
//   double y;
//   double GetX() => x;
//   double GetY() => x;
// }
typedef struct {
  voo_rtti *rtti; // must be first
  double x;
  double y;
} voo_point;

void voo_point_new(voo_point *point, double x, double y);

// class Circle implements IPoint {
//   double r;
//   double x;
//   double y;
//   double GetX() => x;
//   double GetY() => x;
// }
typedef struct {
  voo_rtti *rtti; // must be first
  double r;
  double x;
  double y;
} voo_circle;

void voo_circle_new(voo_circle *circle, double r, double x, double y);

inline double voo_point_callvirt_get_x(void *object) {
  voo_point *self = object;
  return self->rtti->vtable->get_x(self);
}

inline double voo_point_callvirt_get_y(void *object) {
  voo_point *self = object;
  return self->rtti->vtable->get_y(self);
}

#endif //VDISP_VDISP_OOP_H
