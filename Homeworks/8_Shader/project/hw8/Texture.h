#pragma once

class Texture {
private:
	int tex_width;
	int tex_height;
	int tex_channels;
	float* texture_coord; // 纹理坐标
	int size;


public:
	unsigned int texture; // 纹理对象的ID
	Texture(const char* texture_path, float* _texture_coord, int _size);
	void use_Texture();
};