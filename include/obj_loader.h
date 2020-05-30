#ifndef LAB_2_OBJ_LOADER_H
#define LAB_2_OBJ_LOADER_H

#include <fstream>
#include <string>
#include <sstream>
#include "mesh.h"

namespace ObjLoader{
    Mesh* load_obj(std::string const& obj_path);
};

#endif //LAB_2_OBJ_LOADER_H
