#include <algorithm>
#include <fstream>

#include <SDL.h>

struct Header {
    bool is_ascii;
    int width = -1;
    int height = -1;
    int max_val = -1;
};

struct Pixel {
    int r = -1;
    int g = -1;
    int b = -1;

    // Convert RGB values to BGR for SDL
    int rgb2bgr() {
        return (255 << 24) | (b << 16) | (g << 8) | r;
    }

    void reset() {
        r = -1;
        g = -1;
        b = -1;
    }
};

// The window to be rendered to
SDL_Window* gWindow = NULL;

// The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// Start up SDL and create a window
bool Init(int width, int height) {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    } else {
        // Create a window
        gWindow = SDL_CreateWindow("ppmviewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } else {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

// Free media and shut down SDL
void Close() {
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}

// Parse the PPM image into a pixmap array of 32-bit BGR values
unsigned int* ParsePPM(std::ifstream& file, Header& h) {
    char c;

    if (file.is_open()) {
        file.get(c);
        if (c != 'P') {
            printf("File is not in PPM format.\n");
        }
        file.get(c);
        if (c == '3') {
            h.is_ascii = true;
        } else if (c == '6') {
            h.is_ascii = false;
        } else {
            printf("Grayscale formats (P1, P4, P2, P5) are unsupported.\n");
            return NULL;
        }

        char num[10];
        int i = 0;
        int val;
        memset(num, 0, sizeof(num));

        while ((h.width == -1 || h.height == -1 || h.max_val == -1) && file.get(c)) {
            switch (c) {
                case '#':
                    while (file.get(c)) {
                        if (c == '\n') break;
                    }
                    break;
                case '\n':
                case ' ':
                    val = atoi(num);
                    if (val != 0) {
                        if (h.width == -1) {
                            h.width = val;
                        } else if (h.height == -1) {
                            h.height = val;
                        } else if (h.max_val == -1) {
                            h.max_val = val;
                        }

                        // Reset num
                        memset(num, 0, sizeof(num));
                        i = 0;
                    }
                    break;
                default:
                    if (isdigit(c)) {
                        num[i] = c;
                        i++;
                    }
                    break;
            }
        }

        unsigned int* pixmap = new unsigned int[h.width * h.height];
        int j = 0;
        memset(pixmap, 0, h.width * h.height * sizeof(unsigned int));
        Pixel p;

        while (file.get(c)) {
            if (h.is_ascii) {
                switch (c) {
                    case '#':
                        while (file.get(c)) {
                            if (c == '\n') break;
                        }
                        break;
                    case '\n':
                    case ' ':
                        val = atoi(num);
                        if (p.r == -1) {
                            p.r = std::min(val, h.max_val);
                        } else if (p.g == -1) {
                            p.g = std::min(val, h.max_val);;
                        } else if (p.b == -1) {
                            p.b = std::min(val, h.max_val);;

                            pixmap[j++] = p.rgb2bgr();

                            p.reset();
                        }

                        // Reset num
                        memset(num, 0, sizeof(num));
                        i = 0;
                        break;
                    default:
                        if (isdigit(c)) {
                            num[i] = c;
                            i++;
                        }
                        break;
                }
            } else {
                printf("P6 file format is unsupported.\n");
                return NULL;
            }
        }

        return pixmap;
    } else {
        printf("Cannot open specified file!\n");
        return NULL;
    }
}

int main(int argc, char* args[]) {
    if (argc < 2) {
        printf("Supply a file path as an argument. Example:\nppmviewer example\\file\\path.ppm\n");
        return 0;
    }

    std::string filePath = args[1];

    std::ifstream file(filePath);
    Header h;
    unsigned int* pixmap = ParsePPM(file, h);
    file.close();

    // Unable to parse the PPM image
    if (!pixmap) {
        return 0;
    }

    if (!Init(h.width, h.height)) {
        printf("Failed to initialize!\n");
    } else {
        bool quit = false;

        SDL_Event e;

        Uint32 rmask, gmask, bmask, amask;
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0;

        int depth, pitch;
        depth = 32;
        pitch = 4 * h.width;

        SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(pixmap, h.width, h.height, depth, pitch,
                                                     rmask, gmask, bmask, amask);

        // Continue looping and drawing the image until the user quits
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }

            SDL_BlitSurface(surf, NULL, gScreenSurface, NULL);

            SDL_UpdateWindowSurface(gWindow);
        }
    }

    delete pixmap;
    Close();

    return 0;
}
