int global;

int func(int parameter)
{
    int local = 5;
    return local + parameter;
}

struct test
{
    int member;
    
    int mem_func()
    {
        return func(member);
    }
};

int main()
{
    int main_local;
    
    test t;
    t.member = 5;
    for(int i = 0; i < 10; ++i)
        main_local += t.mem_func() + i;
    
    return main_local;
}
