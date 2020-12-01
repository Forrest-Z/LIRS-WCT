#include <iostream>
#include "png2dae.h"
#include "png2stl.h"
#include "utils.h"
#include "world_creator.h"
#include "script_generator.h"
#include <Magick++.h>
#include <chrono>
#include <cstring>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace boost::program_options;
using namespace boost::filesystem;

bool execBlender(const IMAGE2DAE_CONFIG& cfg)
{
    std::string scriptNamePath = cfg.outputDir + "/" "script.py";
    std::string meshFilePath = cfg.outputDir + "/" + cfg.outputModel;
    GenerateScript(meshFilePath, scriptNamePath);
    // move up to the root directory
    auto root_path = Info::GetModulePath().parent_path();
    std::string blender_filename = root_path.normalize().string() + "/ThirdParty/blender/blender";
    if(!File::FileExistBoost(blender_filename)) {
        std::cerr << "Blender doesn't exist!\n";
        return false;
    }
    std::string cmd = blender_filename + " --background --python " + scriptNamePath;
    int status = Exec::exec((char*)cmd.c_str());
    if(status < 0) {
        std::cerr << "Error: " << std::strerror(errno) << '\n';
        return false;
    }
    else {
        if(WIFEXITED(status))
            std::cout << "Program returned normally, exit code: " << WEXITSTATUS(status) << '\n';
        else
            std::cout << "Program exited abnormaly\n";
    }
    boost::filesystem::remove(scriptNamePath);
    return true;
}

