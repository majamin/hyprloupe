#include <cstdint>
#include <format>
#include <regex>
#include <charconv>
#include <strings.h>

#include <iostream>

#include "hyprloupe.hpp"
#include "src/debug/Log.hpp"

static void help() {
    std::cout << "Hyprloupe usage: hyprloupe [arg [...]].\n\nArguments:\n"
              << " -a | --autocopy            | Automatically copies the output to the clipboard (requires wl-clipboard)\n"
              << " -f | --format=fmt          | Specifies the output format (cmyk, hex, rgb, hsl, hsv)\n"
              << " -o | --output-format=fmt   | Specifies how the output color should be formatted e.g. rgb({0}, {1}, {2}) would output rgb(red, green, blue) if --format=rgb\n"
              << " -n | --notify              | Sends a desktop notification when a color is picked (requires notify-send and a notification daemon like dunst)\n"
              << " -b | --no-fancy            | Disables the \"fancy\" (aka. colored) outputting\n"
              << " -h | --help                | Show this help message\n"
              << " -r | --render-inactive     | Render (freeze) inactive displays\n"
              << " -z | --no-zoom             | Disable the zoom lens\n"
              << " -q | --quiet               | Disable most logs (leaves errors)\n"
              << " -v | --verbose             | Enable more logs\n"
              << " -t | --no-fractional       | Disable fractional scaling support\n"
              << " -d | --disable-preview     | Disable live preview of color\n"
              << " -c | --cursor              | Include cursor in the frozen preview\n"
              << " -l | --lowercase-hex       | Outputs the hexcode in lowercase\n"
              << " -s | --scale=scale         | Set the zoom scale (between 1 and 10)\n"
              << " -u | --radius=radius       | Set the circle radius (between 1 and 1000)\n"
              << " -D | --dim=alpha           | Dim the area outside the loupe (0.0 to 1.0)\n"
              << " -V | --version             | Print version info\n";
}

