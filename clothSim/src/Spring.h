#ifndef SPRING_H
#define SPRING_H

#include "ofMain.h"
#include "Particle.h"

struct Spring {
    Particle *a, *b;
    float k;
    float rest_length;
    
    Spring(Particle *a, Particle *b, float k = .2) : a(a), b(b), k(k) {
        rest_length = (b->pos - a->pos).length();
    }
    
    void update() {
        ofVec3f dir = b->pos - a->pos;
        float dist = dir.length();
        if (dist == 0.0) dist = 0.0001; // prevent division by zero
        float f = (rest_length - dist) * k; // linear force spring
        dir.normalize();
        a->addForce(dir * -f);
        b->addForce(dir * f);
    }
};

#endif