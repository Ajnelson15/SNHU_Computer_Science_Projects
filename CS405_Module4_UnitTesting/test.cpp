// Andrew Nelson
// CS405_Module4_Unit_Testing

// Uncomment the next line to use precompiled headers
#include "pch.h"
// uncomment the next line if you do not use precompiled headers
//#include "gtest/gtest.h"
//
// the global test environment setup and tear down
// you should not need to change anything here

class Environment : public ::testing::Environment
{
public:
    ~Environment() override {}

    // Override this to define how to set up the environment.
    void SetUp() override
    {
        //  initialize random seed
        srand(time(nullptr));
    }

    // Override this to define how to tear down the environment.
    void TearDown() override {}
};

// create our test class to house shared data between tests
// you should not need to change anything here

class CollectionTest : public ::testing::Test
{
protected:
    // create a smart point to hold our collection
    std::unique_ptr<std::vector<int>> collection;

    void SetUp() override
    { // create a new collection to be used in the test
        collection.reset(new std::vector<int>);
    }

    void TearDown() override
    { //  erase all elements in the collection, if any remain
        collection->clear();
        // free the pointer
        collection.reset(nullptr);
    }

    // helper function to add random values from 0 to 99 count times to the collection
    void add_entries(int count)
    {
        assert(count > 0);
        for (auto i = 0; i < count; ++i)
            collection->push_back(rand() % 100);
    }
};

// When should you use the EXPECT_xxx or ASSERT_xxx macros?
// Use ASSERT when failure should terminate processing, such as the reason for the test case.
// Use EXPECT when failure should notify, but processing should continue

// Test that a collection is empty when created.
// Prior to calling this (and all other TEST_F defined methods),
//  CollectionTest::StartUp is called.
// Following this method (and all other TEST_F defined methods),
//  CollectionTest::TearDown is called

TEST_F(CollectionTest, CollectionSmartPointerIsNotNull)
{
    // is the collection created
    ASSERT_TRUE(collection);

    // if empty, the size must be 0
    ASSERT_NE(collection.get(), nullptr);
}

// Test that a collection is empty when created.
TEST_F(CollectionTest, IsEmptyOnCreate)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // if empty, the size must be 0
    ASSERT_EQ(collection->size(), 0);
}

/* Comment this test out to prevent the test from running
 * Uncomment this test to see a failure in the test explorer */
 //TEST_F(CollectionTest, AlwaysFail)
 //{
 //    FAIL();
 //}

 // A test to verify adding a single value to an empty collection
TEST_F(CollectionTest, CanAddToEmptyVector)
{
    // Verifies collection is empty
    ASSERT_TRUE(collection->empty());

    // Debug print
    //std::cout << "INIT_SIZE = " << collection->size() << std::endl;

    // Adds 1 entry
    add_entries(1);

    // Verifies collection size is 1
    ASSERT_EQ(collection->size(), 1);

    // Debug print
    //std::cout << "SIZE = " << collection->size() << std::endl;
}

// A test to verify adding five values to collection
TEST_F(CollectionTest, CanAddFiveValuesToVector)
{
    // Verifies collection is empty
    ASSERT_TRUE(collection->empty());

    //Debug Print
    //std::cout << "INIT_SIZE = " << collection->size() << std::endl;

    add_entries(5);

    // Verifies collection size is 5
    ASSERT_EQ(collection->size(), 5);
    
    // Debug print
    //std::cout << "SIZE = " << collection->size() << std::endl;
}

// A test to verify that max size is greater than or equal to size for 0, 1, 5, 10 entries
TEST_F(CollectionTest, MaxSizeGreaterThanEqual) {
    
    ASSERT_TRUE(collection->empty());
    
    // Loop over a list of counts: 0, 1, 5, and 10
    for (int count : {0, 1, 5, 10}) {

        // Clear the collection before each test case
        collection->clear();

        // If the count is greater than 0, add that many entries to the collection
        if (count > 0) {
            add_entries(count);
        }

        // Confirms collection has 'count' number of elements
        ASSERT_EQ(collection->size(), count);

        // Confirms collection's max_size is never less than current size
        ASSERT_GE(collection->max_size(), collection->size());

        //Debug Pring
        //std::cout << " Max " << collection->max_size() << "\n Size " << collection->size() << std::endl;
    }
}

