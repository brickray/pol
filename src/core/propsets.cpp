#include "propsets.h"

namespace pol {
	bool PropSets::HasValue(const string& name) const {
		return value->HasMember(name.c_str());
	}

	bool PropSets::GetBool(const string& name) const {
		return GetBool(name, true);
	}

	bool PropSets::GetBool(const string& name, bool defaultValue) const {
		if (value->HasMember(name.c_str()))
			return (*value)[name.c_str()].GetBool();

		return defaultValue;
	}

	int PropSets::GetInt(const string& name) const {
		return GetInt(name, 0);
	}

	int PropSets::GetInt(const string& name, int defaultValue) const {
		if (value->HasMember(name.c_str()))
			return (*value)[name.c_str()].GetInt();

		return defaultValue;
	}

	Float PropSets::GetFloat(const string& name) const {
		return GetFloat(name, 0);
	}

	Float PropSets::GetFloat(const string& name, Float defaultValue) const {
		if (value->HasMember(name.c_str()))
			return (*value)[name.c_str()].GetDouble();

		return defaultValue;
	}

	string PropSets::GetString(const string& name) const {
		return GetString(name, "");
	}

	string PropSets::GetString(const string& name, string defaultValue) const {
		if (value->HasMember(name.c_str()))
			return (*value)[name.c_str()].GetString();

		return defaultValue;
	}

	Vector3f PropSets::GetVector3f(const string& name) const {
		return GetVector3f(name, Vector3f::Zero());
	}

	Vector3f PropSets::GetVector3f(const string& name, Vector3f defaultValue) const {
		if (value->HasMember(name.c_str()))
			return getVector3f(&(*value)[name.c_str()]);

		return defaultValue;
	}

	Transform PropSets::GetTransform(const string& name) const {
		return GetTransform(name, Transform());
	}

	Transform PropSets::GetTransform(const string& name, Transform defaultValue) const {
		Transform ret = defaultValue;
		if (value->HasMember(name.c_str()))
			ret = Transform(getMatrix4(&(*value)[name.c_str()]));

		return ret;
	}

	Vector3f PropSets::getVector3f(const rapidjson::Value* v) const {
		Vector3f ret;
		rapidjson::Value::ConstValueIterator it = v->Begin();
		for (int i = 0; it != v->End(); ++it, ++i) {
			if (i >= 3) break;
			ret[i] = it->GetDouble();
		}

		return ret;
	}

	Matrix4 PropSets::getMatrix4(const rapidjson::Value* v) const {
		Matrix4 ret;
		rapidjson::Value::ConstValueIterator it = value->Begin();
		for (int i = 0; it != value->End(); ++it, ++i) {
			if (i >= 16) break;

			ret[i / 4][i % 4] = it->GetDouble();
		}

		return ret;
	}
}