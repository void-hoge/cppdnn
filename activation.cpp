#include "util.hpp"
#include "layer.hpp"
#include "binarize.hpp"
#include "activation.hpp"

Linear::Linear() {}
void Linear::update(flt learningrate) {}

tensor_t Linear::forward(tensor_t& data) {
	return data;
}

tensor_t Linear::backward(tensor_t& data) {
	return data;
}

Sigmoid::Sigmoid() {}
void Sigmoid::update(flt learningrate) {}

tensor_t Sigmoid::forward(tensor_t& data) {
	auto batchsize = data.size();
	auto len = data[0].size();
	auto ret = tensor_t(batchsize, vec_t(len));
#pragma omp parallel for
	for (std::size_t b = 0; b < batchsize; b++) {
		for (std::size_t i = 0; i < data[b].size(); i++) {
			ret[b][i] = 1.0/(std::exp(-data[b][i])+1.0);
		}
	}
	this->lastdata = ret;
	return ret;
}

tensor_t Sigmoid::backward(tensor_t& data) {
	assert(data.size() == this->lastdata.size());
	auto batchsize = data.size();
	auto len = data[0].size();
	auto ret = tensor_t(batchsize, vec_t(len));
#pragma omp parallel for
	for (std::size_t b = 0; b < batchsize; b++) {
		assert(this->lastdata[b].size() == data[b].size());
		for (std::size_t i = 0; i < data[b].size(); i++) {
			ret[b][i] = data[b][i] * (1.0-this->lastdata[b][i]) * this->lastdata[b][i];
		}
	}
	return ret;
}

SoftMax::SoftMax() {}
void SoftMax::update(flt learningrate) {}

tensor_t SoftMax::forward(tensor_t& data) {
	auto batchsize = data.size();
	auto len = data[0].size();
	auto ret = tensor_t(batchsize, vec_t(len));
#pragma omp parallel for
	for (std::size_t b = 0; b < batchsize; b++) {
		flt maxelm = *std::max_element(data[b].begin(), data[b].end());
		flt sum = 0.0;
		for (auto& num: data[b]) {
			sum += std::exp(num-maxelm);
		}
		for (std::size_t i = 0; i < data[b].size(); i++) {
			ret[b][i] = std::exp(data[b][i]-maxelm)/sum;
		}
	}
	return ret;
}

tensor_t SoftMax::backward(tensor_t& data) {
	return data;
}

ReLU::ReLU() {}
void ReLU::update(flt learningrate) {}

tensor_t ReLU::forward(tensor_t& data) {
	auto batchsize = data.size();
	auto len = data[0].size();
	this->lastdata = data;
	auto ret = tensor_t(batchsize, vec_t(len));
#pragma omp parallel for
	for (std::size_t b = 0; b < batchsize; b++) {
		for (std::size_t i = 0; i < data[b].size(); i++) {
			ret[b][i] = std::max((flt)0.0, data[b][i]);
		}
	}
	return ret;
}

tensor_t ReLU::backward(tensor_t& data) {
	assert(data.size() == this->lastdata.size());
	auto batchsize = data.size();
	auto len = data[0].size();
	auto ret = tensor_t(batchsize, vec_t(len));
#pragma omp parallel for
	for (std::size_t b = 0; b < batchsize; b++) {
		assert(data[b].size() == this->lastdata[b].size());
		for (std::size_t i = 0; i < data[b].size(); i++) {
			ret[b][i] = this->lastdata[b][i] > 0.0 ? data[b][i] : 0.0;
		}
	}
	return ret;
}

BinaryAct::BinaryAct() {}
void BinaryAct::update(flt learningrate) {}

tensor_t BinaryAct::forward(tensor_t& data) {
	auto batchsize = data.size();
	auto len = data[0].size();
	this->lastdata = data;
	auto ret = tensor_t(batchsize, vec_t(len));
#pragma omp parallel for
	for (std::size_t b = 0; b < batchsize; b++) {
		for (std::size_t i = 0; i < len; i++) {
			ret[b][i] = sign(data[b][i]);
		}
	}
	return ret;
}

tensor_t BinaryAct::backward(tensor_t& data) {
	auto batchsize = data.size();
	auto len = data[0].size();
	assert(batchsize == this->lastdata.size());
	auto ret = tensor_t(batchsize, vec_t(len));
#pragma omp parallel for
	for (std::size_t b = 0; b < batchsize; b++) {
		for (std::size_t i = 0; i < len; i++) {
			ret[b][i] = hard_tanh_back(this->lastdata[b][i]) * data[b][i];
		}
	}
	return ret;
}
