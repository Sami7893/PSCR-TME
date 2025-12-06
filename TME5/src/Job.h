#pragma once

#include "Scene.h"
#include "Image.h"
#include "Ray.h"

using namespace std;
namespace pr {

class Job {
public:
	virtual void run () = 0;
	virtual ~Job() {};
};

// Job concret : exemple


class PixelJob : public Job {
	void calcul (const Scene& scene, Image& img,int ligne,int y) {
		// le point de l'ecran par lequel passe ce rayon
        auto& screenPoint = scene.getScreenPoints()[ligne][y];
        // le rayon a inspecter
        Ray ray(scene.getCameraPos(), screenPoint);
        int targetSphere = scene.findClosestInter(ray);
        if (targetSphere != -1) {
        	const Sphere& obj = scene.getObject(targetSphere);
        	// pixel prend la couleur de l'objet
        	Color finalcolor = scene.computeColor(obj, ray);
        	// mettre a jour la couleur du pixel dans l'image finale.
        	img.pixel( y,ligne) = finalcolor;
		}
	}
	const Scene& scene_;
	Image& img_;
	int ligne_, y_;
public :
	PixelJob(const Scene& scene, Image& img,int ligne,int y) : scene_(scene), img_(img), ligne_(ligne), y_(y) {}
	void run () {
		calcul(scene_,img_,ligne_,y_);
	}
	~PixelJob(){}
};

}
