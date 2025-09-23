#include <stdio.h>
#include <stdlib.h>

#include "vdisp_oop.h"
#include "vdisp_trait.h"

volatile double trash = 0;

void vtr(void) {
  // data is just data:
  vtr_point p = {1, 2};

  // "objects" are data + vtable
  vtr_object_pointer pp;
  vtr_point_into_point_object_ptr(&p, &pp);

  // virtual dispatch is against the object ptr
  double x = vtr_point_object_callvirt_get_x(pp);
  double y = vtr_point_object_callvirt_get_y(pp);
  printf("%lf %lf\n", x, y);
}

void vtr_two(void) {
  vtr_point p = {1, 2};
  vtr_circle c = {1, 2, 3};
  vtr_object_pointer pp;
  vtr_point_into_point_object_ptr(&p, &pp);
  vtr_object_pointer pc;
  vtr_circle_into_point_object_ptr(&c, &pc);
  printf("%lf %lf\n", vtr_point_object_callvirt_get_x(pp), vtr_point_object_callvirt_get_y(pp));
  printf("%lf %lf\n", vtr_point_object_callvirt_get_x(pc), vtr_point_object_callvirt_get_y(pc));
}

void vtr_arr(void) {
  int num_of_elements = 1024;
  vtr_object_pointer *ps = calloc(num_of_elements, sizeof(vtr_object_pointer));
  vtr_point *p = calloc(num_of_elements, sizeof(vtr_point));
  for (int i = 0; i < num_of_elements; i++) {
    p[i].x = i;
    p[i].y = i + num_of_elements;
    vtr_point_into_point_object_ptr(&p[i], &ps[i]);
  }
  for (int i = 0; i < num_of_elements; i++) {
    vtr_object_pointer *pp = &ps[i];
    trash = vtr_point_object_callvirt_get_x(*pp);
    trash = vtr_point_object_callvirt_get_y(*pp);
  }
  free(ps);
  free(p);
}

void voo(void) {
  voo_point p;

  // construction is special now, as vtable has to be added to every instance of the data
  voo_point_new(&p, 1, 2);

  // virtual dispatch is against the unwrapper object pointer
  // and assumes the vtable point is at field offset 0.
  // i.e. is the very first field of the struct.
  double x = voo_point_callvirt_get_x(&p);
  double y = voo_point_callvirt_get_y(&p);
  printf("%lf %lf\n", x, y);
}

void voo_two(void) {
  voo_point p;
  voo_circle c;
  voo_point_new(&p, 1, 2);
  voo_circle_new(&c, 1, 2, 3);
  printf("%lf %lf\n", voo_point_callvirt_get_x(&p), voo_point_callvirt_get_y(&p));
  printf("%lf %lf\n", voo_point_callvirt_get_x(&c), voo_point_callvirt_get_y(&c));
}

void voo_arr(void) {
  int num_of_elements = 1024;
  voo_point *p = calloc(num_of_elements, sizeof(voo_point));
  for (int i = 0; i < num_of_elements; i++) {
    voo_point_new(&p[i], i, i + num_of_elements);
  }
  for (int i = 0; i < num_of_elements; i++) {
    trash = voo_point_callvirt_get_x(&p[i]);
    trash = voo_point_callvirt_get_y(&p[i]);
  }
  free(p);
}

int main(int argc, char **argv) {
  if (argc > 1) {
    printf("Trait\n");
    vtr();
    vtr_two();
    vtr_arr();
  } else {
    printf("\nObject\n");
    voo();
    voo_two();
    voo_arr();
  }
  fflush(stdout);
  return 0;
}
