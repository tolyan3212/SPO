#include "triad.h"

Triad::Triad()
{}

Triad::Triad(Operators op,
             std::vector<TriadArgument> args)
    : op(op)
    , args(args)
{}

bool Triad::operator==(const Triad& other)
{
    if (op != other.op
        || args.size() != other.args.size()) {
        return false;
    }
    for (int i = 0; i < args.size(); i++) {
        // if (args[i].type != other.args[i].type
        //     || args[i].text != other.args[i].text)
        //     return false;
    }
    return true;
}
