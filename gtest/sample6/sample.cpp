#include <gtest/gtest.h>
#include "fun.h"

/*-------------------TYPED_TEST macro and TYPED_TEST_P macro------------------*/  
//Sample 6: This sample shows how to test common properties of multiple  
// implementations of the same interface (aka interface tests).  
  
// First, we define some factory functions for creating instances of  
// the implementations.  You may be able to skip this step if all your  
// implementations can be constructed the same way.  
  
template <class T>  
PrimeTable* CreatePrimeTable();  
  
template <>  
PrimeTable* CreatePrimeTable<OnTheFlyPrimeTable>() {  
    return new OnTheFlyPrimeTable;  
}  
  
template <>  
PrimeTable* CreatePrimeTable<PreCalculatedPrimeTable>() {  
    return new PreCalculatedPrimeTable(10000);  
}  
  
// Then we define a test fixture class template.  
template <class T>  
class PrimeTableTest : public testing::Test {  
protected:  
    // The ctor calls the factory function to create a prime table  
    // implemented by T.  
    PrimeTableTest() : table_(CreatePrimeTable<T>()) {}  
  
    virtual ~PrimeTableTest() { delete table_; }  
  
    // Note that we test an implementation via the base interface  
    // instead of the actual implementation class.  This is important  
    // for keeping the tests close to the real world scenario, where the  
    // implementation is invoked via the base interface.  It avoids  
    // got-yas where the implementation class has a method that shadows  
    // a method with the same name (but slightly different argument  
    // types) in the base interface, for example.  
    PrimeTable* const table_;  
};  
  
#if GTEST_HAS_TYPED_TEST  
  
using testing::Types;  
  
// Google Test offers two ways for reusing tests for different types.  
// The first is called "typed tests".  You should use it if you  
// already know *all* the types you are gonna exercise when you write  
// the tests.  
  
// To write a typed test case, first use  
//  
//   TYPED_TEST_CASE(TestCaseName, TypeList);  
//  
// to declare it and specify the type parameters.  As with TEST_F,  
// TestCaseName must match the test fixture name.  
  
// The list of types we want to test.  
typedef Types<OnTheFlyPrimeTable, PreCalculatedPrimeTable> Implementations;  
  
TYPED_TEST_CASE(PrimeTableTest, Implementations);  
  
// Then use TYPED_TEST(TestCaseName, TestName) to define a typed test,  
// similar to TEST_F.  
TYPED_TEST(PrimeTableTest, ReturnsFalseForNonPrimes) {  
    // Inside the test body, you can refer to the type parameter by  
    // TypeParam, and refer to the fixture class by TestFixture.  We  
    // don't need them in this example.  
  
    // Since we are in the template world, C++ requires explicitly  
    // writing 'this->' when referring to members of the fixture class.  
    // This is something you have to learn to live with.  
    EXPECT_FALSE(this->table_->IsPrime(-5));  
    EXPECT_FALSE(this->table_->IsPrime(0));  
    EXPECT_FALSE(this->table_->IsPrime(1));  
    EXPECT_FALSE(this->table_->IsPrime(4));  
    EXPECT_FALSE(this->table_->IsPrime(6));  
    EXPECT_FALSE(this->table_->IsPrime(100));  
}  
  
TYPED_TEST(PrimeTableTest, ReturnsTrueForPrimes) {  
    EXPECT_TRUE(this->table_->IsPrime(2));  
    EXPECT_TRUE(this->table_->IsPrime(3));  
    EXPECT_TRUE(this->table_->IsPrime(5));  
    EXPECT_TRUE(this->table_->IsPrime(7));  
    EXPECT_TRUE(this->table_->IsPrime(11));  
    EXPECT_TRUE(this->table_->IsPrime(131));  
}  
  
