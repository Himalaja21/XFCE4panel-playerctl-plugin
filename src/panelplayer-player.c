#include "panelplayer-player.h"

G_DEFINE_TYPE (PanelplayerPlayer, panelplayer_player, G_TYPE_OBJECT)

enum
{
  SIGNAL_UPDATED,
  N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0 };

enum
{
  PROP_0,
  PROP_PLAYER_NAME,
  N_PROPS
};

static GParamSpec *properties[N_PROPS] = { NULL };

static void
panelplayer_player_finalize (GObject *object)
{
  PanelplayerPlayer *self = PANELPLAYER_PLAYER (object);

  g_clear_object (&self->player);
  if (self->player_name)
    g_boxed_free (PLAYERCTL_TYPE_PLAYER_NAME, self->player_name);

  G_OBJECT_CLASS (panelplayer_player_parent_class)->finalize (object);
}

static void
panelplayer_player_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  PanelplayerPlayer *self = PANELPLAYER_PLAYER (object);

  switch (prop_id)
    {
    case PROP_PLAYER_NAME:
      g_value_set_pointer (value, self->player_name);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panelplayer_player_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  PanelplayerPlayer *self = PANELPLAYER_PLAYER (object);

  switch (prop_id)
    {
    case PROP_PLAYER_NAME:
      if (self->player_name)
        g_boxed_free (PLAYERCTL_TYPE_PLAYER_NAME, self->player_name);
      self->player_name = g_value_get_pointer (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panelplayer_player_class_init (PanelplayerPlayerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = panelplayer_player_finalize;
  object_class->get_property = panelplayer_player_get_property;
  object_class->set_property = panelplayer_player_set_property;

  signals[SIGNAL_UPDATED] =
    g_signal_new ("updated",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL,
                  G_TYPE_NONE, 0);

  properties[PROP_PLAYER_NAME] =
    g_param_spec_pointer ("player-name", NULL, NULL,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
panelplayer_player_init (PanelplayerPlayer *self)
{
  self->is_playing = FALSE;
}

static void
on_player_metadata_changed (PlayerctlPlayer    *player,
                            GVariant           *metadata,
                            PanelplayerPlayer  *self)
{
  g_signal_emit (self, signals[SIGNAL_UPDATED], 0);
}

static void
on_player_playback_status_changed (PlayerctlPlayer       *player,
                                   PlayerctlPlaybackStatus status,
                                   PanelplayerPlayer      *self)
{
  self->is_playing = (status == PLAYERCTL_PLAYBACK_STATUS_PLAYING);
  g_signal_emit (self, signals[SIGNAL_UPDATED], 0);
}

PanelplayerPlayer *
panelplayer_player_new_from_name (PlayerctlPlayerName *name)
{
  PanelplayerPlayer *self;
  g_autoptr (GError) error = NULL;

  g_return_val_if_fail (name != NULL, NULL);

  self = g_object_new (PANELPLAYER_TYPE_PLAYER,
                       "player-name", name,
                       NULL);

  self->player = playerctl_player_new_from_name (name, &error);
  if (!self->player)
    {
      g_warning ("Failed to create player: %s", error->message);
      g_object_unref (self);
      return NULL;
    }

  /* Connect to player signals */
  g_signal_connect (self->player, "metadata",
                    G_CALLBACK (on_player_metadata_changed), self);
  g_signal_connect (self->player, "playback-status",
                    G_CALLBACK (on_player_playback_status_changed), self);

  /* Get initial state */
  PlayerctlPlaybackStatus status;
  g_object_get (self->player, "playback-status", &status, NULL);
  self->is_playing = (status == PLAYERCTL_PLAYBACK_STATUS_PLAYING);

  return self;
}

void
panelplayer_player_update (PanelplayerPlayer *self)
{
  g_return_if_fail (PANELPLAYER_IS_PLAYER (self));
  g_signal_emit (self, signals[SIGNAL_UPDATED], 0);
}

void
panelplayer_player_play (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;

  g_return_if_fail (PANELPLAYER_IS_PLAYER (self));

  playerctl_player_play (self->player, &error);
  if (error)
    g_warning ("Failed to play: %s", error->message);
}

void
panelplayer_player_pause (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;

  g_return_if_fail (PANELPLAYER_IS_PLAYER (self));

  playerctl_player_pause (self->player, &error);
  if (error)
    g_warning ("Failed to pause: %s", error->message);
}

void
panelplayer_player_play_pause (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;

  g_return_if_fail (PANELPLAYER_IS_PLAYER (self));

  playerctl_player_play_pause (self->player, &error);
  if (error)
    g_warning ("Failed to toggle play/pause: %s", error->message);
}

void
panelplayer_player_stop (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;

  g_return_if_fail (PANELPLAYER_IS_PLAYER (self));

  playerctl_player_stop (self->player, &error);
  if (error)
    g_warning ("Failed to stop: %s", error->message);
}

void
panelplayer_player_next (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;

  g_return_if_fail (PANELPLAYER_IS_PLAYER (self));

  playerctl_player_next (self->player, &error);
  if (error)
    g_warning ("Failed to go to next: %s", error->message);
}

void
panelplayer_player_previous (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;

  g_return_if_fail (PANELPLAYER_IS_PLAYER (self));

  playerctl_player_previous (self->player, &error);
  if (error)
    g_warning ("Failed to go to previous: %s", error->message);
}

gchar *
panelplayer_player_get_title (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;
  gchar *title;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), NULL);

  title = playerctl_player_get_title (self->player, &error);
  if (error)
    {
      g_warning ("Failed to get title: %s", error->message);
      return g_strdup ("");
    }

  return title;
}

gchar *
panelplayer_player_get_artist (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;
  gchar *artist;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), NULL);

  artist = playerctl_player_get_artist (self->player, &error);
  if (error)
    {
      g_warning ("Failed to get artist: %s", error->message);
      return g_strdup ("");
    }

  return artist;
}

gchar *
panelplayer_player_get_album (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;
  gchar *album;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), NULL);

  album = playerctl_player_get_album (self->player, &error);
  if (error)
    {
      g_warning ("Failed to get album: %s", error->message);
      return g_strdup ("");
    }

  return album;
}

