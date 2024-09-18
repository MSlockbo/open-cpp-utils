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

#ifndef OPEN_CPP_UTILS_DIRECTED_TREE_H
#define OPEN_CPP_UTILS_DIRECTED_TREE_H

#include <vector>
#include <deque>
#include <algorithm>

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
    class unordered;

private:
    struct Node_;


// Typedefs ============================================================================================================

public:
	using data_type  = T;
	using node       = uint32_t;
	using node_queue = std::deque<node>;

private:
    using hierarchy = std::vector<Node_>;
    using storage   = std::vector<data_type>;


// Constants ===========================================================================================================

public:
    static constexpr std::integral_constant<node, 0> root{};


// Data Structures =====================================================================================================

private:
	struct Node_
	{
		enum flags
		{
			VALID = 0x0001
		};

		node parent, child, prev_sibling, next_sibling;
		uint32_t flags, depth;

		Node_() : parent(0), child(0), prev_sibling(0), next_sibling(0), flags(VALID), depth(0) { }
	};


// Functions ===========================================================================================================

public:

// Constructors & Destructor -------------------------------------------------------------------------------------------

    /**
     * \brief Default constructor, creates tree with empty root
     */
    directed_tree() : graph_{ Node_() }, data_{ data_type() }, freed_{ } { }
    
    directed_tree(data_type&& data) : graph_{ Node_() }, data_(data), freed_{} { }
    directed_tree(const data_type& data) : graph_{ Node_() }, data_(data), freed_{} { }

    ~directed_tree() = default;


// Tree Navigation -----------------------------------------------------------------------------------------------------

    /**
     * \brief Check whether a node is valid. O(1)
     * \param id Node id to reference
     * \return Whether the valid flag is true in the node
     */
    [[nodiscard]] bool valid(node id) const { return graph_[id].flags & Node_::VALID; }

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
     * \brief Get the first child of a node. O(1)
     * \param id Node id to reference
     * \return Node id of the first child
     */
    [[nodiscard]] node last_child(node id) const
    {
        node c = first_child(id);

        while(c != 0) { if(graph_[c].next_sibling == 0) break; c = graph_[c].next_sibling; }

        return c;
    }

    [[nodiscard]] int child_index(node id)
    {
        if(id == root) return -1;

        int i = 0;
        while(id != root) { id = prev_sibling(id); ++i; }
        return i;
    }

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
    [[nodiscard]] uint32_t depth(node id) const { return graph_[id].depth; }


