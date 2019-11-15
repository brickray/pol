#include "object.h"

namespace pol {
	PolObject::~PolObject() {

	}

	map<string, Constructor>* PolObjectFactory::constructors = nullptr;
	void PolObjectFactory::RegisterClass(const string& name, const Constructor& cons) {
		if (!constructors) {
			constructors = new map<string, Constructor>();
		}

		if (constructors->find(name) != constructors->end()) {
			fprintf(stderr, "Object [\"%s\"] already registed\n", name.c_str());
			return;
		}

		(*constructors)[name] = cons;
	}

	PolObject* PolObjectFactory::CreateInstance(const string& name, const PropSets& props, Scene& scene) {
		if (constructors->find(name) == constructors->end()) {
			fprintf(stderr, "Object [\"%s\"] has no constructor, please register first\n", name.c_str());
			return nullptr;
		}

		//get constructor
		Constructor cons = (*constructors)[name];
		return cons(props, scene);
	}
}