gint64
panelplayer_player_get_position (PanelplayerPlayer *self)
{
  g_autoptr (GError) error = NULL;
  gint64 position;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), 0);

  position = playerctl_player_get_position (self->player, &error);
  if (error)
    {
      g_warning ("Failed to get position: %s", error->message);
      return 0;
    }

  return position;
}

gint64
panelplayer_player_get_duration (PanelplayerPlayer *self)
{
  g_autoptr (GVariant) metadata = NULL;
  g_autoptr (GVariant) length_var = NULL;
  gint64 duration = 0;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), 0);

  /* Get metadata from player properties */
  g_object_get (self->player, "metadata", &metadata, NULL);

  if (metadata)
    {
      length_var = g_variant_lookup_value (metadata, "mpris:length", G_VARIANT_TYPE_INT64);
      if (length_var)
        duration = g_variant_get_int64 (length_var);
    }

  return duration;
}

gboolean
panelplayer_player_get_is_playing (PanelplayerPlayer *self)
{
  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), FALSE);
  return self->is_playing;
}

gboolean
panelplayer_player_get_can_play (PanelplayerPlayer *self)
{
  gboolean can_play;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), FALSE);

  g_object_get (self->player, "can-play", &can_play, NULL);
  return can_play;
}

gboolean
panelplayer_player_get_can_go_next (PanelplayerPlayer *self)
{
  gboolean can_go_next;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), FALSE);

  g_object_get (self->player, "can-go-next", &can_go_next, NULL);
  return can_go_next;
}

gboolean
panelplayer_player_get_can_go_previous (PanelplayerPlayer *self)
{
  gboolean can_go_previous;

  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), FALSE);

  g_object_get (self->player, "can-go-previous", &can_go_previous, NULL);
  return can_go_previous;
}

const gchar *
panelplayer_player_get_name (PanelplayerPlayer *self)
{
  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), NULL);

  if (self->player_name)
    return self->player_name->name;

  return "Unknown";
}

PlayerctlPlayer *
panelplayer_player_get_raw (PanelplayerPlayer *self)
{
  g_return_val_if_fail (PANELPLAYER_IS_PLAYER (self), NULL);
  return self->player;
}
