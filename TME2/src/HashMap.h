#pragma once
#include <iostream>
#include <vector>
#include <forward_list>
#include <utility>
#include <cstddef>

using namespace std;
template<typename K, typename V>
class HashMap {
public:
    // Entry stores a const key and a mutable value
    struct Entry {
        const K key;
        V value;
        Entry(const K& key_, const V& value_):key(key_),value(value_){}
    };

    using Bucket = std::forward_list<Entry>;
    using Table  = std::vector<Bucket>;

    // Construct with a number of buckets (must be >= 1)
    HashMap(std::size_t nbuckets = 1024):buckets_(nbuckets){
        if(nbuckets<1) cerr<<" nbuckets must be >=1"<<endl;
    }

    // Return pointer to value associated with key, or nullptr if not found.
    // Only iterate the appropriate bucket.
    V* get(const K& key){
        std::size_t h = std::hash<K>()(key);
        // On trouve le bon bucket
        Bucket &buck = buckets_[h % buckets_.size()];

        for (Entry &ent : buck)
        {
            if (ent.key == key)
            {
                // Trouvé
                return &ent.value;
            }
        }
        return nullptr;
    }

     //Insert or update (key,value).
     //Returns true if an existing entry was updated, false if a new entry was inserted.
    bool put(const K& key, const V& value){
        std::size_t h = std::hash<K>()(key);
        //  Trouver le bon bucket
        Bucket &buck = buckets_[h % buckets_.size()];

        // Parcours des Entrées du bon bucket
        for (Entry &ent : buck)
        {
            if (ent.key == key)
            {
                // Clé Trouvé
                ent.value = value;
                return true;
            }
        }
        buck.push_front(Entry(key, value));
        ++count_;
        return false;
    }

    // Current number of stored entries
    std::size_t size() const{
        return count_;
    }

    // Convert table contents to a vector of key/value pairs.
    std::vector<std::pair<K,V>> toKeyValuePairs() const{
        std::vector<std::pair<K,V>> vect;
        // Parcours de tout les buckets
        for(const Bucket &buck : buckets_)
        {
            // Parcours de toute les entrée de chaque Bucket
            for (Entry ent : buck)
            {
                vect.push_back(std::pair(ent.key, ent.value));
            }
        }
        return vect;
    }

    // Optional: number of buckets
    // std::size_t bucket_count() const;

private:
    Table buckets_;
    std::size_t count_ = 0;
};
