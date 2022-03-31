:mod:`rpc` --- rpc library
==========================

.. module:: rpc
   :synopsis: rpc library

The ``rpc`` module on the OpenMV Cam allows you to connect your OpenMV Cam to another microcontroller
or computer and execute remote python (or procedure) calls on your OpenMV Cam. The ``rpc`` module also
allows for the reverse too if you want your OpenMV Cam to be able to execute remote procedure
(or python) calls on another microcontroller or computer.

How to use the Library
----------------------

Please checkout the example scripts in OpenMV IDE under ``Remote Control``.

You will need to edit the example code to choose which interface you want to use and to play with
the settings the scripts use.

In general, for the controller device to use the ``rpc`` library you will create an interface object
using the ``rpc`` library. For example::

    interface = rpc.rpc_uart_master(baudrate=115200)

This create a UART interface to talk to an ``rpc`` slave.

Once the interface is created you just need to do::

    memory_view_object_result = interface.call("remote_function_or_method_name", bytes_object_argument)

And the ``rpc`` library will try to execute that ``"remote_function_or_method_name"`` on the slave. The
remote function or method will receive the ``bytes_object_argument`` which can be up to 2^32-1 bytes in
size. Once the remote method finishes executing it will return a ``memory_view_object_result`` which
can also be up to 2^32-1 bytes in size. Because the argument and response are both generic byte
containers you can pass anything through the ``rpc`` library and receive any type of response. A simple
way to pass arguments is to use ``struct.pack()`` to create the argument and ``struct.unpack()`` to
receieve the argument on the other side. For the response, the other side may send a string
object or json string as the result which the master can then interpret.

As for errors, if you try to execute a non-existant function or method name the
``rpc_master.call()`` method will return an empty ``bytes()`` object. If the ``rpc`` library failed to communicate with the
slave the ``rpc`` library will return None.

To keep things simple the ``rpc`` library doesn't maintain a connection between the master and slave
devices. The ``rpc_master.call()`` method encapsulates trying to connect to the slave, starting execution of
the remote function or method, and getting the result.

Now, on the slave side of things you have to create an ``rpc`` interface to communicate with the
master. This looks like::

    interface = rpc.rpc_uart_slave(baudrate=115200)

This will create the UART interface layer to talk to an ``rpc`` master.

Once you create the slave interface you then need to register call backs that the master can call
with the interface object::

    def remote_function_or_method_name(memoryview_object_argument):
        <lots of code>
        return bytes_object_result

    interface.register_callback(remote_function_or_method_name)

You may register as many callbacks as you like on the slave.
Finally, once you are done registering callbacks you just need to execute::

    interface.loop()

On the slave to start the ``rpc`` library up and begin listening for the master. Note that the
``rpc_slave.loop()`` method does not return. Also, to make your slave more robust against errors you may
want to wrap the ``rpc_slave.loop()`` with ``try:`` and ``except:`` for whatever exceptions might be thrown by your
callback methods. The ``rpc`` library will not generate any exceptions itself. Note: passing large data
structures around (like jpeg images) can potentially exhaust the heap on the OpenMV Cam and generate
`MemoryError` exceptions.

And that is it! The ``rpc`` library is designed to be simple to use.

class rpc - rpc virtual class
-----------------------------

The rpc base class is reimplemented by the `rpc_master` and `rpc_slave` classes to create the master
and slave interfaces. It is a pure virtual class and not meant to be used directly.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc()

   Creates an rpc object. This constructor is not meant to be used directly.

Methods
~~~~~~~

.. method:: rpc.get_bytes(buff, timeout_ms):

   This method is meant to be reimplemented by specific interface classes of `rpc_master` and `rpc_slave`.
   It should fill the ``buff`` argument which is either a `bytearray` or `memoryview` object of bytes from the
   interface equal to the length of the ``buff`` object in ``timeout_ms`` milliseconds. On timeout this method
   should return ``None``. Note that for master and slave synchronization this method should try to always
   complete in at least ``timeout_ms`` milliseconds and not faster as the `rpc_master` and `rpc_slave` objects
   will automatically increase the ``timeout_ms`` to synchronize.

.. method:: rpc.put_bytes(data, timeout_ms):

   This method is meant to be reimplemented by specific interface classes of `rpc_master` and `rpc_slave`.
   It should send ``data`` bytes on the interface within ``timeout_ms`` milliseconds. If it completes faster
   than the timeout that is okay. No return value is expected.

