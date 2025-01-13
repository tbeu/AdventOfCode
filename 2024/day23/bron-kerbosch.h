#ifndef BRON_KERBOSCH_H
#define BRON_KERBOSCH_H

#include <forward_list>
#include <unordered_set>
#include <algorithm>
#include <functional>



namespace BronKerbosch
{

template <typename T>
struct Vertex
{
    Vertex (const T id) : id {id}
    {
    }

    bool operator == (const Vertex & other) const
    {
        return this->id == other.id;
    }

    T id;
    std::unordered_set <T> ns;
};



template <typename T> using Graph  = std::forward_list <Vertex <T> >;
template <typename T> using Clique = Graph <T>;



template <typename T>
Vertex <T> pivot (const Graph <T> & P, const Graph <T> & X)
{
    return * std::max (std::max_element (P.begin (), P.end (),
                       [](const Vertex <T> & a, const Vertex <T> & b){return a.ns.size () < b.ns.size ();}),
                       std::max_element (X.begin (), X.end (),
                       [](const Vertex <T> & a, const Vertex <T> & b){return a.ns.size () < b.ns.size ();}),
                           [&](const auto & a, const auto & b){return a == P.end () ? true
                                                                    : b == X.end () ? false
                                                                    : a->ns.size () < b->ns.size ();});

}



template <typename T>
void solve (Graph <T> R, Graph <T> P, Graph <T> X, std::function <void (Graph <T>, Graph <T>, Graph <T>)> act)
{
    if (P.empty () && X.empty () ) {
        act (R, P, X);
        return;
    }

    const auto u = pivot (P, X);
    auto Pu = P;
    Pu.remove_if ([& u](const Vertex <T> & p){return u.ns.count (p.id);});

    for (; !Pu.empty (); Pu.pop_front () ) {
        auto Ri = R;
        auto Pi = P;
        auto Xi = X;
        const auto v = Pu.front ();
        Ri.push_front (v);
        Pi.remove_if ([& v](const Vertex <T> & p){return !v.ns.count (p.id);});
        Xi.remove_if ([& v](const Vertex <T> & x){return !v.ns.count (x.id);});
        solve <T> (Ri, Pi, Xi, act);
        P.remove (v);
        X.push_front (v);
    }
}

}//namespace BronKerbosch

#endif//BRON_KERBOSCH_H
