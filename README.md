# Panel Player

A media player controller plugin for XFCE4 Panel using playerctl.

## Features

- **Now Playing Display**: Shows current track title and artist
- **Progress Bar**: Visual playback progress with time display
- **Transport Controls**: Play/Pause, Next, Previous buttons
- **Multi-Player Support**: Automatically detects active MPRIS players
- **Auto-Update**: Real-time updates as tracks change

## Dependencies

- `libxfce4panel-2.0` (xfce4 panel library)
- `libxfce4ui-2` (xfce4 UI utilities)
- `playerctl-2.0` (media player control)
- `gtk+-3.0` (GTK widgets)
- `glib-2.0` (GLib utilities)
- `cmake` (build system)

## Building

```bash
# Create build directory and configure
mkdir build
cd build
cmake ..

# Build
make

# Install (optional)
sudo make install
```

Or using the Makefile wrapper:

```bash
make
sudo make install
```

## Usage

1. Build and install the plugin
2. Open XFCE Panel preferences
3. Add "Playerctl" plugin to your panel
4. Start playing music in any MPRIS-compatible player (Spotify, VLC, etc.)
5. The plugin will automatically detect and display the current track

## Supported Players

Any player that implements the MPRIS D-Bus interface, including:
- Spotify
- VLC
- mpv (with mpris plugin)
- Firefox/Chrome (for web audio)
- Rhythmbox
- Clementine
- And many more...

## License

GPL-3.0-or-later

## Development

The plugin consists of:

- `src/panelplayer-plugin.c` - Main plugin implementation
- `src/panelplayer-player.c` - Playerctl wrapper
- `panel-plugin/panelplayer.desktop.in` - Plugin registration
