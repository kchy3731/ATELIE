#include "primitives.h"

namespace Primitives {
    std::vector<Vertex> CubeVertices() {
        return {
            { glm::vec3(-0.5f, -0.5f,  0.5f) },
            { glm::vec3( 0.5f, -0.5f,  0.5f) },
            { glm::vec3( 0.5f,  0.5f,  0.5f) },
            { glm::vec3(-0.5f,  0.5f,  0.5f) },
            { glm::vec3(-0.5f, -0.5f, -0.5f) },
            { glm::vec3( 0.5f, -0.5f, -0.5f) },
            { glm::vec3( 0.5f,  0.5f, -0.5f) },
            { glm::vec3(-0.5f,  0.5f, -0.5f) } 
        };
    }

    std::vector<unsigned int> CubeIndices() {
        return {
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            3, 2, 6, 6, 7, 3,
            4, 5, 1, 1, 0, 4 
        };
    }

    std::vector<Vertex> TriangleVertices() {
        return {
            { glm::vec3( 0.5f, -0.5f, 0.0f) },
            { glm::vec3(-0.5f, -0.5f, 0.0f) },
            { glm::vec3( 0.0f,  0.5f, 0.0f) }
        };
    }

    std::vector<unsigned int> TriangleIndices() {
        return { 0, 1, 2 };
    }

    std::vector<Vertex> CylinderVertices() {
        std::vector<Vertex> vertices;
        int segments = 8;
        float radius = 0.5f;
        float halfHeight = 0.5f;
        
        // top center point
        vertices.push_back({ glm::vec3(0.0f, halfHeight, 0.0f) });
        // bottom center point
        vertices.push_back({ glm::vec3(0.0f, -halfHeight, 0.0f) });
        
        for (int i = 0; i < segments; i++) {
            float theta = (float)i / segments * 2.0f * 3.14159265359f;
            float c = cos(theta);
            float s = sin(theta);
            vertices.push_back({ glm::vec3(radius * c, halfHeight, radius * s) });
            vertices.push_back({ glm::vec3(radius * c, -halfHeight, radius * s) });
        }
        return vertices;
    }

    std::vector<unsigned int> CylinderIndices() {
        std::vector<unsigned int> indices;
        int segments = 16;
        for (int i = 0; i < segments; i++) {
            int top1 = 2 + i * 2;
            int bot1 = top1 + 1;
            int top2 = 2 + ((i + 1) % segments) * 2;
            int bot2 = top2 + 1;
            
            // Side
            indices.push_back(top1);
            indices.push_back(bot1);
            indices.push_back(bot2);
            
            indices.push_back(bot2);
            indices.push_back(top2);
            indices.push_back(top1);
            
            // Top
            indices.push_back(0);
            indices.push_back(top1);
            indices.push_back(top2);
            
            // Bottom
            indices.push_back(1);
            indices.push_back(bot2);
            indices.push_back(bot1);
        }
        return indices;
    }
}