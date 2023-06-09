#pragma once

#include "util.hpp"

class Layer {
private:
public:
	virtual tensor_t forward(tensor_t& data) = 0;
	virtual tensor_t backward(tensor_t& data) = 0;
	virtual void update(flt learningrate) = 0;
};
