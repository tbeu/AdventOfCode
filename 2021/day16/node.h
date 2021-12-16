// Simple tree implementation

#pragma once

#include "allocate_unique.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <vector>

template <typename T>
struct Node;

template <typename T>
using NodeAlloc = std::allocator<Node<T> >;

template <typename T>
using D = alloc_deleter<NodeAlloc<T> >;

template <typename T>
using NodeRef = std::unique_ptr<Node<T>, D<T> >;

template <typename T>
using NodeRefAlloc = std::allocator<NodeRef<T> >;

template <typename T>
struct Node
{
    Node()
    {
    }

    Node(const T& val) : val(val)
    {
    }

    Node(T&& val) noexcept : val(std::move(val))
    {
    }

    inline bool empty() const noexcept
    {
        return nodes.empty();
    }

    inline auto addNode(const NodeAlloc<T>& alloc, const T& val)
    {
        return nodes.emplace(nodes.end(), allocate_unique<Node<T> >(alloc, val));
    }

    inline auto addNode(const NodeAlloc<T>& alloc, T&& val)
    {
        return nodes.emplace(nodes.end(), allocate_unique<Node<T> >(alloc, std::move(val)));
    }

    void print(const int depth = 0) const
    {
        for (int i = 0; i < depth; ++i) {
            if (i != depth - 1)
                std::cout << "    ";
            else
                std::cout << "|-- ";
        }
        std::cout << val << std::endl;
        for (size_t i = 0; i < nodes.size(); ++i) {
            nodes[i]->print(depth + 1);
        }
    }

    T val{};
    std::vector<NodeRef<T>, NodeRefAlloc<T> > nodes{};
};
