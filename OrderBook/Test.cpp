#include <gtest/gtest.h>

#include "List.h"

using namespace OrderBook;

// Test case to check if the book is initially empty.
TEST(OrderBookBidsTest, BookIsInitiallyEmpty) {
	List<Side::BID, double> bids{};

	EXPECT_TRUE(bids.empty());
	EXPECT_EQ(bids.topOfBook(), std::nullopt);
}

// Test case to add a single level and check the top of book.
TEST(OrderBookBidsTest, AddSingleLevel) {
	List<Side::BID, double> bids{};

	EXPECT_TRUE(bids.levelUpdate(100.0, 10.0)); // Add a bid of 100.0 at 10.0 quantity
	EXPECT_FALSE(bids.empty());
	auto top = bids.topOfBook();
	ASSERT_TRUE(top.has_value());
	EXPECT_EQ(top->price, 100.0);
	EXPECT_EQ(top->quantity, 10.0);
}

// Test case to update a level's quantity.
TEST(OrderBookBidsTest, UpdateLevelQuantity) {
	List<Side::BID, double> bids{};

	bids.levelUpdate(100.0, 10.0);				// Add a level
	EXPECT_TRUE(bids.levelUpdate(100.0, 20.0)); // Update the same level
	auto top = bids.topOfBook();
	ASSERT_TRUE(top.has_value());
	EXPECT_EQ(top->price, 100.0);
	EXPECT_EQ(top->quantity, 20.0); // Ensure quantity is updated
}

// Test case to delete a level by setting quantity to 0.
TEST(OrderBookBidsTest, DeleteLevel) {
	List<Side::BID, double> bids{};

	EXPECT_TRUE(bids.levelUpdate(100.0, 10.0));
	EXPECT_TRUE(bids.levelUpdate(100.0, 0.0)); // Delete the level
	EXPECT_TRUE(bids.empty());
	EXPECT_EQ(bids.topOfBook(), std::nullopt); // Ensure the book is empty
}

// Test case to delete a non-existent level.
TEST(OrderBookBidsTest, DeleteNonExistentLevel) {
	List<Side::BID, double> bids{};

	EXPECT_FALSE(bids.levelUpdate(200.0, 0.0)); // Try deleting a non-existent level
}

// Test case to add multiple levels and ensure descending order.
TEST(OrderBookBidsTest, MultipleLevelsDescendingOrder) {
	List<Side::BID, double> bids{};

	bids.levelUpdate(100.0, 10.0); // Level at 100
	bids.levelUpdate(105.0, 5.0);  // Level at 105
	bids.levelUpdate(95.0, 20.0);  // Level at 95

	auto it = bids.begin();
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, 105.0); // First level should be the highest price

	++it;
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, 100.0); // Next level is at 100

	++it;
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, 95.0); // Lowest level is at 95

	++it;
	EXPECT_EQ(it, bids.end()); // End of list
}

// Test case for negative prices.
TEST(OrderBookBidsTest, HandleNegativePrices) {
	List<Side::BID, double> bids{};

	bids.levelUpdate(-50.0, 5.0);  // Add a negative price level
	bids.levelUpdate(-10.0, 10.0); // Another negative price level

	auto it = bids.begin();
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, -10.0); // Higher (less negative) should come first

	++it;
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, -50.0); // Lower (more negative) comes second
}

// Test case to update levels with descending order preservation.
TEST(OrderBookBidsTest, LevelOrderAfterUpdate) {
	List<Side::BID, double> bids{};

	bids.levelUpdate(100.0, 10.0); // Initial level
	bids.levelUpdate(105.0, 5.0);  // Higher level
	bids.levelUpdate(95.0, 20.0);  // Lower level

	EXPECT_TRUE(bids.levelUpdate(105.0, 0.0)); // Remove the top level at 105

	auto it = bids.begin();
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, 100.0); // Now 100 should be at the top
	EXPECT_EQ(it->quantity, 10.0);

	++it;
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, 95.0); // Then 95
	EXPECT_EQ(it->quantity, 20.0);
}

