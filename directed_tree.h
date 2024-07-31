// =====================================================================================================================
// Copyright 2024 Medusa Slockbower
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =====================================================================================================================

#ifndef DIRECTEDGRAPH_H
#define DIRECTEDGRAPH_H

#include "template_utils.h"

namespace open_cpp_utils
{

/**
 * \brief Class for creating a directed tree
 * \tparam T Type of the data associated with each node
 *
 * The tree is a series of child nodes in forward linked lists.
 *
 */
template<typename T>
class directed_tree
{
// Forward Definitions =================================================================================================

public:
    class breadth_first;
	class pre_order;
	class in_order;
	class post_order;

private:
    struct director;


// Typedefs ============================================================================================================

public:
	using data_type = T;
	using node = uint32_t;
	using node_queue = std::deque<node>;

private:
    using hierarchy = std::vector<director>;
    using storage   = std::vector<data_type>;


// Constants ===========================================================================================================

public:
    static constexpr constant_value<node, node(0)> root{};


// Data Structures =====================================================================================================

private:
	struct director
	{
		enum flags
		{
			VALID = 0x0001
		};

		node parent, child, prev_sibling, next_sibling;
		uint32_t flags, depth;

		director() : parent(0), child(0), prev_sibling(0), next_sibling(0), flags(VALID) { }
	};


// Functions ===========================================================================================================

public:

// Constructors & Destructor ---------------------------------------------------------------------------------------

    /**
     * \brief Default constructor, creates tree with empty root
     */
    directed_tree() : graph_{ director() }, data_{ data_type() }, freed_{ } { }


// Tree Navigation -------------------------------------------------------------------------------------------------

    /**
     * \brief Get the parent of a node. O(1)
     * \param id Node id to reference
     * \return Node id of the parent
     */
    [[nodiscard]] node parent(node id) const { return graph_[id].parent; }

    /**
     * \brief Get the first child of a node. O(1)
     * \param id Node id to reference
     * \return Node id of the first child
     */
    [[nodiscard]] node first_child(node id) const { return graph_[id].child; }

    /**
     * \brief Get the previous sibling of a node. O(1)
     * \param id Node id to reference
     * \return Node id of the next sibling in the linked list
     */
    [[nodiscard]] node prev_sibling(node id) const { return graph_[id].prev_sibling; }

    /**
     * \brief Get the next sibling of a node. O(1)
     * \param id Node id to reference
     * \return Node id of the next sibling in the linked list
     */
    [[nodiscard]] node next_sibling(node id) const { return graph_[id].next_sibling; }

    /**
     * \brief Get the left most child of a node. O(log(n))
     * \param id Node id to reference
     * \return Node id of the left most child
     */
    [[nodiscard]] node left_most(node id) const
	{
		node current = id;
		while(id = first_child(current)) current = id;
		return current;
	}

    /**
     * \brief Get the depth of a node
     * \param id
     * \return
     */
    [[nodiscard]] uint32_t depth(node id) const
	{
		uint32_t depth = 0;
		while (id)
		{
			id = parent(id);
			++depth;
		}
		return depth;
	}


// Tree Modification ---------------------------------------------------------------------------------------------------

    /**
     * \brief Insert a node into the tree as a child of the provided node
     * \param data Value to insert
     * \param p_id Id of the parent node
     * \return Id of the inserted node
     */
    node insert(const data_type& data, node p_id)
	{
		if(freed_.empty())
		{
			freed_.push_back(static_cast<node>(graph_.size()));
			graph_.push_back(director()); data_.push_back(data);
		}

		node id = freed_.front(); freed_.pop_front();
        director& node   = graph_[id];
        director& parent = graph_[p_id];


        if(parent.child)
        {
            director& nchild = graph_[parent.child];
		    node.prev_sibling = nchild.prev_sibling;
            nchild.prev_sibling = id;

            if(nchild.prev_sibling)
            {
                director& pchild = graph_[nchild.prev_sibling];
                pchild.next_sibling = id;
            }
        }

        // Setup node
		node.parent = p_id;
        node.next_sibling = parent.child;
		node.child = 0;
		node.flags = director::VALID;

		// Set parent's child
		parent.child = id;


		data_[id] = data;

		return id;
	}

