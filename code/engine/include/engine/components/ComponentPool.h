// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
// Modified by Cem Özdemir due to undefined behavior.

#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "../callbacks/Event.h"
#include "../Engine_NS.h"
#include "CComponent.h"

namespace engine::components
{

    /***
     * A component handle can be used to store an indirect reference to
     * a component in the memory pool.
     *
     * @tparam C the type of the component
     * @tparam CC the type of the component pool
     */
    template <typename C, typename CC>
    struct ComponentHandle final
    {
        template <typename, size_t>
        friend
        class ComponentPool;

        ComponentHandle()
                : container(nullptr), id(invalid_id)
        {

        }

        C& get() const
        {
            size_t index = container->id_index_map[id];
            return container->at(index);
        }

        C* operator->() const
        {
            size_t index = container->id_index_map[id];
            return &container->at(index);
        }

        friend bool operator<(const ComponentHandle& l, const ComponentHandle& r)
        {
            return l.id < r.id;
        }

        [[nodiscard]] bool isValid() const
        {
            return id != invalid_id;
        }

    private:
        // Private constructor only available to FComponentPool
        ComponentHandle(CC* container, id_t id)
                : container(container), id(id)
        {
            assert(container != nullptr);
            if (!container->isValid(id))
                throw std::runtime_error("Invalid component id!");
        }

        id_t id = invalid_id;

        CC* container = nullptr;
    };

    /***
     * Base class for Component container realizations (e.g. for unique_ptr)
     */
    class ComponentContainer
    {
    public:
        virtual ~ComponentContainer() = default;
    };

    /***
     * Stores a pool of pre-allocated components that can be efficiently
     * initialized for an entity.
     * Memory for components is allocated upon instantiation.
     *
     * Other memory is still allocated at runtime, e.g. entity-component maps.
     *
     * @tparam C the component type this pool stores.
     * @tparam capacity the maximum number of components provided by this pool.
     */
    template <typename C, size_t capacity>
    class ComponentPool : public ComponentContainer
    {
        // ComponentHandle may call container->at to resolve the reference to a component.
        friend struct ComponentHandle<C, ComponentPool<C, capacity>>;

    public:
        using component_array_t = typename std::array<C, capacity>;
        using index_t = typename component_array_t::size_type;
        using index_vector_t = typename std::vector<index_t>;
        using index_set_t = typename std::set<index_t>;

        using id_set_t = typename std::set<id_t>;
        using id_index_map_t = typename std::map<id_t, index_t>;
        using entity_component_map_t = typename std::map<entities::id_t, id_set_t>;

        using handle_t = typename components::ComponentHandle<C, ComponentPool<C, capacity>>;

        ComponentPool() = default;

        handle_t create(entities::id_t owner)
        {
            assert(head < capacity);

            // new component at end of data
            size_t index = head++;

            // "Placement-new" new(void *) to emplace the
            // object in the pre-allocated memory
            C* component = new(&components[index]) C(next_component_id++, owner);
            id_t id = component->getId();

            // Reset deleted state (default-constructor sets it to true)
            component->deleted = false;

            // Update index map entry
            id_index_map[id] = index;

            // Update entity-component map entry
            auto search = entity_component_map.find(owner);
            if (search != entity_component_map.end())
            {
                auto& set = search->second;
                set.insert(id);
            } else
            {
                auto& set = entity_component_map[owner];
                set.insert(id);
            }

            create_component.fire(handle_t(this, id));
            return handle_t(this, id);
        }

        void erase(handle_t handle)
        {
            erase(handle.id);
        }

        void erase(id_t id)
        {
            components_to_delete.insert(id);

            C& component = components[id_index_map[id]];
            component.deleted = true;

            assert(id == component.id);
        }

        void eraseImmediately(id_t id)
        {
            delete_component.fire(handle_t(this, id));

            index_t index = id_index_map[id];
            C& component = components[index];
            entities::id_t owner = component.getOwner();

            // Remove from index map
            id_index_map.erase(id);

            // Remove from entity-component map
            auto& componentMapEntry = entity_component_map[owner];
            if (componentMapEntry.size() == 1)
            {
                // No other components for this owner
                entity_component_map.erase(owner);
            } else
            {
                // More components for this owner
                componentMapEntry.erase(id);
            }

            // (placement) new calls constructor,
            // so we need to call destructor explicitly
            // without freeing the memory
            component.~C();
            new(&component) C();

            // Compact array
            if (index != head - 1)
            {
                // Move the current head into the gap
                id_index_map[components[head - 1].getId()] = index;
                components[index] = std::move(components[head - 1]);
            }

            // decrement head
            head--;
        }

        void eraseImmediately()
        {
            // List might change during destruction process by hidden calls to destroyActor
            for (auto component_iter = components_to_delete.begin(); component_iter != components_to_delete.end();)
            {
                eraseImmediately(*component_iter);

                // correctly advance the iterator upon destruction
                component_iter = components_to_delete.erase(component_iter);
            }
        }

        void eraseAll(entities::id_t owner)
        {
            auto search = entity_component_map.find(owner);
            if (search != entity_component_map.end())
            {
                for (auto id: search->second)
                {
                    erase(id);
                }
            }
        }

        id_set_t const& getAll(entities::id_t owner)
        {
            return const_cast<id_set_t const&>(entity_component_map.at(owner));
        }

        handle_t getAny(entities::id_t owner)
        {
            try
            {
                return handle_t(this, *entity_component_map.at(owner).begin());
            }
            catch(std::exception& e)
            {
                return handle_t();
            }
        }

        handle_t get(id_t id)
        {
            return handle_t(this, id);
        }

        bool isValid(id_t id)
        {
            return id_index_map.find(id) != id_index_map.end();
        }

        using event_t = callbacks::Event<ComponentPool<C, capacity>, handle_t>;
        event_t create_component;
        event_t delete_component;

        //
        // Iterators for Range-based for loops, e.g. for(auto & c : componentPool) { if (c) ... }
        //

        constexpr typename component_array_t::const_iterator cbegin() const noexcept
        {
            return components.cbegin();
        }

        constexpr typename component_array_t::const_iterator begin() const noexcept
        {
            return components.begin();
        }

        constexpr typename component_array_t::iterator begin() noexcept
        {
            return components.begin();
        }

        constexpr typename component_array_t::const_iterator cend() const noexcept
        {
            return components.cbegin() + head;
        }

        constexpr typename component_array_t::const_iterator end() const noexcept
        {
            return components.begin() + head;
        }

        constexpr typename component_array_t::iterator end() noexcept
        {
            return components.begin() + head;
        }

        C& at(index_t index)
        {
            return components.at(index);
        }
    private:
        component_array_t components;

        id_index_map_t id_index_map;
        entity_component_map_t entity_component_map;

        id_set_t components_to_delete;
        index_t head = 0;

        id_t next_component_id = {0};
    };
}