.. method:: rpc.stream_reader(call_back, queue_depth=1, read_timeout_ms=5000):

   This method is meant to be called directly. After synchronization of the master and slave on return
   of a callback ``stream_reader`` may be called to receive data as fast as possible from the master or
   slave device. ``call_back`` will be called repeatedly with a ``bytes_or_memory_view argument`` that was
   sent by the ``stream_writer``. ``call_back`` is not expected to return anything. ``queue_depth`` defines how
   many frames of data the ``stream_writer`` may generate before slowing down and waiting on the
   ``stream_reader``. Higher ``queue_depth`` values lead to higher performance (up to a point) but require the
   ``stream_reader`` to be able to handle outstanding packets in its interface layer. If you make the
   ``queue_depth`` larger than 1 then ``call_back`` should return very quickly and not block. Finally,
   ``read_timeout_ms`` defines how many milliseconds to wait to receive the ``bytes_or_memory_view`` payload per call_back.

   On any errors ``stream_reader`` will return. The master and slave devices can try to setup the stream
   again afterwards to continue.

   If you need to cancel the ``stream_reader`` just raise an exception in the ``call_back`` and catch it. The
   remote side will automatically timeout.

.. method:: rpc.stream_writer(call_back, write_timeout_ms=5000):

   This method is meant to be called directly. After synchronization of the master and slave on return
   of a ``callback`` ``stream_writer`` may be called to send data as fast as possible from the master or slave
   device. ``call_back`` will be called repeatedly and should return a ``bytes_or_memory_view`` object that
   will be sent to the ``stream_reader``. ``call_back`` should not take any arguments. Finally,
   ``write_timeout_ms`` defines how many milliseconds to wait to send the ``bytes_or_memory_view`` object
   returned by ``call_back``.

   On any errors ``stream_writer`` will return. The master and slave devices can try to setup the stream
   again afterwards to continue.

   If you need to cancel the ``stream_writer`` just raise an exception in the ``call_back`` and catch it. The
   remote side will automatically timeout.

class rpc_master - rpc_master virtual class
-------------------------------------------

The rpc_master is a pure virtual class and not meant to be used directly. Specific interface
classes should reimplement rpc_master.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc_master()

   Creates an rpc_master object. This constructor is not meant to be used directly.

Methods
~~~~~~~

.. method:: rpc_master.call(name, data=bytes(), send_timeout=1000, recv_timeout=1000):

   Executes a remote call on the slave device. ``name`` is a string name of the remote function or method
   to execute. ``data`` is the ``bytes`` like object that will be sent as the argument of the remote function
   or method to exeucte. ``send_timeout`` defines how many milliseconds to wait while trying to connect to
   the slave and get it to execute the remote function or method. Once the master starts sending the
   argument to the slave deivce ``send_timeout`` does not apply. The library will allow the argument to
   take up to 5 seconds to be sent. ``recv_timeout`` defines how many milliseconds to wait after the slave
   started executing the remote method to receive the repsonse. Note that once the master starts
   receiving the repsonse ``recv_timeout`` does not apply. The library will allow the response to take up
   to 5 seconds to be received.

   Note that a new packet that includes a copy of ``data`` will be created internally inside the ``rpc``
   library. You may encounter memory issues on the OpenMV Cam if you try to pass very large data
   arguments.

class rpc_slave - rpc_slave virtual class
-----------------------------------------

The rpc_slave is a pure virtual class and not meant to be used directly. Specific interface
classes should reimplement rpc_slave.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc_slave()

   Creates an rpc_slave object. This constructor is not meant to be used directly.

Methods
~~~~~~~

.. method:: rpc_slave.register_callback(cb):

   Registers a call back that can be executed by the master device. The call back should take one
   argument which will be a ``memoryview`` object and it should return a ``bytes()`` like object as the
   result. The call back should return in less than 1 second if possible.

.. method:: rpc_slave.schedule_callback(cb):

   After you execute ``rpc_slave.loop()`` it is not possible to execute long running operations outside of the ``rpc``
   library. ``schedule_callback`` allows you to break out of the ``rpc`` library temporarily after completion
   of an call back. You should execute ``schedule_callback`` during the execution of an ``rpc`` call back
   method to register a new non-rpc call back that will be executed immediately after the successful
   completion of that call back you executed ``schedule_callback`` in. The function or method should not
   take any arguments. After the the call back that was registered returns it must be registered again
   in the next parent call back. On any error of the parent call back the registered call back will
   not be called and must be registered again. Here's how to use this::

       def some_function_or_method_that_takes_a_long_time_to_execute():
           <do stuff>

       def normal_rpc_call_back(data):
           <process data>
           interface.schedule_callback(some_function_or_method_that_takes_a_long_time_to_execute)
           return bytes(response)

       interface.register_callback(normal_rpc_call_back)

       interface.loop()

   ``schedule_callback`` in particular allows you to use the ``get_bytes`` and ``put_bytes`` methods for
   cut-through data transfer between one device and another without the cost of packetization which
   limits the size of the data moved inside the ``rpc`` library without running out of memory on the
   OpenMV Cam.

