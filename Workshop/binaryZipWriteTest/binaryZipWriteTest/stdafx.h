// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#include "targetver.h"

#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <tchar.h>

#include <memory>
#include <thread>


// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme

#include <utility>      // std::pair
#include <string>       // std::string
#include <fstream>
#include <iostream>
#include <sstream> 
#include <ostream>


#include <map>
#include <vector>
#include <list>

#include <math.h>   
#include <algorithm>  




//Glm libray for matrix, vec 
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "DataRW.h"

#define MIN_MVS_PHOTOCONSISTENCY 0.0001
extern float ZNEAR;//to redefine...
extern float ZFAR;//to redefine...

#include "Camera.h"

#include "MVSPoint.h"

#include "LoadBundleFile.h"
#include "LoadPMVSPoints.h"


#include "PhotoFlyXmlManager.h"
#include <ConfigLib.h>