TYPED_TEST(PrimeTableTest, CanGetNextPrime) {  
    EXPECT_EQ(2, this->table_->GetNextPrime(0));  
    EXPECT_EQ(3, this->table_->GetNextPrime(2));  
    EXPECT_EQ(5, this->table_->GetNextPrime(3));  
    EXPECT_EQ(7, this->table_->GetNextPrime(5));  
    EXPECT_EQ(11, this->table_->GetNextPrime(7));  
    EXPECT_EQ(131, this->table_->GetNextPrime(128));  
}  
  
// That's it!  Google Test will repeat each TYPED_TEST for each type  
// in the type list specified in TYPED_TEST_CASE.  Sit back and be  
// happy that you don't have to define them multiple times.  
  
#endif  // GTEST_HAS_TYPED_TEST  
  
#if GTEST_HAS_TYPED_TEST_P  
  
using testing::Types;  
  
// Sometimes, however, you don't yet know all the types that you want  
// to test when you write the tests.  For example, if you are the  
// author of an interface and expect other people to implement it, you  
// might want to write a set of tests to make sure each implementation  
// conforms to some basic requirements, but you don't know what  
// implementations will be written in the future.  
//  
// How can you write the tests without committing to the type  
// parameters?  That's what "type-parameterized tests" can do for you.  
// It is a bit more involved than typed tests, but in return you get a  
// test pattern that can be reused in many contexts, which is a big  
// win.  Here's how you do it:  
  
// First, define a test fixture class template.  Here we just reuse  
// the PrimeTableTest fixture defined earlier:  
  
template <class T>  
class PrimeTableTest2 : public PrimeTableTest<T> {  
};  
  
// Then, declare the test case.  The argument is the name of the test  
// fixture, and also the name of the test case (as usual).  The _P  
// suffix is for "parameterized" or "pattern".  
TYPED_TEST_CASE_P(PrimeTableTest2);  
  
// Next, use TYPED_TEST_P(TestCaseName, TestName) to define a test,  
// similar to what you do with TEST_F.  
TYPED_TEST_P(PrimeTableTest2, ReturnsFalseForNonPrimes) {  
    EXPECT_FALSE(this->table_->IsPrime(-5));  
    EXPECT_FALSE(this->table_->IsPrime(0));  
    EXPECT_FALSE(this->table_->IsPrime(1));  
    EXPECT_FALSE(this->table_->IsPrime(4));  
    EXPECT_FALSE(this->table_->IsPrime(6));  
    EXPECT_FALSE(this->table_->IsPrime(100));  
}  
  
TYPED_TEST_P(PrimeTableTest2, ReturnsTrueForPrimes) {  
    EXPECT_TRUE(this->table_->IsPrime(2));  
    EXPECT_TRUE(this->table_->IsPrime(3));  
    EXPECT_TRUE(this->table_->IsPrime(5));  
    EXPECT_TRUE(this->table_->IsPrime(7));  
    EXPECT_TRUE(this->table_->IsPrime(11));  
    EXPECT_TRUE(this->table_->IsPrime(131));  
}  
  
TYPED_TEST_P(PrimeTableTest2, CanGetNextPrime) {  
    EXPECT_EQ(2, this->table_->GetNextPrime(0));  
    EXPECT_EQ(3, this->table_->GetNextPrime(2));  
    EXPECT_EQ(5, this->table_->GetNextPrime(3));  
    EXPECT_EQ(7, this->table_->GetNextPrime(5));  
    EXPECT_EQ(11, this->table_->GetNextPrime(7));  
    EXPECT_EQ(131, this->table_->GetNextPrime(128));  
}  
  
// Type-parameterized tests involve one extra step: you have to  
// enumerate the tests you defined:  
REGISTER_TYPED_TEST_CASE_P(  
    PrimeTableTest2,  // The first argument is the test case name.  
    // The rest of the arguments are the test names.  
    ReturnsFalseForNonPrimes, ReturnsTrueForPrimes, CanGetNextPrime);  
  