.. method:: rpc_slave.setup_loop_callback(cb):

   The loop call back is called every loop iteration of ``rpc_slave.loop()``. Unlike the ``rpc.schedule_callback()`` call
   back this call back stays registered after being registered once. You can use the loop call back to
   blink an activity LED or something like that. You should not use the loop call back to execute any
   blocking code as this will get in the way of polling for communication from the master.
   Additionally, the loop call back will be called at a variable rate depending on when and what call
   backs the master is trying to execute. Given this, the loop call back is not suitable for any
   method that needs to be executed at a fixed frequency.

   On the OpenMV Cam, if you need to execute something at a fixed frequency, you should setup a timer
   before executing ``rpc_slave.loop()`` and use a timer interrupt based callback to execute some function or method
   at a fixed frequency. Please see how to Write Interrupt Handlers for more information. Note: The
   `Mutex` library is installed on your OpenMV Cam along with the ``rpc`` library.

.. method:: rpc_slave.loop(recv_timeout=1000, send_timeout=1000):

   Starts execution of the ``rpc`` library on the slave to receive data. This method does not return
   (except via an exception from a call back). You should register all call backs first before
   executing this method. However, it is possible to register new call backs inside of a call back
   previously being registered that is executing.

   ``recv_timeout`` defines how long to wait to receive a command from the master device before trying
   again. ``send_timeout`` defines how long the slave will wait for the master to receive the call back
   response before going back to trying to receive. The loop call back will be executed before trying
   to receive again.

class rpc_can_master - CAN Master Interface
-------------------------------------------

Control another ``rpc`` device over CAN.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc_can_master(message_if=0x7FF, bit_rate=250000, sampling_point=75):

   Creates a CAN ``rpc`` master. This interface can move up to 1 Mb/s.

   * message_id - CAN message to use for data transport on the can bus (11-bit).
   * bit_rate - CAN bit rate.
   * sampling_point - Tseg1/Tseg2 ratio. Typically 75%. (50.0, 62.5, 75, 87.5, etc.)

   NOTE: Master and slave message ids and can bit rates must match. Connect master can high to slave
   can high and master can low to slave can lo. The can bus must be terminated with 120 ohms.

   This uses CAN2 on the STM32.

class rpc_can_slave - CAN Slave Interface
-----------------------------------------

Be controlled by another ``rpc`` device over CAN.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc_can_slave(message_id=0x7FF, bit_rate=250000, sampling_point=75):

   Creates a CAN ``rpc`` slave. This interface can move up to 1 Mb/s.

   * message_id - CAN message to use for data transport on the can bus (11-bit).
   * bit_rate - CAN bit rate.
   * sampling_point - Tseg1/Tseg2 ratio. Typically 75%. (50.0, 62.5, 75, 87.5, etc.)

   NOTE: Master and slave message ids and can bit rates must match. Connect master can high to slave
   can high and master can low to slave can lo. The can bus must be terminated with 120 ohms.

   This uses CAN2 on the STM32.

class rpc_i2c_master - I2C Master Interface
-------------------------------------------

Control another ``rpc`` device over I2C.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc_i2c_master(slave_addr=0x12, rate=100000)

   Creates a I2C ``rpc`` master. This interface can move up to 1 Mb/s.

   * slave_addr - I2C address.
   * rate - I2C Bus Clock Frequency.

   NOTE: Master and slave addresses must match. Connect master scl to slave scl and master sda
   to slave sda. You must use external pull ups. Finally, both devices must share a ground.

   This uses I2C2 on the STM32.

class rpc_i2c_slave - I2C Slave Interface
-----------------------------------------

Be controlled by another ``rpc`` device over I2C.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc_i2c_slave(slave_addr=0x12)

   Creates a I2C ``rpc`` slave. This interface can move up to 1 Mb/s.

   * slave_addr - I2C address.

   NOTE: Master and slave addresses must match. Connect master scl to slave scl and master sda
   to slave sda. You must use external pull ups. Finally, both devices must share a ground.

   This uses I2C2 on the STM32.

class rpc_spi_master - SPI Master Interface
-------------------------------------------

