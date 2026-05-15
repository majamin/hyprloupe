# hyprloupe

Based on [hyprpicker](https://github.com/hyprwm/hyprpicker).

A screen loupe (magnifier) for wlroots-based Wayland compositors, with built-in color picking.

[Watch demo](demo.mp4)

## Usage

Launch it. A magnified loupe follows your cursor. Click to pick a color. Press Escape to exit without picking.

```
hyprloupe [options]
```

## Options

```
 -a | --autocopy            | Automatically copy the picked color to clipboard (requires wl-clipboard)
 -f | --format=fmt          | Output format: cmyk, hex, rgb, hsl, hsv (default: hex)
 -o | --output-format=fmt   | Custom output template e.g. rgb({0}, {1}, {2})
 -n | --notify              | Send a desktop notification on color pick (requires notify-send)
 -b | --no-fancy            | Disable colored output
 -r | --render-inactive     | Freeze and render inactive displays
 -z | --no-zoom             | Disable the zoom loupe
 -d | --disable-preview     | Disable live color preview
 -c | --cursor              | Include cursor in the frozen preview
 -l | --lowercase-hex       | Output hex in lowercase
 -s | --scale=scale         | Loupe zoom scale, 1–10 (default: 10)
 -u | --radius=radius       | Loupe circle radius in pixels, 1–1000 (default: 100)
 -D | --dim=alpha           | Dim the area outside the loupe, 0.0–1.0 (default: 0.0)
 -t | --no-fractional       | Disable fractional scaling support
 -q | --quiet               | Suppress most logs
 -v | --verbose             | Enable verbose logging
 -h | --help                | Show help
 -V | --version             | Print version
```

### Example — loupe as magnifier with dimming

```
hyprloupe -s 2 -u 500 -D 0.5
```

### Example — color picker with autocopy

```
hyprloupe -a -f hex
```

## Building

Dependencies: cmake, pkg-config, pango, cairo, wayland, wayland-protocols, hyprutils, xkbcommon

```sh
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -S . -B ./build
cmake --build ./build --config Release --target hyprloupe -j$(nproc)
sudo cmake --install ./build
```

## Compositor support

Requires `wlr-screencopy-unstable-v1` and `wlr-layer-shell-unstable-v1`.

Works on: **Hyprland**, **Sway**, **river**, **wayfire**, and other wlroots-based compositors.

Does not currently support KDE (KWin) or GNOME (Mutter).

## Attribution

hyprloupe is a fork of [hyprpicker](https://github.com/hyprwm/hyprpicker) by Hypr Development,
used under the BSD 3-Clause License. The original copyright notice is preserved in [LICENSE](LICENSE).

## License

BSD 3-Clause License. See [LICENSE](LICENSE).

Original work copyright (c) 2022, Hypr Development.  
Modifications copyright (c) 2026, Marian Minar.
