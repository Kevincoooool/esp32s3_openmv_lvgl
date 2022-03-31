.. _micropython_lib:

MicroPython libraries
=====================

.. warning::

   Important summary of this section

   * MicroPython provides built-in modules that mirror the functionality of the
     Python standard library (e.g. :mod:`os`, :mod:`time`), as well as
     MicroPython-specific modules (e.g. :mod:`bluetooth`, :mod:`machine`).
   * Most standard library modules implement a subset of the functionality of
     the equivalent Python module, and in a few cases provide some
     MicroPython-specific extensions (e.g. :mod:`array`, :mod:`os`)
   * Due to resource constraints or other limitations, some ports or firmware
     versions may not include all the functionality documented here.
   * To allow for extensibility, the built-in modules can be extended from
     Python code loaded onto the device.

This chapter describes modules (function and class libraries) which are built
into MicroPython. This documentation in general aspires to describe all modules
and functions/classes which are implemented in the MicroPython project.
However, MicroPython is highly configurable, and each port to a particular
board/embedded system may include only a subset of the available MicroPython
libraries.

With that in mind, please be warned that some functions/classes in a module (or
even the entire module) described in this documentation **may be unavailable**
in a particular build of MicroPython on a particular system. The best place to
find general information of the availability/non-availability of a particular
feature is the "General Information" section which contains information
pertaining to a specific :term:`MicroPython port`.

On some ports you are able to discover the available, built-in libraries that
can be imported by entering the following at the :term:`REPL`::

    help('modules')

Beyond the built-in libraries described in this documentation, many more
modules from the Python standard library, as well as further MicroPython
extensions to it, can be found in :term:`micropython-lib`.

Python standard libraries and micro-libraries
---------------------------------------------

The following standard Python libraries have been "micro-ified" to fit in with
the philosophy of MicroPython.  They provide the core functionality of that
module and are intended to be a drop-in replacement for the standard Python
library.

.. toctree::
   :maxdepth: 3

   array.rst
   binascii.rst
   builtins.rst
   cmath.rst
   collections.rst
   errno.rst
   gc.rst
   hashlib.rst
   heapq.rst
   io.rst
   json.rst
   math.rst
   os.rst
   random.rst
   re.rst
   select.rst
   socket.rst
   ssl.rst
   struct.rst
   sys.rst
   time.rst
   uasyncio.rst
   zlib.rst
   _thread.rst


MicroPython-specific libraries
------------------------------

Functionality specific to the MicroPython implementation is available in
the following libraries.

.. toctree::
   :maxdepth: 3

   bluetooth.rst
   btree.rst
   cryptolib.rst
   framebuf.rst
   machine.rst
   micropython.rst
   neopixel.rst
   network.rst
   uctypes.rst
   mutex.rst

Libraries specific to the OpenMV Cam
------------------------------------

The following libraries are specific to the OpenMV Cam.

.. toctree::
   :maxdepth: 3

   pyb.rst
   stm.rst
   omv.sensor.rst
   omv.image.rst
   omv.tf.rst
   omv.gif.rst
   omv.mjpeg.rst
   omv.audio.rst
   omv.micro_speech.rst
   omv.lcd.rst
   omv.fir.rst
   omv.tv.rst
   omv.cpufreq.rst
   omv.buzzer.rst
   omv.imu.rst
   omv.rpc.rst
   omv.rtsp.rst
   omv.omv.rst

Third-party libraries on the OpenMV Cam
---------------------------------------

The following third-party libraries are built-in to your OpenMV Cam's firmware:

:mod:`ulab` --- `numpy-like array manipulation library <https://micropython-ulab.readthedocs.io/en/latest/>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: ulab
   :synopsis: numpy-like array manipulation library

:mod:`pid` --- `Proportional/Integral/Derivative Control <https://github.com/openmv/openmv/blob/master/scripts/libraries/pid.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: pid
   :synopsis: Proportional/Integral/Derivative Control

:mod:`bno055` --- `IMU Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/bno055.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: bno055
   :synopsis: IMU Driver

Examples scripts are located in OpenMV IDE under the ``IMU Shield`` examples folder.

:mod:`ssd1306` --- `OLED Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/ssd1306.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: ssd1306
   :synopsis: OLED Driver

:mod:`tb6612` --- `Stepper Motor Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/tb6612.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: tb6612
   :synopsis: Stepper Motor Driver

Examples scripts are located in OpenMV IDE under the ``Motor Shield`` examples folder.

:mod:`vl53l1x` --- `ToF Distance Sensor Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/vl53l1x.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: vl53l1x
   :synopsis: ToF Distance Sensor Driver

Examples scripts are located in OpenMV IDE under the ``Distance Shield`` examples folder.

:mod:`modbus` --- `modbus protocol library <https://github.com/openmv/openmv/blob/master/scripts/libraries/modbus.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: modbus
   :synopsis: modbus protocol library

Examples scripts are located in OpenMV IDE under the ``Modbus`` examples folder.

:mod:`mqtt` --- `mqtt protocol library <https://github.com/openmv/openmv/blob/master/scripts/libraries/mqtt.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: mqtt
   :synopsis: mqtt protocol library

Examples scripts are located in OpenMV IDE under the ``WiFi Shield`` examples folder.

