#include <vector>

unsigned int gcd(unsigned int a, unsigned int b)
{
    if(a > b) return gcd(a-b, b);
    else if(b > a) return gcd(a, b-a);
    else return a;
}

int gcd(int a, int b)
{
    if(a > 0 && b > 0)
        return gcd(static_cast<unsigned int>(a), static_cast<unsigned int>(b));
    else if(!b) return a;
    else return gcd(b, a % b);
}

int gcd(const std::vector<int> n)
{
    if(n.empty()) return 0;
    
    int d = n[0];
    for(int i = 1; i < n.size(); ++i)
        d = gcd(d, n[i]);
    return d;
}

int main()
{
    
}
