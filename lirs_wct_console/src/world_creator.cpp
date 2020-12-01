//
// Created by optio32 on 13.05.18.
//

#include <fstream>
#include "world_creator.h"

void CreateWorld(const std::string& world_file, const std::string& mesh_path)
{
    std::ofstream buff(world_file);
    buff << "<?xml version=\"1.0\" ?>\n"
            "<sdf version=\"1.4\">\n"
            "<world name=\"default\">\n"
            "<include>\n"
            "<uri>model://sun</uri>\n"
            "</include>\n"
            "<scene>\n"
            "<ambient>0.0 0.0 0.0 1.0</ambient>\n"
            "<shadows>0</shadows>\n"
            "</scene>\n"
            "<model name=\"my_mesh\">\n"
            "<static>true</static>"
            "<link name=\"body\">"
            "<visual name=\"visual\">\n"
            "<pose frame=''>0 0 0 0 0 0</pose>\n"
            "<geometry>\n"
            "<mesh><uri>file://";
    buff << mesh_path.c_str();
    buff << "</uri></mesh>\n"
            "</geometry>\n"
            "</visual>"
            "<collision name=\"collision1\">\n"
            "<geometry>\n"
            "<mesh><uri>file://";
    buff << mesh_path.c_str();
    buff << "</uri></mesh>\n"
            "</geometry>\n"
            "</collision>"
            "</link>\n"
            "</model>\n"
            "</world>\n"
            "</sdf>";
    buff.close();
}
