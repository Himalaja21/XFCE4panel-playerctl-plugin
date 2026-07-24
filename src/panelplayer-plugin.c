#include "panelplayer-plugin.h"

static void panelplayer_plugin_construct (XfcePanelPlugin *plugin);

XFCE_PANEL_PLUGIN_REGISTER (panelplayer_plugin_construct);

static gchar *
format_time (gint64 microseconds)
{
  gint64 seconds = microseconds / G_USEC_PER_SEC;
  gint minutes = seconds / 60;
  seconds %= 60;
  return g_strdup_printf ("%d:%02d", minutes, (int)seconds);
}

static PanelplayerPlugin *
get_plugin_data (XfcePanelPlugin *plugin)
{
  return g_object_get_data (G_OBJECT (plugin), "panelplayer-data");
}

static void
update_play_pause_button (PanelplayerPlugin *pp)
{
  const gchar *icon_name;

  if (pp->current_player && panelplayer_player_get_is_playing (pp->current_player))
    icon_name = "media-playback-pause";
  else
    icon_name = "media-playback-start";

  GtkImage *image = GTK_IMAGE (gtk_bin_get_child (GTK_BIN (pp->play_pause_button)));
  gtk_image_set_from_icon_name (image, icon_name, GTK_ICON_SIZE_SMALL_TOOLBAR);
}

static void
update_now_playing (PanelplayerPlugin *pp)
{
  g_autofree gchar *title = NULL;
  g_autofree gchar *artist = NULL;
  const gchar *player_name;
  g_autofree gchar *markup = NULL;
  g_autofree gchar *title_text = NULL;
  g_autofree gchar *artist_text = NULL;

  if (!pp->current_player)
    {
      gtk_widget_hide (pp->info_box);
      gtk_widget_hide (pp->controls_box);
      gtk_widget_show (pp->no_player_label);
      return;
    }

  gtk_widget_hide (pp->no_player_label);
  gtk_widget_show (pp->info_box);
  gtk_widget_show (pp->controls_box);

  title = panelplayer_player_get_title (pp->current_player);
  artist = panelplayer_player_get_artist (pp->current_player);
  player_name = panelplayer_player_get_name (pp->current_player);

  title_text = g_markup_escape_text (title && title[0] ? title : "Unknown Track", -1);
  artist_text = g_markup_escape_text (artist && artist[0] ? artist : "Unknown Artist", -1);

  markup = g_strdup_printf ("<small><b>%s</b></small>", title_text);
  gtk_label_set_markup (GTK_LABEL (pp->title_label), markup);
  g_free (markup);

  markup = g_strdup_printf ("<small>%s · %s</small>", artist_text, player_name);
  gtk_label_set_markup (GTK_LABEL (pp->artist_label), markup);

  update_play_pause_button (pp);

  gtk_widget_set_sensitive (pp->play_pause_button,
                            panelplayer_player_get_can_play (pp->current_player));
  gtk_widget_set_sensitive (pp->next_button,
                            panelplayer_player_get_can_go_next (pp->current_player));
  gtk_widget_set_sensitive (pp->prev_button,
                            panelplayer_player_get_can_go_previous (pp->current_player));
}

static void
update_progress (PanelplayerPlugin *pp)
{
  gint64 position, duration;

  if (!pp->current_player)
    return;

  position = panelplayer_player_get_position (pp->current_player);
  duration = panelplayer_player_get_duration (pp->current_player);

  if (duration > 0)
    {
      gdouble fraction = (gdouble)position / (gdouble)duration;
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (pp->progress_bar), fraction);

      g_autofree gchar *pos_str = format_time (position);
      g_autofree gchar *dur_str = format_time (duration);
      g_autofree gchar *tooltip = g_strdup_printf ("%s / %s", pos_str, dur_str);
      gtk_widget_set_tooltip_text (pp->progress_bar, tooltip);
    }
  else
    {
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (pp->progress_bar), 0.0);
      gtk_widget_set_tooltip_text (pp->progress_bar, "No track");
    }
}