// At this point the test pattern is done.  However, you don't have  
// any real test yet as you haven't said which types you want to run  
// the tests with.  
  
// To turn the abstract test pattern into real tests, you instantiate  
// it with a list of types.  Usually the test pattern will be defined  
// in a .h file, and anyone can #include and instantiate it.  You can  
// even instantiate it more than once in the same program.  To tell  
// different instances apart, you give each of them a name, which will  
// become part of the test case name and can be used in test filters.  
  
// The list of types we want to test.  Note that it doesn't have to be  
// defined at the time we write the TYPED_TEST_P()s.  
typedef Types<OnTheFlyPrimeTable, PreCalculatedPrimeTable>  
    PrimeTableImplementations;  
INSTANTIATE_TYPED_TEST_CASE_P(OnTheFlyAndPreCalculated,    // Instance name  
    PrimeTableTest2,             // Test case name  
    PrimeTableImplementations);  // Type list  
  
#endif  // GTEST_HAS_TYPED_TEST_P  
  
/*-----------------------------TEST_P macro--------------------------------*/  
//Sample 7: This sample shows how to test common properties of multiple  
// implementations of an interface (aka interface tests) using  
// value-parameterized tests. Each test in the test case has  
// a parameter that is an interface pointer to an implementation  
// tested.  
  
#if GTEST_HAS_PARAM_TEST  
  
using ::testing::TestWithParam;  
using ::testing::Values;  
  
// As a general rule, to prevent a test from affecting the tests that come  
// after it, you should create and destroy the tested objects for each test  
// instead of reusing them.  In this sample we will define a simple factory  
// function for PrimeTable objects.  We will instantiate objects in test's  
// SetUp() method and delete them in TearDown() method.  
typedef PrimeTable* CreatePrimeTableFunc();  
  
PrimeTable* CreateOnTheFlyPrimeTable() {  
    return new OnTheFlyPrimeTable();  
}  
  
template <size_t max_precalculated>  
PrimeTable* CreatePreCalculatedPrimeTable() {  
    return new PreCalculatedPrimeTable(max_precalculated);  
}  
  
// Inside the test body, fixture constructor, SetUp(), and TearDown() you  
// can refer to the test parameter by GetParam().  In this case, the test  
// parameter is a factory function which we call in fixture's SetUp() to  
// create and store an instance of PrimeTable.  
class PrimeTableTest1 : public TestWithParam<CreatePrimeTableFunc*> {  
public:  
    virtual ~PrimeTableTest1() { delete table_; }  
    virtual void SetUp() { table_ = (*GetParam())(); }  
    virtual void TearDown() {  
        delete table_;  
        table_ = NULL;  
    }  
  
protected:  
    PrimeTable* table_;  
};  
  
TEST_P(PrimeTableTest1, ReturnsFalseForNonPrimes) {  
    EXPECT_FALSE(table_->IsPrime(-5));  
    EXPECT_FALSE(table_->IsPrime(0));  
    EXPECT_FALSE(table_->IsPrime(1));  
    EXPECT_FALSE(table_->IsPrime(4));  
    EXPECT_FALSE(table_->IsPrime(6));  
    EXPECT_FALSE(table_->IsPrime(100));  
}  
  
TEST_P(PrimeTableTest1, ReturnsTrueForPrimes) {  
    EXPECT_TRUE(table_->IsPrime(2));  
    EXPECT_TRUE(table_->IsPrime(3));  
    EXPECT_TRUE(table_->IsPrime(5));  
    EXPECT_TRUE(table_->IsPrime(7));  
    EXPECT_TRUE(table_->IsPrime(11));  
    EXPECT_TRUE(table_->IsPrime(131));  
}  
  
