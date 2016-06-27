#ifndef PMVECTOR_H
#define PMVECTOR_H

#include <typeinfo>
#include <typeindex>
#include <map>
#include <vector>

// This class provides a polymorphic class container
// that provides the ability to store different
// types of classes inherited from a common
// base class.

typedef std::map<std::type_index, void*> TypeMap;

template <class Base>
class PMIterator
{
public:
    typedef std::vector<Base> BaseVector;

private:
    const TypeMap &VecMap;
    TypeMap::const_iterator MapItr;
    typename BaseVector::iterator VecItr;

public:
    PMIterator(const TypeMap &vecMap, TypeMap::const_iterator mapItr, typename BaseVector::iterator vecItr)
        : VecMap(vecMap), MapItr(mapItr), VecItr(vecItr) {}

    Base& operator++()
    {
        // Once we have itterated to the last element in the current vector
        // we need to move to the next one in the map

        ++VecItr;

        if (VecItr == ((BaseVector*)(MapItr->second))->end())
        {
            ++MapItr;

            if (MapItr == VecMap.end())
                --MapItr;
            else
                VecItr = ((BaseVector*)(MapItr->second))->begin();
        }

        return *VecItr;
    }

    bool operator!=(const PMIterator<Base> &other)
    {
        return (MapItr == other.MapItr) && (VecItr == other.VecItr);
    }

    Base& operator*()
    {
        return *VecItr;
    }
};

template <class Base>
class PMCollection
{
private:
    TypeMap VecMap;

    template<class Derived>
    std::vector<Derived> &GetVec()
    {
        return *((std::vector<Derived>*)VecMap[std::type_index(typeid(Derived))]);
    }

public:
    // Derived has to be derivative class of base

    template<class Derived, typename... Args>
    Derived& emplace(Args... args)
    {
        GetVec<Derived>().emplace_back(args...);
        return GetVec<Derived>().back();
    }

    template<class Derived>
    void pop_back()
    {
        GetVec<Derived>().pop_back();
    }

    PMIterator<Base> begin() const
    {
        auto vb = VecMap.begin();
        return PMIterator<Base>(VecMap, vb, ((std::vector<Base>*)(vb->second))->begin());
    }

    PMIterator<Base> end() const
    {
        auto ve = --VecMap.end();
        return PMIterator<Base>(VecMap, ve, ((std::vector<Base>*)(ve->second))->end());
    }
};

#endif // PMVECTOR_H
