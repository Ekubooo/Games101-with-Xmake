//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{   
    Vector3f dirLight, inDirLight;
    // TO DO Implement Path Tracing Algorithm here

    // dirlight :sample the light source
    Intersection currentInter =  intersect(ray);
    if (!currentInter.happened)
        return Vector3f(0.);
    if (currentInter.m->hasEmission())    
        return currentInter.m->getEmission();

    Intersection sampleInter;
    float pdf = 0;
    sampleLight(sampleInter, pdf);
    Vector3f p2l = sampleInter.coords - currentInter.coords;
    Vector3f p2l_D = p2l.normalized();
    //float lPow = p2l.x * p2l.x + p2l.y * p2l.y +p2l.z * p2l.z;   // !
    float lPow = dotProduct(p2l, p2l); 
    Ray dirRay(currentInter.coords + EPSILON * currentInter.normal, p2l_D);
    Intersection opentionL = intersect(dirRay);
    if (opentionL.distance - p2l.norm() > -1.0 * EPSILON)
    {
        dirLight = 
            sampleInter.emit 
            * currentInter.m->eval(ray.direction, p2l_D, currentInter.normal)
            * dotProduct(p2l_D, currentInter.normal)
            * dotProduct(-1.0 * p2l_D, sampleInter.normal)
            / lPow / pdf;
    }

    // indirect light: use RR algorithm
    if (get_random_float() > RussianRoulette)
        return dirLight;
    Vector3f bounce_1 = 
        currentInter.m->sample(ray.direction, currentInter.normal).normalized();
    Ray indirRay(currentInter.coords, bounce_1); 
    Intersection bounceInter = intersect(indirRay);
    if (bounceInter.happened && !bounceInter.m->hasEmission())
    {
        float indirPdf = 
            currentInter.m->pdf(ray.direction, bounce_1, currentInter.normal);
        if(indirPdf > EPSILON)
        {
            inDirLight = 
                castRay(indirRay, depth + 1) 
                * currentInter.m->eval(ray.direction, bounce_1, currentInter.normal)
                * dotProduct(bounce_1, currentInter.normal)
                / indirPdf / RussianRoulette;
        }
    }
    return dirLight + inDirLight;
}