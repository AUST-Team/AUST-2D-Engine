#pragma once

#ifndef SDLMISCS_H_
#define SDLMISCS_H_

#include <string>
#include <filesystem>

struct SDL_Texture;
struct SDL_Surface;
struct SDL_Renderer;

/**
* @brief Namespace for SDL (textures, rendering) miscs.
*/
namespace SDLMiscs
{
	/**
	* @brief Function that loads an SDL_Texture (image).
	*
	* @param filePath The path to the file.
	* @param renderer Pointer to the SDL renderer
	*
	* @return The loaded SDL texture or nullptr in case of an error.
	*/
	SDL_Texture* LoadTexture(const std::filesystem::path& filePath, SDL_Renderer* renderer);

	/**
	* @brief Function that loads a surface.
	*
	* @param filePath The path to the file.
	*
	* @return The loaded SDL Surface or nullptr in case of an error.
	*/
	SDL_Surface* LoadSurface(const std::filesystem::path& filePath);

	SDL_Texture* LoadTexture(const std::string& filePath, SDL_Renderer* renderer) = delete;
	SDL_Texture* LoadTexture(const char* filePath, SDL_Renderer* renderer) = delete;
	SDL_Surface* LoadSurface(const std::string& filePath) = delete;
	SDL_Surface* LoadSurface(const char* filePath) = delete;
}

#endif // SDLMISCS_H_