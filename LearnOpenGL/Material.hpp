//
//  Material.hpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/21.
//

#ifndef Material_hpp
#define Material_hpp
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include "System.hpp"

namespace Material
{
    using Json = nlohmann::json;
    struct CT_PBR
    {
        float metallic = 1.0;
        float roughness = 1.0;
        float ao = 1.0;
        glm::vec3 albedo = glm::vec3(0.0, 0.0, 0.0);
        std::string name;
        CT_PBR() {};
    };
    int cIndex = 0;
    CT_PBR cCT_PBR;
    std::vector<CT_PBR> materials;
    
    void loadMaterial()
    {
        std::string path = System::resourcePathWithFile("materials.json");
        std::ifstream file(path);

        if (!file)
        {
            return ;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        Json json = Json::parse(buffer.str());
        for (const auto& j: json)
        {
            for (const auto& i : j)
            {
                CT_PBR pbr;
                pbr.albedo = glm::vec3(i["albedo"][0], i["albedo"][1], i["albedo"][2]);
                pbr.metallic = i["metallic"];
                pbr.roughness = i["roughness"];
                pbr.ao = i["ao"];
                pbr.name = i["name"];
                materials.emplace_back(std::move(pbr));
            }
        }
        cCT_PBR = materials[cIndex];
    }
}
#endif /* Material_hpp */
