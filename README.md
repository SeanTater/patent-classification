patent-classification
=====================

Patent Classification: Data Preparation


Installing
----------

You will need:
-  pugixml
-  boost, at least:
  -  regex
  -  algorithm

sqlite3x is included so you don't need it.

It was developed on Linux with Clang 3.4, but should work with any compiler
that supports C++11.


Changelog
---------
12 July 2014
    Switched from a regex to a trie.
    - It took 9 hours, 50 minutes to complete 71556 successful parses (about 2.02 per second) with the regex.
    - It took 40 minutes, 49 seconds to complete 1002853 successful parses (about 409.49 per second) with the trie.