static gboolean
on_update_timeout (gpointer user_data)
{
  PanelplayerPlugin *pp = user_data;
  update_progress (pp);
  return G_SOURCE_CONTINUE;
}

static void
on_name_appeared (PlayerctlPlayerManager *manager,
                  PlayerctlPlayerName    *name,
                  PanelplayerPlugin      *pp)
{
  g_autoptr (PanelplayerPlayer) pp_new = NULL;

  if (!pp->current_player)
    {
      pp_new = panelplayer_player_new_from_name (name);
      if (pp_new)
        {
          pp->current_player = g_steal_pointer (&pp_new);
          g_signal_connect_swapped (pp->current_player, "updated",
                                    G_CALLBACK (update_now_playing), pp);
          update_now_playing (pp);
          update_progress (pp);
        }
    }
}

static void
on_name_vanished (PlayerctlPlayerManager *manager,
                  PlayerctlPlayerName    *name,
                  PanelplayerPlugin      *pp)
{
  if (pp->current_player)
    {
      const gchar *current_name = panelplayer_player_get_name (pp->current_player);

      if (g_strcmp0 (name->name, current_name) == 0)
        {
          g_clear_object (&pp->current_player);
          update_now_playing (pp);
          update_progress (pp);
        }
    }
}

static void
on_play_pause_clicked (GtkButton         *button,
                       PanelplayerPlugin *pp)
{
  if (pp->current_player)
    panelplayer_player_play_pause (pp->current_player);
}

static void
on_next_clicked (GtkButton         *button,
                 PanelplayerPlugin *pp)
{
  if (pp->current_player)
    panelplayer_player_next (pp->current_player);
}

static void
on_prev_clicked (GtkButton         *button,
                 PanelplayerPlugin *pp)
{
  if (pp->current_player)
    panelplayer_player_previous (pp->current_player);
}

static GtkWidget *
create_control_button (const gchar *icon_name,
                       GCallback    callback,
                       gpointer     user_data)
{
  GtkWidget *button;
  GtkWidget *image;

  button = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
  gtk_widget_set_focus_on_click (button, FALSE);

  image = gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_container_add (GTK_CONTAINER (button), image);

  g_signal_connect (button, "clicked", callback, user_data);

  return button;
}

static void
panelplayer_plugin_free_data (XfcePanelPlugin *plugin,
                              PanelplayerPlugin *pp)
{
  if (pp->update_timeout_id > 0)
    {
      g_source_remove (pp->update_timeout_id);
      pp->update_timeout_id = 0;
    }

  g_clear_object (&pp->current_player);
  g_clear_object (&pp->manager);
  g_slice_free (PanelplayerPlugin, pp);
}

