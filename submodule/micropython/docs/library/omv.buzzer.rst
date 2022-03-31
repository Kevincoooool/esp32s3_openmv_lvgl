:mod:`buzzer` --- buzzer driver
===============================

.. module:: buzzer
   :synopsis: buzzer driver

The ``buzzer`` module is used to control the amplitude and frequency of a buzzer onboard your OpenMV Cam.

.. note::

   The buzzer (and this module) is not present on all OpenMV Cam models.

Functions
---------

.. function:: buzzer.freq(freq)

   Sets the buzzer frequency independently of the volume.

   ``freq`` any frequency to drive the buzzer at.

.. function:: buzzer.duty(duty)

   Sets the buzzer duty cycle independently of the frequency.

   ``duty`` any PWM duty cycle percentage (0-255 for 0-100%).

Constants
---------

.. data:: buzzer.RESONANT_FREQ

   Constant definting the highest volume frequency of the buzzer (typically 4000 Hz).
