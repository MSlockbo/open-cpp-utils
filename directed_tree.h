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
#include <cstring>

namespace open_cpp_utils
{

/**
 * \brief Class for creating a directed tree
 * \tparam T Type of the data associated with each node
 *
 * The tree is a series of child nodes in forward linked lists.
 *
 */
template<typename T, class Alloc = std::allocator<T>>
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
	using node       = size_t;
	using node_queue = std::deque<node>;

private:
    using s_alloc   = Alloc;
    using h_alloc   = typename std::allocator_traits<s_alloc>::template rebind_alloc<Node_>; // Gross
    using hierarchy = Node_*;
    using storage   = data_type*;


// Constants ===========================================================================================================

public:
    static constexpr std::integral_constant<node, 0> root{};


// Data Structures =====================================================================================================

private:
	struct Node_
	{
		enum flags
		{
			valid = 0x0001
		};

		node parent, child, prev_sibling, next_sibling;
		uint32_t flags, depth;

		Node_() : parent(0), child(0), prev_sibling(0), next_sibling(0), flags(0), depth(0) { }
	};


// Functions ===========================================================================================================

private:

// Helpers -------------------------------------------------------------------------------------------------------------

    void grow_()
    {
        hierarchy g_old = graph_;
        storage   d_old = data_;
        size_t    c_old = capacity_;

        if(capacity_ == 0) capacity_  = 10;
        else               capacity_ *= 2;

        graph_ = g_alloc_.allocate(capacity_);
        data_  = d_alloc_.allocate(capacity_);

        if(size_ > 0)
        {
            std::memcpy(graph_, g_old, size_ * sizeof(Node_));
            std::memcpy(data_,  d_old, size_ * sizeof(data_type));

            for(node i = size_; i < capacity_; ++i)
            {
                graph_[i] = Node_();
                std::construct_at(data_ + i);
            }
        }
        else
        {
            for(node i = 0; i < capacity_; ++i)
            {
                graph_[i] = Node_();
                std::construct_at(data_ + i);
            }

            graph_[0].flags = Node_::valid;
        }

        g_alloc_.deallocate(g_old, c_old);
        d_alloc_.deallocate(d_old, c_old);
    }

    node push_back_(const data_type& data)
    {
        if(size_ >= capacity_) grow_();
        std::construct_at(data_ + size_, data);
        return size_++;
    }

    node push_back_(data_type&& data)
    {
        if(size_ >= capacity_) grow_();
        std::construct_at(data_ + size_, std::forward<T>(data));
        return size_++;
    }
    

public:

// Constructors & Destructor -------------------------------------------------------------------------------------------

    /**
     * \brief Default constructor, creates tree with empty root
     */
    directed_tree()
        : size_(0), capacity_(0)
        , graph_(nullptr), data_(nullptr)
    { push_back_(T()); }
    
    directed_tree(data_type&& data)
        : size_(0), capacity_(0)
        , graph_(nullptr), data_(nullptr)
    { push_back_(std::forward<T>(data)); }
    
    directed_tree(const data_type& data)
        : size_(0), capacity_(0)
        , graph_(nullptr), data_(nullptr)
    { push_back_(data); }

    ~directed_tree() = default;


// Tree Navigation -----------------------------------------------------------------------------------------------------

    /**
     * \brief Check whether a node is valid. O(1)
     * \param id Node id to reference
     * \return Whether the valid flag is true in the node
     */
    [[nodiscard]] bool valid(node id) const { return graph_[id].flags & Node_::valid; }

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
        if(freed_.empty()) return static_cast<node>(size_);
        return freed_.front();
    }

    /**
     * \brief Insert a node into the tree as a child of the provided node
     * \param data Value to insert
     * \param p_id Id of the parent node
     * \param sib Child to insert before, passing root specifies to insert to the back
     * \return Id of the inserted node
     */
    node insert(const data_type& data, node p_id, node sib = 0)
    {
        // If there are no freed nodes, create a new node and mark it as freed
        if(freed_.empty())
        {
            freed_.push_back(push_back_(std::forward<T>(data)));
        }
        else
        {
            std::construct_at(data_ + freed_.front(), std::forward<T>(data));
        }

        // Pop a freed node from the stack
        node id   = freed_.front(); freed_.pop_front();
        bool back = sib == 0;
        node s_id = back ? last_child(p_id) : sib;
        Node_& node   = graph_[id];
        Node_& parent = graph_[p_id];

        Node_& sibling = graph_[s_id];
        if(parent.child == root || (s_id == parent.child && !back)) parent.child = id;

        node.next_sibling = node.prev_sibling = 0;
        node.parent = p_id;
        node.depth = parent.depth + 1;
        node.flags = Node_::valid;
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

    /**
     * \brief Insert a node into the tree as a child of the provided node
     * \param data Value to insert
     * \param p_id Id of the parent node
     * \param sib Child to insert before, passing root specifies to insert to the back
     * \return Id of the inserted node
     */
    node insert(data_type&& data, node p_id, node sib = root)
    {
        // If there are no freed nodes, create a new node and mark it as freed
        if(freed_.empty())
        {
            freed_.push_back(push_back_(std::forward<T>(data)));
        }
        else
        {
            std::construct_at(data_ + freed_.front(), std::forward<T>(data));
        }

        // Pop a freed node from the stack
        node id   = freed_.front(); freed_.pop_front();
        bool back = sib == root;
        node s_id = back ? last_child(p_id) : sib;
        Node_& node   = graph_[id];
        Node_& parent = graph_[p_id];

        Node_& sibling = graph_[s_id];
        if(parent.child == root || (s_id == parent.child && !back)) parent.child = id;

        node.next_sibling = node.prev_sibling = 0;
        node.parent = p_id;
        node.depth = parent.depth + 1;
        node.flags = Node_::valid;
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

    void clear()
    {
        for(int i = 0; i < size_; ++i)
        {
            if(valid(i) == false) continue;

            graph_[i].flags = 0;
            std::destroy_at(data_ + i);
            freed_.push_back(i);
        }

        g_alloc_.deallocate(graph_, capacity_);
        d_alloc_.deallocate(data_, capacity_);
        capacity_ = 0; size_ = 0;
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
		erased.flags = 0;
		freed_.push_back(id);
        std::destroy_at(data_ + id);

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
			child.flags = 0;
			freed_.push_back(next);
		    std::destroy_at(data_ + next);

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
	h_alloc   g_alloc_;
    s_alloc   d_alloc_;
    size_t    size_, capacity_;
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
