unsigned long long fact(unsigned long long n) {
    unsigned long long acc = 1;
    for (unsigned long long i = 2; i <= n; ++i) {
        acc *= i;
    }
    return acc;
}
