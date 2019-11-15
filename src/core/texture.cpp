#include "texture.h"
#include "scene.h"

namespace pol {
	Texture::Texture(const PropSets& props, Scene& scene) {
		string textureName = props.GetString("name");
		scene.AddTexture(textureName, this);
	}
}