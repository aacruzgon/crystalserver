target_sources(${PROJECT_NAME}_lib PRIVATE
    functions/game_reload.cpp
    game.cpp
    sound_trace.cpp
    bank/bank.cpp
    cyclopedia_map/cyclopedia_map.cpp
    movement/position.cpp
    movement/teleport.cpp
    scheduling/events_scheduler.cpp
    scheduling/dispatcher.cpp
    scheduling/task.cpp
    scheduling/save_manager.cpp
    worlds/gameworlds.cpp
    zones/zone.cpp
)
