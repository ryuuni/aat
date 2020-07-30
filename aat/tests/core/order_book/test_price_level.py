from aat.core.order_book.price_level import _PriceLevel
from aat.common import _in_cpp
from datetime import datetime

try:
    from aat.binding import (_PriceLevelCpp, _CollectorCpp, SideCpp,  # type: ignore
                             InstrumentCpp, ExchangeTypeCpp, OrderTypeCpp,
                             OrderFlagCpp, OrderCpp)
    _CPP = _in_cpp()
except ImportError:
    _CPP = False


class TestOrderBook:
    def test_price_level(self):
        # just instantiate, validate below
        pl = _PriceLevel(5.0, print)
        assert bool(pl) is False

    def test_price_level_cpp(self):
        if not _CPP:
            return True
        pl = _PriceLevelCpp(5, _CollectorCpp())
        orders = [
            OrderCpp(i, datetime(1970, 1, 1, 0, i), 10+i, 5, SideCpp.BUY,
                     InstrumentCpp('TEST'), ExchangeTypeCpp(""), 0.0,
                     OrderTypeCpp.LIMIT, OrderFlagCpp.NONE, None)
            for i in range(2)
            ]
        for o in orders:  # This causes a segfault
            pl.add(o)
        for o, op in zip(orders, pl):
            assert str(o) == str(op)  # Not ideal, but it works
