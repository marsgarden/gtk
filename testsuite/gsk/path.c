/*
 * Copyright © 2020 Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include <gtk/gtk.h>

static GskPath *
create_random_degenerate_path (guint max_contours)
{
#define N_DEGENERATE_PATHS 12
  GskPathBuilder *builder;
  guint i;

  builder = gsk_path_builder_new ();

  switch (g_test_rand_int_range (0, N_DEGENERATE_PATHS))
  {
    case 0:
      /* empty path */
      break;

    case 1:
      /* a single point */
      gsk_path_builder_move_to (builder, 
                                g_test_rand_double_range (-1000, 1000),
                                g_test_rand_double_range (-1000, 1000));
      break;

    case 2:
      /* N points */
      for (i = 0; i < MIN (10, max_contours); i++)
        {
          gsk_path_builder_move_to (builder, 
                                    g_test_rand_double_range (-1000, 1000),
                                    g_test_rand_double_range (-1000, 1000));
        }
      break;

    case 3:
      /* 1 closed point */
      gsk_path_builder_move_to (builder, 
                                g_test_rand_double_range (-1000, 1000),
                                g_test_rand_double_range (-1000, 1000));
      gsk_path_builder_close (builder);
      break;

    case 4:
      /* the same point closed N times */
      gsk_path_builder_move_to (builder, 
                                g_test_rand_double_range (-1000, 1000),
                                g_test_rand_double_range (-1000, 1000));
      for (i = 0; i < MIN (10, max_contours); i++)
        {
          gsk_path_builder_close (builder);
        }
      break;

    case 5:
      /* a zero-width and zero-height rect */
      gsk_path_builder_add_rect (builder,
                                 &GRAPHENE_RECT_INIT (g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      0, 0));
      break;

    case 6:
      /* a zero-width rect */
      gsk_path_builder_add_rect (builder,
                                 &GRAPHENE_RECT_INIT (g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      0,
                                                      g_test_rand_double_range (-1000, 1000)));
      break;

    case 7:
      /* a zero-height rect */
      gsk_path_builder_add_rect (builder,
                                 &GRAPHENE_RECT_INIT (g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      0));
      break;

    case 8:
      /* a negative-size rect */
      gsk_path_builder_add_rect (builder,
                                 &GRAPHENE_RECT_INIT (g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 0),
                                                      g_test_rand_double_range (-1000, 0)));
      break;

    case 9:
      /* an absolutely random rect */
      gsk_path_builder_add_rect (builder,
                                 &GRAPHENE_RECT_INIT (g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000)));
      break;

    case 10:
      /* an absolutely random rect */
      gsk_path_builder_add_rect (builder,
                                 &GRAPHENE_RECT_INIT (g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000)));
      break;

    case 11:
      /* an absolutely random circle */
      gsk_path_builder_add_circle (builder,
                                   &GRAPHENE_POINT_INIT (g_test_rand_double_range (-1000, 1000),
                                                         g_test_rand_double_range (-1000, 1000)),
                                   g_test_rand_double_range (1, 1000));
      break;

    case N_DEGENERATE_PATHS:
    default:
      g_assert_not_reached ();
  }

  return gsk_path_builder_free_to_path (builder);
}

static GskPath *
create_random_path (guint max_contours);

static void
add_shape_contour (GskPathBuilder *builder)
{
#define N_SHAPE_CONTOURS 3
  switch (g_test_rand_int_range (0, N_SHAPE_CONTOURS))
  {
    case 0:
      gsk_path_builder_add_rect (builder,
                                 &GRAPHENE_RECT_INIT (g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (-1000, 1000),
                                                      g_test_rand_double_range (1, 1000),
                                                      g_test_rand_double_range (1, 1000)));
      break;

    case 1:
      gsk_path_builder_add_circle (builder,
                                   &GRAPHENE_POINT_INIT (g_test_rand_double_range (-1000, 1000),
                                                         g_test_rand_double_range (-1000, 1000)),
                                   g_test_rand_double_range (1, 1000));
      break;

    case 2:
      {
        GskPath *path = create_random_path (1);
        gsk_path_builder_add_path (builder, path);
        gsk_path_unref (path);
      }
      break;

    case N_SHAPE_CONTOURS:
    default:
      g_assert_not_reached ();
      break;
  }
}

static float
random_weight (void)
{
  if (g_test_rand_bit ())
    return g_test_rand_double_range (0, 100);
  else
    return 1.0 / g_test_rand_double_range (1, 100);
}

