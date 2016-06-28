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
class PMCollection
{
private:
    std::vector<std::size_t> offsets;
    char* items;
    std::size_t curAlloc;
    std::size_t curUsed = 0;
    const std::size_t DefAlloc = 5;

public:
    PMCollection()
    {
        items = (char*)malloc(sizeof(Base) * DefAlloc);
        curAlloc = sizeof(Base) * DefAlloc;
    }

    ~PMCollection()
    {
        for (auto itr = begin(); itr != end(); ++itr)
            (*itr)->~Base();

        free(items);
    }

    Base* back()
    {
        return (Base*)(items + curUsed - offsets.back());
    }

    template <class Derived, typename... Args>
    Derived& emplace(Args... args)
    {
        std::size_t addSize = sizeof(Derived);

        if ((curUsed + addSize) > curAlloc)
        {
            if (addSize > curAlloc)
                curAlloc += DefAlloc * addSize;
            else
                curAlloc *= 2;

            items = (char*)realloc(items, curAlloc);
        }

        new ((void*)(items + curUsed)) Derived(args...);
        curUsed += addSize;
        offsets.push_back(addSize);

        return *((Derived*)back());
    }

    void pop_back()
    {
        back()->~Base();
        curUsed -= offsets.back();
    }

    void shrink_to_fit()
    {
        // TODO: use this thing
        if (curUsed < curAlloc)
        {
            curAlloc = curUsed;
            items = (char*)realloc(items, curAlloc);
        }
    }

    class iterator
    {
    private:
        char *CurAddr;
        typename std::vector<std::size_t>::const_iterator OffItr;
    public:
        iterator(char *curAddr, typename std::vector<std::size_t>::const_iterator offItr)
            : CurAddr(curAddr), OffItr(offItr) {}

        Base* operator*()
        {
            return (Base*)CurAddr;
        }

        Base* operator++()
        {
            CurAddr += *OffItr;
            ++OffItr;
            return (Base*)CurAddr;
        }

        bool operator !=(const iterator &other)
        {
            return (CurAddr != other.CurAddr);
        }
    };

    iterator begin() const
    {
        return iterator(items, offsets.begin());
    }

    iterator end() const
    {
        return iterator(items + curUsed, offsets.end());
    }
};

#endif // PMVECTOR_H
