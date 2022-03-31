:mod:`omv` --- OpenMV Cam Information
=====================================

.. module:: omv
   :synopsis: OpenMV Cam Information

The ``omv`` module is used to get OpenMV Cam information.

Functions
---------

.. function:: omv.version_major()

   Returns the major version number (int).

.. function:: omv.version_minor()

   Returns the minor version number (int).

.. function:: omv.version_patch()

   Returns the patch version number (int).

.. function:: omv.version_string()

   Returns the version string (e.g. "2.8.0").

.. function:: omv.arch()

   Returns the board architecture string. This string is really just meant for
   OpenMV IDE but you can get it with this function.

.. function:: omv.board_type()

   Returns the board type string. This string is really just meant for
   OpenMV IDE but you can get it with this function.

.. function:: omv.board_id()

   Returns the board id string. This string is really just meant for
   OpenMV IDE but you can get it with this function.

.. function:: omv.disable_fb([disable])

   When ``disable`` is set to ``True`` the OpenMV Cam will no longer jpeg compress images and stream
   them to OpenMV IDE. The IDE may still poll for images unless ``Disable FB`` is checked in OpenMV
   IDE. You may wish to disable the frame buffer when streaming images over to another system while
   debugging you script with OpenMV IDE. If no arguments are passed this function will return
   ``True`` if the frame buffer is disabled and ``False`` if not.

   .. note::

      This is a different flag than the ``Disable FB`` button in OpenMV IDE.