static void
add_standard_contour (GskPathBuilder *builder)
{
  guint i, n;

  if (g_test_rand_bit ())
    {
      if (g_test_rand_bit ())
        gsk_path_builder_move_to (builder,
                                  g_test_rand_double_range (-1000, 1000),
                                  g_test_rand_double_range (-1000, 1000));
      else
        gsk_path_builder_rel_move_to (builder,
                                      g_test_rand_double_range (-1000, 1000),
                                      g_test_rand_double_range (-1000, 1000));
    }

  /* that 20 is random, but should be enough to get some
   * crazy self-intersecting shapes */
  n = g_test_rand_int_range (1, 20);
  for (i = 0; i < n; i++)
    {
      switch (g_test_rand_int_range (0, 6))
      {
        case 0:
          gsk_path_builder_line_to (builder,
                                    g_test_rand_double_range (-1000, 1000),
                                    g_test_rand_double_range (-1000, 1000));
          break;

        case 1:
          gsk_path_builder_rel_line_to (builder,
                                        g_test_rand_double_range (-1000, 1000),
                                        g_test_rand_double_range (-1000, 1000));
          break;

        case 2:
          gsk_path_builder_curve_to (builder,
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000));
          break;

        case 3:
          gsk_path_builder_rel_curve_to (builder,
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000));
          break;

        case 4:
          gsk_path_builder_conic_to (builder,
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000),
                                     g_test_rand_double_range (-1000, 1000),
                                     random_weight ());
          break;

        case 5:
          gsk_path_builder_rel_conic_to (builder,
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000),
                                         g_test_rand_double_range (-1000, 1000),
                                         random_weight ());
          break;

        default:
          g_assert_not_reached();
          break;
      }
    }

  if (g_test_rand_bit ())
    gsk_path_builder_close (builder);
}

static GskPath *
create_random_path (guint max_contours)
{
  GskPathBuilder *builder;
  guint i, n;

  /* 5% chance for a weird shape */
  if (g_test_rand_int_range (0, 20))
    return create_random_degenerate_path (max_contours);

  builder = gsk_path_builder_new ();
  n = g_test_rand_int_range (1, 10);
  n = MIN (n, max_contours);

  for (i = 0; i < n; i++)
    {
      /* 2/3 of shapes are standard contours */
      if (g_test_rand_int_range (0, 3))
        add_standard_contour (builder);
      else
        add_shape_contour (builder);
    }

  return gsk_path_builder_free_to_path (builder);
}

typedef struct {
  GskPathOperation op;
  graphene_point_t pts[4];
  float weight;
} PathOperation;

static void
_g_string_append_double (GString *string,
                         double   d)
{
  char buf[G_ASCII_DTOSTR_BUF_SIZE];

  g_ascii_dtostr (buf, G_ASCII_DTOSTR_BUF_SIZE, d);
  g_string_append (string, buf);
}

static void
_g_string_append_point (GString                *string,
                        const graphene_point_t *pt)
{
  _g_string_append_double (string, pt->x);
  g_string_append_c (string, ' ');
  _g_string_append_double (string, pt->y);
}

static void
path_operation_print (const PathOperation *p,
                      GString             *string)
{
  switch (p->op)
  {
    case GSK_PATH_MOVE:
      g_string_append (string, "M ");
      _g_string_append_point (string, &p->pts[0]);
      break;

    case GSK_PATH_CLOSE:
      g_string_append (string, " Z");
      break;

    case GSK_PATH_LINE:
      g_string_append (string, " L ");
      _g_string_append_point (string, &p->pts[1]);
      break;

    case GSK_PATH_CURVE:
      g_string_append (string, " C ");
      _g_string_append_point (string, &p->pts[1]);
      g_string_append (string, ", ");
      _g_string_append_point (string, &p->pts[2]);
      g_string_append (string, ", ");
      _g_string_append_point (string, &p->pts[3]);
      break;

    case GSK_PATH_CONIC:
      /* This is not valid SVG */
      g_string_append (string, " O ");
      _g_string_append_point (string, &p->pts[1]);
      g_string_append (string, ", ");
      _g_string_append_point (string, &p->pts[2]);
      g_string_append (string, ", ");
      _g_string_append_double (string, p->weight);
      break;

    default:
      g_assert_not_reached();
      return;
  }
}