int main(int argc, char** argv)
{
    IMAGE2DAE_CONFIG cfg;
    double height_scale = 0.25;
    double sizes_scale = 0.05;
    double x_rotate_angle = -90.0;
    Magick::Image i;
    std::string input_file("");
    std::string output_model("");
    std::string output_folder("");
    std::string texture_image("");
    std::string input_file_ext("");
    ConvertMode mode = ConvertMode::IMAGE2DAE;

    try
    {
        options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("input,i", value<std::string>(&input_file), "Input File")
                ("output,o", value<std::string>(&output_model), "Output Model Name")
                ("folder,f", value<std::string>(&output_folder), "Output Folder")
                ("texture,t", value<std::string>(&texture_image), "Texture File")
                ("pstl,p", "Convert to Stl from Grayscale Image")
                ("pdae,k", "Convert to Dae from Grayscale Image")
                ("sdae,s", "Convert to Dae from Stl")
                ("color-inverse,c", "Color inverse (negate)")
                ("grayscale,g", "Convert to grayscale")
                ("zscale,z", value<double>(&height_scale), "Height scale")
                ("scale,l", value<double>(&sizes_scale), "Sizes scale")
                ("x_rot,x", value<double>(&x_rotate_angle), "X-axis rotate angle");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << '\n';
            return -1;
        }
        if (vm.count("input"))
        {
            input_file.erase(std::remove(input_file.begin(), input_file.end(), '\''), input_file.end());
            input_file_ext = canonical(path(input_file)).extension().string();
            if(input_file_ext != ".stl")
            {
                cfg.inputImage = system_complete(path(input_file)).normalize().string();
                i.read(cfg.inputImage);
                if(i.columns() > 670 && i.rows() > 670)
                {
                    Magick::Geometry geom;
                    geom.width(650);
                    geom.height(650);
                    i.resize(geom);
                }
            }
        }
        else
        {
            std::cerr << "Input image was not set!\n";
            return -1;
        }
        if (vm.count("output"))
        {
            output_model.erase(std::remove(output_model.begin(), output_model.end(), '\''), output_model.end());
            cfg.outputModel = output_model + ".dae";
        }
        else
        {
            std::cerr << "Output model name was not set!\n";
            return -1;
        }
        if (vm.count("folder"))
        {
            output_folder.erase(std::remove(output_folder.begin(), output_folder.end(), '\''), output_folder.end());
            output_folder = system_complete(output_folder).normalize().string();

            if(!boost::filesystem::exists(output_folder))
                boost::filesystem::create_directory(path(output_folder));
            else
            {
                if(!boost::filesystem::is_directory(output_folder))
                    return -1;
            }
            cfg.outputDir = canonical(path(output_folder)).string();
        }
        else
        {
            std::cout << "Output folder was not set!\n";
            cfg.outputDir = boost::filesystem::current_path().string();
            std::cout << "Default directory: " << cfg.outputDir << '\n';
        }
        if (vm.count("texture"))
        {
            texture_image.erase(std::remove(texture_image.begin(), texture_image.end(), '\''), texture_image.end());
            if(!boost::filesystem::exists(texture_image))
            {
                std::cout << "Texture was not set!\n";
            	cfg.texture = "";
            }
            else
            {
                auto old_texture_file = canonical(path(texture_image)).normalize();
                auto texture_file_extension = old_texture_file.extension().string();
                auto new_texture_filename = "model_texture" + texture_file_extension;
                auto new_texture_file = cfg.outputDir + "/" + new_texture_filename;
                boost::filesystem::copy_file(old_texture_file.string(), new_texture_file, copy_option::overwrite_if_exists);
                cfg.texture = new_texture_filename;
        	}
        }
        else
        {
            std::cout << "Texture was not set!\n";
            cfg.texture = "";
        }
        if (vm.count("pstl"))
        {
            std::cout << "Conversion mode: Image to Stl\n";
            mode = ConvertMode::IMAGE2STL;
        }
        if (vm.count("pdae"))
        {
            std::cout << "Conversion mode: Image to Dae\n";
            mode = ConvertMode::IMAGE2DAE;
        }
        if (vm.count("sdae"))
        {
            std::cout << "Conversion mode: Stl to Dae\n";
            mode = ConvertMode::STL2DAE;
        }
        if (vm.count("color-inverse"))
        {
            std::cout << "Color-inverse is applied\n";
            i.negate();
        }
        if (vm.count("grayscale"))
        {
            std::cout << "Convert to grayscale\n";
            i.grayscale(MagickCore::PixelIntensityMethod::Rec709LumaPixelIntensityMethod);
        }
        if (vm.count("zscale"))
        {
            std::cout << "Height scale: " << height_scale << '\n';
            cfg.zAxisScale = height_scale;
        }
        else
        {
            std::cout << "Height scale was set to default (0.25)\n";
            cfg.zAxisScale = 0.25;
        }
        if (vm.count("scale"))
        {
            std::cout << "Model scale: " << sizes_scale << '\n';
            cfg.axisScale = sizes_scale;
        }
        else
        {
            std::cout << "Model scale was set to default (0.05)\n";
            cfg.axisScale = 0.05;
        }
        if (vm.count("x_rot"))
        {
            std::cout << "X-axis rotate angle: " << x_rotate_angle << '\n';
            cfg.xAxisRotationAngle = x_rotate_angle;
        }
        else
        {
            std::cout << "X-axis rotate angle was set to default (-pi/2)\n";
            cfg.xAxisRotationAngle = -90;
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    if(mode == ConvertMode::IMAGE2STL)
    {
        if(input_file_ext == ".png" || input_file_ext == ".jpg" || input_file_ext == ".jpeg")
        {
            i.magick("PNG");
            std::string updatedImagePath = cfg.outputDir + "/" + Random::GetRandomStringName(5)+".png";
            i.write(updatedImagePath);
            cfg.inputImage = updatedImagePath;

            STL_CONFIG sccfg;
            sccfg.imageFile = cfg.inputImage;
            sccfg.outputDir = cfg.outputDir;
            size_t lastindex = cfg.outputModel.find_last_of(".");
            sccfg.outputModel = cfg.outputModel.substr(0, lastindex) + ".stl";
            sccfg.zAxisScale = cfg.zAxisScale; 
            Image2Dae::ImageToStl(sccfg);
        }
        else
        {         	
            std::cerr << "Unsupported input file format!\n";
        }
    }
    else if(mode == ConvertMode::IMAGE2DAE)
    {
        if(input_file_ext == ".jpg" || input_file_ext == ".png" || input_file_ext == ".jpeg")
        {
            i.magick("PNG");
            std::string updatedImagePath = cfg.outputDir + "/" + Random::GetRandomStringName(5)+".png";
            i.write(updatedImagePath);
            cfg.inputImage = updatedImagePath;

            Image2Dae dae(cfg);
            dae.Transform();
            dae.Save();
            std::string worldFilePath = cfg.outputDir + "/" + "generated_world.world";
            std::string meshFilePath = cfg.outputDir + "/" + cfg.outputModel;
            CreateWorld(worldFilePath, meshFilePath);
            execBlender(cfg);
            boost::filesystem::remove(cfg.inputImage);
        }
        else
            std::cerr << "Unsupported input file format!\n";
    }
    else if(mode == ConvertMode::STL2DAE)
    {
        if(input_file_ext == ".stl") {
            std::string stlInputPath = canonical(path(input_file)).string();
            Image2Dae::StlToDae(stlInputPath, cfg);
            execBlender(cfg);
        }
        else
            std::cout << "Unsupported input file format!\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_difference = end_time - start_time;

    std::cout << "Process finished!\n";
    std::cout << "Time elapsed = " << time_difference.count() << "seconds" << '\n';

    return 0;
}
