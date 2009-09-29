#include <gd.h>
#include <gdfontg.h>

int
main (int    argc,
      char **argv)
{
  unsigned char  hex[] = "0123456789abcdef";
  FILE          *file;
  gdFont        *font;
  gdImage       *image;
  int            fg, bg;

  file = fopen (argc > 1 ? argv[1] : "data/captcha-font.png", "w");

  if (!file)
    return 1;

  font  = gdFontGetGiant ();
  image = gdImageCreate (16 * font->w, font->h);

  bg = gdImageColorAllocateAlpha (image, 0, 0, 0, gdAlphaTransparent);
  fg = gdImageColorAllocateAlpha (image, 255, 0, 0, gdAlphaOpaque);

  gdImageString (image, font, 0, 0, hex, fg);

  gdImagePng (image, file);
  gdImageDestroy (image);
  gdFreeFontCache ();
  fclose (file);

  return 0;
}