TEST_P(PrimeTableTest1, CanGetNextPrime) {  
    EXPECT_EQ(2, table_->GetNextPrime(0));  
    EXPECT_EQ(3, table_->GetNextPrime(2));  
    EXPECT_EQ(5, table_->GetNextPrime(3));  
    EXPECT_EQ(7, table_->GetNextPrime(5));  
    EXPECT_EQ(11, table_->GetNextPrime(7));  
    EXPECT_EQ(131, table_->GetNextPrime(128));  
}  
  
// In order to run value-parameterized tests, you need to instantiate them,  
// or bind them to a list of values which will be used as test parameters.  
// You can instantiate them in a different translation module, or even  
// instantiate them several times.  
//  
// Here, we instantiate our tests with a list of two PrimeTable object  
// factory functions:  
INSTANTIATE_TEST_CASE_P(  
    OnTheFlyAndPreCalculated,  
    PrimeTableTest1,  
    Values(&CreateOnTheFlyPrimeTable, &CreatePreCalculatedPrimeTable<1000>));  
  
#else  
  
// Google Test may not support value-parameterized tests with some  
// compilers. If we use conditional compilation to compile out all  
// code referring to the gtest_main library, MSVC linker will not link  
// that library at all and consequently complain about missing entry  
// point defined in that library (fatal error LNK1561: entry point  
// must be defined). This dummy test keeps gtest_main linked in.  
TEST(DummyTest, ValueParameterizedTestsAreNotSupportedOnThisPlatform) {}  
  
#endif  // GTEST_HAS_PARAM_TEST  
  
// Sample 8: This sample shows how to test code relying on some global flag variables.  
// Combine() helps with generating all possible combinations of such flags,  
// and each test is given one combination as a parameter.  
  
#if GTEST_HAS_COMBINE  
  
// Suppose we want to introduce a new, improved implementation of PrimeTable  
// which combines speed of PrecalcPrimeTable and versatility of  
// OnTheFlyPrimeTable (see prime_tables.h). Inside it instantiates both  
// PrecalcPrimeTable and OnTheFlyPrimeTable and uses the one that is more  
// appropriate under the circumstances. But in low memory conditions, it can be  
// told to instantiate without PrecalcPrimeTable instance at all and use only  
// OnTheFlyPrimeTable.  
class HybridPrimeTable : public PrimeTable {  
public:  
    HybridPrimeTable(bool force_on_the_fly, int max_precalculated)  
        : on_the_fly_impl_(new OnTheFlyPrimeTable),  
        precalc_impl_(force_on_the_fly ? NULL :  
        new PreCalculatedPrimeTable(max_precalculated)),  
        max_precalculated_(max_precalculated) {}  
    virtual ~HybridPrimeTable() {  
        delete on_the_fly_impl_;  
        delete precalc_impl_;  
    }  
  
    virtual bool IsPrime(int n) const {  
        if (precalc_impl_ != NULL && n < max_precalculated_)  
            return precalc_impl_->IsPrime(n);  
        else  
            return on_the_fly_impl_->IsPrime(n);  
    }  
  
    virtual int GetNextPrime(int p) const {  
        int next_prime = -1;  
        if (precalc_impl_ != NULL && p < max_precalculated_)  
            next_prime = precalc_impl_->GetNextPrime(p);  
  
        return next_prime != -1 ? next_prime : on_the_fly_impl_->GetNextPrime(p);  
    }  
  
private:  
    OnTheFlyPrimeTable* on_the_fly_impl_;  
    PreCalculatedPrimeTable* precalc_impl_;  
    int max_precalculated_;  
};  
  
using ::testing::TestWithParam;  
using ::testing::Bool;  
using ::testing::Values;  
using ::testing::Combine;  
  