static gboolean
path_operation_equal (const PathOperation *p1,
                      const PathOperation *p2,
                      float                epsilon)
{
  if (p1->op != p2->op)
    return FALSE;

  /* No need to compare pts[0] for most ops, that's just
   * duplicate work. */
  switch (p1->op)
    {
      case GSK_PATH_MOVE:
        return graphene_point_near (&p1->pts[0], &p2->pts[0], epsilon);

      case GSK_PATH_LINE:
      case GSK_PATH_CLOSE:
        return graphene_point_near (&p1->pts[1], &p2->pts[1], epsilon);

      case GSK_PATH_CURVE:
        return graphene_point_near (&p1->pts[1], &p2->pts[1], epsilon)
            && graphene_point_near (&p1->pts[2], &p2->pts[2], epsilon)
            && graphene_point_near (&p1->pts[3], &p2->pts[3], epsilon);

      case GSK_PATH_CONIC:
        return graphene_point_near (&p1->pts[1], &p2->pts[1], epsilon)
            && graphene_point_near (&p1->pts[2], &p2->pts[2], epsilon)
            && G_APPROX_VALUE (p1->weight, p2->weight, epsilon);

      default:
        g_return_val_if_reached (FALSE);
    }
}

static gboolean 
collect_path_operation_cb (GskPathOperation        op,
                           const graphene_point_t *pts,
                           gsize                   n_pts,
                           float                   weight,
                           gpointer                user_data)
{
  g_array_append_vals (user_data,
                       (PathOperation[1]) {
                         op,
                         {
                           GRAPHENE_POINT_INIT(pts[0].x, pts[0].y),
                           GRAPHENE_POINT_INIT(n_pts > 1 ? pts[1].x : 0,
                                               n_pts > 1 ? pts[1].y : 0),
                           GRAPHENE_POINT_INIT(n_pts > 2 ? pts[2].x : 0,
                                               n_pts > 2 ? pts[2].y : 0),
                           GRAPHENE_POINT_INIT(n_pts > 3 ? pts[3].x : 0,
                                               n_pts > 3 ? pts[3].y : 0)
                         },
                         weight
                       },
                       1);
  return TRUE;
}

static GArray *
collect_path (GskPath *path)
{
  GArray *array = g_array_new (FALSE, FALSE, sizeof (PathOperation));

  /* Use -1 here because we want all the flags, even future additions */
  gsk_path_foreach (path, -1, collect_path_operation_cb, array);

  return array;
}

static void
assert_path_equal_func (const char *domain,
                        const char *file,
                        int         line,
                        const char *func,
                        GskPath    *path1,
                        GskPath    *path2,
                        float       epsilon)
{
  GArray *ops1, *ops2;
  guint i;

  ops1 = collect_path (path1);
  ops2 = collect_path (path2);

  for (i = 0; i < MAX (ops1->len, ops2->len); i++)
    {
      PathOperation *op1 = i < ops1->len ? &g_array_index (ops1, PathOperation, i) : NULL;
      PathOperation *op2 = i < ops2->len ? &g_array_index (ops2, PathOperation, i) : NULL;

      if (op1 == NULL || op2 == NULL || !path_operation_equal (op1, op2, epsilon))
        {
          GString *string;
          guint j;

          /* Find the operation we start to print */
          for (j = i; j-- > 0; )
            {
              PathOperation *op = &g_array_index (ops1, PathOperation, j);
              if (op->op == GSK_PATH_MOVE)
                break;
              if (j + 3 == i)
                {
                  j = i - 1;
                  break;
                }
            }

          string = g_string_new (j == 0 ? "" : "... ");
          for (; j < i; j++)
            {
              PathOperation *op = &g_array_index (ops1, PathOperation, j);
              path_operation_print (op, string);
              g_string_append_c (string, ' ');
            }

          g_string_append (string, "\\\n    ");
          if (op1)
            {
              path_operation_print (op1, string);
              if (ops1->len > i + 1)
                g_string_append (string, " ...");
            }
          g_string_append (string, "\n    ");
          if (op1)
            {
              path_operation_print (op2, string);
              if (ops2->len > i + 1)
                g_string_append (string, " ...");
            }

          g_assertion_message (domain, file, line, func, string->str);

          g_string_free (string, TRUE);
        }
    }

  g_array_free (ops1, TRUE);
  g_array_free (ops2, TRUE);
}
#define assert_path_equal(p1,p2) assert_path_equal_func(G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, (p1),(p2), FLOAT_EPSILON)
#define assert_path_equal_with_epsilon(p1,p2, epsilon) \
    assert_path_equal_func(G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, (p1),(p2), (epsilon))

