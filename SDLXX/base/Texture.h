#ifndef SDLXX_TEXTURE_H
#define SDLXX_TEXTURE_H

#include <string>
#include <SDL_render.h>
#include <SDL_image.h>
#include "../ttf/Font.h"

namespace SDLXX {
    class Texture {
    public:
        Texture(SDL_Texture *t) {
            texture = t;
            SDL_QueryTexture(t, &format, &access, &width, &height);
        }

        Texture(const std::string &path, SDL_Renderer *renderer, int w, int h) {
            SDL_Surface *surface = IMG_Load(path.c_str());
            if(surface == nullptr) {
                throw Exception("Unable to load image", IMG_GetError());
            }
            SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0xFF, 0xFF));
            SDL_Rect stretchRect;
            stretchRect.x = 0;
            stretchRect.y = 0;
            stretchRect.w = w;
            stretchRect.h = h;
            //FIXME: chack size of surface aster bliting
            Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#endif

            SDL_Surface *scaled = SDL_CreateRGBSurface(0, w, h, 32,
                                           rmask, gmask, bmask, amask);
            SDL_SetColorKey(scaled, SDL_TRUE, SDL_MapRGB(scaled->format, 0, 0xFF, 0xFF));
            SDL_BlitScaled(surface, NULL, scaled, &stretchRect);

            SDL_FreeSurface(surface);

            texture = SDL_CreateTextureFromSurface(renderer, scaled);
            if(texture == nullptr) {
                throw Exception("Unable to create texture", SDL_GetError());
            }
            width = scaled->w;
            height = scaled->h;
            SDL_FreeSurface(scaled);
        }

        Texture(const std::string &text, const Color &color, const Font &font, SDL_Renderer *renderer) {
            Surface surface = font.render(text, TTF_MODE_BLENDED, color);
            texture = SDL_CreateTextureFromSurface(renderer, surface.getSDLSurface());
            if(texture == nullptr) {
                throw Exception("Unable to create texture", SDL_GetError());
            }
            width = surface.getWidth();
            height = surface.getHeight();
        }

        ~Texture() {
            width = 0;
            height = 0;
            access = SDL_TEXTUREACCESS_STATIC;
            format = SDL_PIXELFORMAT_UNKNOWN;
            if(texture != nullptr) {
                SDL_DestroyTexture(texture);
                texture = nullptr;
            }
        }

        void setColor(Uint8 red, Uint8 green, Uint8 blue) {
            SDL_SetTextureColorMod(texture, red, green, blue);
        }

        void setBlendMode(SDL_BlendMode blending) {
            SDL_SetTextureBlendMode(texture, blending);
        }

        void setAlpha(Uint8 alpha) {
            SDL_SetTextureAlphaMod(texture, alpha);
        }

        //Renders texture at given point
        void render(SDL_Renderer *renderer, SDL_Rect *clip = nullptr, SDL_Rect *dest = nullptr, double angle = 0.0,
                    SDL_Point *center = nullptr,
                    SDL_RendererFlip flip = SDL_FLIP_NONE) {
            //Set rendering space and render to screen


            //Render to screen
            SDL_RenderCopyEx(renderer, texture, clip, dest, angle, center, flip);
        }

        int getWidth() const {
            return width;
        }

        int getHeight() const {
            return height;
        }

        SDL_Texture *getSDLTexture() const {
            return texture;
        }

    private:
        SDL_Texture *texture = nullptr;
        int width = 0;
        int height = 0;
        int access = SDL_TEXTUREACCESS_STATIC;
        Uint32 format = SDL_PIXELFORMAT_UNKNOWN;
    };
}

#endif // SDLXX_TEXTURE_H
