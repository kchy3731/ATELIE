#include "state.h"

glm::mat3 GetRotBasis(const Scene::Object& obj) {
    return glm::mat3_cast(obj.rotation);
}

glm::vec3 GetCameraRight(const AtelieState& state) {
    float az = glm::radians(state.camera.azimuth);
    return glm::vec3(cos(az), 0.0f, -sin(az));
}

glm::vec3 GetCameraFront(const AtelieState& state) {
    float az = glm::radians(state.camera.azimuth);
    float pol = glm::radians(state.camera.polar);
    return glm::vec3(-cos(pol) * sin(az), -sin(pol), -cos(pol) * cos(az));
}

glm::vec3 GetCameraUp(const AtelieState& state) {
    float az = glm::radians(state.camera.azimuth);
    float pol = glm::radians(state.camera.polar);
    return glm::vec3(-sin(pol) * sin(az), cos(pol), -sin(pol) * cos(az));
}