static void
test_create (void)
{
  GskPath *path1, *path2, *built;
  GskPathBuilder *builder;
  guint i;
  char *s;
  GString *str;

  for (i = 0; i < 1000; i++)
    {
      builder = gsk_path_builder_new ();
      path1 = create_random_path (G_MAXUINT);
      gsk_path_builder_add_path (builder, path1);
      path2 = create_random_path (G_MAXUINT);
      gsk_path_builder_add_path (builder, path2);
      built = gsk_path_builder_free_to_path (builder);

      str = g_string_new (NULL);
      gsk_path_print (path1, str);
      if (!gsk_path_is_empty (path1) && !gsk_path_is_empty (path2))
        g_string_append_c (str, ' ');
      gsk_path_print (path2, str);

      s = gsk_path_to_string (built);

      g_assert_cmpstr (s, ==, str->str);
      g_string_free (str, TRUE);
      g_free (s);

      gsk_path_unref (built);
      gsk_path_unref (path2);
      gsk_path_unref (path1);
    }
}

static void
test_segment_start (void)
{
  GskPath *path, *path1;
  GskPathMeasure *measure, *measure1;
  GskPathBuilder *builder;
  float epsilon, length;
  guint i;

  path = create_random_path (G_MAXUINT);
  measure = gsk_path_measure_new (path);
  length = gsk_path_measure_get_length (measure);
  epsilon = MAX (length / 1024, G_MINFLOAT);

  for (i = 0; i < 100; i++)
    {
      float seg_length = length * i / 100.0f;

      builder = gsk_path_builder_new ();
      gsk_path_builder_add_segment (builder, measure, 0, seg_length);
      path1 = gsk_path_builder_free_to_path (builder);
      measure1 = gsk_path_measure_new (path1);

      g_assert_cmpfloat_with_epsilon (seg_length, gsk_path_measure_get_length (measure1), epsilon);

      gsk_path_measure_unref (measure1);
      gsk_path_unref (path1);
    }

  gsk_path_measure_unref (measure);
  gsk_path_unref (path);
}

static void
test_segment_end (void)
{
  GskPath *path, *path1;
  GskPathMeasure *measure, *measure1;
  GskPathBuilder *builder;
  float epsilon, length;
  guint i;

  path = create_random_path (G_MAXUINT);
  measure = gsk_path_measure_new (path);
  length = gsk_path_measure_get_length (measure);
  epsilon = MAX (length / 1024, G_MINFLOAT);

  for (i = 0; i < 100; i++)
    {
      float seg_length = length * i / 100.0f;

      builder = gsk_path_builder_new ();
      gsk_path_builder_add_segment (builder, measure, length - seg_length, length);
      path1 = gsk_path_builder_free_to_path (builder);
      measure1 = gsk_path_measure_new (path1);

      g_assert_cmpfloat_with_epsilon (seg_length, gsk_path_measure_get_length (measure1), epsilon);

      gsk_path_measure_unref (measure1);
      gsk_path_unref (path1);
    }

  gsk_path_measure_unref (measure);
  gsk_path_unref (path);
}

static void
test_segment_chunk (void)
{
  GskPath *path, *path1;
  GskPathMeasure *measure, *measure1;
  GskPathBuilder *builder;
  float epsilon, length;
  guint i;

  path = create_random_path (G_MAXUINT);
  measure = gsk_path_measure_new (path);
  length = gsk_path_measure_get_length (measure);
  epsilon = MAX (length / 1024, G_MINFLOAT);

  for (i = 0; i <= 100; i++)
    {
      float seg_start = length * i / 200.0f;

      builder = gsk_path_builder_new ();
      gsk_path_builder_add_segment (builder, measure, seg_start, seg_start + length / 2);
      path1 = gsk_path_builder_free_to_path (builder);
      measure1 = gsk_path_measure_new (path1);

      g_assert_cmpfloat_with_epsilon (length / 2, gsk_path_measure_get_length (measure1), epsilon);

      gsk_path_measure_unref (measure1);
      gsk_path_unref (path1);
    }

  gsk_path_measure_unref (measure);
  gsk_path_unref (path);
}

