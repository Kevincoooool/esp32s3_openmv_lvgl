.. currentmodule:: network
.. _network.LAN:

class LAN -- control built-in Ethernet interfaces
=================================================

This class provides a driver for Ethernet.  Example usage::

    import network

    lan = network.LAN()
    lan.active(True)
    lan.ifconfig('dhcp')

    # We should have a valid IP now via DHCP
    print(lan.ifconfig())

Constructors
------------

.. class:: LAN()

Create a LAN network interface object.

Methods
-------

.. method:: LAN.active([is_active])

    Activate ("up") or deactivate ("down") network interface, if boolean
    argument is passed. Otherwise, query current state if no argument is
    provided. Most other methods require active interface.

    Statuses:

        * 0: Link Down
        * 1: Link Join
        * 2: Link No-IP
        * 3: Link Up

.. method:: LAN.status([param])

    Return the current status of the connection:

        * 0: Link Down
        * 1: Link Join
        * 2: Link No-IP
        * 3: Link Up

.. method:: LAN.isconnected()

    Returns ``True`` if the link is up and false if not.

.. method:: LAN.ifconfig([(ip, subnet, gateway, dns)])

   Get/set IP-level network interface parameters: IP address, subnet mask,
   gateway and DNS server. When called with no arguments, this method returns
   a 4-tuple with the above information. To set the above values, pass a
   4-tuple with the required information.  For example::

    nic.ifconfig(('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

   For DHCP configuration do ``lan.ifconfig('dhcp')`` to get a DHCP IP.

.. method:: LAN.config('param')
            LAN.config(param=value, ...)

   Get or set general network interface parameters. These methods allow to work
   with additional parameters beyond standard IP configuration (as dealt with by
   `LAN.ifconfig()`). These include network-specific and hardware-specific
   parameters. For setting parameters, keyword argument syntax should be used,
   multiple parameters can be set at once. For querying, parameters name should
   be quoted as a string, and only one parameter can be queries at time::

    # Set params
    lan.config(trace=4)
    # Query params
    print(lan.config('mac'))

   Following are commonly supported parameters:

   =============  ===========
   Parameter      Description
   =============  ===========
   mac            MAC address (bytes)
   trace          Trace flags (int)
   =============  ===========

   When trace flags are set the system will print debug information about packets on the ethernet
   link as they are transmitted or recieved. This may generate a lot of debug text...

   * 1 = TRACE_ASYNC_EV
   * 2 = TRACE_ETH_TX
   * 4 = TRACE_ETH_RX
   * 8 = TRACE_ETH_FULL
