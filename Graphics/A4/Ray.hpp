//
//  Ray.hpp
//  A4
//
//  Created by YANXI FU on 2019-03-06.
//  Copyright © 2019 none. All rights reserved.
//

#ifndef Ray_hpp
#define Ray_hpp

#include <stdio.h>
#include <glm/glm.hpp>
using namespace glm;

class Ray {  // Ray = o + td
    
public:
    const dvec3 s;
    const dvec3 d;
    double t;
    glm::vec2 pixel;
    Ray() {};
    Ray(dvec3 o, dvec3 d);
    dvec3 getPoint(double t);
};


#endif /* Ray_hpp */
