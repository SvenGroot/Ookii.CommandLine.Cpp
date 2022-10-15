/* TODO:
    X Custom function for conversion.
    X Switch
    Multi-Value (use an overload taking a back_insert_iterator?)
    Dictionary?
    X Default value description
    Shell commands
    Generate scripts
    Aliases

*/

#include <iostream>
#include <functional>
#include "inc/ookii/command_line_builder.h"
using namespace std;

struct point
{
    int x;
    int y;
};

namespace ookii
{

template<>
struct lexical_convert<point, char>
{
    static std::optional<point> from_string(std::string, const std::locale &)
    {
        return point{5, 6};
    }
};

}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    int foo{};
    string bar{};
    int baz{};
    point p;
    ookii::switch_argument s{};
    optional<ookii::switch_argument> s2{};
    std::vector<int> l{};
    auto parser = ookii::parser_builder{argv[0]}
        .add_argument("foo", foo).description("Hello").positional().required()
        .add_argument("bar", bar).default_value("none").description("Bye")
        .add_argument("baz", baz).description("Well").positional().converter([](auto value, auto) { return stoi(value) + 1; })
        .add_argument("p", p)
        .add_argument("switch", s)
        .add_argument("switch2", s2)
        .add_multi_value_argument("list", l).separator(';')
        .build();

    for (auto &a : parser.arguments())
    {
        cout << a.name() << " <" << a.value_description() << "> " << a.is_switch() << endl;
    }

    auto result = argc > 1 ? parser.parse(argc, argv) : parser.parse({ "42", "-p", "f", "-baz", "5", "-bar", "hello there", "-switch", "-list", "4", "-list", "6"});
    cout << result.get_error_message() << endl;
    wcout << ookii::get_short_type_name<string, wchar_t>() << endl;
    cout << ookii::value_description<int>::get() << endl;
    cout << ookii::value_description<string>::get() << endl;
    cout << ookii::value_description<wstring>::get() << endl;
    cout << *ookii::lexical_convert<ookii::switch_argument, char>::from_string("TRUE") << endl;

    cout << "foo: " << foo << endl;
    cout << "bar: " << bar << endl;
    cout << "baz: " << baz << endl;
    cout << "p: " << p.x << "," << p.y << endl;
    cout << "s: " << s << endl;
    cout << "s2: " << (s2 ? *s2 : "(null)") << endl;
    cout << "l: ";
    for (const auto &item : l)
    {
        cout << item << "; ";
    }
    cout << endl;
        
    return 0;
}