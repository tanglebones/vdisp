#ifndef VDISP_VDISP_TRAIT_H
#define VDISP_VDISP_TRAIT_H

typedef struct {
  double r;
  double x;
  double y;
} vtr_circle;

typedef struct {
  double x;
  double y;
} vtr_point;

typedef struct {
  double (*get_x)(void *self);

  double (*get_y)(void *self);
} vtr_vtable_for_point;

typedef struct {
  void *data;
  vtr_vtable_for_point *vtable;
} vtr_object_pointer;

void vtr_point_into_point_object_ptr(vtr_point *point, vtr_object_pointer *vpp);

void vtr_circle_into_point_object_ptr(vtr_circle *circle, vtr_object_pointer *vpp);

inline double vtr_point_object_callvirt_get_x(const vtr_object_pointer *vpp) {
  return vpp->vtable->get_x(vpp->data);
}

inline double vtr_point_object_callvirt_get_y(const vtr_object_pointer *vpp) {
  return vpp->vtable->get_y(vpp->data);
}

#endif //VDISP_VDISP_TRAIT_H
