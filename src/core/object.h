#pragma once

#include "../pol.h"

namespace pol {
	//based class of all objects
	class PolObject {
	public:
		virtual ~PolObject();

		//return a brief string summary of the instance(for debugging purposes)
		virtual string ToString() const = 0;
	};
}