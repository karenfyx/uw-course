//
//  Ray.cpp
//  A4
//
//  Created by YANXI FU on 2019-03-06.
//  Copyright © 2019 none. All rights reserved.
//

#include "Ray.hpp"
Ray::Ray(dvec3 eye, dvec3 pixel):s(eye),
                                d(pixel - eye),
                                t(0) {}


dvec3 Ray::getPoint(double t) {
    return s + t*d;
}
