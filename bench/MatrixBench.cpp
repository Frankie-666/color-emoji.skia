#include "SkBenchmark.h"
#include "SkMatrix.h"
#include "SkRandom.h"
#include "SkString.h"

class MatrixBench : public SkBenchmark {
    SkString    fName;
    enum { N = 100000 };
public:
    MatrixBench(void* param, const char name[]) : INHERITED(param) {
        fName.printf("matrix_%s", name);
    }

    virtual void performTest() = 0;

protected:
    virtual const char* onGetName() {
        return fName.c_str();
    }

    virtual void onDraw(SkCanvas* canvas) {
        for (int i = 0; i < N; i++) {
            this->performTest();
        }
    }

private:
    typedef SkBenchmark INHERITED;
};

// we want to stop the compiler from eliminating code that it thinks is a no-op
// so we have a non-static global we increment, hoping that will convince the
// compiler to execute everything
int gMatrixBench_NonStaticGlobal;

#define always_do(pred)                     \
    do {                                    \
        if (pred) {                         \
            ++gMatrixBench_NonStaticGlobal; \
        }                                   \
    } while (0)

class EqualsMatrixBench : public MatrixBench {
public:
    EqualsMatrixBench(void* param) : INHERITED(param, "equals") {}
protected:
    virtual void performTest() {
        SkMatrix m0, m1, m2;

        m0.reset();
        m1.reset();
        m2.reset();
        always_do(m0 == m1);
        always_do(m1 == m2);
        always_do(m2 == m0);
        always_do(m0.getType());
        always_do(m1.getType());
        always_do(m2.getType());
    }
private:
    typedef MatrixBench INHERITED;
};

class ScaleMatrixBench : public MatrixBench {
public:
    ScaleMatrixBench(void* param) : INHERITED(param, "scale") {

        fM0.reset();
        fM1.setScale(fSX, fSY);
        fM2.setTranslate(fSX, fSY);
        fSX = fSY = SkFloatToScalar(1.5f);
    }
protected:
    virtual void performTest() {
        SkMatrix m;
        m = fM0; m.preScale(fSX, fSY);
        m = fM1; m.preScale(fSX, fSY);
        m = fM2; m.preScale(fSX, fSY);
    }
private:
    SkMatrix fM0, fM1, fM2;
    SkScalar fSX, fSY;
    typedef MatrixBench INHERITED;
};

// having unknown values in our arrays can throw off the timing a lot, perhaps
// handling NaN values is a lot slower. Anyway, this guy is just meant to put
// reasonable values in our arrays.
template <typename T> void init9(T array[9]) {
    SkRandom rand;
    for (int i = 0; i < 9; i++) {
        array[i] = rand.nextSScalar1();
    }
}

// Test the performance of setConcat() non-perspective case:
// using floating point precision only.
class FloatConcatMatrixBench : public MatrixBench {
public:
    FloatConcatMatrixBench(void* p) : INHERITED(p, "concat_floatfloat") {
        init9(mya);
        init9(myb);
        init9(myr);
    }
protected:
    static inline void muladdmul(float a, float b, float c, float d,
                                   float* result) {
      *result = a * b + c * d;
    }
    virtual void performTest() {
        const float* a = mya;
        const float* b = myb;
        float* r = myr;
        muladdmul(a[0], b[0], a[1], b[3], &r[0]);
        muladdmul(a[0], b[1], a[1], b[4], &r[1]);
        muladdmul(a[0], b[2], a[1], b[5], &r[2]);
        r[2] += a[2];
        muladdmul(a[3], b[0], a[4], b[3], &r[3]);
        muladdmul(a[3], b[1], a[4], b[4], &r[4]);
        muladdmul(a[3], b[2], a[4], b[5], &r[5]);
        r[5] += a[5];
        r[6] = r[7] = 0.0f;
        r[8] = 1.0f;
    }
private:
    float mya [9];
    float myb [9];
    float myr [9];
    typedef MatrixBench INHERITED;
};

static inline float SkDoubleToFloat(double x) {
    return static_cast<float>(x);
}

// Test the performance of setConcat() non-perspective case:
// using floating point precision but casting up to float for
// intermediate results during computations.
class FloatDoubleConcatMatrixBench : public MatrixBench {
public:
    FloatDoubleConcatMatrixBench(void* p) : INHERITED(p, "concat_floatdouble") {
        init9(mya);
        init9(myb);
        init9(myr);
    }
protected:
    static inline void muladdmul(float a, float b, float c, float d,
                                   float* result) {
      *result = SkDoubleToFloat((double)a * b + (double)c * d);
    }
    virtual void performTest() {
        const float* a = mya;
        const float* b = myb;
        float* r = myr;
        muladdmul(a[0], b[0], a[1], b[3], &r[0]);
        muladdmul(a[0], b[1], a[1], b[4], &r[1]);
        muladdmul(a[0], b[2], a[1], b[5], &r[2]);
        r[2] += a[2];
        muladdmul(a[3], b[0], a[4], b[3], &r[3]);
        muladdmul(a[3], b[1], a[4], b[4], &r[4]);
        muladdmul(a[3], b[2], a[4], b[5], &r[5]);
        r[5] += a[5];
        r[6] = r[7] = 0.0f;
        r[8] = 1.0f;
    }
private:
    float mya [9];
    float myb [9];
    float myr [9];
    typedef MatrixBench INHERITED;
};

// Test the performance of setConcat() non-perspective case:
// using double precision only.
class DoubleConcatMatrixBench : public MatrixBench {
public:
    DoubleConcatMatrixBench(void* p) : INHERITED(p, "concat_double") {
        init9(mya);
        init9(myb);
        init9(myr);
    }
protected:
    static inline void muladdmul(double a, double b, double c, double d,
                                   double* result) {
      *result = a * b + c * d;
    }
    virtual void performTest() {
        const double* a = mya;
        const double* b = myb;
        double* r = myr;
        muladdmul(a[0], b[0], a[1], b[3], &r[0]);
        muladdmul(a[0], b[1], a[1], b[4], &r[1]);
        muladdmul(a[0], b[2], a[1], b[5], &r[2]);
        r[2] += a[2];
        muladdmul(a[3], b[0], a[4], b[3], &r[3]);
        muladdmul(a[3], b[1], a[4], b[4], &r[4]);
        muladdmul(a[3], b[2], a[4], b[5], &r[5]);
        r[5] += a[5];
        r[6] = r[7] = 0.0;
        r[8] = 1.0;
    }
private:
    double mya [9];
    double myb [9];
    double myr [9];
    typedef MatrixBench INHERITED;
};


static SkBenchmark* M0(void* p) { return new EqualsMatrixBench(p); }
static SkBenchmark* M1(void* p) { return new ScaleMatrixBench(p); }
static SkBenchmark* M2(void* p) { return new FloatConcatMatrixBench(p); }
static SkBenchmark* M3(void* p) { return new FloatDoubleConcatMatrixBench(p); }
static SkBenchmark* M4(void* p) { return new DoubleConcatMatrixBench(p); }

static BenchRegistry gReg0(M0);
static BenchRegistry gReg1(M1);
static BenchRegistry gReg2(M2);
static BenchRegistry gReg3(M3);
static BenchRegistry gReg4(M4);
