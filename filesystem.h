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

#ifndef OPEN_CPP_UTILS_FILESYSTEM_H
#define OPEN_CPP_UTILS_FILESYSTEM_H

#include <filesystem>
#include <iostream>
#include <stack>
#include <utility>

#include "directed_tree.h"

namespace open_cpp_utils
{

using path_t = std::filesystem::path;

template<typename T_, typename L_>
class  filesystem
{
// Typedefs ============================================================================================================
    
public:
    class file;
    using data_t = T_;
    using loader = L_;
    using file_tree = directed_tree<file>;
    using fileptr   = file*;
    using file_id = typename file_tree::node;
    static constexpr file_id root = file_tree::root;


// Structs =============================================================================================================

public:
    class file
    {
    private:
        file(filesystem* parent, file_id node, path_t path, data_t* data)
            : parent_(parent), node_(node), path_(std::move(path)), value_(data)
        { }

    public:
        file() : parent_(nullptr), node_(0), path_(""), value_(nullptr) { }
        file(const file&) = default;
        file(file&&) = default;
        ~file() { delete value_; }

        const path_t& path() const { return path_; }

        data_t* get_data() { return value_; }
        const data_t* get_data() const { return value_; }

        filesystem& system() { return *parent_; }
        const filesystem& system() const { return *parent_; }

        bool is_directory() const { return std::filesystem::is_directory(path_); }
        bool empty() const { return is_empty(path_); }
        bool has_subdirectory() const { for(const auto& path : std::filesystem::directory_iterator{ path_ }) { if(std::filesystem::is_directory(path)) return true; } return false; }

        void erase() { delete value_; value_ = nullptr; }

        file_id get_id() const { return node_; }

        data_t* operator->() { return value_; }
        const data_t* operator->() const { return value_; }

        data_t* operator*() { return value_; }
        const data_t* operator*() const { return value_; }

    private:
        filesystem* parent_;
        file_id     node_;
        path_t      path_;
        data_t*     value_;

        friend class filesystem;
    };


// Functions ===========================================================================================================
    

// Helpers -------------------------------------------------------------------------------------------------------------

private:
    
    static path_t resolve_(const path_t& path) { return absolute(canonical(path)); }
    
    static bool is_parent_(const path_t& base, const path_t& path)
    { return std::mismatch(path.begin(), path.end(), base.begin(), base.end()).second == base.end(); }
    
    file_id find_(path_t path) const;
    int get_index_(file_id parent, const path_t& path);
    

// Constructors & Destructor -------------------------------------------------------------------------------------------

public:
    filesystem() = default;
    ~filesystem() = default;

    file& operator[](file_id id) { return tree_[id]; }
    const file& operator[](file_id id) const { return tree_[id]; }

    file_id load_directory(const path_t &directory);
    void    close_directory(file_id id) { tree_.erase(id); }
    
    file_id import(const path_t& path, file_id parent);

    file_id create(const std::string& name, file_id parent);
    file_id create_folder(const std::string& name, file_id parent);

    void rename(file_id id, const std::string& name);

    void erase(file_id id);
    void erase(const path_t& path);

    file_id find(const path_t& path) const;
    
    file_id parent(file_id id) const { return tree_.parent(tree_[id].get_id()); }
    file_id next(file_id id)   const { return tree_.next_sibling(tree_[id].get_id()); }
    file_id prev(file_id id)   const { return tree_.prev_sibling(tree_[id].get_id()); }
    file_id begin(file_id id)  const { return tree_.first_child(tree_[id].get_id()); }
    file_id end(file_id)       const { return file_tree::root; }

    uint32_t    depth(file_id id) const { return tree_.depth(tree_[id].get_id()); }

