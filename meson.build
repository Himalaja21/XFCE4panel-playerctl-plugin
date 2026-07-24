project('panelplayer', 'c',
  version: '0.1.0',
  license: 'GPL-3.0-or-later',
  meson_version: '>= 0.56.0',
  default_options: [
    'warning_level=2',
    'c_std=gnu11',
  ])

# Find dependencies
libxfce4panel = dependency('libxfce4panel-2.0', version: '>= 4.14.0')
libxfce4ui = dependency('libxfce4ui-2', version: '>= 4.14.0')
playerctl = dependency('playerctl', version: '>= 2.0.0')
gtk = dependency('gtk+-3.0', version: '>= 3.22.0')
glib = dependency('glib-2.0', version: '>= 2.50.0')

# Plugin sources
plugin_sources = files(
  'src/panelplayer-plugin.c',
  'src/panelplayer-player.c',
)

# Build plugin shared library
plugin_library = shared_library('panelplayer',
  plugin_sources,
  dependencies: [
    libxfce4panel,
    libxfce4ui,
    playerctl,
    gtk,
    glib,
  ],
  install: true,
  install_dir: join_paths(get_option('libdir'), 'xfce4', 'panel', 'plugins'),
  name_prefix: '',
)

# Install desktop file
install_data(
  'panel-plugin/panelplayer.desktop.in',
  install_dir: join_paths(get_option('datadir'), 'xfce4', 'panel', 'plugins'),
  rename: 'panelplayer.desktop',
)
