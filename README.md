JSMN2
====

[![CircleCI](https://dl.circleci.com/status-badge/img/circleci/WyZ9Bj3QC36K6x6gjqCkfi/BrpYUzNfmeAsbTBn5bq95T/tree/master.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/circleci/WyZ9Bj3QC36K6x6gjqCkfi/BrpYUzNfmeAsbTBn5bq95T/tree/master) [![codecov](https://codecov.io/gh/thenullvek/jsmn2/branch/master/graph/badge.svg)](https://codecov.io/gh/thenullvek/jsmn2)

jsmn2 (pronounced like 'jasmine') is a minimalistic JSON parser in C.  It can be
easily integrated into resource-limited or embedded projects.

You can find more information about JSON format at [json.org][1]

The original source code is available at https://github.com/zserge/jsmn

The web page with some information about jsmn can be found at
[http://zserge.com/jsmn.html][2]

Philosophy
----------

Most JSON parsers offer you a bunch of functions to load JSON data, parse it
and extract any value by its name. jsmn proves that checking the correctness of
every JSON packet or allocating temporary objects to store parsed JSON fields
often is an overkill. 

JSON format itself is extremely simple, so why should we complicate it?

jsmn is designed to be	**robust** (it should work fine even with erroneous
data), **fast** (it should parse data on the fly), **portable** (no superfluous
dependencies or non-standard C extensions). And of course, **simplicity** is a
key feature - simple code style, simple algorithm, simple integration into
other projects.

Features
--------

* no dependencies (even libc!)
* highly portable (tested on x86/amd64, ARM, AVR)
* extremely small code footprint
* API contains only 2 functions
* no dynamic memory allocation
* incremental single-pass parsing
* library code is covered with unit-tests

Design
------

The rudimentary jsmn object is a **token**. Let's consider a JSON string:

	'{ "name" : "Jack", "age" : 27 }'

It holds the following tokens:

* Object: `{ "name" : "Jack", "age" : 27}` (the whole object)
* Strings: `"name"`, `"Jack"`, `"age"` (keys and some values)
* Number: `27`

In jsmn, tokens do not hold any data, but point to token boundaries in JSON
string instead. In the example above jsmn will create tokens like: Object
[0..31], String [3..7], String [12..16], String [20..23], Number [27..29].

Every jsmn token has a type, which indicates the type of corresponding JSON
token. jsmn supports the following token types:

* Object - a container of key-value pairs, e.g.:
	`{ "foo":"bar", "x":0.3 }`
* Array - a sequence of values, e.g.:
	`[ 1, 2, 3 ]`
* String - a quoted sequence of chars, e.g.: `"foo"`
* Primitive - a number, a boolean (`true`, `false`) or `null`

Besides start/end positions, jsmn tokens for complex types (like arrays
or objects) also contain a number of child items, so you can easily follow
object hierarchy.

This approach provides enough information for parsing any JSON data and makes
it possible to use zero-copy techniques.


Other info
----------

This software is distributed under [MIT license](http://www.opensource.org/licenses/mit-license.php),
 so feel free to integrate it in your commercial products.

[1]: http://www.json.org/
