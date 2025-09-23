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
double voo_point_callvirt_get_x(void * object);
double voo_point_callvirt_get_y(void * object);

// class Point implements IPoint {
//   double x;
//   double y;
//   double GetX() => x;
//   double GetY() => x;
// }
typedef struct {
  voo_vtable_for_point * vtable; // must be first
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
  voo_vtable_for_point * vtable; // must be first
  double r;
  double x;
  double y;
} voo_circle;
void voo_circle_new(voo_circle *circle, double r, double x, double y);

#endif //VDISP_VDISP_OOP_H