// To test all code paths for HybridPrimeTable we must test it with numbers  
// both within and outside PreCalculatedPrimeTable's capacity and also with  
// PreCalculatedPrimeTable disabled. We do this by defining fixture which will  
// accept different combinations of parameters for instantiating a  
// HybridPrimeTable instance.  
class PrimeTableTest3 : public TestWithParam< ::std::tr1::tuple<bool, int> > {  
protected:  
    virtual void SetUp() {  
        // This can be written as  
        //  
        // bool force_on_the_fly;  
        // int max_precalculated;  
        // tie(force_on_the_fly, max_precalculated) = GetParam();  
        //  
        // once the Google C++ Style Guide allows use of ::std::tr1::tie.  
        //  
        bool force_on_the_fly = ::std::tr1::get<0>(GetParam());  
        int max_precalculated = ::std::tr1::get<1>(GetParam());  
        table_ = new HybridPrimeTable(force_on_the_fly, max_precalculated);  
    }  
    virtual void TearDown() {  
        delete table_;  
        table_ = NULL;  
    }  
    HybridPrimeTable* table_;  
};  
  
TEST_P(PrimeTableTest3, ReturnsFalseForNonPrimes) {  
    // Inside the test body, you can refer to the test parameter by GetParam().  
    // In this case, the test parameter is a PrimeTable interface pointer which  
    // we can use directly.  
    // Please note that you can also save it in the fixture's SetUp() method  
    // or constructor and use saved copy in the tests.  
  
    EXPECT_FALSE(table_->IsPrime(-5));  
    EXPECT_FALSE(table_->IsPrime(0));  
    EXPECT_FALSE(table_->IsPrime(1));  
    EXPECT_FALSE(table_->IsPrime(4));  
    EXPECT_FALSE(table_->IsPrime(6));  
    EXPECT_FALSE(table_->IsPrime(100));  
}  
  
TEST_P(PrimeTableTest3, ReturnsTrueForPrimes) {  
    EXPECT_TRUE(table_->IsPrime(2));  
    EXPECT_TRUE(table_->IsPrime(3));  
    EXPECT_TRUE(table_->IsPrime(5));  
    EXPECT_TRUE(table_->IsPrime(7));  
    EXPECT_TRUE(table_->IsPrime(11));  
    EXPECT_TRUE(table_->IsPrime(131));  
}  
  
TEST_P(PrimeTableTest3, CanGetNextPrime) {  
    EXPECT_EQ(2, table_->GetNextPrime(0));  
    EXPECT_EQ(3, table_->GetNextPrime(2));  
    EXPECT_EQ(5, table_->GetNextPrime(3));  
    EXPECT_EQ(7, table_->GetNextPrime(5));  
    EXPECT_EQ(11, table_->GetNextPrime(7));  
    EXPECT_EQ(131, table_->GetNextPrime(128));  
}  
  
// In order to run value-parameterized tests, you need to instantiate them,  
// or bind them to a list of values which will be used as test parameters.  
// You can instantiate them in a different translation module, or even  
// instantiate them several times.  
//  
// Here, we instantiate our tests with a list of parameters. We must combine  
// all variations of the boolean flag suppressing PrecalcPrimeTable and some  
// meaningful values for tests. We choose a small value (1), and a value that  
// will put some of the tested numbers beyond the capability of the  
// PrecalcPrimeTable instance and some inside it (10). Combine will produce all  
// possible combinations.  
INSTANTIATE_TEST_CASE_P(MeaningfulTestParameters,  
    PrimeTableTest3,  
    Combine(Bool(), Values(1, 10)));  
  
#else  
  
// Google Test may not support Combine() with some compilers. If we  
// use conditional compilation to compile out all code referring to  
// the gtest_main library, MSVC linker will not link that library at  
// all and consequently complain about missing entry point defined in  
// that library (fatal error LNK1561: entry point must be  
// defined). This dummy test keeps gtest_main linked in.  
TEST(DummyTest, CombineIsNotSupportedOnThisPlatform) {}  
  
#endif  // GTEST_HAS_COMBINE  

int main (int argc, char* argv[])  
{  
    testing::InitGoogleTest(&argc, argv);  
    return RUN_ALL_TESTS();  
  
    return 0;  
}  
  
