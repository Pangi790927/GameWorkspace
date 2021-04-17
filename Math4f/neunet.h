#ifndef NEUNET_H
#define NEUNET_H

#include <functional>
#include "MathLib.h"

struct Layer {

	using FnType = std::function <double (double)>;
	FnType f;
	Math::VMatrix<double> w;
	Math::VMatrix<double> b;
	Math::VMatrix<double> a;
	Math::VMatrix<double> h;

	Math::VMatrix<double> apply_f(const Math::VMatrix<double>& a) {
		int no_neurons = a.size()[0];
		Math::VMatrix<double> ret = a; 
		for (int i = 0; i < no_neurons; i++) {
			ret(i) = f(a(i));
		}
		return ret;
	}
};

struct Neunet {

	Neunet () {
		add_layer([](double x) {return x;}, 0, 0);
	}

	std::vector<Layer> layers;

	Math::VMatrix<double> backpropagate(const Math::VMatrix<double>& x) {

			//Aici vom face backpropagation.
			//Ziua a 7-a, scriu la Backpropagation
			//Ziua a 16-a, Andrei inca crede ca scriu cu advearat cod 
			//Ziua 100,, ii dau push pe Git, ca sa ramana in istorie
		




	}
	
	Math::VMatrix<double> feedforward(const Math::VMatrix<double>& x) {
		layers[0].h = x;
		for(int i = 1; i < layers.size(); i++) {
			layers[i].a = layers[i].w * layers[i-1].h + layers[i].b;
			layers[i].h = layers[i].apply_f(layers[i].a);  
		}	
		return layers[layers.size()-1].h;
	}
	void add_layer(Layer::FnType f, int w_n, int w_m) {
		Layer layer;
		layer.w = Math::VMatrix<double> (w_n, w_m);
		layer.b = Math::VMatrix<double> (w_n);
		layer.f = f;
		layers.push_back(layer);
	}
};

#endif