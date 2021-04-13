# MULTIIT: Manipulate multiple loop indexes at once

The following types of multiiterators are supported:

 * `MultiIterator` is a simple multidimensional iterator, each with its own upper bound, for example: 

```
MultiIterator mi({ 4, 4, 4 });
```

 * `LimitedMultiIterator` is a multidimensional iterator, which is allowed to produce only combinations of indexes with sum not greater than the given limit value, for example:

```
LimitedMultiIterator mi({ 4, 4, 4 }, 5);
```

 * `ChoicesIterator` is an iterator, which indexes are `MultiIterator` or `LimitedMultiIterator` themselves, for example:

```
ChoicesIterator choices(76, {{ 31, 35, 10 }}, 1, 20);
```

Once created, all multiiterators can be iterated with a `next()` function, which shall return `false` when the combinations volume is exhausted.

## Building

```
mkdir build
cd build
cmake ..
make
```

## Testing

```
./multiit_test
MultiIterator mi({ 4, 4, 4 }) : 64 iterations visited
LimitedMultiIterator mi({ 4, 4, 4 }, 5) : 44 iterations visited
ChoicesIterator choices(76, {{ 31, 35, 10 }}, 1, 20) : 8571500 choices visited
```

