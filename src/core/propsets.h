#pragma once

#include "../pol.h"

#include <rapidjson\include\rapidjson\document.h>
#include <rapidjson\include\rapidjson\writer.h>
#include <rapidjson\include\rapidjson\stringbuffer.h>

namespace pol {
	class PropSets {
	private:
		const rapidjson::Value* value;

	public:
		PropSets(const rapidjson::Value* value)
			:value(value) {

		}

		bool HasValue(const string& name) const;
		bool GetBool(const string& name) const;
		bool GetBool(const string& name, bool defaultValue) const;
		int GetInt(const string& name) const;
		int GetInt(const string& name, int defaultValue) const;
		Float GetFloat(const string& name) const;
		Float GetFloat(const string& name, Float defaultValue) const;
		string GetString(const string& name) const;
		string GetString(const string& name, string defaultValue) const;
		Vector3f GetVector3f(const string& name) const;
		Vector3f GetVector3f(const string& name, Vector3f defaultValue) const;
		Transform GetTransform(const string& name) const;
		Transform GetTransform(const string& name, Transform defaultValue) const;
		
	private:
		Vector3f getVector3f(const rapidjson::Value* v) const;
		Matrix4 getMatrix4(const rapidjson::Value* v) const;
	};
}