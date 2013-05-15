



class UnionFind
{
    private:
        int* _classes;
        unsigned _size;

    public:
        UnionFind() {}
        UnionFind(unsigned size)
        {
            _size = size;
            _classes = new int[size];

            for (int i = 0; i < size; i++)
                _classes[i] = i;
        }

        int find(int repr)
        {
            if (_classes[repr] != repr)
                _classes[repr] = find(_classes[repr]);

            return _classes[repr];
        }


        inline void basicUnion(int repr1, int repr2)
        {
            int father1 = find(repr1), father2 = find(repr2);

            _classes[father1] = _classes[father2];
        }


        inline void unionAfterFind(int repr1, int repr2)
        {
            _classes[repr1] = _classes[repr2];
        }


        inline unsigned size(void)
        {
            return _size;
        }
};

            



