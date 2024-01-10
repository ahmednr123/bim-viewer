#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;



int main () {
    std::ifstream f("site_plan_1.bim");
    json data = json::parse(f);

    auto meshes = data["meshes"];
    auto elements = data["elements"];

    for (int i = 0; i < meshes.size(); i++) {
        auto mesh = meshes[i];
        int mesh_id = mesh["mesh_id"];
        auto coordinates = mesh["coordinates"];
        for (int j = 0; j < coordinates.size(); j+=3) {
            std::cout << coordinates[j] << "\t" << coordinates[j+1] << "\t" << coordinates[j+2] << "\n";
            std::cout <<
        }
    }

    std::cout << meshes.size() << std::endl;

    return 0;
}
