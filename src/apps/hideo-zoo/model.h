#pragma once

#include <karm-media/icon.h>
#include <karm-ui/reducer.h>

namespace Hideo::Zoo {

struct Page {
    Mdi::Icon icon;
    Str name;
    Str description;
    Func<Ui::Child()> build;
};

struct State {
    usize page;
};

struct Switch {
    usize page;
};

using Action = Union<Switch>;

void reduce(State &s, Action action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Zoo
