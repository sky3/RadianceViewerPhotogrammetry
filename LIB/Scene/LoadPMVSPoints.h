#ifndef _LOAD_PMVS_POINT_H_
#define _LOAD_PMVS_POINT_H

#include "common.h"
#include "Camera.h"
#include "MVSPoint.h"

//std::vector< std::shared_ptr<MVSPoint> > loadPMVSPoints(std::string image_dir, std::vector< std::shared_ptr<Camera> > & camera_vec);

/* New */
std::vector< std::shared_ptr<MVSPoint> > loadPMVSPoints(std::vector< std::shared_ptr<Camera> > & camera_vec);

#endif