static void
panelplayer_plugin_construct (XfcePanelPlugin *plugin)
{
  PanelplayerPlugin *pp;
  g_autoptr (GError) error = NULL;

  pp = g_slice_new0 (PanelplayerPlugin);
  pp->plugin = plugin;
  g_object_set_data (G_OBJECT (plugin), "panelplayer-data", pp);

  g_signal_connect (plugin, "free-data",
                    G_CALLBACK (panelplayer_plugin_free_data), pp);

  /* Main box */
  pp->box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_container_add (GTK_CONTAINER (plugin), pp->box);

  /* Info box */
  pp->info_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (pp->box), pp->info_box, TRUE, TRUE, 0);

  pp->title_label = gtk_label_new (NULL);
  gtk_label_set_ellipsize (GTK_LABEL (pp->title_label), PANGO_ELLIPSIZE_END);
  gtk_label_set_max_width_chars (GTK_LABEL (pp->title_label), 20);
  gtk_label_set_xalign (GTK_LABEL (pp->title_label), 0.0);
  gtk_box_pack_start (GTK_BOX (pp->info_box), pp->title_label, FALSE, FALSE, 0);

  pp->artist_label = gtk_label_new (NULL);
  gtk_label_set_ellipsize (GTK_LABEL (pp->artist_label), PANGO_ELLIPSIZE_END);
  gtk_label_set_max_width_chars (GTK_LABEL (pp->artist_label), 20);
  gtk_label_set_xalign (GTK_LABEL (pp->artist_label), 0.0);
  gtk_box_pack_start (GTK_BOX (pp->info_box), pp->artist_label, FALSE, FALSE, 0);

  pp->progress_bar = gtk_progress_bar_new ();
  gtk_orientable_set_orientation (GTK_ORIENTABLE (pp->progress_bar),
                                  GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_size_request (pp->progress_bar, 100, -1);
  gtk_box_pack_start (GTK_BOX (pp->info_box), pp->progress_bar, TRUE, TRUE, 0);

  /* Controls box */
  pp->controls_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start (GTK_BOX (pp->box), pp->controls_box, FALSE, FALSE, 0);

  pp->prev_button = create_control_button ("media-skip-backward",
                                            G_CALLBACK (on_prev_clicked), pp);
  gtk_box_pack_start (GTK_BOX (pp->controls_box), pp->prev_button, FALSE, FALSE, 0);

  pp->play_pause_button = create_control_button ("media-playback-start",
                                                  G_CALLBACK (on_play_pause_clicked), pp);
  gtk_box_pack_start (GTK_BOX (pp->controls_box), pp->play_pause_button, FALSE, FALSE, 0);

  pp->next_button = create_control_button ("media-skip-forward",
                                            G_CALLBACK (on_next_clicked), pp);
  gtk_box_pack_start (GTK_BOX (pp->controls_box), pp->next_button, FALSE, FALSE, 0);

  /* No player label */
  pp->no_player_label = gtk_label_new ("No player");
  gtk_box_pack_start (GTK_BOX (pp->box), pp->no_player_label, TRUE, TRUE, 0);

  gtk_widget_hide (pp->info_box);
  gtk_widget_hide (pp->controls_box);

  /* Create player manager */
  pp->manager = playerctl_player_manager_new (&error);
  if (pp->manager)
    {
      g_signal_connect (pp->manager, "name-appeared",
                        G_CALLBACK (on_name_appeared), pp);
      g_signal_connect (pp->manager, "name-vanished",
                        G_CALLBACK (on_name_vanished), pp);

      /* Connect to already-running players */
      g_signal_connect (pp->manager, "player-added",
                        G_CALLBACK (on_name_appeared), pp);

      /* Connect to already-running players via player-names property */
      GList *player_names = NULL;
      g_object_get (G_OBJECT (pp->manager), "player-names", &player_names, NULL);
      for (GList *l = player_names; l != NULL; l = l->next)
        {
          PlayerctlPlayerName *pname = l->data;
          g_autoptr (PanelplayerPlayer) pp_new = panelplayer_player_new_from_name (pname);
          if (pp_new && !pp->current_player)
            {
              pp->current_player = g_steal_pointer (&pp_new);
              g_signal_connect_swapped (pp->current_player, "updated",
                                        G_CALLBACK (update_now_playing), pp);
              playerctl_player_manager_manage_player (pp->manager,
                                                      panelplayer_player_get_raw (pp->current_player));
              update_now_playing (pp);
              update_progress (pp);
            }
        }
    }
  else
    {
      g_warning ("Failed to create player manager: %s", error->message);
    }

  pp->update_timeout_id = g_timeout_add (1000, on_update_timeout, pp);

  gtk_widget_show_all (GTK_WIDGET (plugin));

  /* Hide no-player label if we already found a player */
  if (pp->current_player)
    {
      gtk_widget_hide (pp->no_player_label);
    }
  else
    {
      gtk_widget_hide (pp->info_box);
      gtk_widget_hide (pp->controls_box);
    }
}
