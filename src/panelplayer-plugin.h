#ifndef PANELPLAYER_PLUGIN_H
#define PANELPLAYER_PLUGIN_H

#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
#include <playerctl/playerctl.h>

#include "panelplayer-player.h"

G_BEGIN_DECLS

typedef struct
{
  XfcePanelPlugin *plugin;

  /* Player management */
  PlayerctlPlayerManager *manager;
  PanelplayerPlayer *current_player;

  /* UI widgets */
  GtkWidget *box;
  GtkWidget *controls_box;
  GtkWidget *info_box;
  GtkWidget *prev_button;
  GtkWidget *play_pause_button;
  GtkWidget *next_button;
  GtkWidget *title_label;
  GtkWidget *artist_label;
  GtkWidget *progress_bar;
  GtkWidget *no_player_label;

  /* State update timer */
  guint update_timeout_id;
} PanelplayerPlugin;

G_END_DECLS

#endif /* PANELPLAYER_PLUGIN_H */
