/* Primitive operations on floating point for GNU Emacs Lisp interpreter.

Copyright (C) 1988, 1993-1994, 1999, 2001-2016 Free Software Foundation,
Inc.

Author: Wolfgang Rupprecht (ac
ording to ack.texi)

This file is part of GNU Emacs.

GNU Emacs is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.  */


/* C89 requires only the following math.h functions, and Emacs omits
   the starred functions since we haven't found a use for them:
   acos, asin, atan, atan2, ceil, cos, *cosh, exp, fabs, floor, fmod,
   frexp, ldexp, log, log10 [via (log X 10)], *modf, pow, sin, *sinh,
   sqrt, tan, *tanh.

   C99 and C11 require the following math.h functions in addition to
   the C89 functions.  Of these, Emacs currently exports only the
   starred ones to Lisp, since we haven't found a use for the others:
   acosh, atanh, cbrt, *copysign, erf, erfc, exp2, expm1, fdim, fma,
   fmax, fmin, fpclassify, hypot, ilogb, isfinite, isgreater,
   isgreaterequal, isinf, isless, islessequal, islessgreater, *isnan,
   isnormal, isunordered, lgamma, log1p, *log2 [via (log X 2)], *logb
   (approximately), lrint/llrint, lround/llround, nan, nearbyint,
   nextafter, nexttoward, remainder, remquo, *rint, round, scalbln,
   scalbn, signbit, tgamma, trunc.
 */

#include <config.h>

#include "lisp.h"

#include <math.h>

/* 'isfinite' and 'isnan' cause build failures on Solaris 10 with the
   bundled GCC in c99 mode.  Work around the bugs with simple
   implementations that are good enough.  */
#undef isfinite
#define isfinite(x) ((x) - (x) == 0)
#undef isnan
#define isnan(x) ((x) != (x))

/* Check that X is a floating point number.  */

static void
CHECK_FLOAT (Lisp_Object x)
{
  CHECK_TYPE (FLOATP (x), Qfloatp, x);
}

/* Extract a Lisp number as a `double', or signal an error.  */

double
extract_float (Lisp_Object num)
{
  CHECK_NUMBER_OR_FLOAT (num);

  if (FLOATP (num))
    return XFLOAT_DATA (num);
  return (double) XINT (num);
}

/* Trig functions.  */

DEFUN ("acos", Facos, Sacos, 1, 1, 0,
       doc: /* Return the inverse cosine of ARG.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = acos (d);
  return make_float (d);
}

DEFUN ("asin", Fasin, Sasin, 1, 1, 0,
       doc: /* Return the inverse sine of ARG.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = asin (d);
  return make_float (d);
}

DEFUN ("atan", Fatan, Satan, 1, 2, 0,
       doc: /* Return the inverse tangent of the arguments.
If only one argument Y is given, return the inverse tangent of Y.
If two arguments Y and X are given, return the inverse tangent of Y
divided by X, i.e. the angle in radians between the vector (X, Y)
and the x-axis.  */)
  (Lisp_Object y, Lisp_Object x)
{
  double d = extract_float (y);

  if (NILP (x))
    d = atan (d);
  else
    {
      double d2 = extract_float (x);
      d = atan2 (d, d2);
    }
  return make_float (d);
}

DEFUN ("cos", Fcos, Scos, 1, 1, 0,
       doc: /* Return the cosine of ARG.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = cos (d);
  return make_float (d);
}

DEFUN ("sin", Fsin, Ssin, 1, 1, 0,
       doc: /* Return the sine of ARG.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = sin (d);
  return make_float (d);
}

DEFUN ("tan", Ftan, Stan, 1, 1, 0,
       doc: /* Return the tangent of ARG.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = tan (d);
  return make_float (d);
}

DEFUN ("isnan", Fisnan, Sisnan, 1, 1, 0,
       doc: /* Return non nil if argument X is a NaN.  */)
  (Lisp_Object x)
{
  CHECK_FLOAT (x);
  return isnan (XFLOAT_DATA (x)) ? Qt : Qnil;
}