    template<typename O = typename file_tree::pre_order, typename V>
    void traverse(V& visitor) { tree_.template traverse<O>(visitor); }

private:
    file_tree tree_;
};

template<typename T_, typename L_>
typename filesystem<T_, L_>::file_id filesystem<T_, L_>::find_(path_t path) const
{
    // Check if the path exists
    if(not exists(path)) return file_tree::root;

    // Setup for traversal
    path = resolve_(path);
    file_id dir = tree_.first_child(file_tree::root);

    // Get the parent folder
    while(dir != file_tree::root)
    {
        if(is_parent_(tree_[dir].path(), path)) break;
        dir = tree_.next_sibling(dir);
    }

    // Path does not exist in file system
    if(dir == file_tree::root) return file_tree::root;

    // Get starting point for iteration
    auto parent = tree_[dir].path();
    auto start = std::mismatch(path.begin(), path.end(), parent.begin(), parent.end()).first;

    // Parse down the tree
    for(auto it = start; it != path.end(); ++it)
    {
        for(file_id child = tree_.first_child(dir); child != file_tree::root; child = tree_.next_sibling(child))
        {
            if(tree_[child].path().filename() == it->filename())
            {
                dir = child;
                break;
            }
        }
    }

    return dir;
}

template<typename T_, typename L_>
int filesystem<T_, L_>::get_index_(file_id parent, const path_t &path)
{
    file_id dir = tree_.first_child(parent);

    // Get the insertion index
    int i = 0;
    while(dir != file_tree::root)
    {
        if(tree_[dir].path().filename().compare(path.filename()) > 0) break;

        ++i; dir = tree_.next_sibling(dir);
    }

    return i;
}

template<typename T_, typename L_>
typename filesystem<T_, L_>::file_id filesystem<T_, L_>::load_directory(const path_t &directory)
{
    if(not exists(directory)) return root;
    
    file_id dir = tree_.first_child(file_tree::root);

    // Validate this isn't a subdirectory
    while(dir != file_tree::root)
    {
        if(is_parent_(tree_[dir].path(), directory)) return find(directory);
        dir = tree_.next_sibling(dir);
    }

    dir = get_index_(file_tree::root, directory);
    data_t* data = loader::load(directory);
    dir = tree_.insert(file(this, tree_.next_id(), directory, data), file_tree::root, dir);
    file_id res = dir;

    using iter_t = std::filesystem::directory_iterator;
    std::stack<std::tuple<path_t, file_id, iter_t>> working;
    working.emplace(directory, dir, iter_t(directory));

    while(not working.empty())
    {
        auto& top = working.top();
        const file_id p_dir = std::get<1>(top);
        const iter_t& it    = std::get<2>(top);

        if(std::filesystem::begin(it) == std::filesystem::end(it))
        {
            working.pop();
            continue;
        }
        
        const path_t  path  = *it;
        data = loader::load(path);

        file_id created = tree_.insert(file(this, tree_.next_id(), path, data), p_dir, -1);

        if(is_directory(path))
        {
            working.emplace(path, created, iter_t(path));
        }

        ++std::get<2>(top);
    }

    return res;
}

template<typename T_, typename L_>
typename filesystem<T_, L_>::file_id filesystem<T_, L_>::import(const path_t& path, file_id parent)
{
    if(not exists(path)) return root;

    file& prnt = tree_[parent];
    path_t nloc = prnt.path() / path.filename();
    data_t* data = loader::import(path, nloc);

    return tree_.insert(file(this, tree_.next_id(), nloc, data), parent, get_index_(parent, nloc));
}

template<typename T_, typename L_>
typename filesystem<T_, L_>::file_id filesystem<T_, L_>::create(const std::string &name, file_id parent)
{
    file& prnt = tree_[parent];
    const file_id p_dir = prnt.get_id();
    const path_t path  = prnt.path() / name;
    data_t* data = loader::create(path);
    
    const file_id node  = tree_.insert(file(this, tree_.next_id(), path, data), parent, get_index_(parent, path));
    
    return node;
}

template<typename T_, typename L_>
typename filesystem<T_, L_>::file_id filesystem<T_, L_>::create_folder(const std::string &name, file_id parent)
{
    file& prnt = tree_[parent];
    const path_t path  = prnt.path() / name;
    create_directory(path);
    data_t* data = loader::load(path);
    
    const file_id node  = tree_.insert(file(this, tree_.next_id(), path, data), parent, get_index_(parent, path));

    return tree_[node];
}

template<typename T_, typename L_>
void filesystem<T_, L_>::rename(file_id id, const std::string& name)
{
    file& file = tree_[id];
    const std::string new_name = path_t(name).stem().string() + file.path().extension().string();

    fileptr current = &file;
    while(true)
    {
        fileptr next = &tree_[filesystem::next(current->get_id())];
        fileptr prev = &tree_[filesystem::prev(current->get_id())];

        if(next != &tree_[root] && new_name.compare(next->path().filename().string()) > 0) { tree_.swap(current->get_id(), next->get_id()); current = next; continue; }
        if(next != &tree_[root] && new_name.compare(prev->path().filename().string()) < 0) { tree_.swap(current->get_id(), prev->get_id()); current = prev; continue; }
        break;
    }

    const path_t new_path = current->path().parent_path() / new_name;
    std::filesystem::rename(current->path(), new_path);
    current->path_ = new_path;
}

template<typename T_, typename L_>
void filesystem<T_, L_>::erase(file_id id)
{
    file& file = tree_[id];
    std::filesystem::remove(file.path());
    tree_.erase(id);
}

template<typename T_, typename L_>
void filesystem<T_, L_>::erase(const path_t &path)
{
    const file_id id = find_(path);
    erase(tree_[id]);
}

template<typename T_, typename L_>
typename filesystem<T_, L_>::file_id filesystem<T_, L_>::find(const path_t &path) const
{
    return find_(path);
}
    
}

#endif // OPEN_CPP_UTILS_FILESYSTEM_H
