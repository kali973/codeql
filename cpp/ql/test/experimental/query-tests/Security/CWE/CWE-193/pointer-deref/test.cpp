char *malloc(int size);

void test1(int size) {
    char* p = malloc(size);
    char* q = p + size;
    char a = *q; // BAD
    char b = *(q - 1); // GOOD
    char c = *(q + 1); // BAD
    char d = *(q + size); // BAD [NOT DETECTED]
    char e = *(q - size); // GOOD
    char f = *(q + size + 1); // BAD [NOT DETECTED]
    char g = *(q - size - 1); // GOOD
}

void test2(int size) {
    char* p = malloc(size);
    char* q = p + size - 1;
    char a = *q; // GOOD
    char b = *(q - 1); // GOOD
    char c = *(q + 1); // BAD
    char d = *(q + size); // BAD [NOT DETECTED]
    char e = *(q - size); // GOOD
    char f = *(q + size + 1); // BAD [NOT DETECTED]
    char g = *(q - size - 1); // GOOD
}

void test3(int size) {
    char* p = malloc(size + 1);
    char* q = p + (size + 1);
    char a = *q; // BAD
    char b = *(q - 1); // GOOD
    char c = *(q + 1); // BAD
    char d = *(q + size); // BAD [NOT DETECTED]
    char e = *(q - size); // GOOD
    char f = *(q + size + 1); // BAD [NOT DETECTED]
    char g = *(q - size - 1); // GOOD
}

void test4(int size) {
    char* p = malloc(size - 1);
    char* q = p + (size - 1);
    char a = *q; // BAD
    char b = *(q - 1); // GOOD
    char c = *(q + 1); // BAD
    char d = *(q + size); // BAD [NOT DETECTED]
    char e = *(q - size); // GOOD
    char f = *(q + size + 1); // BAD [NOT DETECTED]
    char g = *(q - size - 1); // GOOD
}

char* mk_array(int size, char** end) {
    char* begin = malloc(size);
    *end = begin + size;

    return begin;
}

void test5(int size) {
    char* end;
    char* begin = mk_array(size, &end);

    for (char* p = begin; p != end; ++p) {
        *p = 0; // GOOD
    }

    for (char* p = begin; p <= end; ++p) {
        *p = 0; // BAD
    }

    for (char* p = begin; p < end; ++p) {
        *p = 0; // GOOD
    }
}

struct array_t {
    char* begin;
    char* end;
};

array_t mk_array(int size) {
    array_t arr;
    arr.begin = malloc(size);
    arr.end = arr.begin + size;

    return arr;
}

void test6(int size) {
    array_t arr = mk_array(size);

    for (char* p = arr.begin; p != arr.end; ++p) {
        *p = 0; // GOOD
    }

    for (char* p = arr.begin; p <= arr.end; ++p) {
        *p = 0; // BAD
    }

    for (char* p = arr.begin; p < arr.end; ++p) {
        *p = 0; // GOOD
    }
}

void test7_callee(array_t arr) {
    for (char* p = arr.begin; p != arr.end; ++p) {
        *p = 0; // GOOD
    }

    for (char* p = arr.begin; p <= arr.end; ++p) {
        *p = 0; // BAD
    }

    for (char* p = arr.begin; p < arr.end; ++p) {
        *p = 0; // GOOD
    }
}

void test7(int size) {
    test7_callee(mk_array(size));
}

void test8(int size) {
    array_t arr;
    char* p = malloc(size);
    arr.begin = p;
    arr.end = p + size;

    for (int i = 0; i < arr.end - arr.begin; i++) {
        *(arr.begin + i) = 0; // GOOD
    }

    for (int i = 0; i != arr.end - arr.begin; i++) {
        *(arr.begin + i) = 0; // GOOD
    }

    for (int i = 0; i <= arr.end - arr.begin; i++) {
        *(arr.begin + i) = 0; // BAD [NOT DETECTED]
    }
}

array_t *mk_array_p(int size) {
    array_t *arr = (array_t*) malloc(sizeof(array_t));
    arr->begin = malloc(size);
    arr->end = arr->begin + size;

    return arr;
}

void test9(int size) {
    array_t *arr = mk_array_p(size);

    for (char* p = arr->begin; p != arr->end; ++p) {
        *p = 0; // GOOD
    }

    for (char* p = arr->begin; p <= arr->end; ++p) {
        *p = 0; // BAD
    }

    for (char* p = arr->begin; p < arr->end; ++p) {
        *p = 0; // GOOD
    }
}

void test10_callee(array_t *arr) {
    for (char* p = arr->begin; p != arr->end; ++p) {
        *p = 0; // GOOD
    }

    for (char* p = arr->begin; p <= arr->end; ++p) {
        *p = 0; // BAD
    }

    for (char* p = arr->begin; p < arr->end; ++p) {
        *p = 0; // GOOD
    }
}

void test10(int size) {
    test10_callee(mk_array_p(size));
}

void deref_plus_one(char* q) {
    char a = *(q + 1); // BAD [NOT DETECTED]
}

void test11(unsigned size) {
    char *p = malloc(size);
    char *q = p + size - 1;
    deref_plus_one(q);
}

void test12(unsigned len, unsigned index) {
    char* p = (char *)malloc(len);
    char* end = p + len;
    
    if(p + index > end) {
        return;
    }
    
    p[index] = '\0'; // BAD
}

void test13(unsigned len, unsigned index) {
    char* p = (char *)malloc(len);
    char* end = p + len;
    
    char* q = p + index;
    if(q > end) {
        return;
    }
    
    *q = '\0'; // BAD
}

bool unknown();

void test14(unsigned long n, char *p) {
  while (unknown()) {
    n++;
    p = (char *)malloc(n);
    p[n - 1] = 'a'; // GOOD
  }
}

void test15(unsigned index) {
  unsigned size = index + 13;
  if(size < index) {
    return;
  }
  int* newname = new int[size];
  newname[index] = 0; // GOOD [FALSE POSITIVE]
}

void test16(unsigned index) {
  unsigned size = index + 13;
  if(size >= index) {
    int* newname = new int[size];
    newname[index] = 0; // GOOD [FALSE POSITIVE]
  }
}

void *realloc(void *, unsigned);

void test17(unsigned *p, unsigned x, unsigned k) {
    if(k > 0 && p[1] <= p[0]){
        unsigned n = 3*p[0] + k;
        p = (unsigned*)realloc(p, n);
        p[0] = n;
        unsigned i = p[1];
        // The following access is okay because:
        // n = 3*p[0] + k >= p[0] + k >= p[1] + k > p[1] = i
        // (where p[0] denotes the original value for p[0])
        p[i] = x; // GOOD [FALSE POSITIVE]
    }
}

void test17(unsigned len)
{
  int *xs = new int[len];
  int *end = xs + len;
  for (int *x = xs; x <= end; x++)
  {
    int i = *x; // BAD
  }
}

void test18(unsigned len)
{
  int *xs = new int[len];
  int *end = xs + len;
  for (int *x = xs; x <= end; x++)
  {
    *x = 0; // BAD
  }
}

void test19(unsigned len)
{
  int *xs = new int[len];
  int *end = xs + len;
  for (int *x = xs; x < end; x++)
  {
    int i = *x; // GOOD [FALSE POSITIVE]
  }
}

void test20(unsigned len)
{
  int *xs = new int[len];
  int *end = xs + len;
  for (int *x = xs; x < end; x++)
  {
    *x = 0; // GOOD [FALSE POSITIVE]
  }
}