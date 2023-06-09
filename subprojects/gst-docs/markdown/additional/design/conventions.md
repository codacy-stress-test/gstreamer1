# Documentation conventions

Due to the potential for exponential growth, several abbreviating
conventions will be used throughout this documentation. These
conventions have grown primarily from extremely in-depth discussions of
the architecture in IRC. This has verified the safety of these
conventions, if used properly. There are no known namespace conflicts as
long as context is rigorously observed.

## Object classes

Since everything starts with Gst, we will generally refer to objects by
the shorter name, i.e. `Element` or `Pad`. These names will always have
their first letter capitalized.

## Function names

Within the context of a given object, functions defined in that object’s
header and/or source file will have their object-specific prefix
stripped. For instance, `gst_element_add_pad()` would be referred to as
simply `*add_pad()`. Note that the trailing parentheses should always be
present, but sometimes may not be. A prefixed asterisk (*) will
always tell you it’s a function, however, regardless of the presence or
absence of the trailing parentheses.

## defines and enums

Values and macros defined as enums and preprocessor macros will be
referred to in all capitals, as per their definition. This includes
object flags and element states, as well as general enums. Examples are
the states `NULL`, `READY`, `PLAYING`, and `PAUSED`; the element flags
`LOCKED_STATE`, and state return values `SUCCESS`, `FAILURE`, and `ASYNC`.
Where there is a prefix, as in the element flags, it is usually dropped
and implied. Note however that element flags should be cross-checked
with the header, as there are currently two conventions in use: with and
without `*FLAGS*` in the middle.

## Drawing conventions

When drawing pictures the following conventions apply:

### objects

Objects are drawn with a box like:

```
+------+
|      |
+------+
```

### pointers

a pointer to an object.

```
     +-----+
*--->|     |
     +-----+
```

an invalid pointer, this is a pointer that should not be used.

```
*-//->
```

### elements

```
 +----------+
 | name     |
sink       src
 +----------+
```

### pad links

```
-----+    +---
     |    |
    src--sink
-----+    +---
```
