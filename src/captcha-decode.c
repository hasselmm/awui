#include "config.h"

#include <libawui/web.h>

int
main (int    argc,
      char **argv)
{
  GError    *error   = NULL;
  GdkPixbuf *captcha = NULL;
  char      *solution;
  int        i;

  g_type_init ();

  if (argc < 2)
    return 2;

  for (i = 1; i < argc; ++i)
    {
      captcha = gdk_pixbuf_new_from_file (argv[i], &error);

      if (!captcha)
        {
          g_printerr ("%s: %s: %s\n", argv[0], argv[i], error->message);
          g_error_free (error);
          return 1;
        }

      solution = aw_captcha_solve (captcha);
      g_print ("%s: %s\n", argv[i], solution);
      g_object_unref (captcha);
      g_free (solution);
    }

  return 0;
}
