#ifndef PANELPLAYER_PLAYER_H
#define PANELPLAYER_PLAYER_H

#include <glib-object.h>
#include <playerctl/playerctl.h>

G_BEGIN_DECLS

#define PANELPLAYER_TYPE_PLAYER (panelplayer_player_get_type ())

G_DECLARE_FINAL_TYPE (PanelplayerPlayer, panelplayer_player, PANELPLAYER, PLAYER, GObject)

struct _PanelplayerPlayer
{
  GObject parent_instance;
  PlayerctlPlayer *player;
  PlayerctlPlayerName *player_name;
  gboolean is_playing;
};

/* Construction */
PanelplayerPlayer *panelplayer_player_new_from_name (PlayerctlPlayerName *name);

/* Signals */
void               panelplayer_player_update         (PanelplayerPlayer *self);

/* Playback control */
void               panelplayer_player_play          (PanelplayerPlayer *self);
void               panelplayer_player_pause         (PanelplayerPlayer *self);
void               panelplayer_player_play_pause    (PanelplayerPlayer *self);
void               panelplayer_player_stop          (PanelplayerPlayer *self);
void               panelplayer_player_next          (PanelplayerPlayer *self);
void               panelplayer_player_previous      (PanelplayerPlayer *self);

/* Getters */
gchar             *panelplayer_player_get_title     (PanelplayerPlayer *self);
gchar             *panelplayer_player_get_artist    (PanelplayerPlayer *self);
gchar             *panelplayer_player_get_album     (PanelplayerPlayer *self);
gint64             panelplayer_player_get_position  (PanelplayerPlayer *self);
gint64             panelplayer_player_get_duration  (PanelplayerPlayer *self);
gboolean           panelplayer_player_get_is_playing(PanelplayerPlayer *self);
gboolean           panelplayer_player_get_can_play  (PanelplayerPlayer *self);
gboolean           panelplayer_player_get_can_go_next(PanelplayerPlayer *self);
gboolean           panelplayer_player_get_can_go_previous(PanelplayerPlayer *self);
const gchar       *panelplayer_player_get_name      (PanelplayerPlayer *self);
PlayerctlPlayer   *panelplayer_player_get_raw       (PanelplayerPlayer *self);

G_END_DECLS

#endif /* PANELPLAYER_PLAYER_H */
