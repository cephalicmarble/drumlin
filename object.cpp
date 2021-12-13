#include "object.h"

namespace drumlin {

Object::Object(Object *parent)
    :m_parent(parent)
{
    if (nullptr != m_parent) {
        m_parent->attachChild(this);
    }
}

void Object::attachChild(Object *child)
{
    if (m_children.end() ==
        std::find(m_children.begin(), m_children.end(), child))
    {
        m_children.push_back(child);
    }
}

}
