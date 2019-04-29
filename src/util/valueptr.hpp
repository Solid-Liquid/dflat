#pragma once

namespace dflat
{

template <typename T>
class ValuePtr
{
    public:
        ValuePtr(T t)
            : _obj(new T(std::move(t)))
        {}

        ValuePtr(ValuePtr const& p)
            : _obj(new T(*p._obj))
        {}

        ValuePtr(ValuePtr&& p)
            : _obj(p._obj)
        {
            p._obj = nullptr;
        }

        ~ValuePtr()
        {
            delete _obj;
        }

        ValuePtr& operator=(T t)
        {
            if (!_obj)
            {
                _obj = new T(std::move(t));
            }
            else
            {
                _obj = std::move(t);
            }

            return *this;
        }
        
        ValuePtr& operator=(ValuePtr const& p)
        {
            if (!_obj)
            {
                _obj = new T(*p._obj);
            }
            else
            {
                _obj = *p._obj;
            }

            return *this;
        }

        ValuePtr& operator=(ValuePtr&& p)
        {
            _obj = p._obj;
            p._obj = nullptr;
            return *this;
        }
        operator T const&() const
        {
            return *_obj;
        }

        operator T&()
        {
            return *_obj;
        }
        
        T const* operator->() const
        {
            return _obj;
        }

        T* operator->()
        {
            return _obj;
        }

    private:
        T* _obj;
};

} // namespace std
