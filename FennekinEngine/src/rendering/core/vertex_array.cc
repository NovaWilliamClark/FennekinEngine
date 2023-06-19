#include "vertex_array.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_vao);
    activate();
}

void VertexArray::activate() {
    glBindVertexArray(m_vao);
}

void VertexArray::deactivate() {
    glBindVertexArray(0);
}

// TODO: Reduce duplication in these methods.
void VertexArray::loadVertexData(const std::vector<char>& t_data) {
    activate();

    if (!m_vbo)
        glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo);
    // TODO: Allow other draw strategies besides GL_STATIC_DRAW.
    glBufferData(GL_ARRAY_BUFFER_ARB, t_data.size(), t_data.data(), GL_STATIC_DRAW);
    m_vertexSizeBytes = t_data.size();
}

void VertexArray::loadVertexData(const void* t_data, unsigned int sizeBytes) {
    activate();

    if (!m_vbo)
        glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo);
    glBufferData(GL_ARRAY_BUFFER_ARB, sizeBytes, t_data, GL_STATIC_DRAW);
    m_vertexSizeBytes = sizeBytes;
}

void VertexArray::allocateInstanceVertexData(unsigned int t_size) {
    activate();

    if (!m_instanceVbo)
        glGenBuffers(1, &m_instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER_ARB, m_instanceVbo);
    glBufferData(GL_ARRAY_BUFFER_ARB, t_size, nullptr, GL_STATIC_DRAW);
}

void VertexArray::loadInstanceVertexData(const std::vector<char>& t_data) {
    activate();

    if (!m_instanceVbo)
        glGenBuffers(1, &m_instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER_ARB, m_instanceVbo);
    glBufferData(GL_ARRAY_BUFFER_ARB, t_data.size(), t_data.data(), GL_STATIC_DRAW);
}

void VertexArray::loadInstanceVertexData(const void* t_data, unsigned int t_size) {
    activate();

    if (!m_instanceVbo)
        glGenBuffers(1, &m_instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER_ARB, m_instanceVbo);
    glBufferData(GL_ARRAY_BUFFER_ARB, t_size, t_data, GL_STATIC_DRAW);
}

void VertexArray::loadElementData(const std::vector<unsigned int>& t_indices) {
    activate();

    if (!m_ebo)
        glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, t_indices.size(), t_indices.data(), GL_STATIC_DRAW);
    m_elementSize = t_indices.size();
}

void VertexArray::loadElementData(const unsigned int* t_indices, unsigned int t_size) {
    activate();

    if (!m_ebo)
        glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, t_size, t_indices, GL_STATIC_DRAW);
    m_elementSize = t_size;
}

void VertexArray::addVertexAttrib(unsigned int t_size, unsigned int t_type, unsigned int t_instanceDivisor) {
    VertexAttrib attrib = {
        .layoutPosition = m_nextLayoutPosition,
        .size = t_size,
        .type = t_type,
        .instanceDivisor = t_instanceDivisor,
    };
    m_attribs.push_back(attrib);
    m_nextLayoutPosition++;
    // TODO: Support types other than float.
    m_stride += t_size * sizeof(float);
}

void VertexArray::finalizeVertexAttribs() {
    activate();

    int offset = 0;
    for (const VertexAttrib& attrib : m_attribs) {
        glVertexAttribPointer(attrib.layoutPosition, attrib.size, attrib.type,
                              /* normalized */ GL_FALSE, m_stride,
                              // TODO: Support types other than float.
                              /* offset */ static_cast<const char*>(nullptr) + offset);
        glEnableVertexAttribArray(attrib.layoutPosition);
        if (attrib.instanceDivisor) {
            glVertexAttribDivisor(attrib.layoutPosition, attrib.instanceDivisor);
        }
        offset += attrib.size * sizeof(float);
    }

    // Clear state to support subsequent runs.
    // We intentionally don't reset nextLayoutPosition_.
    m_attribs.clear();
    m_stride = 0;
}