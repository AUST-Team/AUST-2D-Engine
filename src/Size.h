#pragma once

#ifndef SIZE_H_
#define SIZE_H_

struct SDL_Texture;
struct SDL_FRect;

/**
* @brief Structure that holds a signed width and height.
*/
struct Size
{
	int w = 0;	    /// Width of the element.
	int h = 0;		/// Height of the element.

	bool operator==(const Size& other) const;
	bool operator!=(const Size& other) const;
	Size operator+(int val) const;
	Size operator*(int val) const;
	Size operator-(int val) const;
	Size operator/(int val) const;
	Size& operator+=(int val);
	Size& operator-=(int val);
	Size& operator*=(int val);
	Size& operator/=(int val);
};

/**
* @brief Structure that holds a float width and height.
*/
struct FSize
{
	float w = 0.0f;	/// Width of the element.
	float h = 0.0f;	/// Height of the element.

	bool operator==(const FSize& other) const;
	bool operator!=(const FSize& other) const;
	FSize operator+(float val) const;
	FSize operator*(float val) const;
	FSize operator-(float val) const;
	FSize operator/(float val) const;
	FSize& operator+=(float val);
	FSize& operator-=(float val);
	FSize& operator*=(float val);
	FSize& operator/=(float val);
};

/**
* @brief Structure that holds an unsigned width and height.
*/
struct USize
{
	unsigned int w = 0;		/// Width of the element.
	unsigned int h = 0;	/// Height of the element.

	bool operator==(const USize& other) const;
	bool operator!=(const USize& other) const;
	USize operator+(unsigned int val) const;
	USize operator*(unsigned int val) const;
	USize operator-(unsigned int val) const;
	USize operator/(unsigned int val) const;
	USize& operator+=(unsigned int val);
	USize& operator-=(unsigned int val);
	USize& operator*=(unsigned int val);
	USize& operator/=(unsigned int val);
};

/**
* @brief Scales the dimensions of a texture to the max allowed width and height, with a scale factor.
* 
* @param texture Pointer to the texture to scale.
* @param maxWidth Maximum width of the texture.
* @param maxHeight Maximum height of the texture.
* @param scaleFactor Scale factor for the dimensions.
* @param onlyScaleDown Flag if the size should only scaled down. Set to false if the texture can be scaled both up and down.
* 
* @return The scaled size.
*/
Size GetScaledDimensions(SDL_Texture* texture, float maxWidth, float maxHeight, float scaleFactor = 1.0f, bool onlyScaleDown = false);

/**
* @brief Scales the dimensions of a rectangle to the max height and width, with a scale factor.
* 
* @param srcRect The floating point rectangle of the source.
* @param maxWidth The maximum width of the rectangle.
* @param maxHeight The maximum height of the rectangle.
* @param scaleFactor Scale factor for dimensions.
* @param onlyScaleDown Flag if the size should only scaled down. Set to false if the texture can be scaled both up and down.
* 
* @return The scaled size.
*/
Size GetScaledDimensions(const SDL_FRect& srcRect, float maxWidth, float maxHeight, float scaleFactor = 1.0f, bool onlyScaleDown = false);

#endif // SIZE_H_