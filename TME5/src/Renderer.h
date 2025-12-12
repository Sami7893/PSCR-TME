#pragma once

#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include "Pool.h"
#include "Job.h"
#include <thread>
#include <vector>

using namespace std;
namespace pr {

// Classe pour rendre une scène dans une image
class Renderer {
public:
    // Rend la scène dans l'image
    void render(const Scene& scene, Image& img) {
        // les points de l'ecran, en coordonnées 3D, au sein de la Scene.
        // on tire un rayon de l'observateur vers chacun de ces points
        const Scene::screen_t& screen = scene.getScreenPoints();

        // pour chaque pixel, calculer sa couleur
        for (int x = 0; x < scene.getWidth(); x++) {
            for (int y = 0; y < scene.getHeight(); y++) {
                // le point de l'ecran par lequel passe ce rayon
                auto& screenPoint = screen[y][x];
                // le rayon a inspecter
                Ray ray(scene.getCameraPos(), screenPoint);

                int targetSphere = scene.findClosestInter(ray);

                if (targetSphere == -1) {
                    // keep background color
                    continue;
                } else {
                    const Sphere& obj = scene.getObject(targetSphere);
                    // pixel prend la couleur de l'objet
                    Color finalcolor = scene.computeColor(obj, ray);
                    // mettre a jour la couleur du pixel dans l'image finale.
                    img.pixel(x, y) = finalcolor;
                }
            }
        }
    }

    void renderThreadPerPixel(const Scene& scene, Image& img){
        // les points de l'ecran, en coordonnées 3D, au sein de la Scene.
        // on tire un rayon de l'observateur vers chacun de ces points
        const Scene::screen_t& screen = scene.getScreenPoints();
        vector<thread> threads;
        threads.reserve(scene.getHeight() *scene.getWidth() );
        // pour chaque pixel, calculer sa couleur
        for (int x = 0; x < scene.getWidth(); x++) {
            for (int y = 0; y < scene.getHeight(); y++) {
                threads.emplace_back([&,x,y](){

                // le point de l'ecran par lequel passe ce rayon
                auto& screenPoint = screen[y][x];
                // le rayon a inspecter
                Ray ray(scene.getCameraPos(), screenPoint);

                int targetSphere = scene.findClosestInter(ray);

                if (targetSphere != -1) {
                    const Sphere& obj = scene.getObject(targetSphere);
                    // pixel prend la couleur de l'objet
                    Color finalcolor = scene.computeColor(obj, ray);
                    // mettre a jour la couleur du pixel dans l'image finale.
                    img.pixel(x, y) = finalcolor;
                }
                });
            }
        }
        for(auto &t : threads){
            t.join();
        }
    }

    void renderThreadPerRow(const Scene& scene, Image& img){
        // les points de l'ecran, en coordonnées 3D, au sein de la Scene.
        // on tire un rayon de l'observateur vers chacun de ces points
        const Scene::screen_t& screen = scene.getScreenPoints();
        vector<thread> threads;
        threads.reserve(scene.getHeight() *scene.getWidth() );
        // pour chaque pixel, calculer sa couleur
        for (int x = 0; x < scene.getWidth(); x++) {
            threads.emplace_back([&,x](){
            for (int y = 0; y < scene.getHeight(); y++) {
                    // le point de l'ecran par lequel passe ce rayon
                    auto& screenPoint = screen[y][x];
                    // le rayon a inspecter
                    Ray ray(scene.getCameraPos(), screenPoint);

                    int targetSphere = scene.findClosestInter(ray);

                    if (targetSphere != -1) {
                        const Sphere& obj = scene.getObject(targetSphere);
                        // pixel prend la couleur de l'objet
                        Color finalcolor = scene.computeColor(obj, ray);
                        // mettre a jour la couleur du pixel dans l'image finale.
                        img.pixel(x, y) = finalcolor;
                    }
            }
            });
        }
        for(auto &t : threads){
            t.join();
        }
    }

    void renderThreadManual(const Scene& scene, Image& img, int nbthread){
        // les points de l'ecran, en coordonnées 3D, au sein de la Scene.
        // on tire un rayon de l'observateur vers chacun de ces points
        // h/nbthread par thread
        if (nbthread <= 0 || scene.getHeight() == 0) return;
        const Scene::screen_t& screen = scene.getScreenPoints();
        vector<thread> threads;
        threads.reserve(nbthread);
        // pour chaque pixel, calculer sa couleur
        int limite = scene.getHeight() / nbthread; //nb ligne a faire par thread
        int lastT = scene.getHeight() % nbthread; // reste a ajouter au last thread
        for (int i = 0; i < nbthread; ++i) {
            int mini = i*limite;
            int maxi = (i+1)*limite;
            if(i == nbthread-1){
                maxi = maxi + lastT;
                threads.emplace_back([&,mini,maxi](){
                    for(int ligne = mini; ligne < maxi; ++ligne){
                        for(int y = 0; y < scene.getWidth(); y++){
                            // le point de l'ecran par lequel passe ce rayon
                            auto& screenPoint = screen[ligne][y];
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
                    } 
                });
            }else{
                threads.emplace_back([&,mini,maxi](){
                    for(int ligne = mini; ligne < maxi; ++ligne){
                        for(int y = 0; y < scene.getWidth(); y++){
                            // le point de l'ecran par lequel passe ce rayon
                            auto& screenPoint = screen[ligne][y];
                            // le rayon a inspecter
                            Ray ray(scene.getCameraPos(), screenPoint);

                            int targetSphere = scene.findClosestInter(ray);

                            if (targetSphere != -1) {
                                const Sphere& obj = scene.getObject(targetSphere);
                                // pixel prend la couleur de l'objet
                                Color finalcolor = scene.computeColor(obj, ray);
                                // mettre a jour la couleur du pixel dans l'image finale.
                                img.pixel(y, ligne) = finalcolor;
                            }
                        }
                    }
                });
            }      
        }
        for(auto &t : threads){
            t.join();
        }
    }

    void renderPoolPixel(const Scene& scene, Image& img, int nbthread){
        // les points de l'ecran, en coordonnées 3D, au sein de la Scene.
        // on tire un rayon de l'observateur vers chacun de ces points
        
        Pool pool = Pool(36);
        pool.start(nbthread);
       

        // pour chaque pixel, calculer sa couleur
        for (int x = 0; x < scene.getWidth(); x++) {
            for (int y = 0; y < scene.getHeight(); y++) {
                PixelJob  *j = new PixelJob( scene, img, x, y);
                pool.submit(j);
            }
        }

        pool.stop();
    }
};

} // namespace pr