static void
test_segment (void)
{
  GskPath *path, *path1, *path2, *path3;
  GskPathMeasure *measure, *measure1, *measure2, *measure3;
  GskPathBuilder *builder;
  guint i;
  float split1, split2, epsilon, length;

  for (i = 0; i < 1000; i++)
    {
      path = create_random_path (G_MAXUINT);
      measure = gsk_path_measure_new (path);
      length = gsk_path_measure_get_length (measure);
      /* chosen high enough to stop the testsuite from failing */
      epsilon = MAX (length / 256, 1.f / 1024);

      split1 = g_test_rand_double_range (0, length);
      split2 = g_test_rand_double_range (split1, length);

      builder = gsk_path_builder_new ();
      gsk_path_builder_add_segment (builder, measure, 0, split1);
      path1 = gsk_path_builder_free_to_path (builder);
      measure1 = gsk_path_measure_new (path1);

      builder = gsk_path_builder_new ();
      gsk_path_builder_add_segment (builder, measure, split1, split2);
      path2 = gsk_path_builder_free_to_path (builder);
      measure2 = gsk_path_measure_new (path2);

      builder = gsk_path_builder_new ();
      gsk_path_builder_add_segment (builder, measure, split2, length);
      path3 = gsk_path_builder_free_to_path (builder);
      measure3 = gsk_path_measure_new (path3);

      g_assert_cmpfloat_with_epsilon (split1, gsk_path_measure_get_length (measure1), epsilon);
      g_assert_cmpfloat_with_epsilon (split2 - split1, gsk_path_measure_get_length (measure2), epsilon);
      g_assert_cmpfloat_with_epsilon (length - split2, gsk_path_measure_get_length (measure3), epsilon);

      gsk_path_measure_unref (measure2);
      gsk_path_measure_unref (measure1);
      gsk_path_measure_unref (measure);
      gsk_path_unref (path2);
      gsk_path_unref (path1);
      gsk_path_unref (path);
    }
}

static void
test_get_point (void)
{
  static const guint max_contours = 5;
  static const float tolerance = 0.5;
  GskPath *path;
  GskPathMeasure *measure;
  guint n_discontinuities;
  float length, offset, last_offset;
  graphene_point_t point, last_point;
  guint i, j;

  for (i = 0; i < 10; i++)
    {
      path = create_random_path (max_contours);
      measure = gsk_path_measure_new_with_tolerance (path, tolerance);
      length = gsk_path_measure_get_length (measure);
      n_discontinuities = 0;

      gsk_path_measure_get_point (measure,
                                  0,
                                  &last_point,
                                  NULL);
      /* FIXME: anything we can test with tangents here? */
      last_offset = 0;

      for (j = 1; j <= 1024; j++)
        {
          offset = length * j / 1024;
          gsk_path_measure_get_point (measure,
                                      offset,
                                      &point,
                                      NULL);

          if (graphene_point_distance (&last_point, &point, NULL, NULL) > offset - last_offset + tolerance)
            {
              n_discontinuities++;
              g_assert_cmpint (n_discontinuities, <, max_contours);
            }

          last_offset = offset;
          last_point = point;
        }

      gsk_path_measure_unref (measure);
      gsk_path_unref (path);
    }
}

static void
test_closest_point (void)
{
  static const float tolerance = 0.5;
  GskPath *path, *path1, *path2;
  GskPathMeasure *measure, *measure1, *measure2;
  GskPathBuilder *builder;
  guint i, j;

  for (i = 0; i < 10; i++)
    {
      path1 = create_random_path (G_MAXUINT);
      measure1 = gsk_path_measure_new_with_tolerance (path1, tolerance);
      path2 = create_random_path (G_MAXUINT);
      measure2 = gsk_path_measure_new_with_tolerance (path2, tolerance);

      builder = gsk_path_builder_new ();
      gsk_path_builder_add_path (builder, path1);
      gsk_path_builder_add_path (builder, path2);
      path = gsk_path_builder_free_to_path (builder);
      measure = gsk_path_measure_new_with_tolerance (path, tolerance);

      for (j = 0; j < 100; j++)
        {
          graphene_point_t test = GRAPHENE_POINT_INIT (g_test_rand_double_range (-1000, 1000),
                                                       g_test_rand_double_range (-1000, 1000));
          graphene_point_t p1, p2, p;
          graphene_vec2_t t1, t2, t;
          float offset1, offset2, offset;
          float distance1, distance2, distance;
          gboolean found1, found2, found;

          found1 = gsk_path_measure_get_closest_point_full (measure1,
                                                            &test,
                                                            INFINITY,
                                                            &distance1,
                                                            &p1,
                                                            &offset1,
                                                            &t1);
          found2 = gsk_path_measure_get_closest_point_full (measure2,
                                                            &test,
                                                            INFINITY,
                                                            &distance2,
                                                            &p2,
                                                            &offset2,
                                                            &t2);
          found = gsk_path_measure_get_closest_point_full (measure,
                                                           &test,
                                                           INFINITY,
                                                           &distance,
                                                           &p,
                                                           &offset,
                                                           &t);

          if (found1 && (!found2 || distance1 < distance2 + tolerance))
            {
              g_assert_cmpfloat (distance1, ==, distance);
              g_assert_cmpfloat (p1.x, ==, p.x);
              g_assert_cmpfloat (p1.y, ==, p.y);
              g_assert_cmpfloat (offset1, ==, offset);
              g_assert_true (graphene_vec2_equal (&t1, &t));
            }
          else if (found2)
            {
              g_assert_cmpfloat (distance2, ==, distance);
              g_assert_cmpfloat (p2.x, ==, p.x);
              g_assert_cmpfloat (p2.y, ==, p.y);
              g_assert_cmpfloat_with_epsilon (offset2 + gsk_path_measure_get_length (measure1), offset, MAX (G_MINFLOAT, offset / 1024));
              g_assert_true (graphene_vec2_equal (&t2, &t));
            }
          else
            {
              g_assert (!found);
            }
        }

      gsk_path_measure_unref (measure2);
      gsk_path_measure_unref (measure1);
      gsk_path_measure_unref (measure);
      gsk_path_unref (path2);
      gsk_path_unref (path1);
      gsk_path_unref (path);
    }
}

