int fact(const int N) {
    if (N == 0 || N == 1) {
        return 1;
    }
    return N * fact(N - 1);
}
