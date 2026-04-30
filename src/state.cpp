#include "state.h"

glm::mat3 GetRotBasis(const Scene::Object& obj) {
    glm::mat4 r = glm::mat4(1.0f);
    r = glm::rotate(r, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    r = glm::rotate(r, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    r = glm::rotate(r, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    return glm::mat3(r);
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