// Test case to ensure duplicate prices are correctly handled.
TEST(OrderBookBidsTest, DuplicatePriceUpdates) {
	List<Side::BID, double> bids{};

	EXPECT_TRUE(bids.levelUpdate(100.0, 10.0)); // Add level 100
	EXPECT_TRUE(bids.levelUpdate(100.0, 5.0));	// Update same price

	auto top = bids.topOfBook();
	ASSERT_TRUE(top.has_value());
	EXPECT_EQ(top->price, 100.0);
	EXPECT_EQ(top->quantity, 5.0); // Quantity should be updated
}

// Test case to handle multiple updates at different prices.
TEST(OrderBookBidsTest, MultiplePriceUpdates) {
	List<Side::BID, double> bids{};

	bids.levelUpdate(90.0, 15.0); // Add lower level
	bids.levelUpdate(95.0, 10.0); // Add middle level
	bids.levelUpdate(105.0, 5.0); // Add higher level

	EXPECT_TRUE(bids.levelUpdate(95.0, 0.0)); // Remove middle level
	auto it = bids.begin();
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, 105.0); // Top level should be 105
	++it;
	ASSERT_NE(it, bids.end());
	EXPECT_EQ(it->price, 90.0); // Remaining lower level at 90
}

// Test case to verify levels are correctly iterated over.
TEST(OrderBookBidsTest, IterateOverLevels) {
	List<Side::BID, double> bids{};

	bids.levelUpdate(100.0, 10.0);
	bids.levelUpdate(105.0, 5.0);
	bids.levelUpdate(95.0, 20.0);

	std::vector<double> expectedPrices = {105.0, 100.0, 95.0};
	std::vector<double> expectedQuantities = {5.0, 10.0, 20.0};

	int index = 0;
	for (const auto& level : bids) {
		EXPECT_EQ(level.price, expectedPrices[index]);
		EXPECT_EQ(level.quantity, expectedQuantities[index]);
		++index;
	}
}

// Test case to check if the book is initially empty.
TEST(OrderBookAsksTest, BookIsInitiallyEmpty) {
	List<Side::ASK, double> asks{};

	EXPECT_TRUE(asks.empty());
	EXPECT_EQ(asks.topOfBook(), std::nullopt);
}

// Test case to add a single level and check the top of book.
TEST(OrderBookAsksTest, AddSingleLevel) {
	List<Side::ASK, double> asks{};

	EXPECT_TRUE(asks.levelUpdate(100.0, 10.0)); // Add an ask of 100.0 at 10.0 quantity
	EXPECT_FALSE(asks.empty());
	auto top = asks.topOfBook();
	ASSERT_TRUE(top.has_value());
	EXPECT_EQ(top->price, 100.0);
	EXPECT_EQ(top->quantity, 10.0);
}

// Test case to update a level's quantity.
TEST(OrderBookAsksTest, UpdateLevelQuantity) {
	List<Side::ASK, double> asks{};

	asks.levelUpdate(100.0, 10.0);				// Add a level
	EXPECT_TRUE(asks.levelUpdate(100.0, 20.0)); // Update the same level
	auto top = asks.topOfBook();
	ASSERT_TRUE(top.has_value());
	EXPECT_EQ(top->price, 100.0);
	EXPECT_EQ(top->quantity, 20.0); // Ensure quantity is updated
}

// Test case to delete a level by setting quantity to 0.
TEST(OrderBookAsksTest, DeleteLevel) {
	List<Side::ASK, double> asks{};

	asks.levelUpdate(100.0, 10.0);
	EXPECT_TRUE(asks.levelUpdate(100.0, 0.0)); // Delete the level
	EXPECT_TRUE(asks.empty());
	EXPECT_EQ(asks.topOfBook(), std::nullopt); // Ensure the book is empty
}