#ifdef HAVE_COPYSIGN
DEFUN ("copysign", Fcopysign, Scopysign, 2, 2, 0,
       doc: /* Copy sign of X2 to value of X1, and return the result.
Cause an error if X1 or X2 is not a float.  */)
  (Lisp_Object x1, Lisp_Object x2)
{
  double f1, f2;

  CHECK_FLOAT (x1);
  CHECK_FLOAT (x2);

  f1 = XFLOAT_DATA (x1);
  f2 = XFLOAT_DATA (x2);

  return make_float (copysign (f1, f2));
}
#endif

DEFUN ("frexp", Ffrexp, Sfrexp, 1, 1, 0,
       doc: /* Get significand and exponent of a floating point number.
Breaks the floating point number X into its binary significand SGNFCAND
\(a floating point value between 0.5 (included) and 1.0 (excluded))
and an integral exponent EXP for 2, such that:

  X = SGNFCAND * 2^EXP

The function returns the cons cell (SGNFCAND . EXP).
If X is zero, both parts (SGNFCAND and EXP) are zero.  */)
  (Lisp_Object x)
{
  double f = XFLOATINT (x);
  int exponent;
  double sgnfcand = frexp (f, &exponent);
  return Fcons (make_float (sgnfcand), make_number (exponent));
}

DEFUN ("ldexp", Fldexp, Sldexp, 2, 2, 0,
       doc: /* Return X * 2**EXP, as a floating point number.
EXP must be an integer.   */)
  (Lisp_Object sgnfcand, Lisp_Object exponent)
{
  CHECK_NUMBER (exponent);
  int e = min (max (INT_MIN, XINT (exponent)), INT_MAX);
  return make_float (ldexp (XFLOATINT (sgnfcand), e));
}

DEFUN ("exp", Fexp, Sexp, 1, 1, 0,
       doc: /* Return the exponential base e of ARG.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = exp (d);
  return make_float (d);
}

DEFUN ("expt", Fexpt, Sexpt, 2, 2, 0,
       doc: /* Return the exponential ARG1 ** ARG2.  */)
  (Lisp_Object arg1, Lisp_Object arg2)
{
  double f1, f2, f3;

  CHECK_NUMBER_OR_FLOAT (arg1);
  CHECK_NUMBER_OR_FLOAT (arg2);
  if (INTEGERP (arg1)     /* common lisp spec */
      && INTEGERP (arg2)   /* don't promote, if both are ints, and */
      && XINT (arg2) >= 0) /* we are sure the result is not fractional */
    {				/* this can be improved by pre-calculating */
      EMACS_INT y;		/* some binary powers of x then accumulating */
      EMACS_UINT acc, x;  /* Unsigned so that overflow is well defined.  */
      Lisp_Object val;

      x = XINT (arg1);
      y = XINT (arg2);
      acc = (y & 1 ? x : 1);

      while ((y >>= 1) != 0)
	{
	  x *= x;
	  if (y & 1)
	    acc *= x;
	}
      XSETINT (val, acc);
      return val;
    }
  f1 = FLOATP (arg1) ? XFLOAT_DATA (arg1) : XINT (arg1);
  f2 = FLOATP (arg2) ? XFLOAT_DATA (arg2) : XINT (arg2);
  f3 = pow (f1, f2);
  return make_float (f3);
}

DEFUN ("log", Flog, Slog, 1, 2, 0,
       doc: /* Return the natural logarithm of ARG.
If the optional argument BASE is given, return log ARG using that base.  */)
  (Lisp_Object arg, Lisp_Object base)
{
  double d = extract_float (arg);

  if (NILP (base))
    d = log (d);
  else
    {
      double b = extract_float (base);

      if (b == 10.0)
	d = log10 (d);
#if HAVE_LOG2
      else if (b == 2.0)
	d = log2 (d);
#endif
      else
	d = log (d) / log (b);
    }
  return make_float (d);
}

DEFUN ("sqrt", Fsqrt, Ssqrt, 1, 1, 0,
       doc: /* Return the square root of ARG.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = sqrt (d);
  return make_float (d);
}

DEFUN ("abs", Fabs, Sabs, 1, 1, 0,
       doc: /* Return the absolute value of ARG.  */)
  (register Lisp_Object arg)
{
  CHECK_NUMBER_OR_FLOAT (arg);

  if (FLOATP (arg))
    arg = make_float (fabs (XFLOAT_DATA (arg)));
  else if (XINT (arg) < 0)
    XSETINT (arg, - XINT (arg));

  return arg;
}

