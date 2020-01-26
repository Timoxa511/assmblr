//Stack
namespace cool
{

//#define VAR(...) ##__VA_ARGS__##(__LINE__);

class Stack
    {
    public:

    enum traits {Sz = 5};

    //int name_;

    int data_ [Sz];

    size_t sz_;

    Stack () :
        data_ (),
        sz_ (0)
        {}


    // Element access.

          int& top ()
            {

            if (empty()) throw std::logic_error ("stack is empty");

            return *(data() + size() - 1);
            }

    // getters

    size_t size () const { return sz_; }

    const int* data () const { return data_; }
          int* data ()       { return data_; }

    bool empty () const { return size() == 0; }


    bool push_back (int val)
        {
        if (sz_ == Sz) return false;

        data_[sz_++] = val;

        return true;
        }

    bool pop_back ()
        {
        if (sz_ == 0) return false;

        data_[sz_-- - 1] = 0;

        return true;
        }

    void dump ()
        {
        printf ("Stack:\nsz_ = %zu\n", sz_);
        for (int i = ((int)sz_) - 1; i >= 0; i--)
            printf ("   [%d] = %d\n", i, data_[i]);
        }


    };


};
