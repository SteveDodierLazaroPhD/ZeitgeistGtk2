#include <gtk/gtk.h>

int
main (int argc, char *argv[])
{
  GtkWidget *w;
  GdkWindow *win;
  GdkRegion *region;
  GdkRectangle rect = { 10, 10, 100, 100 };

  gtk_init (&argc, &argv);

  w = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_widget_show (w);

  win = gtk_widget_get_window (w);

  region = gdk_region_rectangle (&rect);
  gdk_window_shape_combine_region (win, region, 0, 0);

  if (argc < 2)
    region = NULL;
  else if (strcmp (argv[1], "--empty") == 0)
    region = gdk_region_new ();
  else if (strcmp (argv[1], "--nonempty") == 0)
    region = gdk_region_rectangle (&rect);

  gdk_window_shape_combine_region (win, region, 0, 0);

  if (region)
    gdk_region_destroy (region);

  gtk_main ();

  return 0;
}