// A test to verify that capacity is greater than or equal to size for 0, 1, 5, 10 entries
TEST_F(CollectionTest, CapacityGreaterThanEqual) {

    ASSERT_TRUE(collection->empty());

    //Debug Print
    //std::cout << "Capacity " << collection->capacity() << " \nSize " << collection->size() << std::endl;

    // Loop over a list of counts: 0, 1, 5, and 10
    for (int count : {0, 1, 5, 10}) {

        // Clear the collection before each test case
        collection->clear();

        // If the count is greater than 0, add that many entries to the collection
        if (count > 0) {
            add_entries(count);
        }
    
        // Capacity must always be >= size
        ASSERT_GE(collection->capacity(), collection->size());

        //Debug Print
        //std::cout << "Capacity " << collection->capacity() << " \nSize " << collection->size() << std::endl;
    }
}

// A test to verify resizing increases the collection
TEST_F(CollectionTest, ResizeIncreaseCollection) {
    ASSERT_TRUE(collection->empty());

    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;

    // Resize collection to 5 elements
    collection->resize(5);

    // Check that new size is 5
    ASSERT_EQ(collection->size(), 5);

    //Debug Print
    //std::cout << "SIZE " << collection->size() << std::endl;
}

// A test to verify resizing decreases the collection
TEST_F(CollectionTest, ResizeDecreaseCollection) {

    add_entries(25);

    ASSERT_EQ(collection->size(), 25);
    
    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;

    // Resize to 10
    collection->resize(10);

    ASSERT_EQ(collection->size(), 10);

    //Debug Print
    //std::cout << "SIZE " << collection->size() << std::endl;
}

// A test to verify resizing decreases the collection to zero
TEST_F(CollectionTest, ResizeToZeroEmptiesCollection) {

    add_entries(15);

    ASSERT_EQ(collection->size(), 15);

    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;

    // Use clear() to remove all elements
    collection->clear();

    ASSERT_TRUE(collection->empty());

    //Debug Print
    //std::cout << "SIZE " << collection->size() << std::endl;
}

// Create a test to verify clear erases the collection
TEST_F(CollectionTest, ClearErasesCollection)
{
    add_entries(20);

    ASSERT_EQ(collection->size(), 20);

    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;

    // Clear the collection
    collection->clear();

    ASSERT_TRUE(collection->empty());

    //Debug Print
    //std::cout << "SIZE " << collection->size() << std::endl;
}

// Create a test to verify erase(begin,end) erases the collection
TEST_F(CollectionTest, EraseBeginEndClearsCollection)
{
    add_entries(25);

    ASSERT_EQ(collection->size(), 25);

    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;

    // Use erase to clear all elements from the beginning to the end
    collection->erase(collection->begin(), collection->end());

    ASSERT_TRUE(collection->empty());

    //Debug Print
    //std::cout << "SIZE " << collection->size() << std::endl;
}

// Create a test to verify reserve increases the capacity but not the size of the collection
TEST_F(CollectionTest, ReserveIncreasesCapacity)
{
    auto oldCapacity = collection->capacity();

    ASSERT_EQ(oldCapacity, 0);

    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;
    //std::cout << "INIT_CAPACITY " << collection->capacity() << std::endl;

    // Add to the capacity
    collection->reserve(oldCapacity + 25);

    // Capacity must increase, but size remains the same
    ASSERT_GT(collection->capacity(), oldCapacity);
    ASSERT_EQ(collection->size(), 0);

    //Debug Print
    //std::cout << "SIZE " << collection->size() << std::endl;
    //std::cout << "CAPACITY " << collection->capacity() << std::endl;
}

// A test to verify the std::out_of_range exception is thrown when calling at() with an index out of bounds
TEST_F(CollectionTest, AtThrowsWhenOutOfBounds)
{
    add_entries(5);

    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;

    // Learned that using a lambda lets EXPECT_THROW catch the exception at the right time
    // Accessing out-of-bounds index should throw exception
    EXPECT_THROW({ collection->at(10);}, std::out_of_range);

    //Debug Print
    //std::cout << "LAST VALID INDEX " << collection->at(collection->size() - 1) << std::endl;
}

// A positive unit tests of my own
TEST_F(CollectionTest, FrontEqualsFirstElement) {

    add_entries(30);

    // front() is equal to the same value as element at index 0
    ASSERT_EQ(collection->front(), collection->at(0));

    //Debug Print
    //std::cout << "Front = " << collection->front() << "\nCollection at index 0 = " << collection->at(0) << std::endl;
}

// A negative unit tests of my own
TEST_F(CollectionTest, ThrowsWhenResizedToZero) {

    add_entries(35);

    //Debug Print
    //std::cout << "INIT_SIZE " << collection->size() << std::endl;

    // Resize to zero removes all elements
    collection->resize(0);

    // Learned that using a lambda lets EXPECT_THROW catch the exception at the right time
    // Accessing index 0 should throw exception
    EXPECT_THROW({ collection->at(0); }, std::out_of_range);

    //Debug Print
    //std::cout << "SIZE " << collection->size() << std::endl;
}
