//
// Created by optio32 on 15.05.18.
//

#include <fstream>
#include "script_generator.h"

void GenerateScript(const std::string& modelName, const std::string& scriptName)
{
    std::ofstream buff(scriptName);
    buff << "import bpy\n"
            "objs = bpy.data.objects\n"
            "objs.remove(objs[\"Cube\"], True)\n";
    buff << "daepath = \"";
    buff << modelName.c_str();
    buff << "\"\n";
    buff << "bpy.ops.wm.collada_import(filepath=daepath)\n"
            "bpy.ops.wm.collada_export(filepath=\"";
    buff << modelName.c_str();
    buff << "\")\n";
    buff.close();
}