static void
test_closest_point_for_point (void)
{
  static const float tolerance = 0.5;
  GskPath *path;
  GskPathMeasure *measure;
  float length, offset, closest_offset, distance;
  graphene_point_t point, closest_point;
  guint i, j;

  for (i = 0; i < 100; i++)
    {
      path = create_random_path (G_MAXUINT);
      if (gsk_path_is_empty (path))
        {
          /* empty paths have no closest point to anything */
          gsk_path_unref (path);
          continue;
        }

      measure = gsk_path_measure_new_with_tolerance (path, tolerance);
      length = gsk_path_measure_get_length (measure);

      for (j = 0; j < 100; j++)
        {
          offset = g_test_rand_double_range (0, length);

          gsk_path_measure_get_point (measure,
                                      offset,
                                      &point,
                                      NULL);
          g_assert_true (gsk_path_measure_get_closest_point_full (measure,
                                                                  &point,
                                                                  tolerance,
                                                                  &distance,
                                                                  &closest_point,
                                                                  &closest_offset,
                                                                  NULL));
          /* should be given due to the TRUE return above, but who knows... */
          g_assert_cmpfloat (distance, <=, tolerance);
          g_assert_cmpfloat (graphene_point_distance (&point, &closest_point, NULL, NULL), <=, tolerance);
          /* can't do == here because points may overlap if we're unlucky */
          g_assert_cmpfloat (closest_offset, <, offset + tolerance);
        }

      gsk_path_measure_unref (measure);
      gsk_path_unref (path);
    }
}

static void
test_parse (void)
{
  int i;

  for (i = 0; i < 1000; i++)
    {
      GskPath *path1, *path2;
      char *string1, *string2;

      path1 = create_random_path (G_MAXUINT);
      string1 = gsk_path_to_string (path1);
      g_assert_nonnull (string1);

      path2 = gsk_path_parse (string1);
      g_assert_nonnull (path2);
      string2 = gsk_path_to_string (path2);
      g_assert_nonnull (string2);

      assert_path_equal_with_epsilon (path1, path2, 1.f / 1024);

      gsk_path_unref (path2);
      gsk_path_unref (path1);
      g_free (string2);
      g_free (string1);
    }
}

