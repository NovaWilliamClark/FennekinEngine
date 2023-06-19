#pragma once

#include <gl/glew.h>

#include <vector>

class VertexArray {
public:
    VertexArray();
    // TODO: Can we have a destructor here?
    unsigned int getVao() const {
        return m_vao;
    }
    unsigned int getVbo() const {
        return m_vbo;
    }
    unsigned int getInstanceVbo() const {
        return m_instanceVbo;
    }
    unsigned int getEbo() const {
        return m_ebo;
    }

    void activate();
    void deactivate();
    void loadVertexData(const std::vector<char>& t_data);
    void loadVertexData(const void* t_data, unsigned int t_size);
    void allocateInstanceVertexData(unsigned int t_size);
    void loadInstanceVertexData(const std::vector<char>& t_data);
    void loadInstanceVertexData(const void* t_data, unsigned int t_size);
    void loadElementData(const std::vector<unsigned int>& t_indices);
    void loadElementData(const unsigned int* t_indices, unsigned int t_size);
    void addVertexAttrib(unsigned int t_size, unsigned int t_type, unsigned int t_instanceDivisor = 0);
    void finalizeVertexAttribs();

private:
    struct VertexAttrib {
        unsigned int layoutPosition;
        unsigned int size;
        unsigned int type;
        unsigned int instanceDivisor;
    };

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_instanceVbo = 0;
    unsigned int m_ebo = 0;

    unsigned int m_vertexSizeBytes = 0;
    unsigned int m_elementSize = 0;

    std::vector<VertexAttrib> m_attribs;
    unsigned int m_nextLayoutPosition = 0;
    unsigned int m_stride = 0;
};