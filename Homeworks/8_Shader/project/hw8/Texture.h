#pragma once

class Texture {
private:
	int tex_width;
	int tex_height;
	int tex_channels;
	float* texture_coord; // ��������
	int size;


public:
	unsigned int texture; // ��������ID
	Texture(const char* texture_path, float* _texture_coord, int _size);
	void use_Texture();
};