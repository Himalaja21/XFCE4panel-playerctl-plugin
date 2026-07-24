# Panel Player

I cant find a genuine playerctl plugin for xfce panel, so I made one. You can do whatever you want with it.
At default its only working horizontally, in the "panelplayer-plugin.c" you can change that to vertical.

## Features

- **Now Playing Display**: Shows current track title and artist
- **Progress Bar**: Visual playback progress with time display
- **Transport Controls**: Play/Pause, Next, Previous buttons
- **Multi-Player Support**: Automatically detects active MPRIS players

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

If it won't show up at the item after the build then just add it to the panel plugin directories.
```bash
/usr/local/lib/xfce4/panel/plugins/libpanelplayer.so
/usr/local/share/xfce4/panel/plugins/panelplayer.desktop
/usr/local/share/icons/hicolor/scalable/apps/playerctl.svg
```
## Usage

1. Build and install the plugin
2. Open XFCE Panel preferences
3. Add "Playerctl" plugin to your panel
4. Start playing music in any MPRIS-compatible player (Spotify, VLC, etc.)
5. The plugin will automatically detect and display the current track

## Supported Players

Any player that implements the MPRIS D-Bus interface



