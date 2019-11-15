#pragma once

#include "../pol.h"
#include "propsets.h"

#include <functional>

namespace pol {
	//based class of all objects
	class PolObject {
	public:
		virtual ~PolObject();

		//return a brief string summary of the instance(for debugging purposes)
		virtual string ToString() const = 0;
	};

	class Scene;
	typedef std::function<PolObject * (const PropSets&, Scene& scene)> Constructor;
	class PolObjectFactory {
	private:
		static map<string, Constructor>* constructors;

	public:
		static void RegisterClass(const string& name, const Constructor& cons);
		static PolObject* CreateInstance(const string& name, const PropSets& props, Scene& scene);
	};

#define POL_REGISTER_CLASS(T/*class*/, name/*string of class*/)\
		T* Create##T(const PropSets& props, Scene& scene){\
			return new T(props, scene);\
		}\
		\
		static struct T##Creator{\
			T##Creator(){ PolObjectFactory::RegisterClass(name, Create##T); }\
		}__T##Creator;
}