DEFUN ("float", Ffloat, Sfloat, 1, 1, 0,
       doc: /* Return the floating point number equal to ARG.  */)
  (register Lisp_Object arg)
{
  CHECK_NUMBER_OR_FLOAT (arg);

  if (INTEGERP (arg))
    return make_float ((double) XINT (arg));
  else				/* give 'em the same float back */
    return arg;
}

DEFUN ("logb", Flogb, Slogb, 1, 1, 0,
       doc: /* Returns largest integer <= the base 2 log of the magnitude of ARG.
This is the same as the exponent of a float.  */)
  (Lisp_Object arg)
{
  Lisp_Object val;
  EMACS_INT value;
  double f = extract_float (arg);

  if (f == 0.0)
    value = MOST_NEGATIVE_FIXNUM;
  else if (isfinite (f))
    {
      int ivalue;
      frexp (f, &ivalue);
      value = ivalue - 1;
    }
  else
    value = MOST_POSITIVE_FIXNUM;

  XSETINT (val, value);
  return val;
}


/* the rounding functions  */

static Lisp_Object
rounding_driver (Lisp_Object arg, Lisp_Object divisor,
		 double (*double_round) (double),
		 EMACS_INT (*int_round2) (EMACS_INT, EMACS_INT),
		 const char *name)
{
  CHECK_NUMBER_OR_FLOAT (arg);

  if (! NILP (divisor))
    {
      EMACS_INT i1, i2;

      CHECK_NUMBER_OR_FLOAT (divisor);

      if (FLOATP (arg) || FLOATP (divisor))
	{
	  double f1, f2;

	  f1 = FLOATP (arg) ? XFLOAT_DATA (arg) : XINT (arg);
	  f2 = (FLOATP (divisor) ? XFLOAT_DATA (divisor) : XINT (divisor));
	  if (! IEEE_FLOATING_POINT && f2 == 0)
	    xsignal0 (Qarith_error);

	  f1 = (*double_round) (f1 / f2);
	  if (FIXNUM_OVERFLOW_P (f1))
	    xsignal3 (Qrange_error, build_string (name), arg, divisor);
	  arg = make_number (f1);
	  return arg;
	}

      i1 = XINT (arg);
      i2 = XINT (divisor);

      if (i2 == 0)
	xsignal0 (Qarith_error);

      XSETINT (arg, (*int_round2) (i1, i2));
      return arg;
    }

  if (FLOATP (arg))
    {
      double d = (*double_round) (XFLOAT_DATA (arg));
      if (FIXNUM_OVERFLOW_P (d))
	xsignal2 (Qrange_error, build_string (name), arg);
      arg = make_number (d);
    }

  return arg;
}

static EMACS_INT
ceiling2 (EMACS_INT i1, EMACS_INT i2)
{
  return i1 / i2 + ((i1 % i2 != 0) & ((i1 < 0) == (i2 < 0)));
}

static EMACS_INT
floor2 (EMACS_INT i1, EMACS_INT i2)
{
  return i1 / i2 - ((i1 % i2 != 0) & ((i1 < 0) != (i2 < 0)));
}

static EMACS_INT
truncate2 (EMACS_INT i1, EMACS_INT i2)
{
  return i1 / i2;
}

static EMACS_INT
round2 (EMACS_INT i1, EMACS_INT i2)
{
  /* The C language's division operator gives us one remainder R, but
     we want the remainder R1 on the other side of 0 if R1 is closer
     to 0 than R is; because we want to round to even, we also want R1
     if R and R1 are the same distance from 0 and if C's quotient is
     odd.  */
  EMACS_INT q = i1 / i2;
  EMACS_INT r = i1 % i2;
  EMACS_INT abs_r = eabs (r);
  EMACS_INT abs_r1 = eabs (i2) - abs_r;
  return q + (abs_r + (q & 1) <= abs_r1 ? 0 : (i2 ^ r) < 0 ? -1 : 1);
}

/* The code uses emacs_rint, so that it works to undefine HAVE_RINT
   if `rint' exists but does not work right.  */
#ifdef HAVE_RINT
#define emacs_rint rint
#else
static double
emacs_rint (double d)
{
  double d1 = d + 0.5;
  double r = floor (d1);
  return r - (r == d1 && fmod (r, 2) != 0);
}
#endif

