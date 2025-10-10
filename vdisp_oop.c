#include "vdisp_oop.h"

double voo_impl_point_get_x_for_point(voo_point *this) {
  return this->x;
}

double voo_impl_point_get_y_for_point(voo_point *this) {
  return this->y;
}

static voo_vtable_for_point voo_impl_point_for_point = {
  (double(*)(void *this)) voo_impl_point_get_x_for_point,
  (double(*)(void *this)) voo_impl_point_get_y_for_point,
};

static voo_rtti voo_point_rtti = {
  &voo_impl_point_for_point
};

void voo_point_new(voo_point *point, double x, double y) {
  point->x = x;
  point->y = y;
  point->rtti = &voo_point_rtti;
}

double voo_impl_point_get_x_for_circle(voo_circle *this) {
  return this->x;
}

double voo_impl_point_get_y_for_circle(voo_circle *this) {
  return this->y;
}

static voo_vtable_for_point voo_impl_point_for_circle = {
  (double(*)(void *this)) voo_impl_point_get_x_for_circle,
  (double(*)(void *this)) voo_impl_point_get_y_for_circle,
};

static voo_rtti voo_circle_rtti = {
  &voo_impl_point_for_circle
};

void voo_circle_new(voo_circle *circle, double r, double x, double y) {
  circle->r = r;
  circle->x = x;
  circle->y = y;
  circle->rtti = &voo_circle_rtti;
}
