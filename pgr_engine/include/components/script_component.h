#pragma once

#include <scene/entity.h>
#include <timer.h>

namespace pgre {

class entity_script_t
{
protected:
    scene::entity_t _entity;

public:
    virtual ~entity_script_t() = default;

    entity_script_t() : _entity(entt::null, nullptr){};
    virtual void update(const interval_t& delta) = 0;
    virtual void on_entity_set() {};
    void set_entity(scene::entity_t entity) { _entity = entity; on_entity_set();}
};

namespace component {
    class script_component_t
    {
        std::unique_ptr<entity_script_t> _script_instance{};

    public:
        script_component_t(scene::entity_t& entity,
                           std::unique_ptr<entity_script_t> script_instance)
          : _script_instance(std::move(script_instance)) {
            _script_instance->set_entity(entity);
        };

        void update(const interval_t& delta) { _script_instance->update(delta); }
    };

} // namespace component
} // namespace pgre