int main(int argc, char** argv, char** envp) {
    g_pHyprloupe = std::make_unique<CHyprloupe>();

    while (true) {
        int                  option_index   = 0;
        static struct option long_options[] = {{"autocopy", no_argument, nullptr, 'a'},
                                               {"format", required_argument, nullptr, 'f'},
                                               {"output-format", required_argument, nullptr, 'o'},
                                               {"help", no_argument, nullptr, 'h'},
                                               {"no-fancy", no_argument, nullptr, 'b'},
                                               {"notify", no_argument, nullptr, 'n'},
                                               {"render-inactive", no_argument, nullptr, 'r'},
                                               {"no-zoom", no_argument, nullptr, 'z'},
                                               {"no-fractional", no_argument, nullptr, 't'},
                                               {"quiet", no_argument, nullptr, 'q'},
                                               {"verbose", no_argument, nullptr, 'v'},
                                               {"disable-preview", no_argument, nullptr, 'd'},
                                               {"cursor", no_argument, nullptr, 'c'},
                                               {"lowercase-hex", no_argument, nullptr, 'l'},
                                               {"version", no_argument, nullptr, 'V'},
                                               {"scale", required_argument, nullptr, 's'},
                                               {"radius", required_argument, nullptr, 'u'},
                                               {"dim", required_argument, nullptr, 'D'},
                                               {nullptr, 0, nullptr, 0}};

        int                  c = getopt_long(argc, argv, ":f:o:hnbarzqvtdlcVs:u:D:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'f':
                if (strcasecmp(optarg, "cmyk") == 0)
                    g_pHyprloupe->m_bSelectedOutputMode = OUTPUT_CMYK;
                else if (strcasecmp(optarg, "hex") == 0)
                    g_pHyprloupe->m_bSelectedOutputMode = OUTPUT_HEX;
                else if (strcasecmp(optarg, "rgb") == 0)
                    g_pHyprloupe->m_bSelectedOutputMode = OUTPUT_RGB;
                else if (strcasecmp(optarg, "hsl") == 0)
                    g_pHyprloupe->m_bSelectedOutputMode = OUTPUT_HSL;
                else if (strcasecmp(optarg, "hsv") == 0)
                    g_pHyprloupe->m_bSelectedOutputMode = OUTPUT_HSV;
                else {
                    Debug::log(NONE, "Unrecognized format %s", optarg);
                    exit(1);
                }
                break;
            case 'o': g_pHyprloupe->m_sOutputFormat = optarg; break;
            case 'h': help(); exit(0);
            case 'b': g_pHyprloupe->m_bFancyOutput = false; break;
            case 'n': g_pHyprloupe->m_bNotify = true; break;
            case 'a': g_pHyprloupe->m_bAutoCopy = true; break;
            case 'r': g_pHyprloupe->m_bRenderInactive = true; break;
            case 'z': g_pHyprloupe->m_bNoZoom = true; break;
            case 't': g_pHyprloupe->m_bNoFractional = true; break;
            case 'q': Debug::quiet = true; break;
            case 'v': Debug::verbose = true; break;
            case 'd': g_pHyprloupe->m_bDisablePreview = true; break;
            case 'c': g_pHyprloupe->m_bIncludeCursor = true; break;
            case 'l': g_pHyprloupe->m_bUseLowerCase = true; break;
            case 'V': {
                std::cout << "hyprloupe v" << HYPRLOUPE_VERSION << "\n";
                exit(0);
            }
            case 's': {
                float value;
#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION < 200000
                try {
                    size_t pos;
                    value = std::stof(optarg, &pos);
                    if (pos != strlen(optarg))
                        throw std::invalid_argument("");
                } catch (...) {
                    std::cerr << "Invalid scale value: " << optarg << "\n";
                    exit(1);
                }
#else
                auto  result = std::from_chars(optarg, optarg + strlen(optarg), value);

                if (result.ec != std::errc() || result.ptr != optarg + strlen(optarg)) {
                    std::cerr << "Invalid scale value: " << optarg << "\n";
                    exit(1);
                }
#endif

                if (value < 1.0f || value > 10.0f) {
                    std::cerr << "Scale must be between 1 and 10!\n";
                    exit(1);
                }

                g_pHyprloupe->m_fZoomScale = value;
                break;
            }

            case 'u': {
                int  value;
                auto result = std::from_chars(optarg, optarg + strlen(optarg), value);

                if (result.ec != std::errc() || result.ptr != optarg + strlen(optarg)) {
                    std::cerr << "Invalid radius value: " << optarg << "\n";
                    exit(1);
                }

                if (value < 1 || value > 1000) {
                    std::cerr << "Radius must be between 1 and 1000!\n";
                    exit(1);
                }

                g_pHyprloupe->m_iCircleRadius = value;
                break;
            }

            case 'D': {
                float value;
                auto  result = std::from_chars(optarg, optarg + strlen(optarg), value);

                if (result.ec != std::errc() || result.ptr != optarg + strlen(optarg)) {
                    std::cerr << "Invalid dim value: " << optarg << "\n";
                    exit(1);
                }

                if (value < 0.0f || value > 1.0f) {
                    std::cerr << "Dim alpha must be between 0.0 and 1.0!\n";
                    exit(1);
                }

                g_pHyprloupe->m_fDimAlpha = value;
                break;
            }
            default: help(); exit(1);
        }
    }

    if (!isatty(fileno(stdout)) || getenv("NO_COLOR"))
        g_pHyprloupe->m_bFancyOutput = false;

    if (g_pHyprloupe->m_sOutputFormat.empty()) {
        switch (g_pHyprloupe->m_bSelectedOutputMode) {
            case OUTPUT_CMYK: g_pHyprloupe->m_sOutputFormat = "{}% {}% {}% {}%"; break;
            case OUTPUT_HEX: g_pHyprloupe->m_sOutputFormat = "#{}{}{}"; break;
            case OUTPUT_RGB: g_pHyprloupe->m_sOutputFormat = "{} {} {}"; break;
            case OUTPUT_HSL: g_pHyprloupe->m_sOutputFormat = "{} {}% {}%"; break;
            case OUTPUT_HSV: g_pHyprloupe->m_sOutputFormat = "{} {}% {}%"; break;
        }
    }
    try {
        std::array<uint8_t, 4> dummy = {0, 0, 0, 0};
        (void)std::vformat(g_pHyprloupe->m_sOutputFormat, std::make_format_args(dummy[0], dummy[1], dummy[2], dummy[3]));
    } catch (const std::format_error& e) {
        Debug::log(NONE, "Invalid --output-format: %s", e.what());
        exit(1);
    }

    g_pHyprloupe->init();

    return 0;
}
