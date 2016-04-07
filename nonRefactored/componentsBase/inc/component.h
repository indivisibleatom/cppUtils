// Contains the component based architecture interfaces
#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <vector>
#include <string>

namespace cppUtils {

class IComponent {
  public:
    virtual ~IComponent() = 0 {};
    virtual unsigned int getID();
};

class IEntity {
  public:
    virtual ~IEntity() = 0;
    virtual void addComponent(std::shared_ptr<IComponent> pComponent);
  protected:
    std::vector<std::shared_ptr<IComponent>> m_pComponents;
};

class IMessage {
  public:
    virtual std::string getMessage();
};

}

#endif _COMPONENT_H_
