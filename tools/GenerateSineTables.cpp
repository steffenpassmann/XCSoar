#ifndef FIXED_MATH
#define FIXED_MATH
#endif

#include "Math/Constants.h"
#include "Math/fixed.hpp"
#include "Computer/ThermalLocator.hpp"

#include <math.h>
#include <stdio.h>

static inline double
thermal_fn(int x)
{
  return exp((-0.2/ThermalLocator::TLOCATOR_NMAX)*pow((double)x, 1.5));
}

static inline double
INT_TO_DEG(int x)
{
  return DEG_TO_RAD * ((double)x * 360) / 4096;
}

int
main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  puts("const fixed SINETABLE[4096] = {");
  for (unsigned i = 0; i < 4096; i++)
    printf("  fixed(%.20e),\n", sin(INT_TO_DEG(i)));
  puts("};");

  puts("const fixed COSTABLE[4096] = {");
  for (unsigned i = 0; i < 4096; i++)
    printf("  fixed(%.20e),\n", cos(INT_TO_DEG(i)));
  puts("};");

  puts("const short ISINETABLE[4096] = {");
  for (unsigned i = 0; i < 4096; i++)
    printf("  %d,\n", (int)lround(sin(INT_TO_DEG(i)) * 1024));
  puts("};");

  puts("const short ICOSTABLE[4096] = {");
  for (unsigned i = 0; i < 4096; i++)
    printf("  %d,\n", (int)lround(cos(INT_TO_DEG(i)) * 1024));
  puts("};");

  puts("const fixed INVCOSINETABLE[4096] = {");
  for (unsigned i = 0; i < 4096; i++) {
    double x = cos(INT_TO_DEG(i));
    if ((x >= 0) && (x < 1.0e-8))
      x = 1.0e-8;

    if ((x < 0) && (x > -1.0e-8))
      x = -1.0e-8;

    printf("  fixed(%.20e),\n", 1.0 / x);
  }
  puts("};");

  printf("#define THERMALRECENCY_SIZE %d\n", ThermalLocator::TLOCATOR_NMAX);
  printf("const fixed THERMALRECENCY[%d] = {", ThermalLocator::TLOCATOR_NMAX);
  for (unsigned i = 0; i < ThermalLocator::TLOCATOR_NMAX; i++)
    printf("  fixed(%.20e),\n", thermal_fn(i));
  puts("};");

  return 0;
}
