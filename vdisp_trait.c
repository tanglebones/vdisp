#include "vdisp_trait.h"

double vtr_impl_point_get_x_for_point(vtr_point *self) {
  return self->x;
}

double vtr_impl_point_get_y_for_point(vtr_point *self) {
  return self->y;
}

static vtr_vtable_for_point vtl_impl_point_for_point = {
  (double(*)(void *self)) vtr_impl_point_get_x_for_point,
  (double(*)(void *self)) vtr_impl_point_get_y_for_point,
};

void vtr_point_into_point_object_ptr(vtr_point *point, vtr_object_pointer *vpp) {
  vpp->data = point;
  vpp->vtable = &vtl_impl_point_for_point;
}

double vtr_impl_point_get_x_for_circle(vtr_circle *self) {
  return self->x;
}

double vtr_impl_point_get_y_for_circle(vtr_circle *self) {
  return self->y;
}

static vtr_vtable_for_point vtl_impl_point_for_circle = {
  (double(*)(void *self)) vtr_impl_point_get_x_for_circle,
  (double(*)(void *self)) vtr_impl_point_get_y_for_circle,
};

void vtr_circle_into_point_object_ptr(vtr_circle *circle, vtr_object_pointer *vpp) {
  vpp->data = circle;
  vpp->vtable = &vtl_impl_point_for_circle;
}

double vtr_point_object_callvirt_get_x(const vtr_object_pointer *vpp) {
  return vpp->vtable->get_x(vpp->data);
}

double vtr_point_object_callvirt_get_y(const vtr_object_pointer *vpp) {
  return vpp->vtable->get_y(vpp->data);
}
