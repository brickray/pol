#include "parser.h"
#include "propsets.h"
#include "scene.h"
#include "directory.h"
#include "../shape/triangle.h"

#include <fstream>
#include <sys\stat.h>

namespace pol {
	int GetFileLength(const char* filename) {
		struct stat st;
		stat(filename, &st);
		return st.st_size;  
	}

	bool Parser::Parse(const string& file, Scene& scene) {
		//directory of file
		string base = file.substr(0, file.find_last_of('/') + 1);
		Directory::base = base;
		fstream in(file.c_str());
		if (!in.good()) {
			fprintf(stderr, "Scene file [\"%s\"] is not good\n", file.c_str());
			return false;
		}

		int fileSize = GetFileLength(file.c_str());
		char* buffer = new char[fileSize + 1];
		memset(buffer, 0, fileSize + 1);
		in.read(buffer, fileSize);
		in.close();

		rapidjson::Document doc;
		doc.Parse(buffer);
		if (doc.HasParseError()) {
			fprintf(stderr, "Parse scene error: %d\n", doc.GetParseError());
			return false;
		}

		//parse camera
		if (doc.HasMember("camera")) {
			rapidjson::Value& camera = doc["camera"];
			string type = camera["type"].GetString();
			PropSets props(&camera);
			PolObjectFactory::CreateInstance(type, props, scene);
		}

		//parse integrator
		if (doc.HasMember("integrator")) {
			rapidjson::Value& integrator = doc["integrator"];
			string type = integrator["type"].GetString();
			PropSets props(&integrator);
			PolObjectFactory::CreateInstance(type, props, scene);
		}

		//parse accelerator
		if (doc.HasMember("accelerator")) {
			rapidjson::Value& accelerator = doc["accelerator"];
			string type = accelerator["type"].GetString();
			PropSets props(&accelerator);
			PolObjectFactory::CreateInstance(type, props, scene);
		}

		//parse sampler
		if (doc.HasMember("sampler")) {
			rapidjson::Value& sampler = doc["sampler"];
			string type = sampler["type"].GetString();
			PropSets props(&sampler);
			PolObjectFactory::CreateInstance(type, props, scene);
		}

		//parse texture
		if (doc.HasMember("texture")) {
			rapidjson::Value& textures = doc["texture"];
			if (!textures.IsArray()) {
				fprintf(stderr, "Texture parse error\n");
				exit(1);
			}

			rapidjson::Value::ValueIterator it = textures.Begin();
			rapidjson::Value::ValueIterator ited = textures.End();
			for (; it != ited; ++it) {
				string type = (*it)["type"].GetString();
				PropSets props(&(*it));
				PolObjectFactory::CreateInstance(type, props, scene);
			}
		}

		//parse material
		if (doc.HasMember("material")) {
			rapidjson::Value& materials = doc["material"];
			if (!materials.IsArray()) {
				fprintf(stderr, "Materials parse error\n");
				exit(1);
			}

			rapidjson::Value::ValueIterator it = materials.Begin();
			rapidjson::Value::ValueIterator ited = materials.End();
			for (; it != ited; ++it) {
				string type = (*it)["type"].GetString();
				PropSets props(&(*it));
				PolObjectFactory::CreateInstance(type, props, scene);
			}
		}

		//parse primitive
		if (doc.HasMember("primitive")) {
			rapidjson::Value& primitives = doc["primitive"];
			if (!primitives.IsArray()) {
				fprintf(stderr, "Primitives parse error\n");
				exit(1);
			}

			rapidjson::Value::ValueIterator it = primitives.Begin();
			rapidjson::Value::ValueIterator ited = primitives.End();
			for (; it != ited; ++it) {
				string type = (*it)["type"].GetString();
				PropSets props(&(*it));
				PolObjectFactory::CreateInstance(type, props, scene);
			}
		}

		//parse light
		if (doc.HasMember("light")) {
			rapidjson::Value& lights = doc["light"];
			if (!lights.IsArray()) {
				fprintf(stderr, "Lights parse error\n");
				exit(1);
			}

			rapidjson::Value::ValueIterator it = lights.Begin();
			rapidjson::Value::ValueIterator ited = lights.End();
			for (; it != ited; ++it) {
				string type = (*it)["type"].GetString();
				PropSets props(&(*it));
				Light* light = dynamic_cast<Light*>(PolObjectFactory::CreateInstance(type, props, scene));
				if (type == "area") {
					rapidjson::Value& shapeValue = (*it)["shape"];
					string shapeType = shapeValue["type"].GetString();
					PropSets shapeProps(&shapeValue);
					PolObject* object = PolObjectFactory::CreateInstance(shapeType, shapeProps, scene);
					if (shapeType != "trianglemesh") {
						Shape* shape = dynamic_cast<Shape*>(object);
						shape->SetLight(light);
						light->SetShape(shape);
					}
					else {
						TriangleMesh* mesh = dynamic_cast<TriangleMesh*>(object);
						for (int i = 0; i < mesh->triangles.size(); ++i) {
							if (i == 0) {
								mesh->triangles[i]->SetLight(light);
								light->SetShape(mesh->triangles[i]);
							}
							else {
								Light* light = dynamic_cast<Light*>(PolObjectFactory::CreateInstance(type, props, scene));
								mesh->triangles[i]->SetLight(light);
								light->SetShape(mesh->triangles[i]);
							}
						}

						mesh->triangles.clear();
					}
				}
			}
		}

		string lightSampleStrategy = "spatial";
		if (doc.HasMember("global")) {
			PropSets props(&doc["global"]);
			lightSampleStrategy = props.GetString("lightSampleStrategy", "spatial");
		}

		scene.Prepare(lightSampleStrategy);
	}
}