/*
rat.h

A single header library implementing rational numbers and a limited set of arithmetic operators.
*/
#ifndef RAT_H
#define RAT_H

#include <stdint.h>

// Change both lines when changing the type of rat_memb
typedef int64_t rat_memb;
#define RAT_MEMB_MAX INT64_MAX

typedef struct rat_t {
  rat_memb num;
  rat_memb div;
} rat_t;

// Converts the rational number to a different numerical type
#define rat_cast(type_name, r) (((type_name)((r).num)) / ((type_name)((r).div)))

#define rat_from_int(value) ((rat_t){value, 1})

// Computes the GCF of two integers. Always returns a nonzero result.
rat_memb rat_gcf(rat_memb a, rat_memb b);

// Tests if the denominator is zero.
int rat_is_inf(rat_t r);

// Tests if two rational numbers are equal, even if their members are different
// values. For example, passing 1/2 and 2/4 would return true.
int rat_equals(rat_t a, rat_t b);

// Compares a to b. -1 for a<b, 0 for a==b, 1 for a>b.
int rat_compare(rat_t a, rat_t b);

// Reduces a fraction to the simplest form.
rat_t rat_reduce(rat_t r);

// Inverts the fraction by swapping numerator and denominator.
rat_t rat_inv(rat_t r);

// Adds two rational numbers and returns the simplest result.
rat_t rat_add(rat_t a, rat_t b);

// Subtracts two rational numbers and returns the simplest result.
rat_t rat_sub(rat_t a, rat_t b);

// Multiplies two rational numbers and returns the simplest result.
rat_t rat_mul(rat_t a, rat_t b);

// Divides two rational numbers and returns the simplest result.
rat_t rat_div(rat_t a, rat_t b);

// Computes the remainder between a and b.
rat_t rat_mod(rat_t a, rat_t b);

// Returns the best representation of d as a rational number.
rat_t rat_from_double(double d);

// Returns a more limited representation of d as a rational number.
// Limits the size of the denominator to max_denom, which should be a positive integer.
rat_t rat_from_double_limited(double d, rat_memb max_denom);


#endif  // RAT_H

#ifdef RAT_IMPL

#include <math.h>

rat_memb rat_gcf(rat_memb a, rat_memb b) {
  if (a < 0) {
    a = -a;
  }
  if (b < 0) {
    b = -b;
  }
  while (b != 0) {
    rat_memb t = b;
    b = a % b;
    a = t;
  }
  return a;
}

int rat_is_inf(rat_t r) { return r.div == 0; }

int rat_equals(rat_t a, rat_t b) {
  a = rat_reduce(a);
  b = rat_reduce(b);
  return a.num == b.num && a.div == b.div;
}

int rat_compare(rat_t a, rat_t b) {
  a = rat_reduce(a);
  b = rat_reduce(b);
  a.num *= b.div;
  b.num *= a.div;
  if (a.num < b.num) return -1;
  if (a.num > b.num) return 1;
  return 0;
}

rat_t rat_reduce(rat_t r) {
  rat_memb gcf = rat_gcf(r.num, r.div);
  return (rat_t){r.num / gcf, r.div / gcf};
}

rat_t rat_inv(rat_t r) { return (rat_t){r.div, r.num}; }

rat_t rat_add(rat_t a, rat_t b) {
  a = rat_reduce(a);
  b = rat_reduce(b);
  rat_t r = {a.num * b.div + b.num * a.div, a.div * b.div};
  return rat_reduce(r);
}

rat_t rat_sub(rat_t a, rat_t b) {
  b.num = -b.num;
  return rat_add(a, b);
}

rat_t rat_mul(rat_t a, rat_t b) {
  rat_memb gcfab = rat_gcf(a.num, b.div);
  rat_memb gcfba = rat_gcf(b.num, a.div);
  rat_t r = {(a.num / gcfab) * (b.num / gcfba),
             (a.div / gcfba) * (b.div / gcfab)};
  return rat_reduce(r);
}

rat_t rat_div(rat_t a, rat_t b) { return rat_mul(a, rat_inv(b)); }

rat_t rat_mod(rat_t a, rat_t b) {
  rat_t c = {a.num * b.div, a.div * b.div};
  c.num = c.num % (b.num * a.div);
  return rat_reduce(c);
}

// From https://rosettacode.org/wiki/Convert_decimal_number_to_rational#C
static void rat_approx(double f, rat_memb md, rat_memb *num, rat_memb *denom) {
  /*  a: continued fraction coefficients. */
  rat_memb a, h[3] = {0, 1, 0}, k[3] = {1, 0, 0};
  rat_memb x, d, n = 1;
  int i, neg = 0;

  if (md <= 1) {
    *denom = 1;
    *num = (rat_memb)f;
    return;
  }

  if (f < 0) {
    neg = 1;
    f = -f;
  }

  while (f != floor(f)) {
    n <<= 1;
    f *= 2;
  }
  d = (rat_memb) f;

  /* continued fraction and check denominator each step */
  for (i = 0; i < 64; i++) {
    a = n ? d / n : 0;
    if (i && !a) break;

    x = d;
    d = n;
    n = x % n;

    x = a;
    if (k[1] * a + k[0] >= md) {
      x = (md - k[0]) / k[1];
      if (x * 2 >= a || k[1] >= md)
        i = 65;
      else
        break;
    }

    h[2] = x * h[1] + h[0];
    h[0] = h[1];
    h[1] = h[2];
    k[2] = x * k[1] + k[0];
    k[0] = k[1];
    k[1] = k[2];
  }
  *denom = k[1];
  *num = neg ? -h[1] : h[1];
}

rat_t rat_from_double(double d) {
  rat_t r = {0, 0};
  rat_approx(d, RAT_MEMB_MAX, &r.num, &r.div);
  return r;
}

rat_t rat_from_double_limited(double d, rat_memb max_denom) {
  rat_t r = {0, 0};
  rat_approx(d, max_denom, &r.num, &r.div);
  return r;
}

#endif  // RAT_IMPL
