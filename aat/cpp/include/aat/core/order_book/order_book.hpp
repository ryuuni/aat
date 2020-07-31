#pragma once
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <aat/common.hpp>
#include <aat/core/order_book/price_level.hpp>
#include <aat/core/order_book/collector.hpp>
#include <aat/core/exchange/exchange.hpp>
#include <aat/core/models/event.hpp>
#include <aat/core/models/order.hpp>

using namespace aat::common;

namespace aat {
namespace core {
  class OrderBook;  // fwd declare

  template<bool is_const>
  class OrderBookIterator {
   public:
    explicit OrderBookIterator(
      const OrderBook& book, std::size_t price_index = 0,
      std::size_t level_index = 0, Side side = Side::SELL, bool pass = false)
      : order_book(book)
      , price_index(price_index)
      , level_index(level_index)
      , side(side)
      , second_pass(pass) { boundsUpdate(true); }

    OrderBookIterator(const OrderBookIterator&) = default;

    template<bool was_const, class = std::enable_if_t<is_const && !was_const>>
    OrderBookIterator(const OrderBookIterator<was_const>& rhs)
      : order_book(rhs.order_book)
      , price_index(rhs.price_index)
      , level(rhs.level)
      , level_index(rhs.level_index)
      , side(rhs.side)
      , second_pass(rhs.second_pass) {}

    OrderBookIterator& operator++();
    std::shared_ptr<Order> operator*();
    bool operator==(const OrderBookIterator& that);

    using iterator_category = std::forward_iterator_tag;
    using value_type = std::shared_ptr<Order>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

   private:
    void boundsUpdate(bool init);
    const OrderBook& order_book;
    std::size_t price_index;
    std::size_t level_index;
    Side side;
    std::shared_ptr<PriceLevel> level;
    bool second_pass = false;
  };

  class OrderBook {
   public:
    explicit OrderBook(const Instrument& instrument);
    OrderBook(const Instrument& instrument, const ExchangeType& exchange);
    OrderBook(
      const Instrument& instrument, const ExchangeType& exchange, std::function<void(std::shared_ptr<Event>)> callback);

    void setCallback(std::function<void(std::shared_ptr<Event>)> callback);

    void add(std::shared_ptr<Order> order);
    void cancel(std::shared_ptr<Order> order);
    void change(std::shared_ptr<Order> order);
    std::shared_ptr<Order> find(std::shared_ptr<Order> order);

    std::map<Side, std::vector<double>> topOfBookMap() const;  // For Binding
    std::vector<double> topOfBook() const;

    double spread() const;

    std::vector<double> level(uint_t level) const;
    std::vector<std::shared_ptr<PriceLevel>> level(double price) const;

    std::map<Side, std::vector<std::vector<double>>> levelsMap(uint_t levels) const;  // For Binding
    std::vector<std::vector<double>> levels(uint_t levels) const;

    str_t toString() const;

    // iterator
    template<bool B>
    friend class OrderBookIterator;
    using iterator = OrderBookIterator<false>;
    using const_iterator = OrderBookIterator<true>;
    iterator begin() noexcept;
    iterator end() noexcept;

    const_iterator
    begin() const noexcept {
        return cbegin();
    }
    const_iterator
    end() const noexcept {
        return cend();
    }
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

   private:
    void clearOrders(std::shared_ptr<Order> order, uint_t amount);
    double getTop(Side side, uint_t cleared);
    bool insort(std::vector<double>& levels, double value);  // NOLINT

    Collector collector;
    const Instrument& instrument;
    const ExchangeType& exchange;
    std::function<void(std::shared_ptr<Event>)> callback;

    std::vector<double> buy_levels;
    std::vector<double> sell_levels;

    std::unordered_map<double, std::shared_ptr<PriceLevel>> buys;
    std::unordered_map<double, std::shared_ptr<PriceLevel>> sells;
  };
}  // namespace core
}  // namespace aat
