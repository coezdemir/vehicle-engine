// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
// used as base, but class was created by Cem Özdemir
#pragma once
#include <functional>
#include <algorithm>
#include <list>

namespace engine::callbacks
{

    /***
     * A event that users can listen to by placing a callback.
     * @tparam Owner Only the owner of an event can fire the event.
     * @tparam Payload The types of payload (arguments) for callbacks.
     */
    template <typename Owner, typename ... Payload>
    struct ComponentEvent
    {
        using callback_t = typename std::function<void(Payload ...)>;

        /***
         * Supplied callback will be executed when the event is fired.
         * @param callback The event handler.
         */
        void subscribe(callback_t callback)
        {
            ComponentEvent::callback = callback;
        }

        void unsubscribe()
        {
            callback = nullptr;
        }

    private:
        using owner_t = Owner;
        friend owner_t;

        ComponentEvent() : callback(nullptr)
        {

        }

        void fire(Payload ... payload)
        {
            if(callback)
                callback(std::forward<Payload>(payload) ...);
        }

        callback_t callback;
    };
}
