/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
Fusion Tuples Library

This is a re-implementation of the TR tuples with iterators, views and
algorithms. The structure is somewhat modeled after MPL. It is code-named
"fusion" because the library is the "fusion" of compile time
metaprogramming with runtime programming.

Overall structure:

    The library is composed of three sub-modules. Iterators, Sequences and
    Views and Algorithms.

Iterators:

    Generic iteration for heterogeneous types. The library is based on
    iterators.

    filter_view_iterator
        filtering iterator. Given an MPL predicate, skips elements to
        present only the items that passes the predicate. See filter_view
        below.

    joint_view_iterator
        Holds four iterators (two begin/end pairs). Iterates over the
        first pair then switches over to the next pair to present
        a contiguous whole. See joint_view below.

    single_view_iterator
        A single element iterator. See single_view below.
        
    cons_iterator
        An iterator into a cons list. See cons below.

    transform_view_iterator
        Given a transform-function, transforms the elements being
        iterated. See transform_view below.

Sequences and Views:

    Holds a begin/end iterator. Sequences and views may be composed to form
    more complex sequences and views. View/sequence composition is a very
    nice concept. These are extremely lighweight classes and can be passed
    around by value quite inexpensively. For instance, rather than working
    directly on tuples, the algorithms work on sequences and return
    sequences.

    tuple
        The basic tuple structure

    tupleN
        Fixed sized tuples (where N = 0 to a predefined limit)

    filter_view
        Given an MPL predicate, filters the view to present only the items
        that passes the predicate.

    single_view
        A single element view

    joint_view
        A two-sequence view (concatenates two sequences)
        
    cons
        A cons-cell that can be used to build a singly-linked list.

    range
        Holds an iterator pair that represents a begin/end range.

    transform_view
        Transforms a sequence given a transform-function

    MPL sequences are also, automatically, fusion sequences. All algorithms
    and fusion functions that work on fusion sequences can also take in
    MPL seqneces.

Basic functions and meta_functions on sequences:

    I/O             : TR1-tuples compatible I/O routines
    operator        : tuple operators ==, !=, <, >, <=, >=
    begin           : start of sequence
    end             : end of sequence
    make_tuple      : make a tuple
    tie             : make a tuple of references
    generate        : given a fusion sequence, generate a tuple
    get<N>          : get the nth element of a tuple
    is_sequence     : checks if a type is a fusion sequence
    tuple_element   : get the nth type in a tuple
    tuple_size      : get the number of elements in a tuple

Algorithms:

    With very complex composition of algorithms, it is not desirable to
    work directly on tuples. Like MPL, and unlike STL, the algorithms take
    in sequences/views and *return* the result by value; the algorithms are
    purely functional and do not (cannot) have side effects. We cannot have
    an out parameter that is passed by reference where the result is
    placed. The algorithms, instead, work on sequences and views and
    generate views.

    This strategy is very efficient. You can think of the algorithms as
    "lazy". The values are generated only wnen it is needed -for example
    when generating a tuple. When you are sure that you need a tuple, you
    can "generate" it from the sequence: generate(sequence);

        erase
        filter
        find
        find_if
        fold
        for_each
        insert
        push_back
        push_front
        remove
        remove_if
        replace
        transform
        any

TODO:
    iteration is limited to input_iterator
    fusion sequences to mpl mapping is not complete
    some relational operators are missing <, <=, >, >=
    some algorithms are missing (reverse, count, count_if, contains, append, etc.)
    some views are missing (reverse_view, set, map)
    get<N> should also work for all random access sequences, not only tuples