#define N_PATHS 3
static void
test_in_fill_union (void)
{
  GskPath *path;
  GskPathMeasure *measure, *measures[N_PATHS];
  GskPathBuilder *builder;
  guint i, j, k;

  for (i = 0; i < 100; i++)
    {
      builder = gsk_path_builder_new ();
      for (k = 0; k < N_PATHS; k++)
        {
          path = create_random_path (G_MAXUINT);
          measures[k] = gsk_path_measure_new (path);
          gsk_path_builder_add_path (builder, path);
          gsk_path_unref (path);
        }
      path = gsk_path_builder_free_to_path (builder);
      measure = gsk_path_measure_new (path);
      gsk_path_unref (path);

      for (j = 0; j < 100; j++)
        {
          graphene_point_t test = GRAPHENE_POINT_INIT (g_test_rand_double_range (-1000, 1000),
                                                       g_test_rand_double_range (-1000, 1000));
          GskFillRule fill_rule;

          for (fill_rule = GSK_FILL_RULE_WINDING; fill_rule <= GSK_FILL_RULE_EVEN_ODD; fill_rule++)
            {
              guint n_in_fill = 0;
              gboolean in_fill;

              for (k = 0; k < N_PATHS; k++)
                {
                  if (gsk_path_measure_in_fill (measures[k], &test, GSK_FILL_RULE_EVEN_ODD))
                    n_in_fill++;
                }

              in_fill = gsk_path_measure_in_fill (measure, &test, GSK_FILL_RULE_EVEN_ODD);

              switch (fill_rule)
              {
                case GSK_FILL_RULE_WINDING:
                  if (n_in_fill == 0)
                    g_assert_false (in_fill);
                  else if (n_in_fill == 1)
                    g_assert_true (in_fill);
                  /* else we can't say anything because the winding rule doesn't give enough info */
                  break;

                case GSK_FILL_RULE_EVEN_ODD:
                  g_assert_cmpint (in_fill, ==, n_in_fill & 1);
                  break;

                default:
                  g_assert_not_reached ();
                  break;
              }
            }
        }

      gsk_path_measure_unref (measure);
      for (k = 0; k < N_PATHS; k++)
        {
          gsk_path_measure_unref (measures[k]);
        }
    }
}
#undef N_PATHS

/* This is somewhat sucky because using foreach breaks up the contours
 * (like rects and circles) and replaces everything with the standard
 * contour.
 * But at least it extensively tests the standard contour.
 */
static gboolean
rotate_path_cb (GskPathOperation        op,
                const graphene_point_t *pts,
                gsize                   n_pts,
                float                   weight,
                gpointer                user_data)
{
  GskPathBuilder **builders = user_data;

  switch (op)
  {
    case GSK_PATH_MOVE:
      gsk_path_builder_move_to (builders[0], pts[0].x, pts[0].y);
      gsk_path_builder_move_to (builders[1], pts[0].y, -pts[0].x);
      break;

    case GSK_PATH_CLOSE:
      gsk_path_builder_close (builders[0]);
      gsk_path_builder_close (builders[1]);
      break;

    case GSK_PATH_LINE:
      gsk_path_builder_line_to (builders[0], pts[1].x, pts[1].y);
      gsk_path_builder_line_to (builders[1], pts[1].y, -pts[1].x);
      break;

    case GSK_PATH_CURVE:
      gsk_path_builder_curve_to (builders[0], pts[1].x, pts[1].y, pts[2].x, pts[2].y, pts[3].x, pts[3].y);
      gsk_path_builder_curve_to (builders[1], pts[1].y, -pts[1].x, pts[2].y, -pts[2].x, pts[3].y, -pts[3].x);
      break;

    case GSK_PATH_CONIC:
      gsk_path_builder_conic_to (builders[0], pts[2].x, pts[2].y, pts[3].x, pts[3].y, weight);
      gsk_path_builder_conic_to (builders[1], pts[2].y, -pts[2].x, pts[3].y, -pts[3].x, weight);
      break;

    default:
      g_assert_not_reached ();
      return FALSE;
  }

  return TRUE;
}

static void
test_in_fill_rotated (void)
{
  GskPath *path;
  GskPathBuilder *builders[2];
  GskPathMeasure *measures[2];
  guint i, j;

#define N_FILL_RULES 2
  /* if this triggers, you added a new enum value to GskFillRule, so the define above needs
   * an update */
  g_assert_null (g_enum_get_value (g_type_class_ref (GSK_TYPE_FILL_RULE), N_FILL_RULES));

  for (i = 0; i < 100; i++)
    {
      path = create_random_path (G_MAXUINT);
      builders[0] = gsk_path_builder_new ();
      builders[1] = gsk_path_builder_new ();
      /* Use -1 here because we want all the flags, even future additions */
      gsk_path_foreach (path, -1, rotate_path_cb, builders);
      gsk_path_unref (path);

      path = gsk_path_builder_free_to_path (builders[0]);
      measures[0] = gsk_path_measure_new (path);
      gsk_path_unref (path);
      path = gsk_path_builder_free_to_path (builders[1]);
      measures[1] = gsk_path_measure_new (path);
      gsk_path_unref (path);

      for (j = 0; j < 100; j++)
        {
          GskFillRule fill_rule = g_random_int_range (0, N_FILL_RULES);
          float x = g_test_rand_double_range (-1000, 1000);
          float y = g_test_rand_double_range (-1000, 1000);
  
          g_assert_cmpint (gsk_path_measure_in_fill (measures[0], &GRAPHENE_POINT_INIT (x, y), fill_rule),
                           ==,
                           gsk_path_measure_in_fill (measures[1], &GRAPHENE_POINT_INIT (y, -x), fill_rule));
          g_assert_cmpint (gsk_path_measure_in_fill (measures[0], &GRAPHENE_POINT_INIT (y, x), fill_rule),
                           ==,
                           gsk_path_measure_in_fill (measures[1], &GRAPHENE_POINT_INIT (x, -y), fill_rule));
        }

      gsk_path_measure_unref (measures[0]);
      gsk_path_measure_unref (measures[1]);
    }
#undef N_FILL_RULES
}

