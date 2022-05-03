#pragma once

#include <scene/entity.h>
#include <timer.h>

#include <cereal/types/polymorphic.hpp>
#include <cerealization/archive_types.h>

namespace pgre {

class entity_script_t
{
protected:
    scene::entity_t _entity;
    std::string _name;
public:
    virtual ~entity_script_t() = default;

    entity_script_t(const std::string& name) : _entity(entt::null, nullptr), _name(name){};
    /**
     * @brief Called every frame.
     * 
     * @param delta 
     */
    virtual void update(const interval_t& delta) = 0;
    
    /**
     * @brief Deriving classes should perform all initialisation in this method.
     * The owning entity is not known before this method is called.
     * 
     */
    virtual void init() {};
    /**
     * @brief Event callback. Each script component will get the event, even if other script
     * components handled it.
     *
     * @param event
     */
    virtual void on_event(event_t& /*unused*/) {}
    const std::string& get_name() { return _name; }
    friend class component::script_component_t;
};

namespace component {
    class script_component_t
    {
        std::unique_ptr<entity_script_t> _script_instance{};

    public:
        script_component_t() = default;
        script_component_t(const scene::entity_t& entity,
                           std::unique_ptr<entity_script_t> script_instance)
          : _script_instance(std::move(script_instance)) {
            _script_instance->_entity  = entity;
            _script_instance->init();
        };

        void update(const interval_t& delta) { if (_script_instance) _script_instance->update(delta); }
        void on_event(event_t& event) { if (_script_instance) _script_instance->on_event(event); }

        template <typename Archive>
        void serialize(Archive& ar){
            //TODO: figure this shit out
        }
    };

} // namespace component
} // namespace pgre

#define PGRE_DECLARE_ENTITY_SCRIPT_SERIALIZABLE(class) CEREAL_REGISTER_TYPE(class);\
                                                       CEREAL_REGISTER_POLYMORPHIC_RELATION(pgre::entity_script_t, class)
