#include "config.h"
#include "captcha.h"
#include "captcha-font.h"

static int
count_pixels (const GdkPixbuf *pixbuf,
              int              channel)
{
  const guchar *pixels, *p;
  int           x, y, dp, nc;
  int           count = 0;

  pixels = gdk_pixbuf_get_pixels (pixbuf);
  dp     = gdk_pixbuf_get_rowstride (pixbuf);
  nc     = gdk_pixbuf_get_n_channels (pixbuf);

  for (y = gdk_pixbuf_get_height (pixbuf); y > 0; --y, pixels += dp)
    for (x = gdk_pixbuf_get_width (pixbuf), p = pixels; x > 0; --x, p += nc)
      if (p[channel])
        ++count;

  return count;
}

static int
guess_digit (GdkPixbuf       *captcha,
             const GdkPixbuf *font,
             int              x0,
             int              y0)
{
  int n_errors_min = G_MAXINT;
  int best_guess   = -1;
  int i, w, h;

  w = gdk_pixbuf_get_width (font) / 16;
  h = gdk_pixbuf_get_height (font);

  for (i = 0; i < 16; ++i)
    {
      GdkPixbuf *canvas, *tmp;
      int        n_errors;

      /* Sub pixbufs share their data with the source pixbuf.
       * As we want to modify the pixbuf a copy must be created.
       */
      tmp    = gdk_pixbuf_new_subpixbuf (captcha, x0, y0, w, h);
      canvas = gdk_pixbuf_copy (tmp);
      g_object_unref (tmp);

      gdk_pixbuf_composite (font, canvas, 0, 0, w, h, -w * i, 0,
                            1, 1, GDK_INTERP_NEAREST, 255);
      n_errors = count_pixels (canvas, 1); /* scan green channel */
      g_object_unref (canvas);

      if (n_errors < n_errors_min)
        {
          n_errors_min = n_errors;
          best_guess = i;
        }
    }

  return best_guess;
}

static void
find_captcha (GdkPixbuf *captcha,
              int        width,
              int        height,
              int       *x0,
              int       *y0)
{
  int n_pixels_max = -1;
  int x, y, xe, ye;

  xe = gdk_pixbuf_get_width  (captcha) - width;
  ye = gdk_pixbuf_get_height (captcha) - height;

  for (y = 0; y < ye; ++y)
    for (x = 0; x < xe; ++x)
      {
        int        n_pixels;
        GdkPixbuf *canvas;

        canvas = gdk_pixbuf_new_subpixbuf (captcha, x, y, width, height);
        n_pixels = count_pixels (canvas, 1); /* scan green channel */
        g_object_unref (canvas);

        if (n_pixels >= n_pixels_max)
          {
            n_pixels_max = n_pixels;
            *x0 = x; *y0 = y;
          }
      }
}

static int
find_bottom_margin (GdkPixbuf *pixbuf)
{
  int        y, dx, dy;
  int        n_pixels;
  GdkPixbuf *row;

  dx = gdk_pixbuf_get_width  (pixbuf);
  dy = gdk_pixbuf_get_height (pixbuf);

  for (y = dy - 1; y >= 0; --y)
  {
    row = gdk_pixbuf_new_subpixbuf (pixbuf, 0, y, dx, 1);
    n_pixels = count_pixels (row, 0); /* scan red channel */
    g_object_unref (row);

    if (n_pixels)
      break;
  }

  return dy - y - 1;
}

static int
find_top_margin (GdkPixbuf *pixbuf)
{
  int        y, dx, dy;
  int        n_pixels;
  GdkPixbuf *row;

  dx = gdk_pixbuf_get_width  (pixbuf);
  dy = gdk_pixbuf_get_height (pixbuf);

  for (y = 0; y < dy; ++y)
  {
    row = gdk_pixbuf_new_subpixbuf (pixbuf, 0, y, dx, 1);
    n_pixels = count_pixels (row, 0); /* scan red channel */
    g_object_unref (row);

    if (n_pixels)
      break;
  }

  return y;
}

static gboolean
strip_diagonal_line (GdkPixbuf *pixbuf)
{
  int     x0, y0 = 0;
  int     x1, y1 = 0;
  int     cx, cy, dp, nc;
  int     dx, dy, x, y, n;
  guint8 *pixels, *p = NULL;

  cx     = gdk_pixbuf_get_width      (pixbuf);
  cy     = gdk_pixbuf_get_height     (pixbuf);
  dp     = gdk_pixbuf_get_rowstride  (pixbuf);
  nc     = gdk_pixbuf_get_n_channels (pixbuf);
  pixels = gdk_pixbuf_get_pixels     (pixbuf);

  for (x0 = 0; x0 < cx; ++x0)
    for (y0 = 0, p = pixels + x0 * nc; y0 < cy; ++y0, p += dp)
      if (p[0] || p[1] || p[2])
        goto start_found;

  return FALSE;

start_found:

  for (x1 = cx - 1; x1 >= 0; --x1)
    for (y1 = 0, p = pixels + x1 * nc; y1 < cy; ++y1, p += dp)
      if (p[0] || p[1] || p[2])
        goto end_found;

  return FALSE;

end_found:
  dx = x1 - x0;
  dy = y1 - y0;
  n  = dx / 2;

  if (dy < 0)
    {
      dy = -dy;
      dp = -dp;
    }

  for (x = 0, y = y0, p = pixels + y0 * dp; x < cx; ++x, p += nc)
    {
      p[0] = p[1] = p[2] = 0;

      if ((n += dy) > dx)
        {
          n -= dx;
          p += dp;
        }
    }

  return TRUE;
}

char *
aw_captcha_solve (GdkPixbuf *captcha)
{
  GdkPixbuf *font = NULL;
  int        x0   = 0;
  int        y0   = 0;
  int        dx, dy, i;
  int        solution;

  g_return_val_if_fail (GDK_IS_PIXBUF (captcha), NULL);

  font = gdk_pixbuf_new_from_inline (G_N_ELEMENTS (aw_captcha_font),
                                     aw_captcha_font, FALSE, NULL);

  dx = gdk_pixbuf_get_width (font) / 16;
  dy = gdk_pixbuf_get_height (font)
     - find_bottom_margin (font)
     - find_top_margin (font);
  dy = MAX (dy, 0);

  captcha = gdk_pixbuf_copy (captcha);

  strip_diagonal_line (captcha);
  find_captcha (captcha, 5 * dx, dy, &x0, &y0);
  y0 -= find_top_margin (font);

  for (i = 0, solution = 0; i < 5; ++i)
    {
      solution *= 16;
      solution += guess_digit (captcha, font, x0, y0);
      x0       += gdk_pixbuf_get_width (font) / 16;
    }

  g_object_unref (captcha);
  g_object_unref (font);

  return g_strdup_printf ("%05x", solution);
}
