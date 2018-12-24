/*
 * scene.h
 *
 *  Created on: Dec 24, 2018
 *      Author: jmiller
 */

#ifndef LIB_RAY_SCENE_H_
#define LIB_RAY_SCENE_H_

#include "icamera.h"
#include "isphere.h"
#include "ilight.h"

typedef struct _Scene {
    iCamera* camera;
    iSphere* spheres;
    iLight* lights;
    int nSphere;
    int nLight;
} Scene;

#endif /* LIB_RAY_SCENE_H_ */