Control another ``rpc`` device over SPI.

Constructors
~~~~~~~~~~~~

.. class:: rpc.rpc_spi_master(cs_pin="P3", freq=10000000, clk_polarity=1, clk_phase=0)

   Creates a SPI ``rpc`` master. This interface can move up to 80 Mb/s.

   * cs_pin - Slave Select Pin.
   * freq - SPI Bus Clock Frequency.
   * clk_polarity - Idle clock level (0 or 1).
   * clk_phase - Sample data on the first (0) or second edge (1) of the clock.

   NOTE: Master and slave settings much match. Connect CS, SCLK, MOSI, MISO to CS, SCLK, MOSI, MISO.
   Finally, both devices must share a common ground.

   This uses SPI2 on the STM32.

class rpc_spi_slave - SPI Slave Interface
-----------------------------------------

Be controlled by another ``rpc`` device over SPI.

.. class:: rpc.rpc_spi_slave(cs_pin="P3", clk_polarity=1, clk_phase=0)

   Creates a SPI ``rpc`` slave. This interface can move up to 80 Mb/s.

   * cs_pin - Slave Select Pin.
   * clk_polarity - Idle clock level (0 or 1).
   * clk_phase - Sample data on the first (0) or second edge (1) of the clock.

   NOTE: Master and slave settings much match. Connect CS, SCLK, MOSI, MISO to CS, SCLK, MOSI, MISO.
   Finally, both devices must share a common ground.

   This uses SPI2 on the STM32.

class rpc_uart_master - UART Master Interface
---------------------------------------------

Control another ``rpc`` device over Async Serial (UART).

.. class:: rpc.rpc_uart_master(baudrate=115200)

   Creates a UART ``rpc`` master. This interface can move up to 7.5 Mb/s.

   * baudrate - Serial Baudrate.

   NOTE: Master and slave baud rates must match. Connect master tx to slave rx and master rx to
   slave tx. Finally, both devices must share a common ground.

   This uses UART3 on the STM32.

class rpc_uart_slave - UART Slave Interface
-------------------------------------------

Be controlled by another ``rpc`` device over Async Serial (UART).

.. class:: rpc.rpc_uart_slave(baudrate=115200)

   Creates a UART ``rpc`` slave. This interface can move up to 7.5 Mb/s.

   * baudrate - Serial Baudrate.

   NOTE: Master and slave baud rates must match. Connect master tx to slave rx and master rx to
   slave tx. Finally, both devices must share a common ground.

   This uses UART3 on the STM32.

class rpc_usb_vcp_master - USB VCP Master Interface
---------------------------------------------------

Control another ``rpc`` device over a USB Virtual COM Port.

.. class:: rpc.rpc_usb_vcp_master()

   Creates a USB VCP ``rpc`` master. This interface can move up to 12 Mb/s.

class rpc_usb_vcp_slave - USB VCP Slave Interface
-------------------------------------------------

Be controlled by another ``rpc`` device over a USB Virtual COM Port.

.. class:: rpc.rpc_usb_vcp_slave()

   Creates a USB VCP ``rpc`` slave. This interface can move up to 12 Mb/s.

class rpc_wifi_master - WiFi Master Interface
---------------------------------------------

Control another ``rpc`` device over a WiFi.

.. class:: rpc.rpc_wifi_master(ssid, ssid_key, ssid_security, ip, port=0x1DBA, mode=network.WINC.MODE_STA, static_ip=None)

   Creates a WiFi ``rpc`` master. This interface can move over 12 Mb/s.

   * ssid - WiFi network to connect to.
   * ssid_key - WiFi network password.
   * ssid_security - WiFi security.
   * ip - Slave IP Address.
   * port - Port to route traffic to.
   * mode - Regular or access-point mode.
   * static_ip - If not None then a tuple of the (IP Address, Subnet Mask, Gateway, DNS Address)

class rpc_wifi_master - WiFi Master Interface
---------------------------------------------

Be controlled by another ``rpc`` device over WiFi.

.. class:: rpc.rpc_wifi_slave(ssid, ssid_key, ssid_security, port=0x1DBA, mode=network.WINC.MODE_STA, static_ip=None)

   Creates a WiFi ``rpc`` slave. This interface can move over 12 Mb/s.

   * ssid - WiFi network to connect to.
   * ssid_key - WiFi network password.
   * ssid_security - WiFi security.
   * port - Port to route traffic to.
   * mode - Regular or access-point mode.
   * static_ip - If not None then a tuple of the (IP Address, Subnet Mask, Gateway, DNS Address)
