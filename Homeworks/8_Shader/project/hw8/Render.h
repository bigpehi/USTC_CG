#pragma once

class Render {
public:
	Render();

	void prepare();
	
	void onRender(class RawModel*& model);
};