static void
check_distance_at_position (GskPathMeasure *measure,
                            GskPathMeasure *measure2,
                            float           distance,
                            float           position)
{
  graphene_point_t p;
  graphene_point_t s;
  float d;

  gsk_path_measure_get_point (measure, position, &p, NULL);
  gsk_path_measure_get_closest_point (measure2, &p, &s);

  d = graphene_point_distance (&p, &s, NULL, NULL);
  g_assert_true (d <= distance + gsk_path_measure_get_tolerance (measure2));
}

static void
check_path_distance (GskPath *path,
                     GskPath *path2,
                     float    distance)
{
  GskPathMeasure *measure;
  GskPathMeasure *measure2;
  float length;
  float t;
  int i;

  measure = gsk_path_measure_new_with_tolerance (path, 0.1);
  measure2 = gsk_path_measure_new_with_tolerance (path2, 0.1);

  length = gsk_path_measure_get_length (measure);

  for (i = 0; i < 1000; i++)
    {
      t = g_test_rand_double_range (0, length);
      check_distance_at_position (measure, measure2, distance, t);
    }

  gsk_path_measure_unref (measure);
  gsk_path_measure_unref (measure2);
}

static gboolean
check_spaced_out (GskPathOperation        op,
                  const graphene_point_t *pts,
                  gsize                   n_pts,
                  float                   weight,
                  gpointer                user_data)
{
  float *distance = user_data;
  return graphene_point_distance (&pts[0], &pts[n_pts - 1], NULL, NULL) >= *distance;
}

static gboolean
path_is_spaced_out (GskPath *path,
                    float    distance)
{
  return gsk_path_foreach (path, GSK_PATH_FOREACH_ALLOW_CURVE | GSK_PATH_FOREACH_ALLOW_CONIC, check_spaced_out, &distance);
}

static void
test_path_stroke_distance (void)
{
  GskPath *path;
  GskPath *stroke_path;
  GskStroke *stroke;
  int i;
  float width = 10;

  stroke = gsk_stroke_new (width);
  gsk_stroke_set_line_cap (stroke, GSK_LINE_CAP_ROUND);
  gsk_stroke_set_line_join (stroke, GSK_LINE_JOIN_ROUND);

  i = 0;
  while (i < 1000)
    {
      path = create_random_path (1);
      /* We know the stroker can't robustly handle paths with
       * too close neighbouring points
       */
      if (!path_is_spaced_out (path, width / 2))
        continue;

      stroke_path = gsk_path_stroke (path, stroke);

      check_path_distance (path, stroke_path, width / 2);
      /* This relies on round joins */
      check_path_distance (stroke_path, path, width / 2);

      gsk_path_unref (stroke_path);
      gsk_path_unref (path);

      i++;
    }

  gsk_stroke_free (stroke);
}

int
main (int   argc,
      char *argv[])
{
  gtk_test_init (&argc, &argv, NULL);

  g_test_add_func ("/path/create", test_create);
  g_test_add_func ("/path/segment_start", test_segment_start);
  g_test_add_func ("/path/segment_end", test_segment_end);
  g_test_add_func ("/path/segment_chunk", test_segment_chunk);
  g_test_add_func ("/path/segment", test_segment);
  g_test_add_func ("/path/get_point", test_get_point);
  g_test_add_func ("/path/closest_point", test_closest_point);
  g_test_add_func ("/path/closest_point_for_point", test_closest_point_for_point);
  g_test_add_func ("/path/parse", test_parse);
  g_test_add_func ("/path/in-fill-union", test_in_fill_union);
  g_test_add_func ("/path/in-fill-rotated", test_in_fill_rotated);
  g_test_add_func ("/path/stroke/distance", test_path_stroke_distance);

  return g_test_run ();
}