    /**
     * \brief Erase a node in the tree
     * \param id Id of the node to erase
     */
    void erase(node id)
	{
		if(id == 0) return;

		director& erased = graph_[id];
		erased.Flags &= ~director::VALID;
		freed_.push_back(id);

		graph_[erased.parent].Child = erased.Sibling;

		node_queue stack{ erased.Child };

		while(stack.empty() == false)
		{
			node next = stack.front(); stack.pop_front();
			director& child = graph_[next];
			child.Flags &= ~director::VALID;
			freed_.push_back(next);

			if(child.Sibling) stack.push_front(child.Sibling);
			if(child.Child) stack.push_front(child.Child);
		}
	}


// Tree Access ---------------------------------------------------------------------------------------------------------

    /**
     * \brief Getter for data associated with a node
     * \param id Id of the node to access
     * \return Reference to the node's data
     */
    data_type& operator[](node id) { return data_[id]; }

    /**
     * \brief Constant getter for data associated with a node
     * \param node Id of the node to access
     * \return Reference to the node's data
     */
	[[nodiscard]] const data_type& operator[](node id) const { return data_[id]; }


// Visitor Pattern -----------------------------------------------------------------------------------------------------

    /**
     * \brief Traverser-Visitor pattern for accessing the tree
     * \tparam V Visitor type.
     * \tparam O Order type. Defaults to Pre-Order Traversal.
     * \param visitor
     */
    template<typename O = pre_order, typename V>
	void traverse(V& visitor)
	{
		traverser<V, O> traverser(*this, visitor);
		traverser();
	}


// Variables =======================================================================================================

private:
	hierarchy graph_;
	storage   data_;
	node_queue freed_;


// Navigation ======================================================================================================

public:

    /**
     * \brief Breadth first traversal
     */
    class breadth_first
	{
	public:
		breadth_first(directed_tree& graph) : graph_(graph), visit_queue_(0) { }

		node operator()(node node)
		{
			node = visit_queue_.back(); visit_queue_.pop_back();
			director& current = graph_.graph_[node];

			if(current.next_sibling) visit_queue_.push_back(current.next_sibling);
			if(current.child) visit_queue_.push_front(current.child);

			if(visit_queue_.empty()) return 0;
			return node;
		}

	private:
		directed_tree& graph_;
		node_queue      visit_queue_;
	};


    /**
     * \brief Pre-order traversal
     */
	class pre_order
	{
	public:
		pre_order(directed_tree& graph) : graph_(graph) { }

		node operator()(node id)
		{
			director& current = graph_.graph_[id];

			if(current.next_sibling) visit_queue_.push_front(current.next_sibling);
			if(current.child) visit_queue_.push_front(current.child);

			if(visit_queue_.empty()) return 0;
			node next = visit_queue_.front(); visit_queue_.pop_front();
			return next;
		}

	private:
		directed_tree& graph_;
		node_queue      visit_queue_;
	};


    /**
     * \brief In-order traversal
     */
	class in_order
	{
	public:
		in_order(directed_tree& graph) : graph_(graph) { }

		node operator()(node node)
		{
			if(node == 0) visit_queue_.push_back(graph_.left_most(node));

			node = visit_queue_.front(); visit_queue_.pop_front();
			director& current = graph_.graph_[node];

			if(current.Sibling)
			{
				if(graph_.next_sibling(current.Sibling)) visit_queue_.push_back(current.parent);
				visit_queue_.push_back(graph_.left_most(current.Sibling));
			}

			return node;
		}

	private:
		directed_tree& graph_;
		node_queue      visit_queue_;
	};


    /**
     * \brief Post-order traversal
     */
	class post_order
	{
	public:
		post_order(directed_tree& graph) : graph_(graph) { }

		node operator()(node node)
		{
			if(visit_queue_.empty()) visit_queue_.push_back(graph_.left_most(node));

			node = visit_queue_.front(); visit_queue_.pop_front();
			if(node == 0) return node;
			director& current = graph_.graph_[node];

			visit_queue_.push_back(current.Sibling ? graph_.left_most(current.Sibling) : graph_.parent(node));

			return node;
		}

	private:
		directed_tree& graph_;
		node_queue     visit_queue_;
	};


    /**
     * \brief Visitor pattern for traversing the tree
     */
	template<typename V, typename O>
	class traverser
	{
	public:
		using visitor_type = V;
		using order_type = O;

		traverser(directed_tree& graph, visitor_type& visitor) : graph_(graph), visitor_(visitor), order_(graph) { }

		void operator()()
		{
			node node = 0;
			while(node = order_(node))
			{
				if(visitor_(graph_[node], node)) break;
			}
		}

	private:
		directed_tree& graph_;
		visitor_type&   visitor_;
		order_type      order_;
	};
};
}

#endif //DIRECTEDGRAPH_H
