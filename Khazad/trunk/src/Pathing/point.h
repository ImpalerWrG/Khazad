#ifndef point_HEADER
#define point_HEADER

#include <assert.h>
#include <string.h>

class point
{
public:
    point() { } // uninitialized constructor
    virtual ~point() {}

    point(unsigned x, unsigned y, unsigned z)
    {
        x_[0] = x;
        x_[1] = y;
        x_[2] = z;
    }
    static const unsigned dim = 3;

    const unsigned& operator[] (unsigned i) const
    {
        assert(i < dim);
        return x_[i];
    }
    void set (unsigned i, unsigned val)
    {
        assert(i < dim);
        x_[i] = val;
    }

    bool operator == (const point& other) const
    {
        return memcmp(x_,other.x_,dim*sizeof(unsigned))==0;
        /*for (unsigned i = 0; i < dim; ++i) {
          if (x_[i] != other[i]) { return false; }
        }
        return true;*/
    }

    /* Arbitrary total ordering. */
    bool operator < (const point& other) const
    {
        for (unsigned i = 0; i < dim; ++i)
        {
            if (x_[i] < other[i])
            {
                return true;
            }
            if (x_[i] > other[i])
            {
                return false;
            }
        }
        return false;
    }

private:
    unsigned x_[dim];

public:
    struct hash
    {
        unsigned operator()(const point& p) const
        {
            unsigned retval = 1;
            for (unsigned i = 0; i < point::dim; ++i)
                retval += p.x_[i] * (1 << (5*i));
            return retval;
        }
    };

};

template <typename T>
struct pointerEqualityPredicate
{
    const T* to_find;

    pointerEqualityPredicate(const T* t) : to_find(t) {}

    bool operator()(const T* other) const
    {
        return *to_find == *other;
    }
};

#endif
