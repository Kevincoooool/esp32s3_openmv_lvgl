:mod:`rtsp` --- rtsp library
============================

.. module:: rtsp
   :synopsis: rtsp library

The ``rtsp`` module on the OpenMV Cam allows you to stream video from your OpenMV Cam to any
compatible RTSP client (like `VLC <https://www.videolan.org/vlc/index.html>`_).

How to use the Library
----------------------

Please checkout the example scripts in OpenMV IDE under ``Web Servers``.

The `rtsp_server` is very easy to use. After being created you just need to call the `rtsp_server.stream()`
method with a call back function to generate image objects. For example::

    server = rtsp.rtsp_server("SSID", "SSID_KEY", network.WINC.WPA_PSK)
    server.stream(lambda pathname, session: sensor.snapshot())

Note that not all RTSP clients can decode all types of JPEG images streamed. For best results please
use the OV2640/OV5640 camera modules `sensor.JPEG` mode for streaming RTSP JPEG video.

class rtsp_server - rtsp_server class
-------------------------------------

The `rtsp_server` class creates a single connection RTSP web server on your OpenMV Cam.

Constructors
~~~~~~~~~~~~

.. class:: rtsp.rtsp_server(ssid, ssid_key, ssid_security, port=554, mode=network.WINC.MODE_STA, static_ip=None)

   Creates a WiFi ``rtsp`` server.

   * ssid - WiFi network to connect to.
   * ssid_key - WiFi network password.
   * ssid_security - WiFi security.
   * port - Port to listen to (554).
   * mode - Regular or access-point mode.
   * static_ip - If not None then a tuple of the (IP Address, Subnet Mask, Gateway, DNS Address)

Methods
~~~~~~~

.. method:: rtsp_server.register_setup_cb(cb)

   Bind a call back (``cb``) to be executed when a client sets up a RTSP connection with the `rtsp_server`.

   Registering a call back is not required for the `rtsp_server` to work.

   The call back should accept two arguments:

   ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
   needed. Otherwise, you can use it to determine what image object to return. By default the
   ``pathname`` will be "/".

   ``session`` is random number that will change when a new connection is established. You can use
   ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

.. method:: rtsp_server.register_play_cb(cb)

   Bind a call back (``cb``) to be executed when a client wants to start streaming.

   Registering a call back is not required for the `rtsp_server` to work.

   The call back should accept two arguments:

   ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
   needed. Otherwise, you can use it to determine what image object to return. By default the
   ``pathname`` will be "/".

   ``session`` is random number that will change when a new connection is established. You can use
   ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

.. method:: rtsp_server.register_pause_cb(cb)

   Bind a call back (``cb``) to be executed when a client wants to pause streaming.

   Registering a call back is not required for the `rtsp_server` to work.

   NOTE: When you click the pause button on `VLC <https://www.videolan.org/vlc/index.html>`_ in does not tell the server to pause.

   The call back should accept two arguments:

   ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
   needed. Otherwise, you can use it to determine what image object to return. By default the
   ``pathname`` will be "/".

   ``session`` is random number that will change when a new connection is established. You can use
   ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

.. method:: rtsp_server.register_teardown_cb(cb)

   Bind a call back (``cb``) to be executed when a client wants tear down a RTSP connection with the `rtsp_server`.

   Registering a call back is not required for the `rtsp_server` to work.

   The call back should accept two arguments:

   ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
   needed. Otherwise, you can use it to determine what image object to return. By default the
   ``pathname`` will be "/".

   ``session`` is random number that will change when a new connection is established. You can use
   ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

.. method:: rtsp_server.stream(cb, quality=90)

   Starts running the `rtsp_server` logic and does not return. Make sure to setup everything you
   want to first before calling this method. Once called the `rtsp_server` will start accepting
   connections and streaming video data.

   ``cb`` should be a call back that returns an `Image` object which the RTSP library will jpeg
   compress and stream to the remote client. You are free to modify a `sensor.snapshot()` image
   as much as you like before returning the image object to be sent.

   ``quality`` is the JPEG compression quality to use while streaming.

   The call back should accept two arguments:

   ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
   needed. Otherwise, you can use it to determine what image object to return. By default the
   ``pathname`` will be "/".

   ``session`` is random number that will change when a new connection is established. You can use
   ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.
