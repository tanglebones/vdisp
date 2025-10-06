#include <stdio.h>
#include <stdlib.h>

#include "ptime.h"
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
  double x = vtr_point_object_callvirt_get_x(&pp);
  double y = vtr_point_object_callvirt_get_y(&pp);
  printf("%lf %lf\n", x, y);
}

void vtr_two(void) {
  vtr_point p = {1, 2};
  vtr_circle c = {1, 2, 3};
  vtr_object_pointer pp;
  vtr_point_into_point_object_ptr(&p, &pp);
  vtr_object_pointer pc;
  vtr_circle_into_point_object_ptr(&c, &pc);
  printf("%lf %lf\n", vtr_point_object_callvirt_get_x(&pp), vtr_point_object_callvirt_get_y(&pp));
  printf("%lf %lf\n", vtr_point_object_callvirt_get_x(&pc), vtr_point_object_callvirt_get_y(&pc));
}

typedef struct {
  vtr_object_pointer *ps;
  vtr_point *p;
  size_t num_of_elements;
} vtr_arr_data;

void vtr_arr_setup(vtr_arr_data *data, const size_t num_of_elements) {
  data->ps = calloc(num_of_elements, sizeof(vtr_object_pointer));
  data->p = calloc(num_of_elements, sizeof(vtr_point));
  data->num_of_elements = num_of_elements;

  for (size_t i = 0; i < num_of_elements; i++) {
    data->p[i].x = (double) i;
    data->p[i].y = (double) (i + num_of_elements);
    vtr_point_into_point_object_ptr(&data->p[i], &data->ps[i]);
  }
}

void vtr_arr_teardown(const vtr_arr_data *data) {
  free(data->ps);
  free(data->p);
}

void vtr_arr(const vtr_arr_data *data) {
  const size_t num_of_elements = data->num_of_elements;
  const vtr_object_pointer *ps = data->ps;

  for (int i = 0; i < num_of_elements; i++) {
    const vtr_object_pointer *pp = &ps[i];
    trash = vtr_point_object_callvirt_get_x(pp);
    trash = vtr_point_object_callvirt_get_y(pp);
  }
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

typedef struct {
  voo_point *ps;
  voo_point *p;
  size_t num_of_elements;
} voo_arr_data;

void voo_arr_setup(voo_arr_data *data, const size_t num_of_elements) {
  data->ps = calloc(num_of_elements, sizeof(voo_point));
  data->p = calloc(num_of_elements, sizeof(voo_point));
  data->num_of_elements = num_of_elements;
  for (size_t i = 0; i < num_of_elements; i++) {
    voo_point_new(&data->p[i], (double) i, (double) (i + num_of_elements));
  }
}

void voo_arr_teardown(const voo_arr_data *data) {
  free(data->ps);
  free(data->p);
}

void voo_arr(const voo_arr_data *data) {
  const size_t num_of_elements = data->num_of_elements;
  voo_point *p = data->p;

  for (int i = 0; i < num_of_elements; i++) {
    trash = voo_point_callvirt_get_x(&p[i]);
    trash = voo_point_callvirt_get_y(&p[i]);
  }
}

static int compare_double(const void *a, const void *b) {
  const double diff = (*(const double *) a - *(const double *) b);
  return (diff > 0) - (diff < 0);
}

static const size_t counts[] = {
  1024 * 8,
  1024 * 16,
  1024 * 24,
  1024 * 32,
  1024 * 40,
  1024 * 48,
  1024 * 64,
  1024 * 72,
  1024 * 80,
  1024 * 88,
  1024 * 96,
  1024 * 104,
  1024 * 112,
  1024 * 120,
  1024 * 128,
  1024 * 136,
  1024 * 144,
  1024 * 152,
  1024 * 160,
  1024 * 168,
  1024 * 176,
  1024 * 184,
  1024 * 192,
  1024 * 200,
  1024 * 208,
  1024 * 216,
  1024 * 224,
  1024 * 232,
  1024 * 240,
  1024 * 248,
  1024 * 256
};
// static const size_t counts[] = {
//   1024 * 8, 1024 * 16, 1024 * 32, 1024 * 64, 1024 * 128, 1024 * 256, 1024 * 512, 1024 * 1024, 1024 * 1024 * 2,
//   1024 * 1024 * 4, 1024 * 1024 * 8
// };

void bench(
  char const *n,
  void *arr_data,
  void vdisp_setup(void *, size_t),
  void vdisp_teardown(const void *),
  void vdisp_arr(const void *)
) {
  for (int i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
    const size_t count = counts[i];
    double r[] = {
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0
    };
    const size_t rlen = sizeof(r) / sizeof(r[0]);
    vdisp_setup(arr_data, count);
    for (size_t j = 0; j < rlen; j++) {
      const ptime_point start = ptime_now();
      vdisp_arr(arr_data);
      const ptime_point end = ptime_now();
      r[j] = (double) ptime_ns_between(start, end);
      // printf("%s %zu %zu: %0.0lf\n", n, j, count, r[j]);
    }
    vdisp_teardown(arr_data);
    qsort(r, rlen, sizeof(double), compare_double);

    printf("%s,%zu,%0.0lf,%0.0lf,%0.0lf\n", n, count, r[1], r[rlen / 2], r[rlen - 2]);
  }
  fflush(stdout);
}

int main() {
  printf("Trait\n");
  vtr();
  vtr_two();
  vtr_arr_data vtr_arr_data;
  vtr_arr_setup(&vtr_arr_data, 1024);
  vtr_arr(&vtr_arr_data);
  vtr_arr_teardown(&vtr_arr_data);

  printf("\nObject\n");
  voo();
  voo_two();
  voo_arr_data voo_arr_data;
  voo_arr_setup(&voo_arr_data, 1024);
  voo_arr(&voo_arr_data);
  voo_arr_teardown(&voo_arr_data);

  printf("\n");

  bench("voo", &voo_arr_data, voo_arr_setup, voo_arr_teardown, voo_arr);
  bench("vtr", &vtr_arr_data, vtr_arr_setup, vtr_arr_teardown, vtr_arr);

  fflush(stdout);
  return 0;
}
