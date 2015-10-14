modernize-use-using
=====================

The check converts all the usages of ``typedef`` keyword to use the new C++11
``using`` type alias.

Example
-------

.. code-block:: c++

  typedef int my_int;


transforms to:

.. code-block:: c++

  using my_int = int;

Limitations
-----------

For the moment, multiple type definitions with a single ``typedef`` will not be
fixed automatically, only a diagnostic will be emitted.
