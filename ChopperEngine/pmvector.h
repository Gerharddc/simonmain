#ifndef PMVECTOR_H
#define PMVECTOR_H

#include <typeinfo>
#include <typeindex>
#include <map>
#include <vector>
#include <memory>

// This class provides a polymorphic class container
// that provides the ability to store different
// types of classes inherited from a common
// base class.

template <class Base>
class BaseVecIterator
{
public:
    virtual ~BaseVecIterator() {}

    virtual Base* operator++() = 0;
    virtual Base* operator*() = 0;
    virtual bool operator!=(const std::shared_ptr<BaseVecIterator<Base>> &other) = 0;
};

template <class Base, class Derived>
class DerivedVecIterator : public BaseVecIterator<Base>
{
private:
    typename std::vector<Derived>::iterator Itr;

public:
    DerivedVecIterator(typename std::vector<Derived>::iterator itr) : Itr(itr) {}

    Base* operator++()
    {
        ++Itr;
        return &(*Itr);
    }

    Base* operator*()
    {
        return &(*Itr);
    }

    bool operator!=(const std::shared_ptr<BaseVecIterator<Base>> &other)
    {
        return (this->operator *() != **other);
    }
};

template <class Base>
class BaseVector
{
public:
    virtual ~BaseVector() {}

    virtual std::shared_ptr<BaseVecIterator<Base>> begin() = 0;
    virtual std::shared_ptr<BaseVecIterator<Base>> end() = 0;
};

template <class Base, class Derived>
class DerivedVector : public BaseVector<Base>
{
private:
    typename std::vector<Derived>::iterator Itr;

public:
    std::vector<Derived> Vect;

    std::shared_ptr<BaseVecIterator<Base>> begin()
    {
        return std::make_shared<DerivedVecIterator<Base, Derived>>(Vect.begin());
    }

    std::shared_ptr<BaseVecIterator<Base>> end()
    {
        return std::make_shared<DerivedVecIterator<Base, Derived>>(Vect.end());
    }
};

template <class Base>
class PMIterator
{
private:
    typedef std::map<std::type_index, std::shared_ptr<BaseVector<Base>>> TypeMap;
    const TypeMap &VecMap;
    typename TypeMap::const_iterator MapItr;
    std::shared_ptr<BaseVecIterator<Base>> VecItr;

public:
    PMIterator(const TypeMap &vecMap, typename TypeMap::const_iterator mapItr, std::shared_ptr<BaseVecIterator<Base>> vecItr)
        : VecMap(vecMap), MapItr(mapItr), VecItr(vecItr) {}

    Base* operator++()
    {
        // Once we have itterated to the last element in the current vector
        // we need to move to the next one in the map

        VecItr->operator ++();

        if (VecItr->operator !=(MapItr->second->end()))
        {
            ++MapItr;

            if (MapItr == VecMap.end())
                --MapItr;
            else
                VecItr = MapItr->second->begin();
        }

        return VecItr->operator *();
    }

    bool operator!=(const PMIterator<Base> &other)
    {
        return (MapItr != other.MapItr) || (VecItr->operator !=(other.VecItr));
    }

    Base* operator*()
    {
        return VecItr->operator *();
    }
};

// TODO: we could theoretically use normal pointers instead of shared ones in the map

template <class Base>
class PMCollection
{
private:
    typedef std::map<std::type_index, std::shared_ptr<BaseVector<Base>>> TypeMap;
    TypeMap VecMap;

    // Derived has to be derivative class of base
    template<class Derived>
    std::vector<Derived> &GetVec()
    {
        if (VecMap.count(std::type_index(typeid(Derived))) == 0)
            VecMap[std::type_index(typeid(Derived))] = std::make_shared<DerivedVector<Base, Derived>>();

        return std::static_pointer_cast<DerivedVector<Base, Derived>>(VecMap[std::type_index(typeid(Derived))])->Vect;
    }

public:
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
        return PMIterator<Base>(VecMap, vb, vb->second->begin());
    }

    PMIterator<Base> end() const
    {
        auto ve = --VecMap.end();
        return PMIterator<Base>(VecMap, ve, ve->second->end());
    }
};

#endif // PMVECTOR_H
