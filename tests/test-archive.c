#include "config.h"

#include <bzlib.h>
#include <glib.h>

static void
test_archive_bzip2 (void)
{
  GError    *error = NULL;
  char      *data  = NULL;
  gsize      length;

  char       buffer[16] = { 0, };
  bz_stream  strm       = { 0, };
  int        rc;

  if (!g_file_get_contents ("tests/data/hello-world.bz2", &data, &length, &error))
    {
      g_error ("%s: %s", G_STRLOC, error->message);
      g_assert_not_reached ();
    }

  g_assert_cmpuint (length, ==, 56);

  rc = BZ2_bzDecompressInit (&strm, FALSE, FALSE);
  g_assert_cmpint (BZ_OK, ==, rc);

  strm.next_in  = data;
  strm.avail_in = length;

  strm.next_out  = buffer;
  strm.avail_out = 5;

  rc = BZ2_bzDecompress (&strm);

  g_assert_cmpint (BZ_OK,  ==, rc);
  g_assert_cmpint (5,      ==, strm.total_out_lo32);
  g_assert_cmpstr (buffer, ==, "Hello");

  strm.avail_out = sizeof buffer - (strm.next_out - buffer);

  rc = BZ2_bzDecompress (&strm);

  g_assert_cmpint (BZ_STREAM_END, ==, rc);
  g_assert_cmpint (12,            ==, strm.total_out_lo32);
  g_assert_cmpstr (buffer,        ==, "Hello World\n");

  rc = BZ2_bzDecompressEnd (&strm);
  g_assert_cmpint (BZ_OK, ==, rc);

  g_free (data);
}

static void
read_file (bz_stream  *strm,
           const char *filename,
           gsize       filesize)
{
  char  buffer[512] = { 0, };
  gsize length;
  int   rc, i;

  strm->next_out  = buffer;
  strm->avail_out = sizeof buffer;

  rc = BZ2_bzDecompress (strm);

  g_assert_cmpint (BZ_OK,  ==, rc);
  g_assert_cmpint (0,      ==, strm->avail_out);
  g_assert_cmpstr (buffer, ==, filename);

  if (filename[0])
    {
      rc = sscanf (buffer + 124, "%011o", &length);

      g_assert_cmpint (rc,       ==, 1);
      g_assert_cmpint (filesize, ==, length);

      for (i = (length + 511) / 512; i > 0; --i)
        {
          strm->next_out  = buffer;
          strm->avail_out = sizeof buffer;

          rc = BZ2_bzDecompress (strm);

          g_assert_cmpint (BZ_OK, ==, rc);
          g_assert_cmpint (0,     ==, strm->avail_out);
        }
    }
}

static void
test_archive_tarball (void)
{
  bz_stream  strm  = { 0, };
  GError    *error = NULL;
  char      *data  = NULL;
  gsize      length;
  int        rc;

  if (!g_file_get_contents ("tests/data/export-all.tar.bz2",
                            &data, &length, &error))
    {
      g_error ("%s: %s", G_STRLOC, error->message);
      g_assert_not_reached ();
    }

  g_assert_cmpuint (length, ==, 142471);

  rc = BZ2_bzDecompressInit (&strm, FALSE, FALSE);
  g_assert_cmpint (BZ_OK, ==, rc);

  strm.next_in  = data;
  strm.avail_in = length;

  read_file (&strm, "starmap.csv",    54761);
  read_file (&strm, "alliances.csv",  42445);
  read_file (&strm, "planets.csv",   265690);
  read_file (&strm, "player.csv",     93133);
  read_file (&strm, "trade.csv",        285);
  read_file (&strm, "battles.csv",    19170);
  read_file (&strm, "",                   0);

  rc = BZ2_bzDecompressEnd (&strm);
  g_assert_cmpint (BZ_OK, ==, rc);

  g_free (data);
}

int
main (int    argc,
      char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/archive/bzip2",   test_archive_bzip2);
  g_test_add_func ("/archive/tarball", test_archive_tarball);

  return g_test_run ();
}