// Test case to delete a non-existent level.
TEST(OrderBookAsksTest, DeleteNonExistentLevel) {
	List<Side::ASK, double> asks{};

	EXPECT_FALSE(asks.levelUpdate(200.0, 0.0)); // Try deleting a non-existent level
}

// Test case to add multiple levels and ensure ascending order.
TEST(OrderBookAsksTest, MultipleLevelsAscendingOrder) {
	List<Side::ASK, double> asks{};

	asks.levelUpdate(100.0, 10.0); // Level at 100
	asks.levelUpdate(105.0, 5.0);  // Level at 105
	asks.levelUpdate(95.0, 20.0);  // Level at 95

	auto it = asks.begin();
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, 95.0); // First level should be the lowest price

	++it;
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, 100.0); // Next level is at 100

	++it;
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, 105.0); // Highest level is at 105

	++it;
	EXPECT_EQ(it, asks.end()); // End of list
}

// Test case for negative prices.
TEST(OrderBookAsksTest, HandleNegativePrices) {
	List<Side::ASK, double> asks{};

	asks.levelUpdate(-50.0, 5.0);  // Add a negative price level
	asks.levelUpdate(-10.0, 10.0); // Another negative price level

	auto it = asks.begin();
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, -50.0); // Lower (more negative) should come first

	++it;
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, -10.0); // Higher (less negative) comes second
}

// Test case to update levels with ascending order preservation.
TEST(OrderBookAsksTest, LevelOrderAfterUpdate) {
	List<Side::ASK, double> asks{};

	asks.levelUpdate(100.0, 10.0); // Initial level
	asks.levelUpdate(95.0, 5.0);   // Lower level
	asks.levelUpdate(105.0, 20.0); // Higher level

	EXPECT_TRUE(asks.levelUpdate(95.0, 0.0)); // Remove the lowest level at 95

	auto it = asks.begin();
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, 100.0); // Now 100 should be at the top
	EXPECT_EQ(it->quantity, 10.0);

	++it;
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, 105.0); // Highest level is at 105
	EXPECT_EQ(it->quantity, 20.0);
}

// Test case to ensure duplicate prices are correctly handled.
TEST(OrderBookAsksTest, DuplicatePriceUpdates) {
	List<Side::ASK, double> asks{};

	EXPECT_TRUE(asks.levelUpdate(100.0, 10.0)); // Add level 100
	EXPECT_TRUE(asks.levelUpdate(100.0, 5.0));	// Update same price

	auto top = asks.topOfBook();
	ASSERT_TRUE(top.has_value());
	EXPECT_EQ(top->price, 100.0);
	EXPECT_EQ(top->quantity, 5.0); // Quantity should be updated
}

// Test case to handle multiple updates at different prices.
TEST(OrderBookAsksTest, MultiplePriceUpdates) {
	List<Side::ASK, double> asks{};

	asks.levelUpdate(90.0, 15.0); // Add lower level
	asks.levelUpdate(95.0, 10.0); // Add middle level
	asks.levelUpdate(105.0, 5.0); // Add higher level

	EXPECT_TRUE(asks.levelUpdate(95.0, 0.0)); // Remove middle level
	auto it = asks.begin();
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, 90.0); // Top level should be 90
	++it;
	ASSERT_NE(it, asks.end());
	EXPECT_EQ(it->price, 105.0); // Remaining higher level at 105
}

// Test case to verify levels are correctly iterated over.
TEST(OrderBookAsksTest, IterateOverLevels) {
	List<Side::ASK, double> asks{};

	asks.levelUpdate(100.0, 10.0);
	asks.levelUpdate(95.0, 5.0);
	asks.levelUpdate(105.0, 20.0);

	std::vector<double> expectedPrices = {95.0, 100.0, 105.0};
	std::vector<double> expectedQuantities = {5.0, 10.0, 20.0};

	int index = 0;
	for (const auto& level : asks) {
		EXPECT_EQ(level.price, expectedPrices[index]);
		EXPECT_EQ(level.quantity, expectedQuantities[index]);
		++index;
	}
}