// Tree Modification ---------------------------------------------------------------------------------------------------

    /**
     * \brief Get the next id that would be used if insert() were called
     * \return Next node id
     */
    node next_id() const
    {
        if(freed_.empty()) return static_cast<node>(graph_.size());
        return freed_.front();
    }

    /**
     * \brief Insert a node into the tree as a child of the provided node
     * \param data Value to insert
     * \param p_id Id of the parent node
     * \param last Whether to insert at the back of the array
     * \return Id of the inserted node
     */
    node insert(const data_type& data, node p_id, int child = 0)
	{
        // If there are no freed nodes, create a new node and mark it as freed
		if(freed_.empty())
		{
			freed_.push_back(static_cast<node>(graph_.size()));
			graph_.push_back(Node_()); data_.push_back(data);
		}

        // Pop a freed node from the stack
		node id   = freed_.front(); freed_.pop_front();
        node s_id = first_child(p_id);
        Node_& node   = graph_[id];
        Node_& parent = graph_[p_id];
        bool back = s_id == 0 ? false : child < 0;

        while(child > 0 && s_id != root)
        {
            s_id = next_sibling(s_id); --child;
            back |= (s_id == root);
        }

        Node_& sibling = graph_[s_id];
        if(s_id == parent.child && !back) parent.child = id;

        node.next_sibling = node.prev_sibling = 0;
        node.parent = p_id;
        node.depth = parent.depth + 1;
        node.flags = Node_::VALID;
        node.child = 0;

        if(s_id == 0) return id;

        if(back)
        {
            node.next_sibling = sibling.next_sibling;
            node.prev_sibling = s_id;

            sibling.next_sibling = id;
        }
        else
        {
            node.next_sibling = s_id;
            node.prev_sibling = sibling.prev_sibling;

            sibling.prev_sibling = id;
        }

        return id;
	}

    void swap(node a, node b)
    {
        Node_& A = graph_[a];
        Node_& B = graph_[b];

        std::swap(A, B);

        if(graph_[B.parent].child == a) graph_[B.parent].child = b;
        if(graph_[A.parent].child == b) graph_[A.parent].child = a;
    }

    /**
     * \brief Erase a node in the tree. O(n)
     * \param id Id of the node to erase
     */
    void erase(node id)
	{
		if(id == root) return;

        // Mark node as invalid and push it to the freed list
		Node_& erased = graph_[id];
		erased.flags &= ~Node_::VALID;
		freed_.push_back(id);
        data_[id].~T();

        // Update the parent's child
		graph_[erased.parent].child = erased.next_sibling;

        // Update siblings
        if(erased.next_sibling) graph_[erased.next_sibling].prev_sibling = erased.prev_sibling;
        if(erased.prev_sibling) graph_[erased.prev_sibling].next_sibling = erased.next_sibling;

        // Erase children - essentially breadth first propagation down the tree
		node_queue stack{ erased.child };
		while(stack.empty() == false)
		{
			node next = stack.front(); stack.pop_front();
			Node_& child = graph_[next];
			child.flags &= ~Node_::VALID;
			freed_.push_back(next);
            data_[next].~T();

			if(child.next_sibling) stack.push_front(child.next_sibling);
			if(child.child)        stack.push_front(child.child);
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

    class unordered
    {
    public:
        unordered(directed_tree& graph) : graph_(graph), current_(root) { }

        node operator()(node id)
        {
            while(!graph_.valid(current_) || current_ == root)
            {
                ++current_;
            }

            id = current_;
            current_ ++;

            return id == graph_.graph_.size() ? 0 : id;
        }

    private:
        directed_tree& graph_;
        node           current_;
    };

    /**
     * \brief Breadth first traversal
     */
    class breadth_first
	{
	public:
		breadth_first(directed_tree& graph) : graph_(graph), visit_queue_(0) { }

		node operator()(node id)
		{
			id = visit_queue_.back(); visit_queue_.pop_back();
			Node_& current = graph_.graph_[id];

			if(current.next_sibling) visit_queue_.push_back(current.next_sibling);
			if(current.child) visit_queue_.push_front(current.child);

			if(visit_queue_.empty()) return 0;
			return id;
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
			Node_& current = graph_.graph_[id];

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

		node operator()(node id)
		{
			if(id == 0) visit_queue_.push_back(graph_.left_most(id));

			id = visit_queue_.front(); visit_queue_.pop_front();
			Node_& current = graph_.graph_[id];

			if(current.Sibling)
			{
				if(graph_.next_sibling(current.Sibling)) visit_queue_.push_back(current.parent);
				visit_queue_.push_back(graph_.left_most(current.Sibling));
			}

			return id;
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

		node operator()(node id)
		{
			if(visit_queue_.empty()) visit_queue_.push_back(graph_.left_most(id));

			id = visit_queue_.front(); visit_queue_.pop_front();
			if(id == 0) return id;
			Node_& current = graph_.graph_[id];

			visit_queue_.push_back(current.Sibling ? graph_.left_most(current.Sibling) : graph_.parent(id));

			return id;
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
			node id = 0;
			while(id = order_(id))
			{
				if(visitor_(graph_[id], id)) break;
			}
		}

	private:
		directed_tree& graph_;
		visitor_type&   visitor_;
		order_type      order_;
	};
};
}

#endif // OPEN_CPP_UTILS_DIRECTED_TREE_H