static double
double_identity (double d)
{
  return d;
}

DEFUN ("ceiling", Fceiling, Sceiling, 1, 2, 0,
       doc: /* Return the smallest integer no less than ARG.
This rounds the value towards +inf.
With optional DIVISOR, return the smallest integer no less than ARG/DIVISOR.  */)
  (Lisp_Object arg, Lisp_Object divisor)
{
  return rounding_driver (arg, divisor, ceil, ceiling2, "ceiling");
}

DEFUN ("floor", Ffloor, Sfloor, 1, 2, 0,
       doc: /* Return the largest integer no greater than ARG.
This rounds the value towards -inf.
With optional DIVISOR, return the largest integer no greater than ARG/DIVISOR.  */)
  (Lisp_Object arg, Lisp_Object divisor)
{
  return rounding_driver (arg, divisor, floor, floor2, "floor");
}

DEFUN ("round", Fround, Sround, 1, 2, 0,
       doc: /* Return the nearest integer to ARG.
With optional DIVISOR, return the nearest integer to ARG/DIVISOR.

Rounding a value equidistant between two integers may choose the
integer closer to zero, or it may prefer an even integer, depending on
your machine.  For example, (round 2.5) can return 3 on some
systems, but 2 on others.  */)
  (Lisp_Object arg, Lisp_Object divisor)
{
  return rounding_driver (arg, divisor, emacs_rint, round2, "round");
}

DEFUN ("truncate", Ftruncate, Struncate, 1, 2, 0,
       doc: /* Truncate a floating point number to an int.
Rounds ARG toward zero.
With optional DIVISOR, truncate ARG/DIVISOR.  */)
  (Lisp_Object arg, Lisp_Object divisor)
{
  return rounding_driver (arg, divisor, double_identity, truncate2,
			  "truncate");
}


Lisp_Object
fmod_float (Lisp_Object x, Lisp_Object y)
{
  double f1, f2;

  f1 = FLOATP (x) ? XFLOAT_DATA (x) : XINT (x);
  f2 = FLOATP (y) ? XFLOAT_DATA (y) : XINT (y);

  f1 = fmod (f1, f2);

  /* If the "remainder" comes out with the wrong sign, fix it.  */
  if (f2 < 0 ? f1 > 0 : f1 < 0)
    f1 += f2;

  return make_float (f1);
}

DEFUN ("fceiling", Ffceiling, Sfceiling, 1, 1, 0,
       doc: /* Return the smallest integer no less than ARG, as a float.
\(Round toward +inf.)  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = ceil (d);
  return make_float (d);
}

DEFUN ("ffloor", Fffloor, Sffloor, 1, 1, 0,
       doc: /* Return the largest integer no greater than ARG, as a float.
\(Round towards -inf.)  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = floor (d);
  return make_float (d);
}

DEFUN ("fround", Ffround, Sfround, 1, 1, 0,
       doc: /* Return the nearest integer to ARG, as a float.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  d = emacs_rint (d);
  return make_float (d);
}

DEFUN ("ftruncate", Fftruncate, Sftruncate, 1, 1, 0,
       doc: /* Truncate a floating point number to an integral float value.
Rounds the value toward zero.  */)
  (Lisp_Object arg)
{
  double d = extract_float (arg);
  if (d >= 0.0)
    d = floor (d);
  else
    d = ceil (d);
  return make_float (d);
}

void
syms_of_floatfns (void)
{
  defsubr (&Sacos);
  defsubr (&Sasin);
  defsubr (&Satan);
  defsubr (&Scos);
  defsubr (&Ssin);
  defsubr (&Stan);
  defsubr (&Sisnan);
#ifdef HAVE_COPYSIGN
  defsubr (&Scopysign);
#endif
  defsubr (&Sfrexp);
  defsubr (&Sldexp);
  defsubr (&Sfceiling);
  defsubr (&Sffloor);
  defsubr (&Sfround);
  defsubr (&Sftruncate);
  defsubr (&Sexp);
  defsubr (&Sexpt);
  defsubr (&Slog);
  defsubr (&Ssqrt);

  defsubr (&Sabs);
  defsubr (&Sfloat);
  defsubr (&Slogb);
  defsubr (&Sceiling);
  defsubr (&Sfloor);
  defsubr (&Sround);
  defsubr (&Struncate);
}
