#include "dielectric.h"

namespace pol {
	Dielectric::Dielectric(Texture* specular, Float etai, Float etat)
		:specular(specular), etai(etai), etat(etat) {

	}

	//dielectic is a delta bsdf
	bool Dielectric::IsDelta() const {
		return true;
	}

	void Dielectric::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		Float ei = etai, et = etat;
		Float cosi = Frame::CosTheta(in);
		bool enter = cosi > 0;
		if (!enter) swap(ei, et);

		//snell's law
		//sini*etai = sint*etat
		Float eta = ei / et;
		Float sint = Frame::SinTheta(in) * eta;
		if (sint >= 1) {
			//here is total internal reflection
			//the energy of refraction is zero
			//calculate reflection direction
			Vector3f reflectDir = Vector3f(-in.X(), in.Y(), -in.Z());
			out = reflectDir;
			fr = specular->Evaluate(isect) / Frame::AbsCosTheta(out);
			pdf = 1;
			return;
		}

		//calculate fresnel coefficient
		Float cost = sqrt(1 - sint * sint);
		Float fresnel = DielectricFresnel(abs(cosi), cost, ei, et);
		if (u.x < fresnel) {
			//reflection part
			//calculate reflection direction
			Vector3f reflectDir = Vector3f(-in.X(), in.Y(), -in.Z());
			out = reflectDir;
			fr = fresnel * specular->Evaluate(isect) / abs(cosi);
			pdf = fresnel;
		}
		else {
			//refraction part
			//calculate refraction direction
			if (enter) cost = -cost;
			Vector3f refractDir = Vector3f(-in.X() * eta, cost, -in.Z() * eta);
		
			out = refractDir;
			fr = (1 - fresnel) * eta * eta * specular->Evaluate(isect) / abs(cost);
			pdf = 1 - fresnel;
		}
	}

	void Dielectric::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		pdf = 0;
		return;
	}

	string Dielectric::ToString() const {
		string ret;
		ret += "Dielectric[\n  specular = " + specular->ToString()
			+ ",\n  etai = " + to_string(etai)
			+ ",\n  etat = " + to_string(etat)
			+ "\n]";

		return ret;
	}

	Dielectric* CreateDielectricBsdf(Texture* specular, Float etai, Float etat) {
		return new Dielectric(specular, etai, etat);
	}
}